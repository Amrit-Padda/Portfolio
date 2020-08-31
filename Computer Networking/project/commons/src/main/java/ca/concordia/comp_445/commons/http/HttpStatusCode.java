package ca.concordia.comp_445.commons.http;

/**
 * An enum used to represent the possible status codes in a {@link HttpResponse}
 */
public enum HttpStatusCode {
    OK(200, "OK"),
    CREATED(201, "Created"),
    BAD_REQUEST(400, "Bad Request"),
    FORBIDDEN(403, "Forbidden"),
    NOT_FOUND(404, "Not found");

    private int value;
    private String name;

    private HttpStatusCode(int value, String name) {
        this.value = value;
        this.name = name;
    }

    public String toString() {
        return this.value + " " + this.name;
    }

    public int valueOf() {
        return this.value;
    }

    public String valueOfName() {
        return this.name;
    }
}
