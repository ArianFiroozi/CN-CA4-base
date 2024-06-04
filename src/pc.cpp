#include "./headers/pc.h"

using namespace std;

PC::PC(int _id, IP* _ip, Port* _port, QObject *parent)
    : QObject{parent}
{
    id = _id;
    ip = _ip;
    port = _port;
    dhcp = false;
    ipSet = true;
}

PC::PC(int _id, Port *_port, QObject *parent)
{
    id = _id;
    port = _port;
    dhcp = true;
    ipSet = false;
}

void PC::start()
{
    if (dhcp)
        sendDhcpRequest();
    else
        sendHello();
}

void PC::recievePacket(QSharedPointer<Packet> packet)
{
    if (packet->getType() == MSG)
    {

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
    else if (packet->getType() == DHCP_OFFER)
    {
        if (ipSet) return; // ignore broadcasts
        if (packet->getString().split(",")[1].toInt() != id) return;

        ip = new IPv4("255.255.255.255", packet->getString().split(",")[0]);
        qDebug() << "pc" << id << "new ip is:" << ip->getIPStr();
        ipSet = true;
        sendHello();
    }
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

void PC::sendDhcpRequest()
{
    qDebug() << "sending req";
    QSharedPointer<Packet> packet(new Packet(QString::number(id), DHCP_REQUEST, IPV4, IPv4("255.255.255.255", "255.255.255.255"),
                                             IPv4("255.255.255.255", "255.255.255.255")));
    port->write(packet);
}

bool PC::hasIP()
{
    return ipSet;
}
