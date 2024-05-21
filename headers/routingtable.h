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
    Port* link;
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
    QVector<Route> findRoutes(IP* ip);
    void print();
};

#endif // ROUTINGTABLE_H
