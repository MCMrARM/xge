#pragma once

#include <vector>
#include <functional>
#include <chrono>
#include <queue>
#include <set>
#include <map>
#include "DatagramSocket.h"
#include "NetAddress.h"

namespace xge {

    class ConnectionHandler;

    class Connection {

    public:
        typedef std::function<void (int)> ACKCallback;

    protected:
        friend class ConnectionHandler;

        ConnectionHandler &handler;
        sockaddr_in addr;
        unsigned short mtu = 1500;

        std::chrono::milliseconds reliableResendTime;
        std::chrono::milliseconds reliablePacketIdReuseTime;

        struct PacketId {
            int special : 2;
            int system : 1;
            int reliable : 1;
            int ordered : 1;
            int channelZero : 1;
            int ack : 1;
            int fragmented : 1;
        };
        struct ReliableSendPacket {
            std::chrono::steady_clock::time_point time;
            std::vector<char> data;
            unsigned short index;

            bool operator <(const ReliableSendPacket &pk) const {
                return (time > pk.time);
            }
        };
        struct ReliableReceiveRemove {
            std::chrono::steady_clock::time_point time;
            unsigned short index;

            bool operator <(const ReliableReceiveRemove &pk) const {
                return (time > pk.time);
            }
        };
        struct FragmentedPacketData {
            std::vector<char> data;
            std::vector<bool> received;
            unsigned int neededFragments;

            void setSize(size_t size, unsigned int neededFragments) {
                if (data.size() == size && received.size() == neededFragments)
                    return;
                data.resize(size);
                received.resize(neededFragments, false);
                this->neededFragments = neededFragments;
            }
        };

        unsigned short sendReliableIndex = 0;
        unsigned short sendFragmentPkIndex = 0;
        unsigned short sendOrderIndex[256];
        std::priority_queue<ReliableSendPacket> sendReliablePackets;
        std::vector<unsigned short> receivedSentReliablePacketIndexes;
        std::map<int, FragmentedPacketData> fragmentedPackets;

        std::set<unsigned short> receivedReliablePacketIndexes;
        std::priority_queue<ReliableReceiveRemove> receivedReliablePacketIndexesRemoveQueue;
        std::map<int, std::vector<char>> receivedOrderedPackets[256];
        unsigned short receivedOrderedPacketIndex[256];

        unsigned short getNextReliableIndex();
        unsigned short getNextFragmentPacketIndex();
        unsigned short getNextOrderIndex(unsigned char channel);

        // internal function responsible for sending packets
        void send(PacketId pkId, char *msg, size_t len, unsigned char channel,
                  const ACKCallback &ack, const ACKCallback &nack);

        void sendRaw(char *data, size_t len);
        void sendAndQueueReliableRaw(std::vector<char> data, unsigned short reliableId);

        void handlePacket(char *msg, size_t len);

        void resendPackets();
        void removeQueuedReceivedReliablePacketIndexes();

    public:
        Connection(ConnectionHandler &handler, const NetAddress &addr);

        /**
         * This function will send an unreliable packet. The packet may arrive with a delay, before or after other
         * packets or may be lost. It's recommended not to use this for any important data. The NACK (not acknowledged)
         * callback is called when the packet is considered dropped. It's generally recommended not to use this callback
         * unless really needed.
         */
        void sendUnreliable(char *msg, size_t len, const ACKCallback &ack = nullptr, const ACKCallback &nack = nullptr);
        inline void sendUnreliable(std::vector<char> msg, const ACKCallback &ack = nullptr, const ACKCallback &nack = nullptr) {
            sendUnreliable(msg.data(), msg.size(), ack, nack);
        }

        /**
         * This function sends a reliable, unordered packet. The packet may arrive earlier than other packets sent at
         * the same time, or later. It's generally recommended to use send, however it'll consume a little bit more
         * bandwidth.
         */
        int sendUnordered(char *msg, size_t len, const ACKCallback &ackCallback = nullptr);
        inline int sendUnordered(std::vector<char> msg, const ACKCallback &ackCallback = nullptr) {
            sendUnordered(msg.data(), msg.size(), ackCallback);
        }

        /**
         * This function sends a reliable, ordered packet. An ACK callback is optional, and if specified will cause some
         * more network traffic.
         * As this packet is reliable, there's no NACK callback as the only way to have the packet
         * not delivered at all is a disconnect.
         * The packets on a single channel will always be received in the send order in your code. Every channel has
         * it's own ordering.
         */
        int send(char *msg, size_t len, unsigned char channel = 0, const ACKCallback &ackCallback = nullptr);
        inline int send(std::vector<char> msg, unsigned char channel = 0,
                         const ACKCallback &ackCallback = nullptr) {
            return send(msg.data(), msg.size(), channel, ackCallback);
        }

    };

}
