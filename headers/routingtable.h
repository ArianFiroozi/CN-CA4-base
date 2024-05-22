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
    IP* dest;
    Mask mask;
    IP* gateway;
    Port* port;
    int metric; //not implemented
    void print()
    {
        cout << dest->getIPStr().toStdString() << " via " << gateway->getIPStr().toStdString() << endl;
    }
};

class RoutingTable
{
    QVector<Route> routes;
public:
    RoutingTable();

    void addRoute(Route newRoute);
    QVector<Route> findAllRoutes(IP* ip);
    Route findBestRoute(IP* ip); //based on metric
    void print();
};

#endif // ROUTINGTABLE_H
