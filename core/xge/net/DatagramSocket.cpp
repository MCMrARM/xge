#include "DatagramSocket.h"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../util/Log.h"

using namespace xge;

DatagramSocket::DatagramSocket(NetAddress addr) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        Log::error("ServerSocket", "Unable to open socket");
        return;
    }
    {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    }
    if (bind(fd, (sockaddr *) &addr.addr, sizeof(addr.addr)) < 0) {
        Log::error("DatagramSocket", "Failed to bind socket");
        ::close(fd);
        fd = -1;
    }
}

void DatagramSocket::close() {
    if (fd < 0)
        return;
    ::close(fd);
    fd = -1;
}

Datagram DatagramSocket::receive() {
    Datagram ret;
    socklen_t clientAddrLen = sizeof(ret.addr);
    ret.dataSize = (ssize_t) recvfrom(fd, ret.data, sizeof(ret.data), 0, (sockaddr*) &ret.addr, &clientAddrLen);
    return ret;
}

bool DatagramSocket::receive(Datagram &dg, bool canBlock) {
    socklen_t clientAddrLen = sizeof(dg.addr);
    dg.dataSize = (ssize_t) recvfrom(fd, dg.data, sizeof(dg.data), (canBlock ? 0 : MSG_DONTWAIT), (sockaddr*) &dg.addr, &clientAddrLen);
    return dg;
}

bool DatagramSocket::send(Datagram const &dg, bool canBlock) {
    sendto(fd, dg.data, (size_t) dg.dataSize, (canBlock ? 0 : MSG_DONTWAIT), (sockaddr*) &dg.addr, sizeof(dg.addr));
}

bool DatagramSocket::send(sockaddr_in addr, const char *data, size_t len, bool canBlock) {
    sendto(fd, data, len, (canBlock ? 0 : MSG_DONTWAIT), (sockaddr*) &addr, sizeof(addr));
}