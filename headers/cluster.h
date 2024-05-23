#ifndef CLUSTER_H
#define CLUSTER_H

#include "router.h"

#include <QVector>

class Cluster
{
public:
    QVector<Router*> routers;
};

class Mesh:public Cluster
{
private:
    int x, y;
public:
    Mesh(int _x, int _y, IPv4 netAddIP);
};

class RingStar:public Cluster
{
private:
public:
    RingStar();
};

#endif // CLUSTER_H
