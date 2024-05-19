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
