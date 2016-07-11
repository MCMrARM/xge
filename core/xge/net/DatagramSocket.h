#pragma once

#include <string>
#include <netinet/in.h>

namespace xge {

    struct Datagram {
        sockaddr_in addr;
        char data[65507];
        ssize_t dataSize;

        operator bool() {
            return (dataSize > 0);
        }
    };

    class DatagramSocket {

    private:
        int fd;

    public:
        /**
         * Constructs a datagram socket. Supports only IPv4 addressing right now.
         */
        DatagramSocket(std::string ip, unsigned short port);

        void close();

        Datagram receive();
        void send(Datagram const &dg);

        inline int getFileDescriptor() {
            return fd;
        }

        operator bool() {
            return (fd >= 0);
        }

    };

}