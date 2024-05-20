#include "routingtable.h"

RoutingTable::RoutingTable() {}

void RoutingTable::addRoute(Route newRoute)
{
    routes.append(newRoute);
}

QVector<Route> RoutingTable::findRoutes(IP* ip)
{
    QVector<Route> compatibleRoutes;
    for (Route route : routes)
        if (route.dest->includes(ip))
            compatibleRoutes.append(route);
    return compatibleRoutes;
}
