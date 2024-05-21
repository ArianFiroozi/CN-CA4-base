#include <iostream>
#include <stdio.h>

#include "./headers/packet.h"
#include "./headers/ip.h"

using namespace std;

bool packet_string_correct()
{
    Packet myPacket("hello world", IPV4, IPv4("255.255.255.252", "192.168.10.141"), IPv4("255.255.255.252", "192.168.13.110"));
    if(myPacket.toStr() != "IPv4 192.168.10.141/30 192.168.13.110/30 hello world")
        return false;
    return true;
}

int run_packet_tests()
{
    int errors = 0;

    errors += (packet_string_correct()==true)?0:1;

    return errors;
}
