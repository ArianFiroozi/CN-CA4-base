#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <QVector>
#include <iostream>

#include "ip.h"
#include "port.h"

using namespace std;

enum RoutingProtocol
{
    BGP,
    OSPF
};

struct Route
{
    RoutingProtocol protocol;
    IPv4 dest;
    Mask mask;
    IPv4 gateway;
    Port* port;
    int metric; //not implemented

    Route(RoutingProtocol protocol, IPv4 dest, const Mask &mask, IPv4 gateway, Port *port);

    void print()
    {
        cout << dest.getIPStr().toStdString() << " via " << gateway.getIPStr().toStdString() << endl;
    }
};

class RoutingTable
{
public:
    RoutingTable();

    QVector<Route> routes;
    void addRoute(Route newRoute);
    QVector<Route> findAllRoutes(IPv4 ip);
    Route findBestRoute(IPv4 ip); //based on

    void print();
    void initFromFile(QString address);
};

#endif // ROUTINGTABLE_H
