#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;
void Timer::tick_handle(const size_t ticks, const uint16_t window_size, std::queue<TCPSegment> &segments_out) {
    if (isRunning && retransmissionTimeout <= ticks) {
        // If the retransmission timeout has expired, retransmit the first outstanding segment
        segments_out.push(outstandingSegments.front().first);
        if (window_size != 0) {
            // If the window size is not zero, double the base retransmission timeout and increment the number of
            // consecutive retransmissions
            numConsecutiveRetransmissions++;
            baseRetransmissionTimeout *= 2;
        }
        // Reset the retransmission timeout
        retransmissionTimeout = baseRetransmissionTimeout;
    } else
        retransmissionTimeout -= ticks;
}

uint64_t Timer::ack_handle(const uint64_t ackno) {
    uint64_t ret = 0;
    numConsecutiveRetransmissions = 0;
    // Remove all segments that have been acknowledged from the queue
    while (!outstandingSegments.empty() && outstandingSegments.front().second <= ackno) {
        // Reset the retransmission timeout
        retransmissionTimeout = baseRetransmissionTimeout = initialRetransmissionTimeout;
        // Add the length of the acknowledged segment to the return value
        ret += outstandingSegments.front().first.length_in_sequence_space();
        outstandingSegments.pop();
    }
    // If there are no more outstanding segments, stop the timer
    isRunning = !outstandingSegments.empty();
    return ret;
}
//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _timer(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    // Calculate the end of the window. If the window size is 0, use 1 instead
    uint64_t end_seqno = (_window_size > 0 ? static_cast<uint64_t>(_window_size) : 1ul) + _ackno;
    // Continue until we've sent a FIN, reached the end of the window, or have no more data to send
    while (!_fin_sended && _next_seqno < end_seqno && (!_stream.buffer_empty() || _next_seqno == 0 || _stream.eof())) {
        _timer.on();
        TCPSegment seg;
        // If this is the first segment, set the SYN flag
        seg.header().syn = _next_seqno == 0;
        // Wrap the sequence number to fit within the TCP sequence number space
        seg.header().seqno = wrap(_next_seqno, _isn);
        // Calculate the length of the payload, taking into account the window size, the buffer size, and the maximum
        // payload size
        size_t read_len =
            min(min(static_cast<size_t>(end_seqno - _next_seqno) - seg.header().syn, _stream.buffer_size()),
                TCPConfig::MAX_PAYLOAD_SIZE);
        seg.payload() = Buffer(_stream.read(read_len));
        size_t seg_len = seg.length_in_sequence_space();
        // If we have room in the window and have reached the end of the stream, set the FIN flag
        if (seg_len < end_seqno - _next_seqno) {
            seg.header().fin = _stream.eof();
            seg_len += _stream.eof();
            _fin_sended = _stream.eof();
        }
        // Update the number of bytes in flight and the next sequence number
        _bytes_in_flight += seg_len;
        _next_seqno += seg_len;
        // Add the segment to the queue and to the timer
        _segments_out.push(seg);
        _timer.push_outstanding(seg, _next_seqno);
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t unwrapped_ackno = unwrap(ackno, _isn, _next_seqno);
    // Only update state if the received acknowledgement number is not older than the next sequence number
    if (unwrapped_ackno <= _next_seqno) {
        _ackno = unwrapped_ackno;
        _window_size = window_size;
        // Update the number of bytes in flight by subtracting the number of bytes acknowledged
        _bytes_in_flight -= _timer.ack_handle(_ackno);
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    // Handle the tick event, updating the timer and possibly retransmitting segments
    _timer.tick_handle(ms_since_last_tick, _window_size, _segments_out);
}

unsigned int TCPSender::consecutive_retransmissions() const { return _timer.consecutive_retransmissions(); }

void TCPSender::send_empty_segment() {
    TCPSegment segment;
    // Wrap the sequence number to fit within the TCP sequence number space
    segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(segment);
}