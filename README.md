# Producer-Consumer Project

This program implements a multithreaded producer-consumer model using POSIX threads (Pthreads) in C++. 
Producers generate items and place them into a shared buffer.
while consumers removes items from the buffer for processing. 
The program demonstrate synchronization between threads to manage access to the shared buffer.

## Contributors

- Abdullah Alshatti
- Abdulaziz Alshatti

## Compilation Instructions

A Makefile is provided for easy compilation. Run the following command in the terminal:

make

This command compiles the source files and produces an executable named "prodcons".


Execute the program with:
./prodcons <buffer size> <number of producers> <number of consumers>

Example:

./prodcons 10 5 3


