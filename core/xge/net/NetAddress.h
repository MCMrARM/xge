#pragma once
#include <string>
#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

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
#ifdef _WINDOWS
			return std::_Hash_seq((unsigned char *) &x.addr, sizeof(x.addr));
#else
            return std::_Hash_impl::hash(&x.addr, sizeof(x.addr));
#endif
        }
    };

}