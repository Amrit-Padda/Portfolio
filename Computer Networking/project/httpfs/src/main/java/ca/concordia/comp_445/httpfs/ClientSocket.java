package ca.concordia.comp_445.httpfs;

import java.io.*;
import java.net.*;
import java.util.HashMap;

import ca.concordia.comp_445.commons.http.*;
import ca.concordia.comp_445.utils.*;

public class ClientSocket implements Runnable {
    private Socket socket;
    private Server server;
    private PrintWriter out;
    private InputStream in;
    private byte[] inputBuffer;

    public ClientSocket(Socket socket, Server server) {
        this.socket = socket;
        this.server = server;
        this.out = null;
        this.in = null;
        this.inputBuffer = new byte[1024];
    }

    public void run() {
        server.logInfo("Connection established");

        try {
            openConnection();

            int currSize = in.read(inputBuffer);
            if (currSize <= 0) {
                server.logError("No data received");

                var response =
                        new HttpResponse.Builder()
                                .setResponseLine(HttpVersion.HTTP_1_0, HttpStatusCode.BAD_REQUEST)
                                .setHeaders(new HashMap<>())
                                .setBody("")
                                .create();

                sendToClient(response.toString());

                closeConnection();
                return;
            }

            var line = new HttpRequest.Line();
            HttpRequest request =
                    new HttpRequest.Builder()
                            .setRequestLine(line.getMethod(), line.getPath(), line.getVersion())
                            .setHeaders(new HashMap<>())
                            .setBody("")
                            .create();

            StringBuilder builder = new StringBuilder();

            boolean parsedRequestLine = false;
            boolean parsedRequest = false;

            while (!parsedRequest) {
                builder.append(new String(inputBuffer, 0, currSize));

                String[] data = builder.toString().lines().toArray(size -> new String[size]);

                if (!parsedRequestLine) {
                    var parseResult = this.parseRequestLine(data[0]);

                    if (parseResult.second == HttpError.NO_ERROR) {
                        request = parseResult.first;
                        parsedRequestLine = true;
                    } else if (parseResult.second == HttpError.INVALID_LINE_ELEMENT_COUNT) {
                        server.logError("Received a bad request");

                        var response = new HttpResponse.Builder()
                                               .setResponseLine(HttpVersion.HTTP_1_0,
                                                       HttpStatusCode.BAD_REQUEST)
                                               .setHeaders(new HashMap<>())
                                               .setBody("")
                                               .create();

                        this.sendToClient(response.toString());
                        this.closeConnection();

                        return;
                    }
                }

                if (parsedRequestLine) {
                    int endHeaders = 0;
                    for (int i = 1; i < data.length; ++i) {
                        var keyValuePair = data[i].split(":", 2);

                        if (keyValuePair.length == 2) {
                            request.addHeader(keyValuePair[0], keyValuePair[1]);
                        } else {
                            endHeaders = i;
                            break;
                        }
                    }

                    switch (request.getRequestLine().getMethod()) {
                        case GET: {
                            if (data[endHeaders].isEmpty()) {
                                parsedRequest = true;
                            }
                            break;
                        }
                        case POST: {
                            int startOfBody = 0;
                            if (data[endHeaders].isEmpty()) {
                                startOfBody = endHeaders + 1;
                            }

                            if (startOfBody >= data.length) {
                                var response = new HttpResponse.Builder()
                                                       .setResponseLine(HttpVersion.HTTP_1_0,
                                                               HttpStatusCode.BAD_REQUEST)
                                                       .setHeaders(new HashMap<>())
                                                       .setBody("")
                                                       .create();

                                sendToClient(response.toString());

                                closeConnection();
                                return;
                            }

                            int bodyLength = 0;
                            for (var header : request.getHeaders().entrySet()) {
                                if (header.getKey().equalsIgnoreCase("Content-Length")) {
                                    try {
                                        bodyLength =
                                                Integer.parseUnsignedInt(header.getValue().strip());
                                    } catch (Exception e) {
                                        var response =
                                                new HttpResponse.Builder()
                                                        .setResponseLine(HttpVersion.HTTP_1_0,
                                                                HttpStatusCode.BAD_REQUEST)
                                                        .setHeaders(new HashMap<>())
                                                        .setBody("")
                                                        .create();

                                        sendToClient(response.toString());

                                        closeConnection();
                                        return;
                                    }
                                }
                            }

                            if (bodyLength == 0) {
                                var response = new HttpResponse.Builder()
                                                       .setResponseLine(HttpVersion.HTTP_1_0,
                                                               HttpStatusCode.BAD_REQUEST)
                                                       .setHeaders(new HashMap<>())
                                                       .setBody("")
                                                       .create();

                                sendToClient(response.toString());

                                closeConnection();
                                return;
                            }

                            StringBuilder bodyBuilder = new StringBuilder();

                            for (int i = startOfBody; i < data.length; ++i) {
                                if (bodyLength != 0) {
                                    var charArray = data[i].toCharArray();
                                    if (charArray.length >= bodyLength) {
                                        bodyBuilder.append(data[i]);

                                        bodyLength -= charArray.length;
                                    } else {
                                        for (char c : charArray) {
                                            if (bodyLength != 0) {
                                                bodyBuilder.append(c);
                                                --bodyLength;
                                            }
                                        }
                                    }
                                }
                            }

                            if (bodyLength == 0) {
                                parsedRequest = true;

                                request.setBody(bodyBuilder.toString());
                            }

                            break;
                        }
                    }
                }

                if (!parsedRequest) {
                    currSize = in.read(inputBuffer);
                }
            }

            server.logInfo("Received request from client:\n");
            server.logInfo(request.toString() + "\n");

            switch (request.getRequestLine().getMethod()) {
                case GET:
                    sendToClient(server.serverResponseToGetRequest(request).toString());
                    break;
                case POST:
                    sendToClient(server.serverResponseToPostRequest(request).toString());
                    break;
                default: {
                    server.logError("Received a bad request");

                    server.logError("Here is the faulty request: \n");
                    server.logError(request.toString() + "\n");

                    var response = new HttpResponse.Builder()
                                           .setResponseLine(request.getRequestLine().getVersion(),
                                                   HttpStatusCode.BAD_REQUEST)
                                           .setHeaders(request.getHeaders())
                                           .setBody("")
                                           .create();

                    response.addHeader("Content-Length", "0");

                    sendToClient(response.toString());
                    break;
                }
            }

            closeConnection();
            return;
        } catch (IOException e) {
            server.logError(e.getMessage());

            closeConnection();
            return;
        }
    }

    private void openConnection() throws IOException {
        out = new PrintWriter(socket.getOutputStream());
        in = socket.getInputStream();
    }

    private void closeConnection() {
        try {
            in.close();
            out.close();
            socket.close();
        } catch (IOException e) {
            server.logError(e.getMessage());
        }

        server.logInfo("Connection closed");
    }

    private void sendToClient(String text) {
        server.logInfo("Sending to client:\n");
        server.logInfo(text + "\n");

        out.write(text);
        out.flush();
    }

    private Pair<HttpRequest, HttpError> parseRequestLine(String data) {
        var elems = data.split(" ");

        if (elems.length == 3) {
            var method = parseMethod(elems[0]);
            if (method.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_METHOD);
            }

            var uri = parseURI(elems[1]);
            if (uri.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_URI);
            }

            var version = parseVersion(elems[2]);
            if (version.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_HTTP_VERSION);
            }

            HttpRequest request =
                    new HttpRequest.Builder()
                            .setRequestLine(method.first, uri.first.toString(), version.first)
                            .setHeaders(new HashMap<>())
                            .setBody("")
                            .create();

            return Pair.make(request, HttpError.NO_ERROR);

        } else {
            return Pair.make(null, HttpError.INVALID_LINE_ELEMENT_COUNT);
        }
    }

    private Pair<HttpRequest.Method, HttpError> parseMethod(String method) {
        if (method.compareToIgnoreCase(HttpRequest.Method.GET.toString()) == 0) {
            return Pair.make(HttpRequest.Method.GET, HttpError.NO_ERROR);
        } else if (method.compareToIgnoreCase(HttpRequest.Method.POST.toString()) == 0) {
            return Pair.make(HttpRequest.Method.POST, HttpError.NO_ERROR);
        } else {
            return Pair.make(HttpRequest.Method.GET, HttpError.INVALID_METHOD);
        }
    }

    private Pair<URI, HttpError> parseURI(String version) {
        try {
            return Pair.make(new URI(version), HttpError.NO_ERROR);
        } catch (URISyntaxException e) {
            return Pair.make(null, HttpError.INVALID_URI);
        }
    }

    private Pair<HttpVersion, HttpError> parseVersion(String version) {
        if (version.equalsIgnoreCase(HttpVersion.HTTP_1_0.valueOf())) {
            return Pair.make(HttpVersion.HTTP_1_0, HttpError.NO_ERROR);
        } else if (version.equalsIgnoreCase(HttpVersion.HTTP_1_1.valueOf())) {
            return Pair.make(HttpVersion.HTTP_1_1, HttpError.NO_ERROR);
        } else {
            return Pair.make(HttpVersion.HTTP_1_0, HttpError.INVALID_HTTP_VERSION);
        }
    }
}
