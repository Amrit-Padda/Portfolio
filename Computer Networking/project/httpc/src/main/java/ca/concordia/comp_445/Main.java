package ca.concordia.comp_445;

import com.beust.jcommander.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.List;

import ca.concordia.comp_445.commons.http.HttpRequest;
import ca.concordia.comp_445.commons.http.HttpVersion;
import ca.concordia.comp_445.httpc.Httpc;
import ca.concordia.comp_445.parser.commands.*;
import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.ConsoleAppender;

public class Main {
    private static final Logger logger = LoggerFactory.getLogger(Main.class);

    public static void main(String... argv) {
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
        var getCommand = new GetCommand();
        var postCommand = new PostCommand();
        var helpCommand = new HelpCommand();

        JCommander parser = JCommander.newBuilder()
                                    .addObject(main)
                                    .addCommand("GET", getCommand)
                                    .addCommand("POST", postCommand)
                                    .addCommand("HELP", helpCommand)
                                    .build();

        try {
            parser.parse(argv);
        } catch (ParameterException e) {
            logger.error(e.getMessage());
            System.exit(-1);
        }

        HttpRequest request = null;

        if (parser.getParsedCommand().equalsIgnoreCase("GET")) {
            request = new HttpRequest.Builder()
                              .setRequestLine(HttpRequest.Method.GET,
                                      getCommand.serverHost.getPath(), HttpVersion.HTTP_1_0)
                              .setHeaders(formatHeaders(getCommand.headers))
                              .setBody("")
                              .create();

            var routerAddr =
                    new InetSocketAddress(getCommand.routerHost.getHost(), getCommand.routerPort);
            var serverAddr =
                    new InetSocketAddress(getCommand.serverHost.getHost(), getCommand.serverPort);

            if (getCommand.verbose) {
                rootLogger.setLevel(Level.DEBUG);
            }

            var httpc = new Httpc(routerAddr, serverAddr);

            httpc.connectAndSend(request);

        } else if (parser.getParsedCommand().equalsIgnoreCase("POST")) {
            if (postCommand.inlideData != null && postCommand.filepath != null) {
                throw new ParameterException("Cannot use paramater -d and -f at the same time");
            }

            if (postCommand.inlideData != null) {
                request = new HttpRequest.Builder()
                                  .setRequestLine(HttpRequest.Method.POST,
                                          getCommand.serverHost.getPath(), HttpVersion.HTTP_1_0)
                                  .setHeaders(formatHeaders(getCommand.headers))
                                  .setBody(postCommand.inlideData)
                                  .create();
            }

            if (postCommand.filepath != null) {
                StringBuilder builder = new StringBuilder();
                try {
                    Files.lines(postCommand.filepath).forEach(x -> builder.append(x));
                } catch (IOException e) {
                    System.exit(-1);
                }

                request = new HttpRequest.Builder()
                                  .setRequestLine(HttpRequest.Method.POST,
                                          postCommand.url.getPath(), HttpVersion.HTTP_1_0)
                                  .setHeaders(formatHeaders(postCommand.headers))
                                  .setBody(builder.toString())
                                  .create();

                request.addHeader("Content-Length", String.valueOf(builder.length()));

                var routerAddr = new InetSocketAddress(
                        postCommand.routerHost.getHost(), postCommand.routerPort);
                var serverAddr =
                        new InetSocketAddress(postCommand.url.getHost(), postCommand.portNumber);

                if (postCommand.verbose) {
                    rootLogger.setLevel(Level.DEBUG);
                }

                var httpc = new Httpc(routerAddr, serverAddr);

                httpc.connectAndSend(request);
            }

        } else {
            System.out.println(helpCommand.toString());
        }
    }

    private static HashMap<String, String> formatHeaders(List<String> parsedHeaders) {
        HashMap<String, String> map = new HashMap<>();

        for (String string : parsedHeaders) {
            var keyValue = string.split(":", 2);

            map.put(keyValue[0], keyValue[1]);
        }

        return map;
    }
}
