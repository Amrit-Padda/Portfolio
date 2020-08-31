package ca.concordia.comp_445.httpfs;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.DatagramChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import ca.concordia.comp_445.commons.http.HttpError;
import ca.concordia.comp_445.commons.http.HttpRequest;
import ca.concordia.comp_445.commons.http.HttpResponse;
import ca.concordia.comp_445.commons.http.HttpStatusCode;
import ca.concordia.comp_445.commons.http.HttpVersion;
import ca.concordia.comp_445.commons.net.Packet;
import ca.concordia.comp_445.httpfs.events.CloseClientEvent;
import ca.concordia.comp_445.utils.*;

public class Client extends Subject {
    private static final long TIME_TO_WAIT = 5000;

    private Logger logger = LoggerFactory.getLogger(Client.class);

    private ServerUDP server;
    private InetSocketAddress address;

    private HttpRequest request;
    private HttpResponse response;

    private StringBuilder bodyBuilder;

    private Packet.Builder packetBuilder;

    private Map<Integer, Long> packetTimer;
    private Set<Integer> ackedPackets;
    private List<Packet> packets;

    private int sequenceNumber = 0;
    private int lastAcked = 0;
    private int lastSent = 0;
    private int bodyLength = 0;
    private boolean wasSynAcked = false;
    private boolean handshake = false;
    private boolean parsedRequestLine = false;
    private boolean isRequestParsed = false;

    public Client(ServerUDP server, InetSocketAddress address) {
        this.packetTimer = new HashMap<>();
        this.ackedPackets = new HashSet<>();
        this.server = server;
        this.address = address;
        this.bodyBuilder = new StringBuilder();
    }

    public void listen(DatagramChannel channel, Packet message, SocketAddress router) {
        if (!handshake) {
            if (!wasSynAcked) {
                if (message.getType() == Packet.Type.SYN) {
                    logger.debug("Client [{}]: Starting handshake", address.toString());

                    logger.debug("Client [{}]: {} packet #{} received", address.toString(),
                            Packet.Type.SYN.toString(), message.getSequenceNumber());
                } else {
                    logger.debug("Client [{}]: Was expecting SYN packet, closing client...",
                            address.toString());
                    notify(new CloseClientEvent(address));

                    return;
                }
            }

            if (message.getType() == Packet.Type.SYN) {
                Packet synack = new Packet.Builder()
                                        .setType(Packet.Type.SYN_ACK)
                                        .setSequenceNumber(sequenceNumber++)
                                        .setAcknowledgmentNumber(message.getSequenceNumber())
                                        .setPeerAddress(message.getPeerAddress())
                                        .setPeerPort(message.getPeerPort())
                                        .setPayload(new byte[0])
                                        .create();

                packetBuilder = synack.toBuilder();

                try {
                    channel.send(synack.toByteBuffer(), router);

                    logger.debug("Client [{}]: Sending {} packet #{} to client", address.toString(),
                            Packet.Type.SYN_ACK.toString(), sequenceNumber - 1);

                    wasSynAcked = true;
                } catch (IOException e) {
                    logger.error("Failed to send SYN_ACK packet: {}", e.getMessage());
                    notify(new CloseClientEvent(address));

                    return;
                }
            }

            if (message.getType() == Packet.Type.ACK && wasSynAcked) {
                handshake = true;

                logger.debug("Client [{}]: {} packet #{} received", address.toString(),
                        Packet.Type.ACK.toString(), message.getSequenceNumber());

                logger.debug("Client [{}]: Handshake completed.", address.toString());

                if (message.getSequenceNumber() != lastAcked) {
                    var payload = new String(message.getPayload());

                    String[] data = payload.lines().toArray(size -> new String[size]);

                    if (!parsedRequestLine) {
                        var parseResult = this.parseRequestLine(data[0]);

                        if (parseResult.second == HttpError.NO_ERROR) {
                            request = parseResult.first;
                            parsedRequestLine = true;
                        } else if (parseResult.second == HttpError.INVALID_LINE_ELEMENT_COUNT) {
                            response = new HttpResponse.Builder()
                                               .setResponseLine(HttpVersion.HTTP_1_0,
                                                       HttpStatusCode.BAD_REQUEST)
                                               .setHeaders(new HashMap<>())
                                               .setBody("")
                                               .create();
                        }
                    }
                    if (parsedRequestLine) {
                        int endHeaders = 0;
                        for (int i = 1; i < data.length; ++i) {
                            var keyValuePair = data[i].split(":", 2);

                            if (keyValuePair.length == 2) {
                                if (keyValuePair[0].equalsIgnoreCase("content-length")) {
                                    this.bodyLength = Integer.parseUnsignedInt(keyValuePair[1]);
                                }

                                request.addHeader(keyValuePair[0], keyValuePair[1]);
                            } else {
                                endHeaders = i;
                                break;
                            }
                        }

                        if (request.getRequestLine().getMethod() == HttpRequest.Method.GET) {
                            logger.debug("Client [{}]: Http request parsed", address.toString());

                            isRequestParsed = true;

                            logger.debug("Client [{}]: Preparing Response", address.toString());

                            if (response != null) {
                                this.packets = prepPackets(response.toByteBuffer());
                            } else {
                                this.response = server.serverResponseToGetRequest(request);

                                this.packets = prepPackets(this.response.toByteBuffer());
                            }
                        } else {
                            // check if post
                            for (int i = endHeaders; i < data.length; ++i) {
                                bodyBuilder.append(data[i]);
                            }

                            if (bodyBuilder.length() == bodyLength) {
                                this.request.setBody(this.bodyBuilder.toString());

                                logger.debug(
                                        "Client [{}]: Http request parsed", address.toString());

                                isRequestParsed = true;

                                logger.debug("Client [{}]: Preparing Response", address.toString());

                                if (response != null) {
                                    this.packets = prepPackets(response.toByteBuffer());
                                } else {
                                    this.response = server.serverResponseToPostRequest(request);

                                    this.packets = prepPackets(this.response.toByteBuffer());
                                }
                            }
                        }
                    }
                }

                if (!isRequestParsed) {
                    Packet ack = new Packet.Builder()
                                         .setType(Packet.Type.ACK)
                                         .setSequenceNumber(sequenceNumber++)
                                         .setAcknowledgmentNumber(message.getSequenceNumber())
                                         .setPeerAddress(message.getPeerAddress())
                                         .setPeerPort(message.getPeerPort())
                                         .setPayload(new byte[0])
                                         .create();

                    this.packetBuilder = ack.toBuilder();

                    try {
                        channel.send(ack.toByteBuffer(), router);

                        this.lastAcked = message.getSequenceNumber();
                    } catch (IOException e) {
                    }

                    this.logger.debug("Client [{}]: Sending {} packet #{} to client",
                            address.toString(), Packet.Type.ACK.toString(),
                            ack.getSequenceNumber());
                }
            }
        } else {
            if (message.getType() == Packet.Type.ACK && !isRequestParsed) {
                logger.debug("Client [{}]: {} packet #{} received", address.toString(),
                        Packet.Type.ACK.toString(), message.getSequenceNumber());

                if (message.getSequenceNumber() != lastAcked) {
                    StringBuilder builder = new StringBuilder();
                    builder.append(new String(message.getPayload()));

                    String[] data = builder.toString().lines().toArray(size -> new String[size]);

                    for (int i = 0; i < data.length; ++i) {
                        this.bodyBuilder.append(data[i]);
                    }

                    if (bodyBuilder.length() == bodyLength) {
                        this.request.setBody(this.bodyBuilder.toString());

                        logger.debug("Client [{}]: Http request parsed", address.toString());

                        isRequestParsed = true;

                        logger.debug("Client [{}]: Preparing Response", address.toString());

                        if (response != null) {
                            this.packets = prepPackets(response.toByteBuffer());
                        } else {
                            if (request.getRequestLine().getMethod() == HttpRequest.Method.GET) {
                                this.response = server.serverResponseToGetRequest(request);
                                this.response.getBody().toString();
                                this.response.getHeaders().toString();

                                this.packets = prepPackets(this.response.toByteBuffer());
                            } else {
                                this.response = server.serverResponseToPostRequest(request);
                                this.response.getBody().toString();
                                this.response.getHeaders().toString();

                                this.packets = prepPackets(this.response.toByteBuffer());
                            }
                        }
                    }
                }

                if (!isRequestParsed) {
                    Packet ack = new Packet.Builder()
                                         .setType(Packet.Type.ACK)
                                         .setSequenceNumber(sequenceNumber++)
                                         .setAcknowledgmentNumber(message.getSequenceNumber())
                                         .setPeerAddress(message.getPeerAddress())
                                         .setPeerPort(message.getPeerPort())
                                         .setPayload(new byte[0])
                                         .create();

                    this.packetBuilder = ack.toBuilder();

                    try {
                        channel.send(ack.toByteBuffer(), router);
                    } catch (IOException e) {
                    }

                    logger.debug("Client [{}]: Sending {} packet #{} to client", address.toString(),
                            Packet.Type.ACK.toString(), ack.getSequenceNumber());
                }
            }
        }
    }

    public void respond(DatagramChannel channel) throws IOException {
        if (isRequestParsed && this.lastSent < packets.size()) {
            ByteBuffer buffer = ByteBuffer.allocate(Packet.MAX_LEN).order(ByteOrder.BIG_ENDIAN);
            var current = this.packets.get(this.lastSent);

            if (!packetTimer.containsKey(current.getSequenceNumber())) {
                channel.send(current.toByteBuffer(), address);

                packetTimer.put(current.getSequenceNumber(), System.currentTimeMillis());

                logger.debug("Sending {} packet {} to router at {}", current.getType().toString(),
                        current.getSequenceNumber(), address);
            } else {
                buffer.clear();
                channel.receive(buffer);
                buffer.flip();

                if (buffer.remaining() > 0) {
                    var packet = Packet.fromBuffer(buffer);

                    if (packet.getType() == Packet.Type.ACK) {
                        this.packetBuilder.setAcknowledgmentNumber(packet.getSequenceNumber());

                        logger.debug("Received {} packet {} to router at {}",
                                packet.getType().toString(), packet.getAcknowledgmentNumber(),
                                address);

                        packetTimer.remove(current.getSequenceNumber());

                        this.lastSent++;
                    }
                } else {
                    long dt = System.currentTimeMillis()
                            - packetTimer.get(current.getSequenceNumber());

                    if (dt >= TIME_TO_WAIT) {
                        packetTimer.remove(current.getSequenceNumber());
                    }
                }
            }
        }
    }

    private Pair<HttpRequest, HttpError> parseRequestLine(String data) {
        var elems = data.split(" ");

        if (elems.length == 3) {
            var method = parseMethod(elems[0]);
            if (method.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_METHOD);
            }

            var uri = parseURI(elems[1]);
            if (uri.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_URI);
            }

            var version = parseVersion(elems[2]);
            if (version.second != HttpError.NO_ERROR) {
                return Pair.make(null, HttpError.INVALID_HTTP_VERSION);
            }

            HttpRequest request =
                    new HttpRequest.Builder()
                            .setRequestLine(method.first, uri.first.toString(), version.first)
                            .setHeaders(new HashMap<>())
                            .setBody("")
                            .create();

            return Pair.make(request, HttpError.NO_ERROR);

        } else {
            return Pair.make(null, HttpError.INVALID_LINE_ELEMENT_COUNT);
        }
    }

    private Pair<HttpRequest.Method, HttpError> parseMethod(String method) {
        if (method.compareToIgnoreCase(HttpRequest.Method.GET.toString()) == 0) {
            return Pair.make(HttpRequest.Method.GET, HttpError.NO_ERROR);
        } else if (method.compareToIgnoreCase(HttpRequest.Method.POST.toString()) == 0) {
            return Pair.make(HttpRequest.Method.POST, HttpError.NO_ERROR);
        } else {
            return Pair.make(HttpRequest.Method.GET, HttpError.INVALID_METHOD);
        }
    }

    private Pair<URI, HttpError> parseURI(String version) {
        try {
            return Pair.make(new URI(version), HttpError.NO_ERROR);
        } catch (URISyntaxException e) {
            return Pair.make(null, HttpError.INVALID_URI);
        }
    }

    private Pair<HttpVersion, HttpError> parseVersion(String version) {
        if (version.equalsIgnoreCase(HttpVersion.HTTP_1_0.valueOf())) {
            return Pair.make(HttpVersion.HTTP_1_0, HttpError.NO_ERROR);
        } else if (version.equalsIgnoreCase(HttpVersion.HTTP_1_1.valueOf())) {
            return Pair.make(HttpVersion.HTTP_1_1, HttpError.NO_ERROR);
        } else {
            return Pair.make(HttpVersion.HTTP_1_0, HttpError.INVALID_HTTP_VERSION);
        }
    }

    private List<Packet> prepPackets(ByteBuffer data) {
        var packets = new ArrayList<Packet>();

        int packetCount = (int) Math.ceil(data.remaining() / 1013.0f);

        for (int i = 0; i < packetCount; ++i) {
            byte[] payload;

            if (i == packetCount - 1) {
                payload = new byte[data.remaining()];
                data = data.get(payload, 0, data.remaining());
            } else {
                payload = new byte[1013];
                data = data.get(payload, 0, 1013);
            }

            packets.add(this.packetBuilder.setType(Packet.Type.ACK)
                                .setSequenceNumber(this.sequenceNumber++)
                                .setPayload(payload)
                                .create());
        }

        return packets;
    }
}
