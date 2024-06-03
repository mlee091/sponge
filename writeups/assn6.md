Assignment 6 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
The add_route method takes four parameters: the route prefix, the prefix length, an optional next hop address, and the interface number. The route prefix and prefix length define the network part of the IP address. The next hop is the next router to which packets should be forwarded. If the next hop is not provided, it means that the destination is directly connected to this router. The interface number is the number of the network interface used to send packets. This method adds a new route to the router's routing table, which is a vector of RouteRecord objects.

The route_one_datagram method is responsible for routing a single Internet datagram. It first checks if the Time-To-Live (TTL) of the datagram is greater than 1. If not, it returns without doing anything, as the datagram has reached its maximum hop limit. If the TTL is sufficient, the method then looks for the best route in the routing table. The best route is the one with the longest prefix that matches the destination IP address of the datagram. If such a route is found, the method updates the TTL of the datagram, determines the next hop, and sends the datagram using the appropriate network interface.

Implementation Challenges:
I kept encountering this error: left shift of negative value [-Werror=shift-negative-value]
int mask = current_route._prefix_length == 0 ? full_mask : full_mask & (~0 << (31 - current_route._prefix_length));
It took me very long to figure out that I had to use 0xFFFFFFFF instead of ~0 to ensure that the shift operation is performed on a positive number. I wasn't aware that ~0 would result in a negative number in signed integer context, and shifting a negative number is undefined behavior in C++.

Remaining Bugs:
I don't think there should be any remaining bugs.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
mlee091: void Router::route_one_datagram(InternetDatagram &dgram) {
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

explain the detailed flow of route_one_datagram and what route_one_datagram eventually does. you must include line numbers in your explanation

GitHub Copilot: Sure, here's a line-by-line explanation of the `route_one_datagram` function:

1-2: The function takes an `InternetDatagram` object by reference. This object represents a datagram that needs to be routed.

3-4: If the Time to Live (TTL) of the datagram is less than or equal to 1, the function returns without doing anything. This is because the datagram has either reached its destination or cannot be forwarded anymore.

5-6: Two variables are initialized. `best_route_id` is set to the size of the route table, and `longest_prefix` is set to -1. These will be used to find the best route for the datagram.

7-15: The function iterates over each `RouteRecord` in the route table. For each route, it calculates a mask based on the prefix length of the route. It then checks if the route prefix (ANDed with the mask) matches the destination address of the datagram (ANDed with the mask). If it does, and the prefix length of the current route is greater than the longest prefix found so far, the function updates `longest_prefix` and `best_route_id`.

16-18: If no suitable route was found (i.e., `best_route_id` is still equal to the size of the route table), the function returns without doing anything.

19: The function retrieves the best route from the route table.

20: The TTL of the datagram is decremented by 1. This is because the datagram is about to be forwarded to another router.

21-22: The next hop and the interface number are retrieved from the chosen route.

23-25: The datagram is sent through the appropriate interface. If the next hop is specified in the route, it is used as the destination. Otherwise, the destination address of the datagram is used.

In summary, `route_one_datagram` is responsible for routing a single datagram. It does this by finding the best route from the route table, decrementing the TTL of the datagram, and then sending the datagram through the appropriate interface.
