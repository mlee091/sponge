Assignment 4 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 10 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [0.37, 0.36]

Program Structure and Design of the TCPConnection:

# Member Functions: 
- segment_received(const TCPSegment &seg): Handles incoming TCP segments, including handling resets, acknowledgments, streams, and non-empty segments.
- active() const: Returns whether the connection is active.
- write(const string &data): Writes data to the sender’s input stream and updates the window.
- tick(const size_t ms_since_last_tick): Updates the sender, refreshes the queue, and handles retransmissions or clean close.
- end_input_stream(): Signals the end of input data for the sender.
- connect(): Initializes the sender and refreshes the queue.
- ~TCPConnection(): Destructor that handles clean shutdown if the connection is active.

# Private Helper Functions:
- handle_reset(const TCPSegment &seg): Handles reset segments.
- handle_ack(const TCPSegment &seg): Handles acknowledgment segments.
- handle_streams(): Checks stream states and updates _linger_after_streams_finish.
- handle_non_empty_segment(const TCPSegment &seg): Handles non-empty segments.
- remaining_outbound_capacity() const: Returns remaining capacity in the sender’s input stream.
- bytes_in_flight() const: Returns the number of bytes in flight.
- unassembled_bytes() const: Returns the number of unassembled bytes in the receiver.
- time_since_last_segment_received() const: Returns the idle duration.
- attempt_clean_close(): Attempts to close the connection cleanly.
- dispatch_reset(): Dispatches a reset segment.
- refresh_queue(): Updates the queue.

# Variables:
- _is_active: Indicates whether the connection is active.
- _idle_duration: Tracks the time since the last segment was received.
- _linger_after_streams_finish: Indicates whether to linger after streams finish.
- _sender: Manages sender-related functionality.
- _receiver: Manages receiver-related functionality.
- _segments_out: Stores outgoing segments.

Implementation Challenges:
The handling of events in its proper sequence during termination, and memory management were a struggle, as seen in the test cases failed (especially the segfaults, which suggest memory-related issues).

Remaining Bugs:
I couldn't pass four test cases:
The following tests FAILED:
         37 - t_passive_close (Failed)
         39 - t_ack_rst (SEGFAULT)
         46 - t_winsize (SEGFAULT)
        137 - t_icR_128K_8K_l (Failed)

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
