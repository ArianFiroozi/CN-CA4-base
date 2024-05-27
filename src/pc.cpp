#include "./headers/pc.h"

using namespace std;

PC::PC(int _id, IP* _ip, Port* _port, QObject *parent)
    : QObject{parent}
{
    id = _id;
    ip = _ip;
    port = _port;
}

void PC::recievePacket(QSharedPointer<Packet> packet)
{
    buffer.append(packet);

    QString path;
    for (auto routerID: packet->getPath())
    {
        path.append(routerID);
        path.append("  ");
    }

    cout<<"pc "<<id<<" recieved msg: "<<packet->getString().toStdString()
         <<" with latency: " << packet->getWaitCycles() <<" through path: "<< path.toStdString() <<endl;
    emit packetReceived();
}

void PC::sendPacket(QSharedPointer<Packet> packet)
{
    port->write(packet);
}
