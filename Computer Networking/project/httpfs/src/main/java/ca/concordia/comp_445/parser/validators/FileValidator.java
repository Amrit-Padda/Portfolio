package ca.concordia.comp_445.parser.validators;

import com.beust.jcommander.*;

import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.nio.file.Paths;

public class FileValidator implements IParameterValidator {
    public void validate(String name, String value) throws ParameterException {
        try {
            Path path = Paths.get(value);

            if (Files.notExists(path)) {
                throw new ParameterException("Parameter " + name
                        + " should have a filepath leading to a file (found " + value + ")");
            }

        } catch (InvalidPathException e) {
            throw new ParameterException(
                    "Parameter " + name + " should have a valid filepath (found " + value + ")");
        }
    }
}
