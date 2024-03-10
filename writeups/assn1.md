Assignment 1 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 9 hours to do (including the time on studying, designing, and writing the code).

# Program Structure and Design of the StreamReassembler:
The program is designed around the `StreamReassembler` class, which is responsible for reassembling out-of-order data chunks into a contiguous stream. The class maintains an internal buffer `_stream` to hold the data chunks, using a circular buffer approach. Each element in the vector is a pair consisting of a character and a boolean flag indicating whether the character is part of the assembled stream.

The main function in the class is `push_substring`, which takes a chunk of data, its position in the stream, and a flag indicating if it's the last chunk. The function calculates the start and end indices of the chunk in the stream, checks for inconsistent EOF indexes, processes the chunk, assembles the contiguous substrings, and writes them into the output stream.

The design of the program is modular, with each function performing a specific task. This makes the code easier to understand and maintain. The use of clear and descriptive variable names and comments also improves the readability of the code.

The program has a time complexity of O(n) for the `push_substring` function. This is achieved by using a vector to store the data chunks, allowing for fast insertion and retrieval of chunks in a circular buffer fashion.

The program also uses exception handling to deal with errors, making the code more robust and easier to debug, as errors can be caught and handled in a controlled manner.

# Implementation Challenges:
The most challenging part was handling out-of-order and overlapping data chunks. It was tricky to ensure that each chunk was placed at the correct position in the stream, and that overlapping chunks didn't overwrite each other. I overcame this challenge by using a vector as a circular buffer (`_stream`), with each element representing a pair of a character and a boolean flag indicating its presence in the stream. By checking for existing chunks before insertion, I ensured the correct placement of data in the stream.

Ensuring the correctness of the code was another challenge. I used defensive programming techniques, such as checking preconditions and throwing exceptions when something was wrong.

Debugging and testing the code was done using unit tests and print statements. The unit tests were particularly helpful in identifying and fixing bugs. They also provided a way to verify that the code was working as expected.

# Remaining Bugs:
There are no known bugs in the code. However, the code may not handle extremely large data chunks efficiently due to the use of a map for storing the chunks. A more efficient data structure could be used to improve the performance.

In addition, if the input data is not valid, the code may not behave as expected. More error checking could be added to handle invalid input data.

The code has not been thoroughly tested with edge cases. More testing is needed to ensure that the code can handle all possible scenarios.


- Optional: I had unexpected difficulty with: I finally managed to set up remote VSC, as I was unable to do so in the previous assignment and hence did not manage to submit a .git file.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
