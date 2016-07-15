#include "ConnectionHandler.h"

#include <cstring>
#include "DatagramSocket.h"
#include "Connection.h"

using namespace xge;

void ConnectionHandler::onConnected(Connection &connection) {
    // no need to lock mutex - this will always be called from a context that has locked the mutex already
    if (connectedCallback != nullptr) {
        connectedCallback(connection);
    }
}

void ConnectionHandler::addUserPacket(Connection &connection, char *data, size_t len) {
    // no need to lock mutex
    if (receiveCallback != nullptr) {
        receiveCallback(connection, data, len);
        return;
    }
    std::vector<char> v(len);
    memcpy(v.data(), data, len);
    packets.push_back(PacketData(&connection, std::move(v)));
}

void ConnectionHandler::addUserPacket(Connection &connection, std::vector<char> data) {
    // no need to lock mutex
    if (receiveCallback != nullptr) {
        receiveCallback(connection, data.data(), data.size());
        return;
    }
    packets.push_back({ &connection, std::move(data) });
}

std::shared_ptr<Connection> ConnectionHandler::open(NetAddress addr) {
    std::lock_guard<std::mutex> lock (mutex);
    if (connections.count(addr) > 0)
        return connections.at(addr);
    std::shared_ptr<Connection> connection (new Connection(*this, addr));
    connections.insert({ addr, connection });
    return connection;
}

void ConnectionHandler::loopThread() {
    while (!shouldStopThread) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ConnectionHandler::update() {
    std::lock_guard<std::mutex> lock (mutex);

    Datagram dg;
    while (socket.receive(dg, false)) {
        if (connections.count(dg.addr) > 0)
            connections.at(dg.addr)->handlePacket(dg.data, (size_t) dg.dataSize);
        else {
            if (!acceptConnections || (connectCallback != nullptr && !connectCallback(dg.addr)))
                continue;
            std::shared_ptr<Connection> connection (new Connection(*this, dg.addr, true));
            connections.insert({ dg.addr, connection });
            connection->handlePacket(dg.data, (size_t) dg.dataSize);
        }
    }
    for (auto it = connections.begin(); it != connections.end(); ) {
        it->second->update();
        if (it->second->isDead())
            it = connections.erase(it);
        else
            it++;
    }
}