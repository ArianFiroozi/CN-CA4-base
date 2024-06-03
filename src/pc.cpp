#include "./headers/pc.h"

using namespace std;

PC::PC(int _id, IP* _ip, Port* _port, QObject *parent)
    : QObject{parent}
{
    id = _id;
    ip = _ip;
    port = _port;
}

void PC::start()
{
    sendHello();
}

void PC::recievePacket(QSharedPointer<Packet> packet)
{
    if (packet->getType() != MSG)
        return;

    buffer.append(packet);

    QString path;
    for (auto routerID: packet->getPath())
    {
        path.append(routerID);
        path.append("  ");
    }

    qDebug() <<"pc "<<id<<" recieved msg: "<<packet->getString().toStdString()
             <<" with queue waiting: " << packet->getQueueWaitCycles() << ", total wait: "
             << packet->getWaitCycles() <<" through path: "<< path.toStdString();

    emit packetReceived(packet);
}

void PC::sendPacket(QSharedPointer<Packet> packet)
{
    port->write(packet);
    emit packetSent();
}

void PC::sendHello()
{
    QSharedPointer<Packet> packet(new Packet("0", HELLO, IPV4, IPv4(ip->mask, ip->ipAddr), IPv4(ip->mask, ip->ipAddr)));
    port->write(packet);
}
