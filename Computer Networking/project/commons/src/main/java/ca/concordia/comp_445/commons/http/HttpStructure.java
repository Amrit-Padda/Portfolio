package ca.concordia.comp_445.commons.http;

import java.nio.ByteBuffer;
import java.util.HashMap;

public class HttpStructure {
    private HashMap<String, String> headers;
    private String body;

    public HttpStructure() {
        this.headers = new HashMap<String, String>();
        this.body = new String();
    }

    public HttpStructure(HashMap<String, String> headers, String body) {
        this.headers = headers;
        this.body = body;
    }

    protected String formatHeaders() {
        StringBuilder builder = new StringBuilder();

        this.headers.forEach((k, v) -> builder.append(k + ":" + v + "\r\n"));

        return builder.toString();
    }

    public void addHeader(String key, String value) {
        this.headers.put(key, value);
    }

    public ByteBuffer toByteBuffer() {
        var headers = formatHeaders().getBytes();
        var body = this.body.getBytes();

        ByteBuffer buffer = ByteBuffer.allocate(headers.length + body.length);
        buffer.put(headers);
        buffer.put(body);

        return buffer;
    }

    /* ---- Getters and Setters ---- */

    public HttpStructure setBody(String body) {
        this.body = body;
        return this;
    }

    public String getBody() {
        return this.body;
    }

    public HttpStructure setHeader(String key, String value) {
        this.headers.put(key, value);
        return this;
    }

    public HashMap<String, String> getHeaders() {
        return this.headers;
    }
}
