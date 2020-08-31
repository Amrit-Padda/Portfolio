package ca.concordia.comp_445.httpfs;

import static java.nio.charset.StandardCharsets.UTF_8;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import ca.concordia.comp_445.commons.http.HttpRequest;
import ca.concordia.comp_445.commons.http.HttpResponse;
import ca.concordia.comp_445.commons.http.HttpStatusCode;
import ca.concordia.comp_445.commons.http.HttpVersion;
import ca.concordia.comp_445.commons.net.Packet;
import ca.concordia.comp_445.httpfs.events.*;
import ca.concordia.comp_445.utils.Event;
import ca.concordia.comp_445.utils.Observer;

public class ServerUDP implements Observer {
    private static final Logger logger = LoggerFactory.getLogger(ServerUDP.class);

    boolean isVerbose;
    int portNumber = 8080;
    Path homeFilePath = Paths.get(System.getProperty("user.home"));

    InetSocketAddress routerAddr = new InetSocketAddress("localhost", 3000);

    private Map<InetSocketAddress, Client> clients = new HashMap<>();

    private Pattern pattern;
    private Pattern pattern1;

    public ServerUDP() {
        this.pattern = Pattern.compile("^[a-zA-Z]*(\\/\\.\\.\\/)", Pattern.MULTILINE);
        this.pattern1 = Pattern.compile("^/\\s", Pattern.MULTILINE);
    }

    public ServerUDP setPort(int portNumber) {
        if (portNumber != 0) {
            this.portNumber = portNumber;
        }
        return this;
    }

    public ServerUDP setHomeDir(Path filepath) {
        if (filepath != null) {
            this.homeFilePath = filepath;
        }
        return this;
    }

    public ServerUDP setRouterAddr(InetSocketAddress routerAddr) {
        this.routerAddr = routerAddr;
        return this;
    }

    /**
     * Start the server.
     */
    public void start() {
        logger.info("Server Started on port: {}", portNumber);

        try (Selector selector = Selector.open()) {
            DatagramChannel channel = DatagramChannel.open();
            channel.configureBlocking(false);
            channel.bind(new InetSocketAddress(portNumber));

            channel.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE);

            logger.info("Server ready for connections...");

            ByteBuffer buffer = ByteBuffer.allocate(Packet.MAX_LEN).order(ByteOrder.BIG_ENDIAN);

            while (true) {
                int readyChannelCount = selector.selectNow();

                if (readyChannelCount > 0) {
                    Set<SelectionKey> readyKeys = selector.selectedKeys();
                    var iterator = readyKeys.iterator();

                    while (iterator.hasNext()) {
                        SelectionKey key = iterator.next();

                        buffer.clear();

                        SocketAddress router = channel.receive(buffer);

                        buffer.flip();
                        if (buffer.remaining() != 0) {
                            Packet packet = Packet.fromBuffer(buffer);
                            buffer.flip();

                            InetSocketAddress clientAddr = new InetSocketAddress(
                                    packet.getPeerAddress().getHostName(), packet.getPeerPort());

                            if (!clients.containsKey(clientAddr)) {
                                var client = new Client(this, clientAddr);
                                client.attach(this);

                                clients.put(clientAddr, client);

                                logger.debug("New client {}", clientAddr.toString());
                            }

                            if (clients.containsKey(clientAddr)) {
                                clients.get(clientAddr).listen(channel, packet, router);
                            }
                        }

                        for (var client : clients.values()) {
                            client.respond(channel);
                        }

                        iterator.remove();
                    }
                }
            }

        } catch (IOException e) {
            logger.error("Failed to create server: {}", e.getMessage());
            System.exit(-1);
        }

        logger.info("Server Closing");
    }

    synchronized public HttpResponse serverResponseToGetRequest(HttpRequest request) {
        try {
            var relativeFilepath = request.getRequestLine().getPath().substring(1);
            // do checking on filepath
            Matcher matcher = pattern.matcher(relativeFilepath);
            Matcher matcher1 = pattern1.matcher(relativeFilepath);

            if (matcher.find()) {
                logger.error("Cannot access parent directory");

                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                .setHeaders(request.getHeaders())
                                .setBody("")
                                .create();

                return response;
            }

            if (matcher1.find()) {
                logger.error("Cannot get entire directory");

                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
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
                                           .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.OK)
                                           .setHeaders(request.getHeaders())
                                           .setBody(fileData)
                                           .create();

                    response.addHeader("Content-Length", Long.toString(fileData.length()));
                    response.addHeader("Content-Type", "text/html");

                    return response;

                } else {
                    var response =
                            new HttpResponse.Builder()
                                    .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                    .setHeaders(request.getHeaders())
                                    .setBody("")
                                    .create();

                    return response;
                }
            } else {
                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                .setHeaders(request.getHeaders())
                                .setBody("")
                                .create();

                return response;
            }
        } catch (IOException e) {
            logger.error("{}", e.getMessage());
        }
        return null;
    }

    synchronized public HttpResponse serverResponseToPostRequest(HttpRequest request) {
        try {
            var relativeFilepath = request.getRequestLine().getPath().substring(1);
            Matcher matcher = pattern.matcher(relativeFilepath);
            Matcher matcher1 = pattern1.matcher(relativeFilepath);

            if (matcher.find()) {
                logger.error("Cannot access parent directory");

                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                .setHeaders(request.getHeaders())
                                .setBody("")
                                .create();

                return response;
            }

            if (matcher1.find()) {
                logger.error("Cannot get entire directory");

                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                .setHeaders(request.getHeaders())
                                .setBody("")
                                .create();

                return response;
            }

            Path filePath = Path.of(homeFilePath.toString(), relativeFilepath);
            if (Files.exists(filePath)) {
                if (Files.isHidden(filePath) || !Files.isReadable(filePath)
                        || !Files.isWritable(filePath)) {
                    var response =
                            new HttpResponse.Builder()
                                    .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.FORBIDDEN)
                                    .setHeaders(request.getHeaders())
                                    .setBody("")
                                    .create();

                    return response;
                }
            }
            logger.error(filePath.toString());

            var writer = Files.newBufferedWriter(filePath);
            writer.write(request.getBody());
            writer.flush();
            writer.close();

            var response = new HttpResponse.Builder()
                                   .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.OK)
                                   .setHeaders(request.getHeaders())
                                   .setBody("")
                                   .create();

            return response;
        } catch (IOException e) {
            System.out.print("File IO Exception");
            System.out.println("File does not exist");

            var response = new HttpResponse.Builder()
                                   .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.NOT_FOUND)
                                   .setHeaders(request.getHeaders())
                                   .setBody("")
                                   .create();

            return response;
        }
    }

    public void onNotify(Event e) {
        if (e instanceof CloseClientEvent) {
            var clientAddress = ((CloseClientEvent) e).getClientAddress();
            this.clients.remove(clientAddress);

            logger.info("Client {} was removed", clientAddress.toString());
        }
    }
}
