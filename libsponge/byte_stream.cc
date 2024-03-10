#include "byte_stream.hh"

#include <algorithm>
using namespace std;

ByteStream::ByteStream(const size_t _capacity)
    : buffer(), capacity(_capacity), end_write(false), end_read(false), _write_bytes(0), _read_bytes(0) {}

size_t ByteStream::write(const string &data) {
    size_t canWrite = capacity - buffer.size();
    size_t actualWrite = min(canWrite, data.length());
    for (size_t i = 0; i < actualWrite; i++) {
        buffer.push_back(data[i]);
    }
    _write_bytes += actualWrite;
    return actualWrite;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t canPeek = min(len, buffer.size());
    string out = "";
    for (size_t i = 0; i < canPeek; i++) {
        out += buffer[i];
    }
    return out;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > buffer.size()) {
        set_error();
        return;
    }
    for (size_t i = 0; i < len; i++) {
        buffer.pop_front();
    }
    _read_bytes += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string output = "";
    if (len > buffer.size()) {
        set_error();
        return output;
    }
    for (size_t i = 0; i < len; i++) {
        output += buffer.front();
        buffer.pop_front();
    }
    _read_bytes += len;
    return output;
}

void ByteStream::end_input() { end_write = true; }

bool ByteStream::input_ended() const { return end_write; }

size_t ByteStream::buffer_size() const { return buffer.size(); }

bool ByteStream::buffer_empty() const { return buffer.empty(); }

bool ByteStream::eof() const { return end_write && buffer.empty(); }

size_t ByteStream::bytes_written() const { return _write_bytes; }

size_t ByteStream::bytes_read() const { return _read_bytes; }

size_t ByteStream::remaining_capacity() const { return capacity - buffer.size(); }
