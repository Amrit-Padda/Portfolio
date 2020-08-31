package ca.concordia.comp_445.parser.commands;

import com.beust.jcommander.*;

import ca.concordia.comp_445.parser.validators.CommandValidator;

/**
 * A HELP command format used by {@link JCommander} for parsing.
 */
public class HelpCommand {
    @Parameter(description = "Command name", validateWith = CommandValidator.class)
    public String main;

    @Override
    public String toString() {
        if (main == null) {
            return "httpc is a curl-like application but supports HTTP protocol only.\n"
                    + "Usage:\n\thttpc command [arguments]\n"
                    + "The commands are:\n"
                    + "\tGET\texecutes a HTTP GET request and prints the response.\n"
                    + "\tPOST\texecutes a HTTP POST request and prints the response.\n"
                    + "\tHELP\tprints this screen.\n\n"
                    + "Use \"httpc help [command]\" for more information about a command.";
        }

        if (main.equalsIgnoreCase("GET")) {
            return "usage: httpc GET [options] URL\n\n"
                    + "Get executes a HTTP GET request for a given URL.\n\n"
                    + "Options:\n"
                    + "\t-v, --verbose\t\t \tPrints the detail of the response such as protocol, status, and headers.\n"
                    + "\t-h, --header\t  key:value\tAssociates headers to HTTP Request with the format 'key:value'.\n"
                    + "\t-p, --port\t  port-number\tDefine the server's post number. REQUIRED.\n"
                    + "\t-R, --router-host URL\t\tDefine the router's host URL. REQUIRED.\n"
                    + "\t-P, --router-port port-number\tDefine the router's port number. REQUIRED.";
        }

        if (main.equalsIgnoreCase("POST")) {
            return "usage: httpc POST [options] URL\n\n"
                    + "Get executes a HTTP GET request for a given URL.\n\n"
                    + "Options:\n"
                    + "\t-v, --verbose\t\t \tPrints the detail of the response such as protocol, status, and headers.\n"
                    + "\t-h, --header\t  key:value\tAssociates headers to HTTP Request with the format 'key:value'.\n"
                    + "\t-d, --inlide-data string\tAssociates an inline data to the body HTTP POST request.\n"
                    + "\t-f, --file\t  file\t\tAssociates the content of a file to the body HTTP POST request.\n"
                    + "\t-p, --port\t  port-number\tDefine the server's post number. REQUIRED.\n"
                    + "\t-R, --router-host URL\t\tDefine the router's host URL. REQUIRED.\n"
                    + "\t-P, --router-port port-number\tDefine the router's port number. REQUIRED.\n"
                    + "Either [-d] or [-f] can be used but not both.";
        }

        return "";
    }
}
