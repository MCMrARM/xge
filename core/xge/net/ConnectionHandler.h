#pragma once

#include <stddef.h>
#include <vector>
#include <unordered_map>
#include <deque>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <cstring>
#include <netinet/in.h>
#include "DatagramSocket.h"
#include "NetAddress.h"

namespace xge {

    class DatagramSocket;
    class Connection;

    struct PacketData {
        Connection *connection = nullptr;
        std::vector<char> data;

        PacketData() {
            //
        }
        PacketData(Connection *connection, std::vector<char> data) : connection(connection), data(std::move(data)) {
            //
        }

        operator bool() {
            return connection != nullptr;
        }
    };

    class ConnectionHandler {

    public:
        typedef std::function<void (Connection &, char *, size_t)> PacketReceiveCallback;

    protected:
        friend class Connection;

        DatagramSocket &socket;
        PacketReceiveCallback receiveCallback = nullptr;
        std::deque<PacketData> packets;
        std::unordered_map<NetAddress, std::shared_ptr<Connection>> connections;
        std::thread thread;
        std::atomic<bool> shouldStopThread;

        void addUserPacket(Connection &connection, char *data, size_t len);
        void addUserPacket(Connection &connection, std::vector<char> data);

        void loopThread();

    public:
        /**
         * This function creates a connection handler object. It'll handle incoming packets and timeouts, etc.
         * If 'manual' is set to true, then you'll need to call ConnectionHandler::update() manually every time you want
         * to update connections (handle incoming packets, timeouts, etc). When it's set to false (default and
         * recommended) it'll start an additional thread handling those stuff.
         */
        ConnectionHandler(DatagramSocket &socket, bool manual = false) : socket(socket), shouldStopThread(false) {
            if (!manual)
                thread = std::thread(&ConnectionHandler::loopThread, this);
        }

        ~ConnectionHandler() {
            if (thread.joinable()) {
                shouldStopThread = true;
                thread.join();
            }
        }

        inline DatagramSocket &getSocket() const {
            return socket;
        }

        std::shared_ptr<Connection> open(NetAddress addr);

        /**
         * This function allows you to set a packet receive callback. This callback will be called when a packet has
         * been received and ready to be passed to your code. If this callback is set, packet queue (the .receive()
         * function) will be disabled.
         * Keep in mind this callback will be called from an update thread unless a manual mode is used.
         */
        void setPacketReceiveCallback(const PacketReceiveCallback &callback = nullptr) {
            receiveCallback = callback;
        }

        /**
         * This function returns a packet from the internal receive queue. This function won't return any packets if
         * a packet receive callback is specified.
         */
        bool receive(PacketData &data) {
            if (packets.size() > 0) {
                data = std::move(packets.front());
                packets.pop_front();
                return true;
            }
            return false;
        }

        /**
         * This function returns a packet from the internal receive queue. This function won't return any packets if
         * a packet receive callback is specified.
         */
        inline PacketData receive() {
            PacketData ret;
            receive(ret);
            return ret;
        }

        /**
         * This function updates connections (handles incoming packets, timeouts, etc). It should not be called if
         * you haven't set 'manual' to true in the constructor.
         */
        void update();

    };

}