#include "Connection.h"

#include <cstring>
#include <climits>
#ifndef _WINDOWS
#include <arpa/inet.h>
#endif
#include <xge/util/Log.h>
#include <xge/util/DynamicStackArray.h>
#include "ConnectionHandler.h"

using namespace xge;

char Connection::magicValue[8] = { 'x', 'g', 'e', '.', 'C', 'o', 'n', 'n' };
int Connection::mtuValues[] = { 1500, 1492, 1400, 576 };

Connection::Connection(ConnectionHandler &handler, const NetAddress &addr, bool isServerConnection) :
        handler(handler), addr(addr.addr), isServerConnection(isServerConnection), reliableResendTime(3000),
        reliablePacketIdReuseTime(30000), mtuAttemptDelayTime(300), ackSendTime(100), hasAcceptedHandshake(false),
        hasNegotiatedMTU(false) {
    memset(sendOrderIndex, 0, sizeof(sendOrderIndex));
    memset(receivedOrderedPacketIndex, 0, sizeof(receivedOrderedPacketIndex));

    lastReliableAckSend = std::chrono::steady_clock::now();
    lastUnreliableAckSend = std::chrono::steady_clock::now();

    if (!isServerConnection)
        sendHandshakeStart();
}

void Connection::sendRaw(char *data, size_t len) {
    handler.getSocket().send(addr, data, len, false);
}

void Connection::sendAndQueueReliableRaw(std::vector<char> data, unsigned short reliableId) {
    using namespace std::chrono;
    handler.getSocket().send(addr, data.data(), data.size(), false);

    std::lock_guard<std::recursive_mutex> lock (sendReliablePacketsMutex);
    steady_clock::time_point tp = steady_clock::now() + reliableResendTime;
    sendReliablePackets.push({tp, std::move(data), reliableId});
    sendReliablePacketsIndexesToReceive.insert(reliableId);
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
        if (pkId.ack) {
            headerSize = 3;
            ((unsigned short &) header[1]) = 0; // TODO: get actual id
        }
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
            size_t size = std::min<size_t>(mtu - fragHeaderSize, fragTotalSize - off);
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
        size_t size = std::min<size_t>(len - 10, (size_t) fragPkSize);
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
            ackMutex.lock();
            ackReliableIds.insert(reliableId);
            ackMutex.unlock();
            receivedReliablePacketIndexes.insert(reliableId);
            steady_clock::time_point tp = steady_clock::now() + reliablePacketIdReuseTime;
            receivedReliablePacketRemoveQueueMutex.lock();
            receivedReliablePacketIndexesRemoveQueue.push({tp, reliableId});
            receivedReliablePacketRemoveQueueMutex.unlock();
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
                    return;
                }
                if (receivedOrderedPacketIndex[channel] >= USHRT_MAX)
                    receivedOrderedPacketIndex[channel] = 0;
                else
                    receivedOrderedPacketIndex[channel]++;
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
            unsigned short ackIndex = (unsigned short &) msg[1];
            headerSize = 3;
            ackMutex.lock();
            ackUnreliableIds.insert(ackIndex);
            ackMutex.unlock();
        }
        if (pkId.system) {
            handleSystemMessage(&msg[headerSize], len - headerSize);
        } else {
            handler.addUserPacket(*this, &msg[headerSize], len - headerSize);
        }
    }
}

void Connection::handleSystemMessage(char *msg, size_t len) {
    if (len < 1)
        return;
    if (msg[0] == 0x01) { // handshake start
        if (len < 9 || !isServerConnection)
            return;
        if (memcmp(&msg[1], magicValue, 8) == 0) {
            hasAcceptedHandshake = true;
            sendHandshakeAccepted();
        }
    } else if (msg[0] == 0x02) { // handshake accepted
        if (len < 9 || isServerConnection)
            return;
        if (memcmp(&msg[1], magicValue, 8) == 0)
            hasAcceptedHandshake = true; // we will continue in update()
    } else if (msg[0] == 0x03) { // mtu test
        if (len < 3 || !isServerConnection || hasNegotiatedMTU)
            return;
        unsigned short pkMtu = ((unsigned short &) msg[1]);
        if (pkMtu < 100 || pkMtu >= 3000)
            return;
        if (len >= (size_t) (pkMtu - 1)) {
            mtu = pkMtu;
            hasNegotiatedMTU = true;
            sendMTUAccepted(mtu);

            handler.onConnected(*this);
        }
    } else if (msg[0] == 0x04) { // mtu accepted
        if (len < 3 || isServerConnection || hasNegotiatedMTU)
            return;
        unsigned short pkMtu = ((unsigned short &) msg[1]);
        mtu = pkMtu;
        hasNegotiatedMTU = true;

        handler.onConnected(*this);
    } else if (msg[0] == 0x10 || msg[0] == 0x11) { // ack reliable/unreliable
        if (len < 5 || !hasNegotiatedMTU)
            return;
        unsigned short pkId = ((unsigned short &) msg[1]);
        unsigned short count = ((unsigned short &) msg[3]);
        if ((size_t) (count * 4 + 5) > len)
            return; // not enough data
        size_t off = 5;
        sendReliablePacketsMutex.lock();
        if (msg[0] == 0x10) { // ack reliable
            for (int i = count; i > 0; i--) {
                unsigned short b = (unsigned short &) msg[off];
                off += 2;
                unsigned short e = (unsigned short &) msg[off];
                off += 2;

                auto bi = sendReliablePacketsIndexesToReceive.lower_bound(b);
                auto ei = sendReliablePacketsIndexesToReceive.upper_bound(e);
                if (bi == sendReliablePacketsIndexesToReceive.end() ||
                    ei == sendReliablePacketsIndexesToReceive.begin())
                    continue;
                ei--;
                if (*bi > *ei || *bi < b || *ei > e)
                    continue;
                sendReliablePacketsIndexesToReceive.erase(bi, ++ei);
            }
        }
        sendReliablePacketsMutex.unlock();
        sendAACK(pkId, (msg[0] == 0x10));
    } else if (msg[0] == 0x12 || msg[0] == 0x13) { // aack reliable/unreliable
        if (len < 3 || !hasNegotiatedMTU)
            return;
        std::lock_guard<std::mutex> m (ackMutex);
        unsigned short pkId = ((unsigned short &) msg[1]);
        auto *list = &sentAckReliablePackets;
        auto *alist = &ackReliableIds;
        if (msg[0] == 0x13) { // unreliable
            list = &sentAckUnreliablePackets;
            alist = &ackUnreliableIds;
        }
        auto ei = list->find(pkId);
        if (ei == list->end())
            return;
        for (const auto &p : ei->second) {
            auto b = alist->lower_bound(p.first);
            auto e = alist->upper_bound(p.second);
            if (b == alist->end() || e == alist->begin())
                continue;
            e--;
            if (*b > *e || *b < p.first || *e > p.second)
                continue;
            alist->erase(b, ++e);
        }
        int fPkId = pkId - USHRT_MAX / 2;
        if (fPkId > 0) {
            auto bi = list->lower_bound((unsigned short) fPkId);
            if (bi != list->end())
                list->erase(bi, ++ei);
        } else {
            list->erase(list->begin(), ++ei);
            fPkId += USHRT_MAX;
            auto bi = list->lower_bound((unsigned short) fPkId);
            if (bi != list->end())
                list->erase(bi, list->end());
        }
    } else {
        Log::trace("Connection", "Unknown System Packet ID: %i", msg[0]);
    }
}

int Connection::send(char *msg, size_t len, unsigned char channel, const ACKCallback &ackCallback) {
    XGEAssert(hasNegotiatedMTU);
    PacketId pkId = { 0, 0, 1, 1, (channel == 0 ? 1 : 0), (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, channel, ackCallback, nullptr);
	return 0;
}

int Connection::sendUnordered(char *msg, size_t len, const ACKCallback &ackCallback) {
    XGEAssert(hasNegotiatedMTU);
    PacketId pkId = { 0, 0, 1, 0, 0, (ackCallback != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ackCallback, nullptr);
	return 0;
}

void Connection::sendUnreliable(char *msg, size_t len, const ACKCallback &ack, const ACKCallback &nack) {
    XGEAssert(hasNegotiatedMTU);
    PacketId pkId = { 0, 0, 1, 0, 0, (ack != nullptr || nack != nullptr ? 1 : 0), 0 };
    send(pkId, msg, len, 0, ack, nack);
}

void Connection::sendHandshakeStart() {
    char msg[9];
    msg[0] = 0x01; // handshake start
    memcpy(&msg[1], magicValue, 8);

    // reliable system message
    PacketId pkId = { 0, 1, 1, 0, 0, 0, 0 };
    send(pkId, msg, sizeof(msg), 0, nullptr, nullptr);
}

void Connection::sendHandshakeAccepted() {
    char msg[9];
    msg[0] = 0x02; // handshake accepted
    memcpy(&msg[1], magicValue, 8);

    // reliable system message
    PacketId pkId = { 0, 1, 1, 0, 0, 0, 0 };
    send(pkId, msg, sizeof(msg), 0, nullptr, nullptr);
}

void Connection::sendMTUTest(unsigned short mtu) {
    // unreliable system message
    PacketId pkId = { 0, 1, 0, 0, 0, 0, 0 };

    XGEAssert(mtu > 4);
    StackArray(char, msg, mtu);
    msg[0] = *((char *) &pkId);
    msg[1] = 0x03; // mtu test
    ((unsigned short &) msg[2]) = mtu;
    memset(&msg[4], 0, (size_t) (mtu - 4));

    sendRaw(&msg[0], mtu); // send raw, so it won't get fragmented in any way
}

void Connection::sendMTUAccepted(unsigned short mtu) {
    char msg[3];
    msg[0] = 0x04; // mtu accepted
    ((unsigned short &) msg[1]) = mtu;

    // reliable system message
    PacketId pkId = { 0, 1, 1, 0, 0, 0, 0 };
    send(pkId, msg, sizeof(msg), 0, nullptr, nullptr);
}

void Connection::sendAACK(unsigned short ackId, bool isReliable) {
    char msg[3];
    msg[0] = (char) (isReliable ? 0x12 : 0x13); // aack
    ((unsigned short &) msg[1]) = ackId;

    // unreliable system message
    PacketId pkId = { 0, 1, 0, 0, 0, 0, 0 };
    send(pkId, msg, sizeof(msg), 0, nullptr, nullptr);
}

void Connection::buildAndSendACKList(const std::set<unsigned short> &list, bool isReliable) {
    // byte pkId, byte sysMsgId, short ackNum, short elCount, { short start, end }[]
    const size_t headerBaseSize = 1 + 1 + 2 + 2;
    auto it = list.begin();
    std::vector<std::pair<unsigned short, unsigned short>> acks;
    acks.push_back({*it, *it});
    it++;
    for ( ; it != list.end(); it++) {
        if (acks.back().second + 1 == *it) {
            acks.back().second = *it;
        } else {
            if (acks.size() * 4 + headerBaseSize >= mtu)
                break;
            acks.push_back({*it, *it});
        }
    }
	size_t msgSize = headerBaseSize + acks.size() * 4;
    StackArray(char, msg, msgSize);
    PacketId pkId = { 0, 1, 0, 0, 0, 0, 0 };
    msg[0] = *((char *) &pkId);
    msg[1] = (char) (isReliable ? 0x10 : 0x11); // ack (reliable / unreliable)
    unsigned short index = sendACKIndex++;
    ((unsigned short &) msg[2]) = index;
    ((unsigned short &) msg[4]) = (unsigned short) acks.size();
    size_t off = headerBaseSize;
    for (auto e : acks) {
        ((unsigned short &) msg[off]) = e.first; off += 2;
        ((unsigned short &) msg[off]) = e.second; off += 2;
    }
    if (isReliable)
        sentAckReliablePackets.insert({index, std::move(acks)});
    else
        sentAckUnreliablePackets.insert({index, std::move(acks)});
    sendRaw(msg, msgSize);
}

void Connection::update() {
    if (dead)
        return;
    auto t = std::chrono::steady_clock::now();
    if (!isServerConnection && !hasNegotiatedMTU && hasAcceptedHandshake) {
        if (mtuSendAttempt == -1 || (t - lastMtuSendTime) > mtuAttemptDelayTime) {
            mtuSendAttempt++;
            int valNum = mtuSendAttempt / mtuAttemptsPerValue;
            if (valNum >= mtuValuesCount) {
                // this connection is dead
                dead = true;
                return;
            }
            sendMTUTest((unsigned short) mtuValues[valNum]);
            lastMtuSendTime = t;
        }
    }

    // send acks
    ackMutex.lock();
    if (ackReliableIds.size() > 0 && t - lastReliableAckSend > ackSendTime) {
        buildAndSendACKList(ackReliableIds, true);
        lastReliableAckSend = t;
    }
    if (ackUnreliableIds.size() > 0 && t - lastUnreliableAckSend > ackSendTime) {
        buildAndSendACKList(ackUnreliableIds, false);
        lastUnreliableAckSend = t;
    }
    ackMutex.unlock();

    // resend reliable packets
    sendReliablePacketsMutex.lock();
    while (!sendReliablePackets.empty()) {
        if (sendReliablePackets.top().time > t)
            break;

        // resend
        unsigned short i = sendReliablePackets.top().index;
        if (sendReliablePacketsIndexesToReceive.count(i) <= 0) {
            sendReliablePackets.pop();
            continue;
        }
        std::vector<char> d = std::move(sendReliablePackets.top().data);
        sendReliablePackets.pop();
        sendAndQueueReliableRaw(std::move(d), i);
    }
    sendReliablePacketsMutex.unlock();

    // remove old reliable packets ids so they can be received again
    receivedReliablePacketRemoveQueueMutex.lock();
    while (!receivedReliablePacketIndexesRemoveQueue.empty()) {
        if (receivedReliablePacketIndexesRemoveQueue.top().time > t)
            break;
        receivedReliablePacketIndexes.erase(receivedReliablePacketIndexesRemoveQueue.top().index);
        receivedReliablePacketIndexesRemoveQueue.pop();
    }
    receivedReliablePacketRemoveQueueMutex.unlock();
}