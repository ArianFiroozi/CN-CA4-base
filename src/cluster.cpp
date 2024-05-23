#include "../headers/cluster.h"

Mesh::Mesh(int _x, int _y, IPv4 netAddrIP)
{
    x = _x;
    y = _y;

    for(int i=0;i<y;i++)
    {
        for (int j=0;j<x;j++)
        {
            routers.append(new Router(i*j+j+1));

            IPv4 newIP(netAddrIP.mask, netAddrIP.ipAddr);
            newIP.ipAddr.hostID += i*j + j + 1;

            routers.last()->ip = new IPv4(netAddrIP.mask, netAddrIP.ipAddr);


            //only down can send to up TODO:make it both ways for dynamic
            //connecting network
            if (j==0)
            {
                routers.last()->addPort(new Port(2));
                if (i != y-1)
                    routers.last()->addPort(new Port(3));

                if (i != 0)
                    QObject::connect(routers[i*(j-1)+j]->getPortsWithID(3)[0], &Port::getPacket, routers.last(), &Router::recievePacket);
            }
            else if (j==x-1)
            {
                if (i != y-1)
                    routers.last()->addPort(new Port(3));

                QObject::connect(routers[i*j+j-1]->getPortsWithID(2)[0], &Port::getPacket, routers.last(), &Router::recievePacket);
                if (i != 0)
                    QObject::connect(routers[i*(j-1)+j]->getPortsWithID(3)[0], &Port::getPacket, routers.last(), &Router::recievePacket);
            }
            else
            {
                routers.last()->addPort(new Port(2));
                if (i != y-1)
                    routers.last()->addPort(new Port(3));

                QObject::connect(routers[i*j+j-1]->getPortsWithID(2)[0], &Port::getPacket, routers.last(), &Router::recievePacket);
                if (i != 0)
                    QObject::connect(routers[i*(j-1)+j]->getPortsWithID(3)[0], &Port::getPacket, routers.last(), &Router::recievePacket);
            }

            //TODO: get tables according to static/dynamic type
        }
    }
}
