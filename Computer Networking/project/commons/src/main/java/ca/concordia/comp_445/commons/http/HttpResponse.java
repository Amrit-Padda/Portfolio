package ca.concordia.comp_445.commons.http;

import java.nio.ByteBuffer;
import java.util.HashMap;

public class HttpResponse extends HttpStructure {
    private Line statusLine;

    /**
     * Create a {@link HttpResponse}.
     *
     * @param statusLine The status line ({@link Line}) used by the response.
     * @param headers The headers used by the response.
     * @param body The body of the response.
     */
    public HttpResponse(Line statusLine, HashMap<String, String> headers, String body) {
        super(headers, body);

        this.statusLine = statusLine;
    }

    /**
     * the {@link HttpResponse} package data representation as a string.
     *
     * @return the {@link HttpResponse} object into a string representation of the data.
     */
    @Override
    public String toString() {
        return this.statusLine.toString() + formatHeaders() + "\r\n" + this.getBody();
    }

    public Line getLine() {
        return this.statusLine;
    }

    /**
     * Get a byte representation of the {@link HttpResponse}.
     *
     * @return the {@link HttpResponse} represented as a {@link ByteBuffer}.
     */
    @Override
    public ByteBuffer toByteBuffer() {
        var line = statusLine.toString().getBytes();
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
     * A class used to represent the request line of a {@link HttpResponse}.
     */
    public static class Line {
        private HttpVersion version;
        private HttpStatusCode statusCode;

        /**
         * Default constructor.
         */
        public Line() {
            this(HttpVersion.HTTP_1_0, HttpStatusCode.BAD_REQUEST);
        }

        /**
         * Create a response line for a {@link HttpResponse} object.
         *
         * @param version The http version ({@link HttpVersion}) of the {@link HttpResponse}.
         * @param code The status code ({@link HttpStatusCode}) of the {@link HttpResponse}.
         */
        public Line(HttpVersion version, HttpStatusCode code) {
            this.version = version;
            this.statusCode = code;
        }

        /**
         * Give a string representation of the {@link Line} object.
         *
         * @return The {@link Line} represented as a string.
         */
        @Override
        public String toString() {
            return this.version.toString() + " " + this.statusCode.toString() + "\r\n";
        }

        public HttpVersion getVersion() {
            return this.version;
        }
        public HttpStatusCode getStatusCode() {
            return this.statusCode;
        }
    }

    /**
     * A class used to build a {@link HttpResponse} object.
     */
    public static class Builder {
        private Line responseLine;
        private HashMap<String, String> headers;
        private String body;

        public Builder setResponseLine(HttpVersion version, HttpStatusCode statusCode) {
            this.responseLine = new Line(version, statusCode);
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

        public HttpResponse create() {
            return new HttpResponse(responseLine, headers, body);
        }
    }
}
