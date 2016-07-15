#include "Connection.h"

#include <cstring>
#include <climits>
#include <arpa/inet.h>
#include "ConnectionHandler.h"

using namespace xge;

Connection::Connection(ConnectionHandler &handler, const NetAddress &addr) : handler(handler), addr(addr.addr),
                                                                             reliableResendTime(5000),
                                                                             reliablePacketIdReuseTime(30000) {
    memset(sendOrderIndex, 0, sizeof(sendOrderIndex));
    memset(receivedOrderedPacketIndex, 0, sizeof(receivedOrderedPacketIndex));
}

void Connection::sendRaw(char *data, size_t len) {
    handler.getSocket().send(addr, data, len, false);
}

void Connection::sendAndQueueReliableRaw(std::vector<char> data, unsigned short reliableId) {
    using namespace std::chrono;
    handler.getSocket().send(addr, data.data(), data.size(), false);
    steady_clock::time_point tp = steady_clock::now() + reliableResendTime;
    sendReliablePackets.push({tp, std::move(data), reliableId});
}

void Connection::send(PacketId pkId, char *msg, size_t len, unsigned char channel, const ACKCallback &ack,
                      const ACKCallback &nack) {
    char header[16];
    size_t headerSize = 1;
    header[0] = *((char *) &pkId); // convert PacketId (an char-sized struct) to char
    unsigned short reliableId = 0;
    if (pkId.reliable) {
        reliableId = getNextReliableIndex();
        ((unsigned short &) header[1]) = reliableId;
        // reliable
        if (pkId.ordered) {
            // reliable ordered (char type, short reliableId, short orderId, (if channel != 0) char channel, data)
            headerSize = 5 + (channel != 0 ? 1 : 0);
            ((unsigned short &) header[3]) = getNextOrderIndex(channel);
            if (channel != 0)
                header[5] = channel;
        } else {
            // reliable unordered (char type, short reliableId, data)
            headerSize = 3;
        }
    } else {
        // unreliable (char type, (if ack) short ackId, data)
        // no extra data
    }
    if (len + headerSize > mtu) {
        // (char pkId, short fragmentedPkId, uint24 fragTotalSize, short pkSize, short pkIndex)
        size_t fragHeaderSize = 10;
        size_t fragTotalSize = headerSize + len;
        char fragHeader[8];
        pkId.fragmented = 1;
        fragHeader[0] = *((char *) &pkId);
        ((unsigned short &) fragHeader[1]) = getNextFragmentPacketIndex();
        memcpy(&fragHeader[3], &fragTotalSize, 3);
        ((unsigned short &) fragHeader[6]) = (unsigned short) (mtu - fragHeaderSize);

        size_t off = 0;
        while (off < fragTotalSize) {
            size_t size = std::min(mtu - fragHeaderSize, fragTotalSize - off);
            std::vector<char> pk (size + fragHeaderSize);
            memcpy(&pk[0], &fragHeader, 8);
            ((unsigned short &) pk[8]) = (unsigned short) (mtu - fragHeaderSize);
            if (off < headerSize) {
                memcpy(&pk[fragHeaderSize], &header[off], headerSize - off);
                memcpy(&pk[fragHeaderSize + headerSize], &msg[off - headerSize], size - headerSize);
            } else {
                memcpy(&pk[fragHeaderSize], &msg[off - headerSize], size);
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
        memcpy(&pk[headerSize], msg, len);
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

void Connection::handlePacket(char *msg, size_t len) {
    using namespace std::chrono;

    PacketId pkId = *((PacketId *) msg);
    if (pkId.fragmented) {
        if (len < 10)
            return;
        unsigned short fragId = ((unsigned short &) msg[1]);
        unsigned int fragTotalSize = 0;
        memcpy(&fragTotalSize, &msg[3], 3);
        unsigned short fragPkSize = ((unsigned short &) msg[6]);
        unsigned short fragPkIndex = ((unsigned short &) msg[8]);
        FragmentedPacketData &pk = fragmentedPackets[fragId];
        pk.setSize(fragTotalSize, (fragTotalSize + fragPkSize - 1) / fragPkSize);
        size_t off = fragPkIndex * fragPkSize;
        size_t size = std::min(len - 10, (size_t) fragPkSize);
        if (off + size >= fragTotalSize)
            return;
        memcpy(&pk.data[off], msg, size);
        if (!pk.received[fragPkIndex]) {
            pk.received[fragPkIndex] = true;
            if (--pk.neededFragments == 0) {
                // packet complete
                pkId = *((PacketId *) &pk.data[0]);
                if (pkId.fragmented)
                    return; // fragmented packet in a fragmented packet?!
                handlePacket(&pk.data[0], pk.data.size());
                fragmentedPackets.erase(fragId);
            }
        }
    } else {
        size_t headerSize = 1;
        if (pkId.reliable) {
            unsigned short reliableId = (unsigned short &) msg[1];
            if (receivedReliablePacketIndexes.count(reliableId) > 0)
                return; // it was received already, ignore
            // TODO: Send ACK
            receivedReliablePacketIndexes.insert(reliableId);
            steady_clock::time_point tp = steady_clock::now() + reliablePacketIdReuseTime;
            receivedReliablePacketIndexesRemoveQueue.push({tp, reliableId});
            if (pkId.ordered) {
                headerSize = 5 + (pkId.channelZero ? 0 : 1);
                unsigned short orderId = (unsigned short &) msg[3];
                unsigned char channel = 0;
                if (pkId.system)
                    return; // ordered packets can't be system
                if (!pkId.channelZero)
                    channel = (unsigned char) msg[5];
                auto &channelOrderedPacketList = receivedOrderedPackets[channel];
                if (orderId != receivedOrderedPacketIndex[channel]) {
                    std::vector<char> data(len - headerSize);
                    memcpy(data.data(), &msg[headerSize], len - headerSize);
                    channelOrderedPacketList[orderId] = std::move(data);
                    if (receivedOrderedPacketIndex[channel] >= USHRT_MAX)
                        receivedOrderedPacketIndex[channel] = 0;
                    else
                        receivedOrderedPacketIndex[channel]++;
                    return;
                }
                // checks for packets that have arrived later
                for (auto it = channelOrderedPacketList.find(orderId); it != channelOrderedPacketList.end(); ) {
                    if (it->first == receivedOrderedPacketIndex[channel]) {
                        handler.addUserPacket(*this, std::move(it->second));

                        it = channelOrderedPacketList.erase(it);
                        if (receivedOrderedPacketIndex[channel] >= USHRT_MAX) {
                            receivedOrderedPacketIndex[channel] = 0;
                            it = channelOrderedPacketList.begin();
                        } else {
                            receivedOrderedPacketIndex[channel]++;
                        }
                    } else {
                        it++;
                    }
                }
            } else {
                headerSize = 3;
            }
        } else if (pkId.ack) {
            // TODO: Send ACK
        }
        if (pkId.system) {
            // TODO: handle system msg
        } else {
            handler.addUserPacket(*this, &msg[headerSize], len - headerSize);
        }
    }
}

int Connection::send(char *msg, size_t len, unsigned char channel, const ACKCallback &ackCallback) {
    PacketId pkId = { 0, 0, 1, 1, (channel == 0 ? 1 : 0), (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, channel, ackCallback, nullptr);
}

int Connection::sendUnordered(char *msg, size_t len, const ACKCallback &ackCallback) {
    PacketId pkId = { 0, 0, 1, 0, 0, (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ackCallback, nullptr);
}

void Connection::sendUnreliable(char *msg, size_t len, const ACKCallback &ack, const ACKCallback &nack) {
    PacketId pkId = { 0, 0, 1, 0, 0, (ack != nullptr || nack != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ack, nack);
}

void Connection::resendPackets() {
    auto t = std::chrono::steady_clock::now();
    while (!sendReliablePackets.empty()) {
        if (sendReliablePackets.top().time >= t) {
            // resend
            unsigned short i = sendReliablePackets.top().index;
            std::vector<char> d = std::move(sendReliablePackets.top().data);
            sendReliablePackets.pop();
            sendAndQueueReliableRaw(std::move(d), i);
        }
    }
}

void Connection::removeQueuedReceivedReliablePacketIndexes() {
    auto t = std::chrono::steady_clock::now();
    while (!receivedReliablePacketIndexesRemoveQueue.empty()) {
        if (receivedReliablePacketIndexesRemoveQueue.top().time >= t) {
            receivedReliablePacketIndexes.erase(receivedReliablePacketIndexesRemoveQueue.top().index);
            receivedReliablePacketIndexesRemoveQueue.pop();
        }
    }
}