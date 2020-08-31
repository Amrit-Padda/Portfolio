package ca.concordia.comp_445.commons.net;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Packet {
    public static final int MIN_LEN = 11;
    public static final int MAX_LEN = 11 + 1013;

    public enum Type {
        DATA(0),
        ACK(1),
        SYN(2),
        SYN_ACK(3);

        private final int value;

        Type(int value) {
            this.value = (byte) value & 0xFF;
        }

        public byte getValue() {
            return (byte) value;
        }
    }

    private Type type;
    private int sequenceNumber;
    private int acknowledgmentNumber;
    private InetAddress peerAddress;
    private int peerPort;
    private byte[] payload;

    public Packet(Type type, int sequenceNumber, int acknowledgmentNumber, InetAddress peerAddress,
            int peerPort, byte[] payload) {
        this.type = type;
        this.sequenceNumber = sequenceNumber;
        this.acknowledgmentNumber = acknowledgmentNumber;
        this.peerAddress = peerAddress;
        this.peerPort = peerPort;
        this.payload = payload;
    }

    /**
     * Get the {@link Packet} data into a {@link ByteBuffer}.
     *
     * @return the {@link Packet} represented as a {@link ByteBuffer}
     */
    public ByteBuffer toByteBuffer() {
        ByteBuffer buffer = ByteBuffer.allocate(MAX_LEN).order(ByteOrder.BIG_ENDIAN);
        buffer.put(type.getValue());
        buffer.putShort((short) sequenceNumber);
        buffer.putShort((short) acknowledgmentNumber);
        buffer.put(peerAddress.getAddress());
        buffer.putShort((short) peerPort);
        buffer.put(payload);
        buffer.flip();

        return buffer;
    }

    /**
     * Create a {@link Builder} object from the data of the current instance of {@link Packet}.
     *
     * @return A {@link Builder} object made from the current {@link Packet} object.
     */
    public Builder toBuilder() {
        return new Builder()
                .setType(type)
                .setSequenceNumber(sequenceNumber)
                .setAcknowledgmentNumber(acknowledgmentNumber)
                .setPeerAddress(peerAddress)
                .setPeerPort(peerPort)
                .setPayload(payload);
    }

    public Type getType() {
        return this.type;
    }

    public int getSequenceNumber() {
        return this.sequenceNumber;
    }

    public int getAcknowledgmentNumber() {
        return this.acknowledgmentNumber;
    }

    public InetAddress getPeerAddress() {
        return this.peerAddress;
    }

    public int getPeerPort() {
        return this.peerPort;
    }

    public byte[] getPayload() {
        return this.payload;
    }

    /**
     * Create a {@link Packet} object from a {@link ByteBuffer}.
     *
     * @param buf The {@link ByteBuffer} to convert into a packet.
     * @return The {@link Packet} object created from the {@link ByteBuffer}
     * @throws IOException If an error accured reading the {@link ByteBuffer}
     */
    public static Packet fromBuffer(ByteBuffer buf) throws IOException {
        if (buf.limit() < MIN_LEN || buf.limit() > MAX_LEN) {
            throw new IOException("Invalid length");
        }

        Builder builder = new Builder();

        builder.setType(Type.values()[Byte.toUnsignedInt(buf.get())]);
        builder.setSequenceNumber(Short.toUnsignedInt(buf.getShort()));
        builder.setAcknowledgmentNumber(Short.toUnsignedInt(buf.getShort()));

        byte[] host = new byte[] {buf.get(), buf.get(), buf.get(), buf.get()};
        builder.setPeerAddress(Inet4Address.getByAddress(host));
        builder.setPeerPort(Short.toUnsignedInt(buf.getShort()));

        byte[] payload = new byte[buf.remaining()];
        buf.get(payload);
        builder.setPayload(payload);

        return builder.create();
    }

    public static Packet fromBytes(byte[] bytes) throws IOException {
        ByteBuffer buf = ByteBuffer.allocate(MAX_LEN).order(ByteOrder.BIG_ENDIAN);
        buf.put(bytes);
        buf.flip();
        return fromBuffer(buf);
    }

    public static class Builder {
        private Type type;
        private int sequenceNumber;
        private int acknowledgmentNumber;
        private InetAddress peerAddress;
        private int peerPort;
        private byte[] payload;

        public Builder setType(Type type) {
            this.type = type;
            return this;
        }

        public Builder setSequenceNumber(int sequenceNumber) {
            this.sequenceNumber = sequenceNumber;
            return this;
        }

        public Builder setAcknowledgmentNumber(int acknowledgmentNumber) {
            this.acknowledgmentNumber = acknowledgmentNumber;
            return this;
        }

        public Builder setPeerAddress(InetAddress peerAddress) {
            this.peerAddress = peerAddress;
            return this;
        }

        public Builder setPeerPort(int peerPort) {
            this.peerPort = peerPort;
            return this;
        }

        public Builder setPayload(byte[] payload) {
            this.payload = payload;
            return this;
        }

        public Packet create() {
            return new Packet(
                    type, sequenceNumber, acknowledgmentNumber, peerAddress, peerPort, payload);
        }
    }
}
