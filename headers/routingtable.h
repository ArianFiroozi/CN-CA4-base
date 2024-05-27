#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <QVector>
#include <iostream>

#include "ip.h"
#include "port.h"

using namespace std;


struct Route
{
    IPv4 dest;
    Mask mask;
    IPv4 gateway;
    Port* port;
    int metric; // RIP: hops

    Route(IPv4 dest, const Mask &mask, IPv4 gateway, Port *port, int _metric = -1);

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
    Route findBestRoute(IPv4 ip); //based on metric

    void print();
    void initFromFile(QString address);
    void initFromFile(QString address, Port* port);
    void updateFromPacketRIP(QString msg, Port* port);
    QString toStringRIP(IPv4 gateway);
};

#endif // ROUTINGTABLE_H
