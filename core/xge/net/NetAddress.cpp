#include "NetAddress.h"

#include <cstring>
#ifndef _WINDOWS
#include <arpa/inet.h>
#endif

using namespace xge;

NetAddress::NetAddress() {
    memset(&addr, 0, sizeof(addr));
}

NetAddress::NetAddress(std::string ip, unsigned short port) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = (ip.size() > 0 ? inet_addr(ip.c_str()) : INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
}

bool NetAddress::operator==(const NetAddress &a) const {
    return memcmp(&addr, &a.addr, sizeof(a.addr)) == 0;
}