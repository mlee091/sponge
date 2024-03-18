#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // Process SYN flag if it's received for the first time
    if (seg.header().syn && !_received_syn) {
        _received_syn = true;
        _isn = seg.header().seqno;
    }

    // Push payload if ACK is valid and stream output is not fully received
    if (ackno().has_value() && !_reassembler.stream_out().input_ended()) {
        auto &header = seg.header();
        auto stream_index = unwrap(header.seqno, _isn, _checkpoint);
        auto abs_ackno = unwrap(ackno().value(), _isn, _checkpoint);
        // Push substring if it is within the window size
        if (auto length = seg.length_in_sequence_space();
            stream_index + length > abs_ackno && stream_index < abs_ackno + window_size())
            _reassembler.push_substring(seg.payload().copy(), header.syn ? stream_index : stream_index - 1, header.fin);
        _checkpoint = _reassembler.stream_out().bytes_written();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_received_syn)
        return nullopt;

    // Calculate next write position considering if the input has ended, and wrap it
    size_t next_write = _reassembler.stream_out().bytes_written() + (_reassembler.stream_out().input_ended() ? 2 : 1);
    return wrap(next_write, _isn);
}

size_t TCPReceiver::window_size() const { return _reassembler.stream_out().remaining_capacity(); }
