#include "../headers/cluster.h"

void Mesh::connectRouters(int i, int j)
{
    //only down can send to up TODO:make it both ways for dynamic
    //connecting network
    if (j==0)
    {
        routers.last()->addPort(new Port(2));
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        if (i != 0)
            QObject::connect(routers[(i-1)*j+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket);
    }
    else if (j==x-1)
    {
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2), &Port::getPacket,
                         routers.last(), &Router::recievePacket);
        if (i != 0)
            QObject::connect(routers[(i-1)*j+j]->getPortWithID(3), &Port::getPacket,
                             routers.last(), &Router::recievePacket);
    }
    else
    {
        routers.last()->addPort(new Port(2));
        if (i != y-1)
            routers.last()->addPort(new Port(3));

        QObject::connect(routers[i*x+j-1]->getPortWithID(2),
                         &Port::getPacket, routers.last(), &Router::recievePacket);
        if (i != 0)
            QObject::connect(routers[(i-1)*j+j]->getPortWithID(3),
                             &Port::getPacket, routers.last(), &Router::recievePacket);
    }
}

void Mesh::getStaticRoutingTables()
{
    for (Router* router:routers) //static
    {
        QString path("/home/arian/Documents/CN_P3/CN_CHomeworks_3/resources/routingTables/manualMesh4x4/routingTable");
        path.append(QString::number(router->id));
        path.append(".csv");

        for (Port* port : router->ports)
            router->routingTable.initFromFile(path, port);
    }
}

Mesh::Mesh(int _x, int _y, IPv4 netAddrIP) // in this mesh, unlike final version each up router is connected to one pc
{
    x = _x;
    y = _y;

    //this is a test. later this should be done with PC entity
    QVector<QString> upPcIPs;
    upPcIPs.append("192.168.20.1");
    upPcIPs.append("192.168.20.2");
    upPcIPs.append("192.168.20.3");
    upPcIPs.append("192.168.20.4");

    for(int i=0;i<y;i++)
    {
        for (int j=0;j<x;j++)
        {
            routers.append(new Router(i*x+j+1));

            IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
            newIP.ipAddr.hostID += i*x + j + 1;
            routers.last()->ip = new IPv4(netAddrIP.mask, netAddrIP.ipAddr);
            connectRouters(i, j);
        }
    }


    //TODO: get tables according to static/dynamic type
    getStaticRoutingTables();
}

void Cluster::printRoutingTables()
{
    for (Router* router:routers)
    {
        cout << endl << "routing table " << router->id << ":" << endl;
        router->routingTable.print();
    }
}
