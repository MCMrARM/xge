#include "DatagramSocket.h"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../util/Log.h"

using namespace xge;

DatagramSocket::DatagramSocket(std::string ip, unsigned short port) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        Log::error("ServerSocket", "Unable to open socket");
        return;
    }
    {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    }
    sockaddr_in addr;
    memset((void*) &addr, 0, sizeof(addr));
    if (ip.size() > 0) {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (bind(fd, (sockaddr *) &addr, sizeof(addr)) < 0) {
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

void DatagramSocket::send(Datagram const &dg) {
    sendto(fd, dg.data, (size_t) dg.dataSize, 0, (sockaddr*) &dg.addr, sizeof(dg.addr));
}