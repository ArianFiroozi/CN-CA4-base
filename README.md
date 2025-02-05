# CN_CHomeworks_4_base

this is a network simulator for computer networks course built using cpp and QT6 for linux.
overall structure consists of routers clustered in different structures and PCs that exchange packets over network.

routers use simplified versions of iBGP and eBGP as routing protocols and RIP or OSPF as routing algorithms. all PCs use IPv6 but some subnets are IPv4 only. router IPs are configured inside code but PCs get their IPs from a DHCP server located inside the subnet. an EventHandler is used to synchronize the entire project. all entities except PC are located in separete QThreads and run simultaneously. a messaging system decides which number of packets sent from each random PC to other. message generation is based on Poisson distribution.

# *TODO*
File read and write is implemented and packet generation is handled. You will need to edit sendTcpPackets function and add tcp ACK to processPackets inside PC.cpp.

For sending control packets, you will possibly need to edit PC::tick as well as creating a new buffer to process control packets separately.

# *UI Elements*
tick count: controls number of ticks emitted during the execution

tick duration: tick duration

algorithm

lambda: used for generating random packets, depricated

# *Configuration*
Configuration can be done in the "common.h" file (sorry i don't have a json file)

# *Code Structure*

main code is located in src and headers, resources have configuration text files and test is the test files written while developing this project. notice that test files don't work anymore and or outdated. structures in the project are as listed:

## Network Simulator
connects GUI to a network, runs the simulation and prints the results.

## Network
a network is where all these objects get created, connected and started. this class has sender PCs, 3 types of clusters, an event handler and a messaging system, each running in a separete QThread. network assignes a subnet to each cluster and connects clusters to other clusters as well as PCs. router IDs and PC IDs are unique inside a subnet and can be used as MAC addresses for DHCP. network class also has the necessary measurments to analysing the results.

network has 3 clusters. a 4x4 Mesh with IPv6, a 3x3 Torus with IPv4 and a RingStar with IPv6 and ring length of 7.
all PCs are IPv6 and all IPv6 addresses are convertible to IPv4.

![image](https://github.com/ArianFiroozi/CN_CHomeworks_3/assets/126232660/380225b2-a4a7-44f9-a469-7fd7dcf6d6af)

### PC
  PCs are objects with IP addresses, buffers and IP version. each pc IP can be configured by admin or DHCP server. details of DHCP communication is explained in DHCP server.
  PCs can receive or send packets to other PCs. these events are triggered by a new message arriving or messaging system signaling the PC to send a new packet.

### Cluster
  each cluster itself is consisted of some routers connected to each other using ports and a DHCP server connected to first router. clusters are given an IP starting point to set router IPs accordingley. a cluster creates its routers, connects them, sets the port delays and assignes a new thread for each router.

#### Router
  each router has its own forwarding table, ip address and version, ports, buffer, and routing protocol. it can also broadcast messages if the destination is set to 255.255.255.255 which is the broadcast address. looping is prevented by memorizing the path and broadcast messages do not go through external ports. also broadcast is done with no delay so that OSPF and RIP convergence delay does not get confused with initial broadcasting delays.

1. IP: as previously mentioned, router IPs are configured by code. IPv6 addresses are IPv4 mapped for ease of use and IPv6 is not fully implemented. IPv6 routers tunnel IPv6 packets into IPv4 packets when trying to send over IPv4 routers.

2. Buffer: each buffer has limit which can be changed in 'commons.h' file. a router puts packets inside the buffer before sending it and if out buffer is full, router will become congested and next packets will be dropped. 

3. Port: ports are entities with ID and delays. each router can write into its port but it cannot see the incoming ports. receiving messages is done by Signals instead. delays are calculated inside the router. ports IDs are coded counter-clockwise from ids 1 to 4 (1 pointing downwards) in Mesh cluster, in the Torus Cluster it is coded similarley and for RingStar, ports going from port i to star is 10+i+routerID and other ports are similar to previouse clusters. DHCP ports are code with ID 555 and PC ports are subnet ID + PC ID.

4. Forwarding Table: router decides to send each incoming packet according to this table. each router sends the routing table when it is updated (OSPF) or RIP timeout has exceeded (RIP). when a router has received a forwarding table, it updates its own table according to new routes discovered and if any new routes has better metrics than previous ones or has a new destination, router knows that the routing table is updated and it should send the routing table again. eBGP and iBGP are implemented in this class. if router receives a routing table from an external route, it labels new routes as eBGP routes and prefers them over iBGP routes. also ip versions are converted if route destinations are not compatible.

5. Routing Protocol: simplified iBGP and eBGP are implemented. router forwards packets on internal routes simply by choosing the best metric provided. this metric is route delay (OSPF) or hop count (RIP) for iBGP routes and AS count for eBGP routes. routers add AS ID to a route when it has come from an external route. by checking the as path of a route, router knows if any route has looped back into the AS and refeuses to recognize any paths with loop. for iBGP, loops are not prevented and any path length over 15 is discarded and therefore loops don't make packets stormed over the network. OSPF prevents loops by memorizing the path when routing table is shared and similar to eBGP, router prevents loop by recognizing its own ID in the path.

   RIP and OSPF are implemented in similar fashion: all routing tables get advertise for all ports except the ports that the route is received from. this is quite different from real OSPF implementations which send link state messages to other routers, but since we have a perfect network and delays are always accurate, which ever route has less delay will get transfered to router sooner and other routes that contain the same destination will arive later with worse metrics and will not cause the table to get updated and sent again. this approach creates a Dijkstra algorithm for sharing routing tables. metric for OSPF is delay in this project since throughput and baseband does not have a meaning in this context.

    RIP routes on the other hand, can arrive later and have better metrics since the metric is hop count. this will make the routing table update more than once for a single destination, each new better route being a new itaration it creates a Bellman-Ford algorithm and RIP will converge slower than OSPF. RIP routes also have a timeout and if the route does not arive again after the period, it is considered as an invalid route and gets removed from routing table.

#### DHCP Server
  each cluster has its own DHCP server connected to first router. DHCP server is given an IP addressing range and can only assign IPs in that range to PCs. DHCP communication is done in following steps:
  1. DHCP Request: a PC sends request to DHCP server. since the routing tables are not configured yet, this message gets broad casted over the subnet and reaches the DHCP Server. server then checks the table of assigned IPs. if there is no IP corresponding to the ID and there server is not out of IPs, it assigns a new IP address to the IP and creates a DHCP Offer.
  2. DHCP Offer: after assigning new IP, DHCP server then creates a DHCP Offer and broadcasts it back to PC. all PCs receive the new IP address but only the PC with correct ID gets to set its IP address to new IP.
  3. DHCP Lease: after maintaining the IP, each PC should send a lease packet to DHCP server so server knows that PC is still connected. if no lease arrives after a certain amount of time, server decides that the PC is disconnected and therefore removes the assigned IP to create more space for new PCs.

#### Cluster Types
clusters have 3 types according to assignement:
1. Mesh: mesh cluster has an x and y which indicates the rows and columns in the structure.
2. RingStar: this cluster has a ring length for creating the ring part and a vector indicating that which ring routers are connected to star.
3. Torus: inherits from mesh cluster with some extraa connections.
     
### Messaging System:
  in each clock tick, this system generate a Poisson distirbuted random number and two uniform random numbers. first is the number of packets to be generated and latter are sender PC ID and destination ID. packets are generated sent to network and network sends the packets from PCs. 'lambda' for Poission distribution can be changed from GUI.

### Event Handler:
  is a simple object that creates clock ticks in each timestamp for a limited duration. both clock tick duration and simulation duration can be configured from the GUI.

  Each PC is connected to a router. upon connecting to network, PC first acquires an IP address from DHCP server and then sends a Hello packet to the router. router recongnizes the PC with this packet and updates its own routing table and PC gets advertized over the network.

# *GUI*
GUI is pretty straight forward: you enter the number of ticks, duration of each tick in microseconds, lambda and select the routing algorithm. then you hit the start button and program gets executed.

![image](https://github.com/ArianFiroozi/CN_CHomeworks_3/assets/126232660/01dd5070-cead-465c-90eb-9960bf14f5b0)

