#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    EthernetFrame frame;
    frame.header().src = _ethernet_address;

    auto arpIter = arpMapping.find(next_hop_ip);
    bool arpExists = arpIter != arpMapping.end();

    if (!arpExists && ipWaitDurationMapping.find(next_hop_ip) == ipWaitDurationMapping.end()) {
        ARPMessage arp_message;
        arp_message.opcode = ARPMessage::OPCODE_REQUEST;
        arp_message.sender_ethernet_address = _ethernet_address;
        arp_message.sender_ip_address = _ip_address.ipv4_numeric();
        arp_message.target_ethernet_address = {};
        arp_message.target_ip_address = next_hop_ip;

        frame.header().dst = ETHERNET_BROADCAST;
        frame.header().type = EthernetHeader::TYPE_ARP;
        frame.payload() = arp_message.serialize();
        _frames_out.push(frame);

        ipWaitDurationMapping[next_hop_ip] = WAIT_LIMIT;
    }

    if (!arpExists) {
        ipDatagramWaitMapping[next_hop_ip].push_back(dgram);
    } else {
        frame.header().dst = arpIter->second.ethernetAddr;
        frame.header().type = EthernetHeader::TYPE_IPv4;
        frame.payload() = dgram.serialize();
        _frames_out.push(frame);
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if (frame.header().dst != _ethernet_address && frame.header().dst != ETHERNET_BROADCAST)
        return nullopt;

    switch (frame.header().type) {
        case EthernetHeader::TYPE_IPv4: {
            InternetDatagram dgram;
            if (dgram.parse(frame.payload()) == ParseResult::NoError)
                return dgram;
            else
                return nullopt;
            break;
        }
        case EthernetHeader::TYPE_ARP: {
            ARPMessage arp_message;
            if (arp_message.parse(frame.payload()) != ParseResult::NoError)
                return nullopt;

            const auto &src_ip = arp_message.sender_ip_address;
            const auto &src_eth = arp_message.sender_ethernet_address;
            const auto &dst_ip = arp_message.target_ip_address;
            const auto &dst_eth = arp_message.target_ethernet_address;

            if (arp_message.opcode == ARPMessage::OPCODE_REQUEST && dst_ip == _ip_address.ipv4_numeric()) {
                ARPMessage reply;
                reply.opcode = ARPMessage::OPCODE_REPLY;
                reply.sender_ethernet_address = _ethernet_address;
                reply.sender_ip_address = _ip_address.ipv4_numeric();
                reply.target_ethernet_address = src_eth;
                reply.target_ip_address = src_ip;

                EthernetFrame reply_frame;
                reply_frame.header().src = _ethernet_address;
                reply_frame.header().dst = src_eth;
                reply_frame.header().type = EthernetHeader::TYPE_ARP;
                reply_frame.payload() = reply.serialize();
                _frames_out.push(reply_frame);
            }

            if ((arp_message.opcode == ARPMessage::OPCODE_REPLY && dst_eth == _ethernet_address) ||
                arp_message.opcode == ARPMessage::OPCODE_REQUEST) {
                arpMapping[src_ip] = {src_eth, ARP_ENTRY_LIFESPAN};
                auto iter = ipDatagramWaitMapping.find(src_ip);
                if (iter != ipDatagramWaitMapping.end()) {
                    for (const auto &dgram : iter->second) {
                        EthernetFrame new_frame;
                        new_frame.header().src = _ethernet_address;
                        new_frame.header().dst = src_eth;
                        new_frame.header().type = EthernetHeader::TYPE_IPv4;
                        new_frame.payload() = dgram.serialize();
                        _frames_out.push(new_frame);
                    }
                    ipDatagramWaitMapping.erase(iter);
                }
                ipWaitDurationMapping.erase(src_ip);
            }
            break;
        }
    }
    return nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    for (auto iter = arpMapping.begin(); iter != arpMapping.end();) {
        if (iter->second.lifespan <= ms_since_last_tick)
            iter = arpMapping.erase(iter);
        else {
            iter->second.lifespan -= ms_since_last_tick;
            ++iter;
        }
    }

    for (auto iter = ipWaitDurationMapping.begin(); iter != ipWaitDurationMapping.end();) {
        if (iter->second <= ms_since_last_tick) {
            ARPMessage arp_request;
            arp_request.opcode = ARPMessage::OPCODE_REQUEST;
            arp_request.sender_ethernet_address = _ethernet_address;
            arp_request.sender_ip_address = _ip_address.ipv4_numeric();
            arp_request.target_ip_address = iter->first;

            EthernetFrame frame;
            frame.header().dst = ETHERNET_BROADCAST;
            frame.header().src = _ethernet_address;
            frame.header().type = EthernetHeader::TYPE_ARP;
            frame.payload() = arp_request.serialize();
            _frames_out.push(frame);

            iter->second = WAIT_LIMIT;
        } else {
            iter->second -= ms_since_last_tick;
            ++iter;
        }
    }
}
