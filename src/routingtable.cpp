#include "./headers/routingtable.h"

RoutingTable::RoutingTable() {}

void RoutingTable::addRoute(Route newRoute)
{
    routes.append(newRoute);
}

QVector<Route> RoutingTable::findAllRoutes(IP* ip)
{
    QVector<Route> compatibleRoutes;
    for (Route route : routes)
        if (route.dest->includes(ip))
            compatibleRoutes.append(route);
    return compatibleRoutes;
}

Route RoutingTable::findBestRoute(IP* ip)
{
    QVector<Route> maxRoutes = findAllRoutes(ip);
    int maxMetric = -1;
    for (Route route : maxRoutes)
        if (route.metric > maxMetric)
            maxMetric = route.metric;

    for (Route route : maxRoutes)
        if (route.metric == maxMetric)
            return route;

    return maxRoutes[0];
}

void RoutingTable::print()
{
    for (Route route : routes)
        route.print();
}
