package ca.concordia.comp_445.parser.converters;

import com.beust.jcommander.IStringConverter;

import java.net.MalformedURLException;
import java.net.URL;

public class URLConverter implements IStringConverter<URL> {
    @Override
    public URL convert(String value) {
        try {
            return new URL(value);
        } catch (MalformedURLException e) {
            return null;
        }
    }
}
