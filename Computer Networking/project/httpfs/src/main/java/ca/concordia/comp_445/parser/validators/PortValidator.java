package ca.concordia.comp_445.parser.validators;

import com.beust.jcommander.*;

public class PortValidator implements IParameterValidator {
    public void validate(String name, String value) throws ParameterException {
        try {
            int n = Integer.parseInt(value);
            if (n < 0) {
                throw new ParameterException(
                        "Parameter " + name + " should be positive (found " + value + ")");
            }
        } catch (NumberFormatException e) {
            throw new ParameterException("Parameter " + name
                    + " should be a positive intereger value (found " + value + ")");
        }
    }
}
