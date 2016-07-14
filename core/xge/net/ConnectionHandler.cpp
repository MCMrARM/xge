#include "ConnectionHandler.h"

#include <cstring>
#include "DatagramSocket.h"
#include "Connection.h"

using namespace xge;

void ConnectionHandler::addUserPacket(Connection &connection, char *data, size_t len) {
    if (receiveCallback != nullptr) {
        receiveCallback(connection, data, len);
        return;
    }
    std::vector<char> v(len);
    memcpy(v.data(), data, len);
    packets.push_back(PacketData(&connection, std::move(v)));
}

void ConnectionHandler::addUserPacket(Connection &connection, std::vector<char> data) {
    if (receiveCallback != nullptr) {
        receiveCallback(connection, data.data(), data.size());
        return;
    }
    packets.push_back({ &connection, std::move(data) });
}

void ConnectionHandler::loopThread() {
    while (!shouldStopThread) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ConnectionHandler::update() {
    Datagram dg;
    while (socket.receive(dg, false)) {
        if (connections.count(dg.addr) > 0)
            connections[dg.addr] = std::unique_ptr<Connection>(new Connection(*this, dg.addr));
        connections.at(dg.addr)->handlePacket(dg.data, (size_t) dg.dataSize);
    }
    for (auto &it : connections) {
        it.second->resendPackets();
        it.second->removeQueuedReceivedReliablePacketIndexes();
        // TODO: handle disconnects
    }
}