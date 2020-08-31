package ca.concordia.comp_445;

import com.beust.jcommander.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.net.InetSocketAddress;
import java.net.URL;
import java.nio.file.Path;

import ca.concordia.comp_445.httpfs.ServerUDP;
import ca.concordia.comp_445.parser.converters.*;
import ca.concordia.comp_445.parser.validators.*;
import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.ConsoleAppender;

public class Main {
    @Parameter(names = {"-v", "--verbose"},
            description = "Prints the detail of the response such as protocal, status, and headers",
            required = false)
    public boolean verbose;

    @Parameter(names = {"-p", "--server-port"}, description = "The local port of the server",
            validateWith = PortValidator.class, required = false)
    public int serverPort;

    @Parameter(names = {"-P", "--router-port"}, description = "The local port of the server",
            validateWith = PortValidator.class, required = true)
    public int routerPort;

    @Parameter(names = {"-R", "--router_addr"}, description = "The address of the router",
            validateWith = URLValidator.class, converter = URLConverter.class, required = true)
    public URL routerAddr;

    @Parameter(names = {"-d", "--directory"}, description = "The directory of the server",
            validateWith = FileValidator.class, converter = FileConverter.class, required = false)
    public Path directory;

    private static Logger logger = LoggerFactory.getLogger(Main.class);

    public static void main(String[] args) {
        LoggerContext loggerContext = (LoggerContext) LoggerFactory.getILoggerFactory();
        var rootLogger = loggerContext.getLogger(Logger.ROOT_LOGGER_NAME);

        rootLogger.detachAndStopAllAppenders();

        PatternLayoutEncoder encoder = new PatternLayoutEncoder();
        encoder.setContext(loggerContext);
        encoder.setPattern("%date{HH:mm:ss.SSS} %level - %msg%n");
        encoder.start();

        ConsoleAppender<ILoggingEvent> consoleAppender = new ConsoleAppender<ILoggingEvent>();
        consoleAppender.setContext(loggerContext);
        consoleAppender.setEncoder(encoder);
        consoleAppender.start();

        rootLogger.addAppender(consoleAppender);
        rootLogger.setLevel(Level.INFO);

        var main = new Main();

        JCommander parser = JCommander.newBuilder().addObject(main).build();

        try {
            parser.parse(args);
        } catch (ParameterException e) {
            logger.error(e.getMessage());
            System.exit(-1);
        }

        if (main.verbose) {
            rootLogger.setLevel(Level.DEBUG);
        }

        new ServerUDP()
                .setPort(main.serverPort)
                .setHomeDir(main.directory)
                .setRouterAddr(new InetSocketAddress(main.routerAddr.getHost(), main.routerPort))
                .start();
    }
}
