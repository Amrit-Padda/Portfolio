package ca.concordia.comp_445.httpc;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import ca.concordia.comp_445.commons.http.HttpRequest;
import ca.concordia.comp_445.commons.http.HttpResponse;
import ca.concordia.comp_445.commons.net.Packet;

public class Httpc {
    private static final int WINDOW_SIZE = 5;
    private static final int TIME_TO_WAIT = 5000;

    private Logger logger = LoggerFactory.getLogger(Httpc.class);

    private InetSocketAddress routerAddress;
    private InetSocketAddress serverAddress;

    private Packet.Builder packetBuilder;

    private boolean isRequestHandled = false;
    private boolean isHandshakeCompleted = false;
    private boolean isSynPacketHandled = false;
    private boolean isResponseReceived = false;
    private boolean isHeaderParsed = false;

    private Map<Integer, Long> packetTimer;
    private Set<Integer> ackedPackets;

    private int packetNumber;
    private int lastAcked;

    private StringBuilder response;
    private StringBuilder body;
    private int bodyLength;

    public Httpc(InetSocketAddress routerAddress, InetSocketAddress serverAddress) {
        this.routerAddress = routerAddress;
        this.serverAddress = serverAddress;
        this.packetTimer = new HashMap<>();
        this.ackedPackets = new HashSet<>();
        this.response = new StringBuilder();
        this.body = new StringBuilder();
        this.lastAcked = 0;
        this.packetNumber = 0;
        this.bodyLength = 0;
    }

    public void connectAndSend(HttpRequest request) {
        logger.info("Starting Connection...");

        logger.debug("Router Host Address: {}", routerAddress.getAddress());
        logger.debug("Router Port Number: {}", routerAddress.getPort());

        logger.debug("Server Host Address: {}", serverAddress.getAddress());
        logger.debug("Server Port Number: {}", serverAddress.getPort());

        try (Selector selector = Selector.open()) {
            DatagramChannel channel = DatagramChannel.open();
            channel.configureBlocking(false);
            channel.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE);

            logger.info("Client ready");

            ByteBuffer buffer = ByteBuffer.allocate(Packet.MAX_LEN).order(ByteOrder.BIG_ENDIAN);

            Packet syn = new Packet.Builder()
                                 .setType(Packet.Type.SYN)
                                 .setSequenceNumber(this.packetNumber++)
                                 .setAcknowledgmentNumber(0)
                                 .setPeerAddress(serverAddress.getAddress())
                                 .setPeerPort(serverAddress.getPort())
                                 .setPayload(new byte[0])
                                 .create();

            this.packetBuilder = syn.toBuilder();

            while (!isHandshakeCompleted) {
                if (!packetTimer.containsKey(syn.getSequenceNumber())) {
                    channel.send(syn.toByteBuffer(), routerAddress);

                    packetTimer.put(syn.getSequenceNumber(), System.currentTimeMillis());

                    logger.debug("Sending {} packet {} to router at {}", Packet.Type.SYN.toString(),
                            syn.getSequenceNumber(), routerAddress);
                } else {
                    buffer.clear();
                    channel.receive(buffer);
                    buffer.flip();

                    if (buffer.remaining() > 0) {
                        var packet = Packet.fromBuffer(buffer);

                        if (packet.getType() == Packet.Type.SYN_ACK) {
                            this.packetBuilder.setAcknowledgmentNumber(packet.getSequenceNumber());

                            logger.debug("Received {} packet {} to router at {}",
                                    packet.getType().toString(), packet.getAcknowledgmentNumber(),
                                    routerAddress);

                            isHandshakeCompleted = true;
                        }
                    } else {
                        long dt = System.currentTimeMillis()
                                - packetTimer.get(syn.getSequenceNumber());

                        if (dt >= TIME_TO_WAIT) {
                            packetTimer.remove(syn.getSequenceNumber());
                        }
                    }
                }
            }

            var packets = prepPackets(request.toByteBuffer());

            int packetIndex = 0;
            while (packetIndex < packets.size()) {
                var current = packets.get(packetIndex);

                if (!packetTimer.containsKey(current.getSequenceNumber())) {
                    channel.send(current.toByteBuffer(), routerAddress);

                    packetTimer.put(current.getSequenceNumber(), System.currentTimeMillis());

                    logger.debug("Sending {} packet {} to router at {}",
                            current.getType().toString(), current.getSequenceNumber(),
                            routerAddress);
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
                                    routerAddress);

                            packetTimer.remove(syn.getSequenceNumber());

                            packetIndex++;
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

            int last = 0;
            boolean isReceived = false;
            while (!isReceived) {
                buffer.clear();
                channel.receive(buffer);
                buffer.flip();

                if (buffer.remaining() > 0) {
                    var packet = Packet.fromBuffer(buffer);

                    String data = new String(packet.getPayload());

                    logger.info(data);

                    this.packetBuilder = packet.toBuilder().setSequenceNumber(
                            packet.getAcknowledgmentNumber() + 1);

                    isReceived = true;
                }
            }

            var finalP = this.packetBuilder.create();
            channel.send(finalP.toByteBuffer(), routerAddress);

        } catch (IOException e) {
            logger.error("{}", e.getMessage());
            System.exit(-1);
        }
    }

    private void listenAndAck(DatagramChannel channel, Packet packet) throws IOException {
        if (lastAcked != packet.getSequenceNumber()) {
            var payload = new String(packet.getPayload());

            response.append(payload);

            if (!isHeaderParsed) {
                String[] data = payload.lines().toArray(size -> new String[size]);

                int endHeaders = 0;
                for (int i = 1; i < data.length; ++i) {
                    var keyValuePair = data[i].split(":", 2);

                    if (keyValuePair.length == 2) {
                        if (keyValuePair[0].equalsIgnoreCase("content-length")) {
                            this.bodyLength = Integer.parseUnsignedInt(keyValuePair[1]);
                        }
                    } else {
                        endHeaders = i;
                        break;
                    }
                }

                for (int i = endHeaders; i < data.length; ++i) {
                    this.body.append(data[i]);
                }

                this.isHeaderParsed = true;
            } else {
                this.body.append(payload);
            }

            if (body.length() == this.bodyLength) {
                this.isResponseReceived = true;

                logger.info(response.toString());
            }
        }

        logger.debug("Received {} packet {} to router at {}", packet.getType().toString(),
                packet.getAcknowledgmentNumber(), routerAddress);

        Packet ack = new Packet.Builder()
                             .setType(Packet.Type.ACK)
                             .setSequenceNumber(packet.getAcknowledgmentNumber() + 1)
                             .setAcknowledgmentNumber(packet.getSequenceNumber())
                             .setPeerAddress(packet.getPeerAddress())
                             .setPeerPort(packet.getPeerPort())
                             .setPayload(new byte[0])
                             .create();

        channel.send(ack.toByteBuffer(), routerAddress);

        logger.debug("Sending {} packet {} to router at {}", Packet.Type.ACK.toString(),
                packet.getSequenceNumber(), routerAddress);

        lastAcked = packet.getSequenceNumber();
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
                                .setSequenceNumber(this.packetNumber++)
                                .setPayload(payload)
                                .create());
        }

        return packets;
    }
}
