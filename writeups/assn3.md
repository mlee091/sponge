Assignment 3 Writeup
=============

My name: Megan Lee

My POVIS ID: meganlee

My student ID (numeric): 49004702

This assignment took me about 5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
- TCPSender: This class is responsible for sending TCP segments over a network. It maintains a queue of segments to be sent, and it sends them in order as acknowledgments are received. It also manages the window size, ensuring that it does not send more segments than the window allows. The TCPSender class also interacts with the Timer class to manage the retransmission timer.

- Timer: This class is responsible for managing the retransmission timer. It starts the timer when a segment is sent, and stops it when an acknowledgment is received. If the timer expires before an acknowledgment is received, it triggers a retransmission of the segment.

Implementation Challenges:
- Retransmission Timer: Managing the retransmission timer was a complex task. The timer had to be started when a segment was sent, and stopped when an acknowledgment was received. If the timer expired before an acknowledgment was received, the segment had to be retransmitted, and the timer had to be restarted. Additionally, the timeout period had to be doubled after each retransmission, which added another layer of complexity.

- Window Size: Managing the window size was another challenge. The sender had to ensure that it did not send more segments than the window allowed. This required careful management of the queue of segments to be sent, and the tracking of acknowledgments received.

Remaining Bugs:
While the program works okay for most scenarios, there may be edge cases that have not been thoroughly tested. For example, the program may not handle very large or very small window sizes correctly. There may also be issues with the handling of retransmissions and timeouts in certain network conditions.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
