 
This is the very first implementation of a simple Client-Server program.
The goals are not even well defined ...
These two programs make use of the Qt library, but as the  client reads 
its instructions from stdin and its results  to stdout,
it should not be difficult for applications to communicate with the server.
To experiment the program, two example files are provided (data1 and data2) which shows some 
aspects of the proposed syntax.
To launch the application:
./Server &
./Client < data1
(or ./xx < data1)
Two graphs  are given: graph0.txt and ex.tgf. 
The latter one is a binary file and can only be read on x86 compatible machines.
The server and client use a TCP connection on port 4242.
