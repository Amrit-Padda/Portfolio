package ca.concordia.comp_445.commons.http;

import java.nio.ByteBuffer;
import java.util.HashMap;

public class HttpRequest extends HttpStructure {
    /**
     * Represents the possible types of request a {@link HttpRequest} may have.
     */
    public enum Method { GET, POST }

    private Line requestLine;

    /**
     * Create a HttpRequest Package to encapsulate a request to a server
     *
     * @param requestLine the request {@link Line} of the package.
     * @param headers the headers of the package.
     * @param body the body of the package
     */
    public HttpRequest(Line requestLine, HashMap<String, String> headers, String body) {
        super(headers, body);

        this.requestLine = requestLine;
    }

    /**
     * the {@link HttpRequest} package data representation as a string.
     *
     * @return the {@link HttpRequest} object into a string representation of the data.
     */
    @Override
    public String toString() {
        return this.requestLine.toString() + this.formatHeaders() + "\r\n" + this.getBody();
    }

    /**
     * Get a byte representation of the {@link HttpRequest}.
     *
     * @return the {@link HttpRequest} represented as a {@link ByteBuffer}.
     */
    @Override
    public ByteBuffer toByteBuffer() {
        var line = requestLine.toString().getBytes();
        var headers = formatHeaders().getBytes();
        var body = this.getBody().getBytes();

        ByteBuffer buffer = ByteBuffer.allocate(line.length + headers.length + body.length);
        buffer.put(line);
        buffer.put(headers);
        buffer.put(body);
        buffer.flip();

        return buffer;
    }

    /**
     * Get the {@link HttpRequest}'s request line.
     *
     * @return the {@link HttpRequest} request line.
     */
    public Line getRequestLine() {
        return this.requestLine;
    }

    /**
     * A class used to represent the request line of a {@link HttpRequest}.
     */
    public static class Line {
        private Method method;
        private String path;
        private HttpVersion version;

        /**
         * Default constructor.
         */
        public Line() {
            this(null, null, HttpVersion.HTTP_1_0);
        }

        /**
         * Creates a request line for a {@link HttpRequest} object.
         *
         * @param method The method of the {@link HttpRequest}.
         * @param path The relative path of the file to access.
         * @param version The version of HTTP the request is using.
         */
        public Line(Method method, String path, HttpVersion version) {
            this.method = method;
            this.path = path;
            this.version = version;
        }

        /**
         * Give a string representation of the {@link Line} object.
         *
         * @return The {@link Line} represented as a string.
         */
        @Override
        public String toString() {
            return this.method.toString() + " " + this.path + " " + this.version.valueOf() + "\r\n";
        }

        public Method getMethod() {
            return this.method;
        }
        public String getPath() {
            return this.path;
        }
        public HttpVersion getVersion() {
            return this.version;
        }
    }

    /**
     * A class used to build a {@link HttpRequest} object.
     */
    public static class Builder {
        private Line statusLine;
        private HashMap<String, String> headers;
        private String body;

        public Builder setRequestLine(Method method, String path, HttpVersion version) {
            this.statusLine = new Line(method, path, version);
            return this;
        }

        public Builder setHeaders(HashMap<String, String> headers) {
            this.headers = headers;
            return this;
        }

        public Builder setBody(String body) {
            this.body = body;
            return this;
        }

        public HttpRequest create() {
            return new HttpRequest(statusLine, headers, body);
        }
    }
}
