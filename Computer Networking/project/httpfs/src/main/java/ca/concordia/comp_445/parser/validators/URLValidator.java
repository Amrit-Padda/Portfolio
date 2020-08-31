package ca.concordia.comp_445.parser.validators;

import com.beust.jcommander.*;

import java.net.MalformedURLException;
import java.net.URL;

public class URLValidator implements IParameterValidator {
    public void validate(String name, String value) throws ParameterException {
        try {
            var url = new URL(value);
        } catch (MalformedURLException e) {
            throw new ParameterException(
                    "Parameter " + name + " is not a valid URL (found " + value + ")");
        }
    }
}
