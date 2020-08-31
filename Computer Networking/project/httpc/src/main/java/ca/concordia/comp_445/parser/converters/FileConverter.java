package ca.concordia.comp_445.parser.converters;

import com.beust.jcommander.IStringConverter;

import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.nio.file.Paths;

public class FileConverter implements IStringConverter<Path> {
    @Override
    public Path convert(String value) {
        try {
            return Paths.get(value);
        } catch (InvalidPathException e) {
            return null;
        }
    }
}
