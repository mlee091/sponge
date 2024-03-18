Assignment 2 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 4 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
## Wrap/unwrap routines

### wrap method:
Takes an absolute 64-bit sequence number and an initial sequence number (ISN), and returns a WrappingInt32 object that represents the 32-bit sequence number corresponding to the absolute sequence number. It does this by adding the raw value of the ISN to the lower 32 bits of the absolute sequence number.

### unwrap method:
Does the opposite of wrap. It takes a WrappingInt32 object (representing a 32-bit sequence number), an ISN, and a "checkpoint" (a recent absolute sequence number), and returns the absolute 64-bit sequence number that corresponds to the 32-bit sequence number. It calculates the offset from the checkpoint to the 32-bit sequence number, adds this offset to the checkpoint to get the result, and adjusts the result if it's less than 0 by adding 2^32.

## TCPReceiver
Private member variables include: _received_syn (whether a SYN has been received), _isn (initial sequence number), _checkpoint (the last acknowledged byte), and _reassembler (an object to reassemble the received segments).

### segment_received method: 
Called when a new TCP segment is received. It first checks if the SYN flag is set and if this is the first SYN received. If so, it updates _received_syn and _isn. Then, if an ACK is valid and the stream output is not fully received, it pushes the payload into the reassembler. It also updates the _checkpoint to the number of bytes written.

### ackno method: 
Calculates the next acknowledgment number to be sent. If a SYN has not been received, it returns nullopt. Otherwise, it calculates the next write position and wraps it using the initial sequence number.

### window_size method: 
Returns the remaining capacity of the stream output, which is used as the window size for flow control in TCP.

Implementation Challenges:
I was sure that I was utilising the correct usage "std::optional<WrappingInt32>" in tcp_receiver.hh and tcp_receiver.cc, but the error did not go away. After much searching on the Internet, I decided to continue without resolving the error.

Remaining Bugs:
There are still some errors for undefined variables.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
