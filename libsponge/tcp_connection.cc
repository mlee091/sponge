#include "tcp_connection.hh"

#include <iostream>

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPConnection::handle_reset(const TCPSegment&) {
    _receiver.stream_out().set_error();
    _sender.stream_in().set_error();
    _is_active = false;
}

void TCPConnection::handle_ack(const TCPSegment &seg) {
    _sender.ack_received(seg.header().ackno, seg.header().win);
    if (_sender.next_seqno_absolute() != 0) {
        _sender.fill_window();
        refresh_queue();
    }
}

void TCPConnection::handle_streams() {
    bool is_receiver_stream_closed = _receiver.stream_out().eof();
    bool is_sender_stream_open = !_sender.stream_in().eof();

    if (is_receiver_stream_closed && is_sender_stream_open) {
        _linger_after_streams_finish = false;
    }
}

void TCPConnection::handle_non_empty_segment(const TCPSegment &seg) {
    if (_sender.next_seqno_absolute() == 0 && seg.header().syn == 1) {
        _sender.fill_window();
    } else {
        _sender.send_empty_segment();
    }
    refresh_queue();
}

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _idle_duration; }

void TCPConnection::segment_received(const TCPSegment &seg) {
        _idle_duration = 0;
        bool is_reset = seg.header().rst;

        if (is_reset) {
            handle_reset(seg);
            return;
        }

        _receiver.segment_received(seg);
        bool is_ack = seg.header().ack;

        if (is_ack) {
            handle_ack(seg);
        }

        handle_streams();
        attempt_clean_close();

        if (seg.length_in_sequence_space() != 0) {
            handle_non_empty_segment(seg);
        } else if (seg.header().seqno == _receiver.ackno().value() - 1) {
            _sender.send_empty_segment();
            refresh_queue();
        }
}

bool TCPConnection::active() const { return _is_active; }

size_t TCPConnection::write(const string &data) {
    size_t bytesWritten = _sender.stream_in().write(data);
    _sender.fill_window();
    refresh_queue();
    return bytesWritten;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _sender.tick(ms_since_last_tick);
    refresh_queue();
    _idle_duration += ms_since_last_tick;
    bool hasExceededMaxRetransmissions = _sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS;

    if (hasExceededMaxRetransmissions) {
        dispatch_reset();
        return;
    }
    attempt_clean_close();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    refresh_queue();
}

void TCPConnection::connect() {
    _sender.fill_window();
    refresh_queue();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
            dispatch_reset();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

void TCPConnection::attempt_clean_close(){
    bool isReceiverStreamClosed = _receiver.stream_out().eof();
    bool isSenderStreamClosed = _sender.stream_in().eof();
    bool isSenderIdle = _sender.bytes_in_flight() == 0;
    bool isConnectionIdleOrTimeout = !_linger_after_streams_finish || _idle_duration >= _cfg.rt_timeout * 10;
    if (isReceiverStreamClosed && isSenderStreamClosed && isSenderIdle && isConnectionIdleOrTimeout) {
        _is_active = false;
    }
}

void TCPConnection::dispatch_reset(){
    _segments_out = {}; 
    TCPSegment seg = _sender.segments_out().back();
    seg.header().rst = true;
    _segments_out.push(seg);
    _receiver.stream_out().set_error();
    _sender.stream_in().set_error();
    _is_active = false;
}

void TCPConnection::refresh_queue(){
    bool receiverHasAck = _receiver.ackno().has_value();
    while (!_sender.segments_out().empty()) {
        auto& seg = _sender.segments_out().front();
        if (receiverHasAck) {
            auto& header = seg.header();
            header.ack = true;
            header.ackno = _receiver.ackno().value();
            header.win = _receiver.window_size();
        }
        _segments_out.push(seg);
        _sender.segments_out().pop();
    }
}