package ca.concordia.comp_445.parser.validators;

import com.beust.jcommander.*;

public class CommandValidator implements IParameterValidator {
    public void validate(String name, String value) throws ParameterException {
        if (value != null && value.compareToIgnoreCase("GET") != 0
                && value.compareToIgnoreCase("POST") != 0) {
            throw new ParameterException(
                    "Parameter " + name + " should be a valid command name (found " + value + ")");
        }
    }
}
