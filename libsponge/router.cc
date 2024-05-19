#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";
    _route_table.emplace_back(route_prefix, prefix_length, next_hop, interface_num);
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    if (dgram.header().ttl <= 1)
        return;

    uint32_t best_route_id = _route_table.size();
    int longest_prefix = -1;
    for (const RouteRecord &current_route : _route_table) {
        int mask =
            current_route._prefix_length == 0
                ? 0
                : current_route._prefix_length == 32 ? 0xFFFFFFFF : (0xFFFFFFFF) << (32 - current_route._prefix_length);
        if ((current_route._route_prefix & mask) == (dgram.header().dst & mask) &&
            current_route._prefix_length > longest_prefix) {
            longest_prefix = current_route._prefix_length;
            best_route_id = &current_route - &_route_table[0];
        }
    }

    if (best_route_id == _route_table.size())
        return;

    const RouteRecord &chosen_route = _route_table[best_route_id];

    dgram.header().ttl--;
    auto next_hop = chosen_route._next_hop;
    auto interface_num = chosen_route._interface_num;
    _interfaces[interface_num].send_datagram(
        dgram, next_hop.has_value() ? next_hop.value() : Address::from_ipv4_numeric(dgram.header().dst));
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
