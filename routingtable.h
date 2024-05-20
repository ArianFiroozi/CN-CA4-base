#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <QVector>

#include "ip.h"
#include "link.h"

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
    Link* link;
    int metric; //not implemented
};

class RoutingTable
{
    QVector<Route> routes;
public:
    RoutingTable();
    void addRoute(Route newRoute);
    QVector<Route> findRoutes(IP* ip);
};

#endif // ROUTINGTABLE_H
