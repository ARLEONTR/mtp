# Mesh Transport Protocol


For debugging, we develop the MTP LKM as an in-tree module. We will take it out of tree when we finish the development.

## Objective

Although the Internet evolved as a robust network, many aspects are set in stone causing ossification. ASes block traffic other than TCP and UDP. A consequence is the difficulty in extending routing for multicasting and broadcasting. Another challenge is that the legacy application layer protocols follow the client-server paradigm where only a single client can communicate with a single server using legacy transport layer protocols such as TCP or UDP.  We call this approach the single-client-to-single-server (SCSS) paradigm. Moreover, the separation of policy from mechanism is not foreseen in present transport layer protocols. The SCSS paradigm can be extended by developing a transport layer protocol that helps a client communicate with multiple servers concurrently which we refer to as mesh transport protocol (MTP). We encounter mesh networks such as Bluetooth Mesh Network or TRILL at various layers of the stack across domains. However, there is no transport layer protocol that leverages mesh networking technologies. In this project, we will address this gap and design and develop a Mesh Transport Protocol (MTP) separating policy from mechanism. 

## Why we need MTP?

We briefly summarize the alternatives and the difference of MTP from these alternatives.

FLUTE/ALC (RFC6726) is a reliable multicast protocol implemented on UDP. It supports object-oriented data delivery to receiver groups and is not well adapted to byte or message streaming. FLUTE/ALC relies on IP multicasting. NORM (RFC5740) is another object-oriented multicast protocol exploiting IP multicast implemented on UDP. MTP will enable full duplex communication and cooperation among the client and servers. 

TIPC (Transparent Inter Process Communication) is a protocol developed by Ericsson in Linux kernel to implement interprocess communication across machines in a cluster. TIPC implements service discovery and tracking in a cluster, supports multiple transmission modes over UDP and L2 services, and provides reliability in group communication. The RDS (Reliable Datagram Sockets) protocol provides reliable, sequenced delivery of datagrams over Infiniband or TCP and it is developed by Oracle. RDS provides reliable, ordered datagram delivery using a single reliable transport between two nodes. RDS uses TCP as the underlying transport, the application data is encapsulated in an RDS header and tunneled over TCP to the destination node at some well-known TCP port where it will be decapsulated.  RUDP (Reliable UDP Protocol) is an RFC published by IETF and developed by Bell Labs. It extends the UDP functionality to introduce reliable datagram transport and flow control. While TIPS provide reliable group communication, RDS and RUDP still follows the SCSS paradigm. Moreover, TIPS is mostly confined to clusters. MTP will be a generic transport layer protocol that will implement an end-to-end reliable datagram and stream transport service without being confined to clusters. 

Libp2p (https://libp2p.io) is a modular system of protocols, specifications, and libraries that enable the development of peer-to-peer network applications. Libp2p aims to be a modular, general-purpose toolkit for any peer-to-peer application. Libp2p uses the transport layer protocols (TCP, UDP, QUIC, etc.) and MTP may be employed under libp2p for efficient handling of one-to-many (and also many-to-many) communication.

Any Pub/Sub messaging queue (NSQ, NATS, 0MQ, RabbitMQ, etc.): These are mostly application layer systems (more than transport protocols). Our aim is to develop a transport layer protocol where the classical socket interface is extended partially to address one-to-many communication. 

Multi-path Transport Layer Protocols (MPTCP, MRDS, SCCP, etc.) follow the SCSS paradigm exploiting multiple networking interfaces. MTP will exploit multiple interfaces, and further extend the concept of SCSS paradigm. 

## Challenges

Some of the challenges that will be tackled in this project are
1.	IETF’s Transport Services (TAPS) work group defines a new asynchronous event-driven API instead of the socket interface. The applicability of this new API is an open research question. The alternative is to extend the Berkeley socket interface. We have to carefully analyze the trade-off between user-space library-based implementation and the kernel-space implementation. A kernel-space implementation may act as a standalone transport protocol implemented in the transport layer. Whereas, the user-space implementation will employ existing transport layer protocols such as UDP or TCP as the conveyor. A thorough analysis has to be conducted.
2.	How to form the mesh overlay topology is a critical feature since we develop a one-client to many-servers transport layer protocol, the client will create a socket that will connect to multiple servers. Servers then will connect to other servers in the party and discover topology. Topology state information and MTP state information has to be up-to-date by measuring RTTs between clients and server and also among servers so that MTP allows server collaboration for data dissemination.
3.	Collaboration: servers may help the client to convey messages to other servers which requires a relay logic implemented in the transport layer without reflecting it to the application layer. This feature may help decrease response time especially when stream multiplexing is to be supported. 
4.	Error control is another critical challenge. There are many error or fault cases such as closed connections, lost servers, and lack of listeners on remote endpoints. MTP has to handle error cases and raise alarms to the application layer. Health monitoring through heartbeat messages or keep-alive messages will be analyzed. 
5.	Congestion control or rate control: Rate is not a single window parameter in MTP anymore, since we will implement server collaboration, congestion control can be delegated to different endpoints on the Internet.
6.	Flow control is also a significant challenge. Clients and cooperating servers should not be conveying information to other servers more than they can handle.
7.	Joining or leaving the MTP party is another feature that has to be implemented. A client may decide to remove an old server or add a new server during a connection. The connection concept of MTP is way bigger than TCP’s connection.
8.	Security: we plan to incorporate TLS session establishment to connection establishment protocol to reduce delays as done by QUIC. Endpoint authentication, message integrity, confidentiality, replay protection, and traffic analysis countermeasures will have to be analyzed and incorporated into the protocol design. 
9.	Head of line blocking is a very important challenge that paved the way to HTTP/2.0 over TCP and now HTTP 3.0 over QUIC. We will develop stream multiplexing in MTP.
10.	NAT traversal is a big challenge where relay servers sitting on public IP addresses may help MTP overcome this issue. We have to analyze the STUN/TURN/ICE kind of protocols and how to embed them in MTP.
11.	Sequence number management, replica/duplicate packet identification, and acknowledgment of the successful reception of packets to multiple servers will be a distributed computing challenge. MTP may support both byte-stream and datagram (where message boundaries are well-defined) transportation. The preservation of the order of packets (bytes) delivered to the application layer can be implemented with various semantics such as FIFO order, causal order, total-order, and total-order FIFO. MTP should support integrity, validity and agreement properties based on the configurable order semantics. 
12.	Supporting multi-cast addresses of the network layer will increase the efficiency of MTP. The remote endpoint identifiers can be multicast IP addresses. In this case, the client does not have to explicitly define all the IP addresses of servers.
13.	TCP defines reliability as no bit errors, no segment (chunks of byte) losses, and no change of order of bytes (segments). Depending on the application requirements, reliability can be defined in different ways. For instance, no packet losses or bit errors may be required but the application logic may tolerate to change of order of packets. The feature set supported by MTP has to be configurable and reconfigurable. The transport layer service features (reliability requirements, connection-oriented or not, stream- or datagram-based transportation, security requirements, …) will be implemented in a negotiable fashion. The application layer will have the means to configure and reconfigure these parameters. 
14.	Support for threshold-based connection handling is another challenge. This approach requires flexibility at the connection setup phase. This will be an important feature for implementing applications employing threshold cryptography or multi-party computation.
15.	Supporting multi-homed transportation is also a challenge. Many endpoints today have multiple Internet-connected interfaces. Exploiting multiple interfaces will help better handle handovers, increase capacity and enhance reliability.  
16.	The observability of MTP is also a challenge. The implementation of MTP has to provide means for monitoring. In-band telemetry among MTP endpoints can be implemented.


## Master Features

1.	Provide reliable (with various definitions as to bit errors, packet losses, and order) and unreliable message/stream delivery up to a well-defined number of retransmissions in one-to-many or many-to-many fashion,
2.	Provide flow/rate control,
3.	Provide congestion control in a configurable manner,
4.	Have low overhead and high performance, 
5.	Provide service reconfigurability,
6.	Provide a keep-alive (heartbeat) mechanism,
7.	Enhance the Internet Checksum approach for better error control,
8.	Have an extended error control for group communication
9.	Support security natively,
10.	Provide asynchronous event-driven API or extend socket interface,
11.	Group management (server joining or leaving the party)
12.	Exploit the mesh networking technologies provided by the lower layers (L2 and L3)
13.	Exploit the broadcast channels efficiently in L1,
14.	Support threshold-based group management (1 <= t <= n servers out of n are enough to establish a group)
15.	Exploit multiple interfaces and use multi-homing and multi-path
16.	Provide observability and monitoring features,
17.	Enable server collaboration for relaying in the transport layer and for load distribution,
18.	Provide stream multiplexing and interleaving to avoid head-of-line blocking,
19.	Provide both connection-oriented, or connectionless transport,


## Ecosystem

Any peer-to-peer overlay network may employ MTP as the transport layer protocol. MTP can be used in a micro-service environment for replication, load balancing, and establishing a common state among multiple containers. Blockchain infrastructures are another candidate application area for implementing distributed consensus protocols. Furthermore, advanced cryptographic protocols such as multi-party computation or threshold-based cryptosystems require a transport protocol such as MTP.

IETF Work Group, Transport Services (TAPS) is working on defining the architecture and requirements for transport layer services. However, their approach is still following the SCSS paradigm, albeit multipath routing is considered. We may have a chance to extend their view for one-client to many-servers transport service.  

## Project Team


[![Build Status](https://img.shields.io/circleci/project/all-contributors/all-contributors/master.svg)](https://circleci.com/gh/all-contributors/workflows/all-contributors/tree/master)
[![Crowdin](https://d322cqt584bo4o.cloudfront.net/all-contributors/localized.svg)](https://crowdin.com/project/all-contributors)
[![All Contributors](https://img.shields.io/github/all-contributors/all-contributors/all-contributors?color=ee8449&style=flat-square)](#contributors-)
[![Netlify Status](https://api.netlify.com/api/v1/badges/e30784c5-9efd-4f20-8a47-13f3fb899613/deploy-status)](https://app.netlify.com/sites/allcontributors-org/deploys)
[![Star on GitHub](https://img.shields.io/github/stars/all-contributors/all-contributors.svg?style=social)](https://github.com/all-contributors/all-contributors/stargazers)

