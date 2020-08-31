package ca.concordia.comp_445.httpfs.events;

import java.net.InetSocketAddress;

import ca.concordia.comp_445.utils.Event;

public class CloseClientEvent extends Event {
    private InetSocketAddress clientAddr;

    public CloseClientEvent(InetSocketAddress clientAddr) {
        this.clientAddr = clientAddr;
    }

    public InetSocketAddress getClientAddress() {
        return this.clientAddr;
    }
}
