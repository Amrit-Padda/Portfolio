package ca.concordia.comp_445.parser.validators;

import com.beust.jcommander.*;

public class HeaderValidator implements IParameterValidator {
    public void validate(String name, String value) throws ParameterException {
        if (!value.contains(":")) {
            throw new ParameterException("Parameter " + name
                    + " should be a valid \"key:value\" pair (found " + value + ")");
        }
    }
}
