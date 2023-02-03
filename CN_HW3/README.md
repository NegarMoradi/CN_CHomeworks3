# Computer Network assignment
### CA3 Computer Network Programming Assingment

### Part 1:

Overview:
In this project, the implementation of a TCP protocol is done using
UDP sockets. This is done by designing and implementing
handshaking and acknowledgement procedures.

## Project Classes and Data Structures:1. 
### Segment: This class is essentially the class representing packets
of the protocol. Each segment has these fields:
a. src_port: the sender’s port. It has 5 bytes.
b. dst_port: the receiver’s port. It also has 5 bytes.
c. flag: a 2-byte field for flags. If the flag is 1, means it is an
acknowledgement packet.
d. sequence_number: a 4-byte field for the packet’s sequence
number.
e. acknowledgement: a 4-byte field to represent the sequence
number of the packet that this segment is acknowledging.
f. sent_tme: the system time that the packet was sent with the
socket.
g. payload: a byte-stream of characters as payload.
Also, these methods are provided for the segment class:

A. Setters and getters for the fields.
B. serialize: inputs a buffer and fills it with the serialized byte
stream of the segment.
C. deserialize: inputs a buffer and reads its content and fills
the fields of the object.
D. slice: returns a string of the range of buffer to slice.
E. make_fixed_size_str: make a sign-extended version of the
field to make fixed-size streams of bytes.

<image title="segment1" alt="Alt text" src="./pics/segment1.png">

<image title="segment2" alt="Alt text" src="./pics/segment2.png">

### Sender: Creates a process to send the file via packets. It inputs
three ports:
a. Sender Port: the port specified in the application level for
the sender object itself.
b. Receiver port: the port to which the receiver listens.
c. Router port: the port to which the router listens.
The most important part of this class is the methods it provides
to replicate a TCP sender:


### Router: 
this class acts as a midllware between senders and the
receiver. The procedure is explained below:
First we set up the socket connection and bind it to always listen
to the port. After that, we create two threads: one for incomming
packets (getting blocked on the recvfrom) and one for outgoingpackets (getting blocked on the thread lock). These two threads
share a shared resource that is the queue of packets in the router.
This resource is locked before acccess using a mutex called
queue_mutex. Also, the required fields are provided to apply the
RED algorithm.
process_incoming: using select, we receive the packets using
revfrom, deserailze it and update the port map (i.e the mapping
of application port to the os port).


### part 1 questions:
1.
What is TCP?

Transmission Control Protocol (TCP) is connection-oriented, meaning once a connection has been established, data can be transmitted in two directions. TCP has built-in systems to check for errors and to guarantee data will be delivered in the order it was sent, making it the perfect protocol for transferring information like still images, data files, and web pages.

But while TCP is instinctively reliable, its feedback mechanisms also result in a larger overhead, translating to greater use of the available bandwidth on your network. 

What is UDP?

User Datagram Protocol (UDP) is a simpler, connectionless Internet protocol wherein error-checking and recovery services are not required. With UDP, there is no overhead for opening a connection, maintaining a connection, or terminating a connection; data is continuously sent to the recipient, whether or not they receive it. 

Although UDP isn’t ideal for sending an email, viewing a webpage, or downloading a file, it is largely preferred for real-time communications like broadcast or multitask network transmission.

What is the Difference Between TCP and UDP?

TCP is a connection-oriented protocol, whereas UDP is a connectionless protocol. A key difference between TCP and UDP is speed, as TCP is comparatively slower than UDP. Overall, UDP is a much faster, simpler, and efficient protocol, however, retransmission of lost data packets is only possible with TCP. 

Another notable discrepancy with TCP vs UDP is that TCP provides an ordered delivery of data from user to server (and vice versa), whereas UDP is not dedicated to end-to-end communications, nor does it check the readiness of the receiver (requiring fewer overheads and taking up less space).  

TCP vs UDP Speed

The reason for UDP’s superior speed over TCP is that its non-existent ‘acknowledgement’ supports a continuous packet stream. Since TCP connection always acknowledges a set of packets (whether or not the connection is totally reliable), a retransmission must occur for every negative acknowledgement where a data packet has been lost. 

But because UDP avoids the unnecessary overheads of TCP transport, it’s incredibly efficient in terms of bandwidth, and much less demanding of poor performing networks, as well.

<image title="tcpUdp" alt="Alt text" src="./pics/TcpUdp.png">

2.
<image title="selectiveRepeat" alt="Alt text" src="./pics/selectiveRepeat.png">



## Part 2:

The first of this phase is routing on a topology using the DVRP routing algorithm.
In this topology, we use nodes to simulate hosts and routers of the desired network, and edges in the input graph to simulate communication links. You should use the DVRP algorithm to perform routing for all network routers in such a way that each one has the shortest path to each host with a specific IP address. Please note that here, for the sake of simplicity, we omit the subnetting check for different LANs, but in reality it is usually used to increase the addressing space with the number of IP addresses in the base mode.


### commands:

<image title="addHost" alt="Alt text" src="./pics/addHost.png">

<image title="addUpdate" alt="Alt text" src="./pics/addUpdate.png">

<image title="remove" alt="Alt text" src="./pics/remove.png">

<image title="update" alt="Alt text" src="./pics/update.png">

<image title="log" alt="Alt text" src="./pics/log.png">

<image title="run" alt="Alt text" src="./pics/run.png">

<image title="draw" alt="Alt text" src="./pics/draw.png">

<image title="showTable" alt="Alt text" src="./pics/showTable.png">


Question:


2. 

Router Protocols: Distance vector vs Link state

There are different routing classes available for providing a more spherical solution packet. Different networks have special individual needs and different routing protocols have been designed to meet the individual needs of these networks.

There is no straightforward answer on the right routing protocol to use. A variety of parameters need to be investigated before deciding on that. Your investigations should include bandwidth prerequisite, reliability, convergence speed, network architecture and much more.

I won't concentrate on the details of the best routing decision process, but I will try to illustrate the details behind the operation of the different routing classes so that you can make the appropriate decisions yourself.



What are Distance Vector routing protocols?

Distance Vector routing protocols base their decisions on the best path to a given destination based on the distance. Distance is usually measured in hops, though the distance metric could be delay, packets lost, or something similar.

If the distance metric is hop, then each time a packet goes through a router, a hop is considered to have traversed. The route with the least number of hops to a given network is concluded to be the best route towards that network.

The vector shows the direction to that specific network. Distance vector protocols send their entire routing table to directly connected neighbors. Examples of distance vector protocols include RIP - Routing Information Protocol and IGRP - Interior Gateway Routing Protocol.
If you're interested in finding out more information on RIP, check out my articles on how to configure Routing Information Protocol RIPv1 and RIPv2.



What are Link State routing protocols?

Link state protocols are also called shortest-path-first protocols. Link state routing protocols have a complete picture of the network topology. Hence they know more about the whole network than any distance vector protocol.

Three separate tables are created on each link state routing enabled router. One table is used to hold details about directly connected neighbors, one is used to hold the topology of the entire internetwork and the last one is used to hold the actual routing table.

Link state protocols send information about directly connected links to all the routers in the network. Examples of Link state routing protocols include OSPF - Open Shortest Path First and IS-IS - Intermediate System to Intermediate System.

There are also routing protocols that are considered to be hybrid in the sense that they use aspects of both distance vector and link state protocols. EIGRP - Enhanced Interior Gateway Routing Protocol is one of those hybrid routing protocols.



Operation of Distance Vector routing protocols

To illustrate the routing updating process for Distance Vector routing protocols, let's say that we have routers that have been set in service at the same time and all run a distance vector routine protocol. Each router sends its distance vector to its neighbor. Also, each router receives distance vectors from each neighbor as well. Combining the information learned from neighbors with each router's own information, the best estimate route to a given destination is inserted into the routing table.

Upon receiving the routing updates from all neighbors, the final router, which we’ll call RouterH, performs its calculation for estimating the best route to any given destination. 

The result of this process is that the first to the penultimate routers, RouterA and RouterG, informed RouterH how far away RouterD is. RouterH knows that both routers (A and G) are neighbor routers; hence it adds 1 to the hop metric and concludes that it can reach RouterD via both RouterA and RouterG with an overall distance of 2 hops.
Operation of Link State routing protocols

As already mentioned, Link State routing protocols hold 3 distinctive tables: a neighbor table, a topology table, and an actual routing table. Link state routing operation follows four simple steps; each link state enabled router must perform the following:

     Discover its neighbors and build its neighbor table

     Measure the cost (delay, bandwidth, etc) to each of its neighbors

     Construct and send a routing update telling all it has learned to all routers in the network

 Apply the Dijkstra algorithm to construct the shortest path to all possible destinations

