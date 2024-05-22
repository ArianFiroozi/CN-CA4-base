#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/packet.h"
#include "./headers/router.h"

using namespace std;

QString routing_table_finds_path_correctly()
{
    Packet myPack("hello world", IPV4, IPv4("255.255.255.252", "192.168.20.1"), IPv4("255.255.255.252", "20.0.0.1"));

    Port a1(1);
    Router a(1), b(2);
    a.routingTable.addRoute(Route(OSPF, myPack.getDest(), myPack.getSource().mask,
                                  myPack.getSource(), &a1));
    Route found = a.routingTable.findBestRoute(myPack.getDest());

    if (found.dest.getIPStr() != "20.0.0.1/30"
        || found.gateway.getIPStr() != "192.168.20.1/30")
        return "router cannot find path";
    return "";
}

QString router_sends_packet_to_another()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    Packet myPack("hello world", IPV4, IPv4("255.255.255.252", "192.168.20.1"), IPv4("255.255.255.252", "20.0.0.1"));

    Port a1(1);
    Router a(1), b(2);
    a.routingTable.addRoute(Route(OSPF, myPack.getDest(), myPack.getSource().mask,
                                  myPack.getSource(), &a1));
    QThread bthread;
    b.moveToThread(&bthread);
    bthread.start();

    //connect port send to router rec
    QObject::connect(&a1, &Port::getPacket, &b, &Router::recievePacket);

    a.sendPacket(myPack);
    bthread.wait(10);

    Packet bPack = b.buffer[0];

    bthread.exit();
    bthread.wait(10);
    bthread.deleteLater();

    if (bPack.getString() != myPack.getString())
        return "string does not transfer between routers correctly";
    return "";
}

QVector<QString> run_router_tests()
{
    QVector<QString> errors;

    errors += routing_table_finds_path_correctly();
    errors += router_sends_packet_to_another();

    return errors;
}
