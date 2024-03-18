#include "stream_reassembler.hh"

#include <algorithm>
#include <stdexcept>

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity)
    , _capacity(capacity)
    , _stream(capacity)
    , currentIndex(0)
    , eofIndex(std::numeric_limits<uint64_t>::max())
    , numberOfUnassembledBytes(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // figuring out where it should start and end in the stream
    size_t startIndex = std::max(index, currentIndex);
    size_t endIndex =
        std::min(index + data.size(), std::min(currentIndex + _capacity - _output.buffer_size(), eofIndex));

    // if this is the last, set up where the stream should end
    // if the end doesn't match what we expect, throw an error
    if (eof) {
        if (eofIndex == std::numeric_limits<uint64_t>::max()) {
            eofIndex = index + data.size();
        } else if (eofIndex != index + data.size()) {
            throw std::runtime_error("StreamReassembler::push_substring: Inconsistent EOF indexes!");
        }
    }

    // iterating through the substring
    for (size_t i = startIndex, j = startIndex - index; i < endIndex; ++i, ++j) {
        auto &currentPair = _stream[i % _capacity];
        if (currentPair.second == true) {
            // check for inconsistent substrings, and throw an error
            if (currentPair.first != data[j]) {
                throw std::runtime_error("StreamReassembler::push_substring: Inconsistent substrings!");
            }
        } else {
            currentPair = std::make_pair(data[j], true);
            ++numberOfUnassembledBytes;
        }
    }

    // assemble contiguous substrings
    string str;
    while (currentIndex < eofIndex && _stream[currentIndex % _capacity].second == true) {
        str.push_back(_stream[currentIndex % _capacity].first);
        _stream[currentIndex % _capacity] = {0, false};
        --numberOfUnassembledBytes;
        ++currentIndex;
    }

    // write them into output stream
    _output.write(str);
    if (currentIndex == eofIndex) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return numberOfUnassembledBytes; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }