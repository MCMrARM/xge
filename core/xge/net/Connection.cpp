#include "Connection.h"

#include <cstring>
#include <climits>

using namespace xge;

Connection::Connection(DatagramSocket &socket, sockaddr_in addr) : socket(socket), addr(addr) {
    memset(sendOrderIndex, 0, sizeof(sendOrderIndex));
}

void Connection::sendRaw(char *data, size_t len) {
    socket.send(addr, data, len);
}

void Connection::sendAndQueueReliableRaw(std::vector<char> data, unsigned short reliableId) {
    socket.send(addr, data.data(), data.size());
    sendReliablePackets.push({std::chrono::steady_clock::now(), std::move(data), reliableId});
}

void Connection::send(PacketId pkId, char *msg, size_t len, unsigned char channel, const ACKCallback &ack,
                      const ACKCallback &nack) {
    char header[16];
    size_t headerSize = 1;
    header[0] = *((char *) &pkId); // convert PacketId (an char-sized struct) to char
    unsigned short reliableId = 0;
    if (pkId.reliable) {
        reliableId = getNextReliableIndex();
        // reliable
        if (pkId.ordered) {
            // reliable ordered (char type, short reliableId, short orderId, (if channel != 0) char channel, data)
            headerSize = 5 + (channel != 0 ? 1 : 0);
            ((unsigned short &) header[1]) = reliableId;
            ((unsigned short &) header[3]) = getNextOrderIndex(channel);
            if (channel != 0)
                header[5] = channel;
        } else {
            // reliable unordered (char type, short reliableId, data)
            headerSize = 3;
            ((unsigned short &) header[1]) = reliableId;
        }
    } else {
        // unreliable (char type, (if ack) short ackId, data)
        // no extra data
    }
    if (len + headerSize > mtu) {
        // (char pkId, short fragmentedPkId, uint24 fragTotalSize, uint24 fragOff)
        size_t fragHeaderSize = 9;
        size_t fragTotalSize = headerSize - 1 + len;
        char fragHeader[6];
        pkId.fragmented = 1;
        fragHeader[0] = *((char *) &pkId);
        ((unsigned short &) header[1]) = getNextFragmentPacketIndex();
        memcpy(&header[3], &fragTotalSize, 3);

        size_t off = 0;
        while (off < fragTotalSize) {
            size_t size = std::min(mtu - fragHeaderSize, fragTotalSize - off);
            std::vector<char> pk (size + fragHeaderSize);
            memcpy(&pk[0], &fragHeader, 6);
            memcpy(&pk[6], &off, 3);
            if (off < headerSize) {
                memcpy(&pk[9], &header[off], headerSize - off);
                memcpy(&pk[9 + headerSize], &msg[off - headerSize], size - headerSize);
            } else {
                memcpy(&pk[9], &msg[off - headerSize], size);
            }

            if (pkId.reliable)
                sendAndQueueReliableRaw(std::move(pk), reliableId);
            else
                sendRaw(pk.data(), pk.size());

            off += size;
        }
    } else {
        std::vector<char> pk (len + headerSize);
        memcpy(&pk[0], header, headerSize);
        memcmp(&pk[headerSize], msg, len);
        if (pkId.reliable)
            sendAndQueueReliableRaw(std::move(pk), reliableId);
        else
            sendRaw(pk.data(), pk.size());
    }
}

unsigned short Connection::getNextReliableIndex() {
    int index = sendReliableIndex;
    if (index >= USHRT_MAX)
        sendReliableIndex = 0;
    else
        sendReliableIndex++;
    return (unsigned short) index;
}

unsigned short Connection::getNextFragmentPacketIndex() {
    int index = sendFragmentPkIndex;
    if (index >= USHRT_MAX)
        sendFragmentPkIndex = 0;
    else
        sendFragmentPkIndex++;
    return (unsigned short) index;
}

unsigned short Connection::getNextOrderIndex(unsigned char channel) {
    int index = sendOrderIndex[channel];
    if (index >= USHRT_MAX)
        sendOrderIndex[channel] = 0;
    else
        sendOrderIndex[channel]++;
    return (unsigned short) index;
}

int Connection::send(char *msg, size_t len, unsigned char channel, const ACKCallback &ackCallback) {
    PacketId pkId = { 0, 1, 1, (channel == 0 ? 1 : 0), (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, channel, ackCallback, nullptr);
}

int Connection::sendUnordered(char *msg, size_t len, const ACKCallback &ackCallback) {
    PacketId pkId = { 0, 1, 0, 0, (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ackCallback, nullptr);
}

void Connection::sendUnreliable(char *msg, size_t len, const ACKCallback &ack, const ACKCallback &nack) {
    PacketId pkId = { 0, 1, 0, 0, (ack != nullptr || nack != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ack, nack);
}