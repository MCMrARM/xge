#pragma once

#include <string>
#include "NetAddress.h"

namespace xge {

    struct Datagram {
        sockaddr_in addr;
        char data[65507];
        long long int dataSize;

        operator bool() {
            return (dataSize > 0);
        }
    };

    class DatagramSocket {

    private:
#ifdef _WINDOWS
		SOCKET fd;
#else
        int fd;
#endif

    public:
        /**
         * Constructs a datagram socket. Supports only IPv4 addressing right now.
         */
        DatagramSocket(NetAddress addr);

        void close();

        Datagram receive();
        bool receive(Datagram &dg);
        bool send(Datagram const &dg);
        bool send(sockaddr_in addr, const char *data, size_t len, bool canBlock = true);

        inline int getFileDescriptor() {
            return (int) fd;
        }

        operator bool() {
            return (fd >= 0);
        }

    };

}