# What this is about #

* Peer to peer sockets programming. A central server is used as registry of running peers.
### a) Client- Server Computing ###
* The client programs are written such they at starting up, they register their details
with the central server. The server program maintains a list of all the clients that are
running and have registered with it.
### b) Peer-to-peer computing ###
* Upon obtaining the list of other clients from the server, the client program can connect to
any of the client from the list. The clients once connected to each other should have the
capability of relaying text messages directly to each other without further help from the
server. A client that receives the message displays it on its console. Each client
sends an acknowledgement to the other client that a text message has been received and
displayed. Some way of disconnecting or terminating the communication between two peers
is provided(by typing \W or \w). The communication channel between any two clients
is duplex so that both clients are capable of transmitting and receiving messages at
the same time.

### How do I compile? Run the following commands on the terminal ###

* gcc -pthread -o peer peer.c
* gcc -pthread -o server server.c

### How do I run? ###
* Start by running the server:  ./server
* Open other terminal windows or tabs and run the peer on each of them: ./peer
* Proceed by selecting different options on the peers
* Remember to initiate chat to a peer that is waiting

### Who do I talk to? ###

* [Contact Siech](mailto:siele.bernard@gmail.com)