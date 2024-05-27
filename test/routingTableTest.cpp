#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/routingtable.h"

    using namespace std;

QString routing_table_finds_path_correctly()
{
    Packet myPack("hello world", MSG, IPV4, IPv4("255.255.255.252", "192.168.20.1"), IPv4("255.255.255.252", "20.0.0.1"));

    Port a1(1);
    RoutingTable routingTable(new IPv4());
    routingTable.addRoute(Route(myPack.getDest(), myPack.getSource().mask,
                                  myPack.getSource(), &a1));
    Route found = routingTable.findBestRoute(myPack.getDest());

    if (found.dest.getIPStr() != "20.0.0.1/30"
        || found.gateway.getIPStr() != "192.168.20.1/30")
        return "router cannot find path";
    return "";
}

QString routing_table_init_correct()
{
    RoutingTable rt(new IPv4());
    rt.initFromFile("../resources/routingTables/routingTable1.csv");
    if (rt.routes[0].gateway.getIPStr()!="192.168.20.1/30" ||
        rt.routes[0].port == NULL ||
        rt.routes[0].dest.getIPStr()!="20.0.0.1/30")
        return "file read wrong!";
    return "";
}

QVector<QString> run_routing_table_tests()
{
    QVector<QString> errors;

    errors += routing_table_finds_path_correctly();
    errors += routing_table_init_correct();

    return errors;
}
