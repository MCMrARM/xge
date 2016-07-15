#pragma once

#include <string>
#include <netinet/in.h>

namespace xge {

    class NetAddress {

    public:
        sockaddr_in addr;

        NetAddress();
        NetAddress(sockaddr_in addr) : addr(addr) {
            //
        }
        NetAddress(std::string ip, unsigned short port);

        bool operator==(const NetAddress &a) const;

    };

}

namespace std {

    template <>
    struct hash<xge::NetAddress>
    {
        size_t operator()(const xge::NetAddress &x) const
        {
            return std::_Hash_impl::hash(&x.addr, sizeof(x.addr));
        }
    };

}