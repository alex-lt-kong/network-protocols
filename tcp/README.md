# Transmission Control Protocol (TCP)

## Basics

- IP packet format

  ![](./assets/ip-packet-format.svg "ip-packet-format.svg")
    - Transport: a one-byte long field indicating the protocol of the payload.
      Typical values
      are<sup>[[List of IP protocol numbers](https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers)]</sup>:
        - 0x01: ICMP
        - 0x06: TCP
        - 0x11: UDP
    - Source IP Address/Destination IP Address: are actually nothing more than a
      320bit number, just it is usually expressed in dotted-decimal format, like
      192.168.0.1<sup>[[Understand TCP/IP addressing and subnetting basics](https://learn.microsoft.com/en-us/troubleshoot/windows-client/networking/tcpip-addressing-and-subnetting)]</sup>.

- TCP segment
  format <sup>[[Transmission Control Protocol (TCP)](https://www.khanacademy.org/computing/computers-and-internet/xcae6f4a7ff015e7d:the-internet/xcae6f4a7ff015e7d:transporting-packets/a/transmission-control-protocol--tcp)]</sup>

  ![](./assets/tcp-packet-format.svg "tcp-packet-format.svg")

    - TCP standard specifies that both source port and destination port are
      16-bit long only, so it is no coincidence that there are only 65536 ports
      across all platforms.
    - Sequence number: a counter used to keep track of
      every byte sent outward by a host. If a TCP packet contains 1400 bytes of
      data, then the sequence number will be increased by 1400 after the packet
      is
      transmitted. <sup>[[Transmission Control Protocol (TCP)](https://www.ibm.com/docs/en/zos-basic-skills?topic=4-transmission-control-protocol-tcp)]</sup>
    - Acknowledgement number: a counter to keep track of every byte that has
      been received. If 1000 bytes are received by a host, it increases the
      acknowledgement number by 1000 when it sends out a packet in
      response. <sup>[[Transmission Control Protocol (TCP)](https://www.ibm.com/docs/en/zos-basic-skills?topic=4-transmission-control-protocol-tcp)]</sup>
    - You may compare TCP segment format with UDP message
      format [here](https://github.com/alex-lt-kong/network-protocols/blob/main/udp/README.md)

- Note that source IP and destination IP are two values recorded in an IP
  packet's header, i.e., they are recognized by all the (IP-compliant)
  network devices and useful when routing the packet. On the contrary,
  destination port and source port are not, they are defined by TCP/UDP protocol
  only, for an ip packet, they are nothing but a part of the payload


- Question: Does TCP use another port for sending data?

  Answer: [No, it uses the same port](https://stackoverflow.com/questions/15761776/does-tcp-use-another-port-for-sending-data),
  if you think it uses another port, you might be confused by FTP's design

## Socket programming

### TCP Socket API<sup>[[CS 50 Software Design and Implementation](https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming)]</sup>

![](./assets/tcp-sockets.jpg "./assets/tcp-sockets.jpg")

- The steps involved in establishing a TCP socket on the server side are as follows:

    - Create a socket with the `socket()` function;
    - Bind the socket to an address using the `bind()` function;
    - Listen for connections with the `listen()` function;
    - Accept a connection with the `accept()` function system call. This call typically blocks until a client connects
      with the server.


- As shown in the figure, the steps for establishing a TCP socket on the client side are the following:

    - Create a socket using the `socket()` function;
    - Connect the socket to the address of the server using the `connect()` function;
    - Send and receive data by means of the `read()` and `write()` functions.

- The basic building block for communication is the socket. A socket is an
  endpoint of communication to which a name may be bound. Each socket in use has a
  type and one or more associated processes. Sockets exist within communication
  domains. A communication domain is an abstraction introduced to bundle common
  properties of processes communicating through sockets.

### Socket types

* Stream socket: Provides for the bidirectional, reliable, sequenced, and
  unduplicated flow of data withOUT record boundaries. Aside from the
  bidirectional nature of the dataflow, a pair of connected stream sockets
  provides an interface nearly identical to that of pipes.
* Datagram socket: Supports bidirectional flow of data that isn't guaranteed to
  be sequenced, reliable, or unduplicated. That is, a process receiving messages
  on a datagram socket may find messages duplicated, and possibly in an order
  other than the one in which they were sent. An important characteristic of a
  datagram socket is that record boundaries in data are preserved. Datagram
  sockets closely model the facilities found in many contemporary
  packet-switched networks (e.g. Ethernet).
* Raw socket: Provides users access to the underlying communication protocols
  that support socket abstractions. These sockets are normally
  datagram-oriented, though their exact characteristics depend on the interface
  provided by the protocol. Raw sockets aren't intended for the general user;
  they've been provided mainly for anyone interested in developing new
  communication protocols or in gaining access to some of the more esoteric
  facilities of an existing protocol.

* What does "record boundary"/"messge boundary" mean? A simple example explains
  it: If you send "FOO" and then "BAR" over UDP, the other end will receive two
  datagrams, one containing "FOO" and the other containing "BAR". If you send "
  FOO" and then "BAR" over TCP, no message boundary is preserved. The other end
  might get "FOO" and then "BAR". Or it might get "FOOBAR". Or it might get "F"
  and then "OOB" and then "AR". TCP does not make any attempt to preserve
  application message boundaries -- it's just a stream of bytes in each
  direction.

### Creating sockets

* Refer to code.

* A socket call may fail for any of several reasons, including:
    * lack of memory (ENOBUFS)
    * request for an unknown protocol (EPROTONOSUPPORT)
    * request for a type of socket for which there's no supporting protocol (
      EPROTOTYPE).

### Binding local names

* A socket is created without a name. Until a name is bound to a socket,
  processes have no way to reference it and consequently no messages may be
  received on it. Communicating processes are bound by an ``association.''

* In the Internet domain, an association is composed of local and remote
  addresses, and local and remote ports. In most domains, associations must be
  unique; in the Internet domain, there may never be duplicate
  `<local address, local port, remote address, remote port>` tuples.

* The `bind()` function allows a process to specify half of an association (
  `<local_address, local_port>`) while the `connect()` and `accept()` functions
  are used to complete a socket's association.

### Establishing connections

* Establishing a connection is usually asymmetric: one process is a client and
  the other is a server. The server, when willing to offer its advertised
  services, binds a socket to a well-known address associated with the service
  and then passively listens on its socket. An unrelated process can then
  rendezvous with the server.

* The client requests services from the server by initiating a connection to the
  server's socket. To initiate the connection, the client uses a `connect()`
  call.

* If the client's socket is unbound at the time of the connect call, the system
  will automatically select and bind a name to the socket if necessary (this is
  usually how local addresses are bound to a socket).

## Q&A

### Q: Why does accept() create a new socket?

A:

- A listening socket can be used to accept multiple connections, each with a
  different remote IP/port pair. A TCP connection is defined by 4 values: source
  IP, source port, destination IP, destination port. A socket descriptor
  returned
  by `accept()` is associated with such a connection. In contrast, the listening
  socket is only associated with a local IP and port.

- In Linux, when `accept()` is called, the kernel creates a new socket from the
  head of the accept queue.

  ![](./assets/synq-acceptq.png "synq-acceptq.png")

### Q: Why does TCP socket programming need two sockets but UDP only needs one?

A:

* The reason is that TCP has two different kinds of state that you want to
  control, whereas UDP has only one. When listening for TCP connections on a
  port, the networking stack needs to keep track of the port number and
  interface(s) you are listening on with that socket, as well as the backlogged
  list of incoming TCP connection-requests for that socket.

* Once you have accepted a TCP connection, the new TCP socket returned by
  `accept()` has its own connection-specific state that needs to be tracked
  separately -- this state consists of the client's IP address and source port,
  TCP packet ID sequences, TCP window size and send rate, the incoming data that
  has been received for that TCP connection, outgoing data that has been sent
  for that TCP connection, etc. All of this is stored in a separate internal
  data structure that is associated specifically with that new socket.

* Note that the lifetimes of these two types of state are very much independent
  of each other -- for example, you might decide that you don't want to accept
  any more incoming TCP connections and therefore you want to close the first (
  connections-accepting) socket while continuing to use the second (
  TCP-connection-specific) socket to communicate with the already-connected
  client. Or you might do the opposite, and decide that you don't want to
  continue the conversation with that particular client, so you close the second
  socket, but you do want to continue accepting more TCP connections, so you
  leave the first socket open.

* UDP, on the other hand, has no notion of "accepting connections", so there is
  only one kind of state, and that is the set of buffered sent and/or received
  packets (regardless of their source and/or destination). As such, there is
  generally no need to have more than one socket.

### Q: How to reuse an established TCP connection instead of closing it after only one

`read()`/`recv()`?

A: Similar to `accept()`, `recv()` is blocking and it only returns when it
receives something. Therefore, calling `recv()`
in an infinite loop allows us to re-use an established TCP connection.