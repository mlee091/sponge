#include "stream_reassembler.hh"

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : 
    unassembledBase(0),
    unassembledSize(0),
    endOfFile(0),
    dataBuffer(capacity, '\0'),
    bufferBitmap(capacity, false),
    _output(capacity), 
    _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // if the provided data is empty or the index is greater than the capacity, ignore it.
    if (data.empty() || index >= _capacity) {
        return;
    }

    size_t dataStart = index > unassembledBase ? index - unassembledBase : 0; // offset in data
    size_t dataEnd = dataStart + min(data.size(), _capacity - index); // end index in data

    // update the buffer with the data.
    for (size_t i = dataStart; i < dataEnd; ++i) {
        size_t bufferIndex = index + i;
        
        // if the byte is not already assembled, update the buffer and set the corresponding bitmap.
        if (!bufferBitmap[bufferIndex]) {
            dataBuffer[bufferIndex] = data[i];
            bufferBitmap[bufferIndex] = true;
            unassembledSize++;
        }
    }

    // update the endOfFile marker if the provided substring contains an EOF marker.
    if (eof) {
        endOfFile = index + dataEnd - dataStart;
    }

    // check if there are contiguous bytes at the beginning of the buffer, write them to output.
    while (bufferBitmap[unassembledBase]) {
        char currentChar = dataBuffer[unassembledBase];
        _output.write(&currentChar); 
        bufferBitmap[unassembledBase] = false;
        unassembledBase++;
        unassembledSize--;
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembledSize; }

bool StreamReassembler::empty() const { return unassembledSize == 0; }
