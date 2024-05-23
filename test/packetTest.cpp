#include <iostream>
#include <stdio.h>
#include <QVector>

#include "./headers/packet.h"
#include "./headers/ip.h"

using namespace std;

QString packet_string_correct()
{
    Packet myPacket("hello world", MSG, IPV4, IPv4("255.255.255.252", "192.168.10.141"), IPv4("255.255.255.252", "192.168.13.110"));
    if(myPacket.toStr() != "IPv4 192.168.10.141/30 192.168.13.110/30 hello world")
        return "packet to string wrong format!";
    return "";
}

QVector<QString> run_packet_tests()
{
    QVector<QString> errors;

    errors += packet_string_correct();

    return errors;
}
