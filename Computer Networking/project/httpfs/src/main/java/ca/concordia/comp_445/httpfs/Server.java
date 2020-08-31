package ca.concordia.comp_445.httpfs;

import java.io.*;
import java.net.ServerSocket;
import java.nio.file.*;
import java.util.concurrent.*;
import java.util.regex.*;

import ca.concordia.comp_445.commons.http.*;

public class Server {
    private static final int DEFAULT_PORT = 8080;

    // server settings
    private final int coreCount;
    private final boolean isVerbose;
    private final Path homeFilePath;

    private ServerSocket serverSocket;
    private ThreadPoolExecutor threadPool;
    private FileInputStream fileIn = null;
    private FileOutputStream fileOut = null;
    private File fileobj = null;
    private Pattern pattern;

    public Server(int port, boolean isVerbose, Path homeFilePath) {
        this.isVerbose = isVerbose;
        this.homeFilePath = homeFilePath;
        this.coreCount = Runtime.getRuntime().availableProcessors();
        this.threadPool = (ThreadPoolExecutor) Executors.newFixedThreadPool(this.coreCount);
        this.pattern = Pattern.compile("^[a-zA-Z]*(\\/\\.\\.\\/)", Pattern.MULTILINE);

        try {
            this.serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            if (port == DEFAULT_PORT) {
                logError("Failed to use default port " + DEFAULT_PORT);
                logError("Exiting...");
                System.exit(-1);
            }

            logError("Could not listen to port: " + port);
            logError("Attempting to use default port " + DEFAULT_PORT + "...");

            try {
                this.serverSocket = new ServerSocket(DEFAULT_PORT);
                port = DEFAULT_PORT;
            } catch (IOException innerE) {
                logError("Failed to use default port " + DEFAULT_PORT);
                logError("Exiting...");
                System.exit(-1);
            }
        }

        logInfo("Creating server on port: " + port);
        logInfo("Server home directory is at location: " + homeFilePath.toString());
        logInfo("The server may support up to " + this.coreCount + " clients concurrently");
    }

    public void start() {
        boolean isRunning = true;
        while (isRunning) {
            try {
                this.threadPool.execute(new ClientSocket(serverSocket.accept(), this));
            } catch (IOException e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
        this.threadPool.shutdown();
    }

    synchronized public HttpResponse serverResponseToGetRequest(HttpRequest request) {
        try {
            var relativeFilepath = request.getRequestLine().getPath().substring(1);
            // do checking on filepath
            Matcher matcher = pattern.matcher(relativeFilepath);

            if (matcher.find()) {
                logError("Cannot access parent directory");

                var response = new HttpResponse.Builder()
                                       .setResponseLine(request.getRequestLine().getVersion(),
                                               HttpStatusCode.FORBIDDEN)
                                       .setHeaders(request.getHeaders())
                                       .setBody("")
                                       .create();

                return response;
            }

            Path filePath = Path.of(homeFilePath.toString(), relativeFilepath);
            if (Files.exists(filePath)) {
                if (Files.isHidden(filePath) || Files.isReadable(filePath)) {
                    String fileData = new String(Files.readAllBytes(filePath));

                    var response = new HttpResponse.Builder()
                                           .setResponseLine(request.getRequestLine().getVersion(),
                                                   HttpStatusCode.OK)
                                           .setHeaders(request.getHeaders())
                                           .setBody(fileData)
                                           .create();

                    response.addHeader("Content-Length", Long.toString(fileData.length()));
                    response.addHeader("Content-Type", "text/html");

                    return response;

                } else {
                    var response = new HttpResponse.Builder()
                                           .setResponseLine(request.getRequestLine().getVersion(),
                                                   HttpStatusCode.FORBIDDEN)
                                           .setHeaders(request.getHeaders())
                                           .setBody("")
                                           .create();

                    return response;
                }
            } else {
                var response = new HttpResponse.Builder()
                                       .setResponseLine(request.getRequestLine().getVersion(),
                                               HttpStatusCode.FORBIDDEN)
                                       .setHeaders(request.getHeaders())
                                       .setBody("")
                                       .create();

                return response;
            }
        } catch (IOException e) {
            logError("File IO Exception");
        }
        return null;
    }

    synchronized public HttpResponse serverResponseToPostRequest(HttpRequest request) {
        try {
            var relativeFilepath = request.getRequestLine().getPath().substring(1);
            Matcher matcher = pattern.matcher(relativeFilepath);
            if (matcher.find()) {
                logError("Cannot access parent directory");

                var response = new HttpResponse.Builder()
                                       .setResponseLine(request.getRequestLine().getVersion(),
                                               HttpStatusCode.FORBIDDEN)
                                       .setHeaders(request.getHeaders())
                                       .setBody("")
                                       .create();

                return response;
            }

            Path filePath = Path.of(homeFilePath.toString(), relativeFilepath);
            if (Files.exists(filePath)) {
                if (Files.isHidden(filePath) || !Files.isReadable(filePath)
                        || !Files.isWritable(filePath)) {
                    var response = new HttpResponse.Builder()
                                           .setResponseLine(request.getRequestLine().getVersion(),
                                                   HttpStatusCode.FORBIDDEN)
                                           .setHeaders(request.getHeaders())
                                           .setBody("")
                                           .create();

                    return response;
                }
            }
            var writer = Files.newBufferedWriter(filePath);
            writer.write(request.getBody());
            writer.flush();
            writer.close();

            var response = new HttpResponse.Builder()
                                   .setResponseLine(
                                           request.getRequestLine().getVersion(), HttpStatusCode.OK)
                                   .setHeaders(request.getHeaders())
                                   .setBody("")
                                   .create();

            return response;
        } catch (IOException e) {
            System.out.print("File IO Exception");
            System.out.println("File does not exist");

            var response = new HttpResponse.Builder()
                                   .setResponseLine(request.getRequestLine().getVersion(),
                                           HttpStatusCode.NOT_FOUND)
                                   .setHeaders(request.getHeaders())
                                   .setBody("")
                                   .create();

            return response;
        }
    }

    synchronized public void logInfo(String message) {
        if (isVerbose) {
            System.out.println(message);
        }
    }

    synchronized public void logError(String message) {
        System.err.println(message);
    }
}
