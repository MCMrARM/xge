#include "DatagramSocket.h"

#include <cstring>
#ifndef _WINDOWS
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#endif
#include "../util/Log.h"

using namespace xge;

DatagramSocket::DatagramSocket(NetAddress addr) {
#ifdef _WINDOWS
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == INVALID_SOCKET) {
#else
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
#endif
        Log::error("DatagramSocket", "Unable to open socket");
        return;
    }
#ifdef _WINDOWS
	{
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(int));
		u_long val = 1;
		ioctlsocket(fd, FIONBIO, &val);
	}
#else
    {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    }
#endif
    if (bind(fd, (sockaddr *) &addr.addr, sizeof(addr.addr)) < 0) {
        Log::error("DatagramSocket", "Failed to bind socket");
		close();
    }
}

void DatagramSocket::close() {
    if (fd < 0)
        return;
#ifdef _WINDOWS
	closesocket(fd);
#else
    ::close(fd);
#endif
    fd = -1;
}

Datagram DatagramSocket::receive() {
    Datagram ret;
    size_t clientAddrLen = sizeof(ret.addr);
	memset(&ret.addr, 0, sizeof(ret.addr));
#ifdef _WINDOWS
	ret.dataSize = recvfrom(fd, ret.data, sizeof(ret.data), 0, (sockaddr*) &ret.addr, (int *) &clientAddrLen);
#else
    ret.dataSize = recvfrom(fd, ret.data, sizeof(ret.data), 0, (sockaddr*) &ret.addr, (socklen_t *) &clientAddrLen);
#endif
    return ret;
}

bool DatagramSocket::receive(Datagram &dg) {
	size_t clientAddrLen = sizeof(dg.addr);
	memset(&dg.addr, 0, sizeof(dg.addr));
#ifdef _WINDOWS
	dg.dataSize = recvfrom(fd, dg.data, sizeof(dg.data), 0, (sockaddr*) &dg.addr, (int *) &clientAddrLen);
#else
    dg.dataSize = recvfrom(fd, dg.data, sizeof(dg.data), 0, (sockaddr*) &dg.addr, (socklen_t *)&clientAddrLen);
#endif
    return dg;
}

bool DatagramSocket::send(Datagram const &dg) {
#ifdef _WINDOWS
    return sendto(fd, dg.data, (int) dg.dataSize, 0, (sockaddr*) &dg.addr, sizeof(dg.addr)) == dg.dataSize;
#else
	return sendto(fd, dg.data, (size_t) dg.dataSize, 0, (sockaddr*)&dg.addr, sizeof(dg.addr)) == dg.dataSize;
#endif
}

bool DatagramSocket::send(sockaddr_in addr, const char *data, size_t len, bool canBlock) {
#ifdef _WINDOWS
    return sendto(fd, data, (int) len, 0, (sockaddr*) &addr, sizeof(addr)) == len;
#else
	return sendto(fd, data, len, 0, (sockaddr*) &addr, sizeof(addr)) == len;
#endif
}