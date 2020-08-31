package ca.concordia.comp_445.commons.http;

/**
 * An enum used to represent possible errors when parsing an {@link HttpRequest} or {@link
 * HttpResponse}.
 */
public enum HttpError {
    NO_ERROR,
    INVALID_LINE_ELEMENT_COUNT,
    INVALID_METHOD,
    INVALID_HTTP_VERSION,
    INVALID_URI,
    NO_CONTENT_LENGTH;
}
