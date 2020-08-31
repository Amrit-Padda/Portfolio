package ca.concordia.comp_445.parser.commands;

import com.beust.jcommander.*;

import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import ca.concordia.comp_445.parser.converters.*;
import ca.concordia.comp_445.parser.validators.*;

/**
 * A GET command format used by {@link JCommander} for parsing
 */
public class GetCommand {
    @Parameter(description = "The address of the server host", validateWith = URLValidator.class,
            converter = URLConverter.class)
    public URL serverHost;

    @Parameter(names = {"-v", "--verbose"},
            description = "Prints the detail of the response such as protocal, status, and headers",
            required = false)
    public boolean verbose;

    @Parameter(names = {"-h", "--header"},
            description = "Associates headers to HTTP Request with the format \'key:value\'",
            validateWith = HeaderValidator.class, required = false)
    public List<String> headers = new ArrayList<>();

    @Parameter(names = {"-p", "--port"},
            description = "Associates a port number to the server host",
            validateWith = PortValidator.class, required = true)
    public int serverPort;

    @Parameter(names = {"-R", "--router-host"}, description = "Associates a url to the router host",
            validateWith = URLValidator.class, converter = URLConverter.class, required = true)
    public URL routerHost;

    @Parameter(names = {"-P", "--router-port"},
            description = "Associates a port number to the router host",
            validateWith = PortValidator.class, required = true)
    public int routerPort;
};
