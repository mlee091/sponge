Assignment 5 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 7 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:

NetworkInterface is responsible for sending and receiving datagrams and frames, and also handles ARP requests and responses.
### send_datagram
This method is responsible for sending an IP datagram to a next hop address. If the Ethernet address for the next hop IP address is not known, it sends an ARP request and stores the datagram in ipDatagramWaitMapping to be sent later. If the Ethernet address is known, it creates an Ethernet frame with the datagram and pushes it to _frames_out.

### recv_frame
This method is responsible for receiving an Ethernet frame. It checks the type of the frame and handles it accordingly. If it's an IPv4 frame, it parses the payload into an IP datagram and returns it. If it's an ARP frame, it parses the payload into an ARP message and handles the ARP request or response.

### tick
This method is called periodically and is responsible for updating the lifespans in arpMapping and ipWaitDurationMapping. If the lifespan of an ARP entry has expired, it removes the entry. If the wait duration for an IP address has expired, it sends a new ARP request.

In terms of design, maps were used to store the tables and waiting datagrams, which increases lookup and operational efficiency. I also used a queue for outgoing frames to maintain the sequence.

Implementation Challenges:
The tick method took some time as I had to visualise how and when to decrease the lifespans and wait durations, which I managed to get through some trial and error.
The send_datagram and recv_frame methods involved serializing and deserializing IP datagrams and Ethernet frames and converting between their binary and object representations.

Remaining Bugs:
I don't think there should be any remaining bugs.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
