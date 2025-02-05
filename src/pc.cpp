#include "./headers/pc.h"
#include <QFile>

using namespace std;

PC::PC(int _id, IP* _ip, Port* _port, IPVersion _ipVer, QObject *parent)
    : QObject{parent}
{
    ipVer = _ipVer;
    id = _id;
    ip = _ip;
    port = _port;
    dhcp = false;
    ipSet = true;
}

PC::PC(int _id, Port *_port, IPVersion _ipVer, QObject *parent)
    : QObject{parent}
{
    ipVer = _ipVer;
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
    if (packet->getType() == MSG_PACKET)
    {

        buffer.append(packet);

        QString path;
        for (auto routerID: packet->getPath())
        {
            path.append(routerID);
            path.append("  ");
        }
        processPacket(packet);
        // qDebug() <<"pc "<<id<<" recieved msg: "<<packet->getString().toStdString()
        //          <<" with queue waiting: " << packet->getQueueWaitCycles() << ", total wait: "
        //          << packet->getWaitCycles() <<" through path: "<< path.toStdString();
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
    packet->ipVer = ipVer;
    port->write(packet);
    emit packetSent();
}

void PC::sendHello()
{
    QSharedPointer<Packet> packet(new Packet("0", HELLO, IPV6, IPv4(ip->mask, ip->ipAddr), IPv4(ip->mask, ip->ipAddr)));
    port->write(packet);
}

void PC::sendDhcpRequest()
{
    qDebug() << "sending DHCP req";
    QSharedPointer<Packet> packet(new Packet(QString::number(id), DHCP_REQUEST, IPV4, IPv4("255.255.255.255", "255.255.255.255"),
                                             IPv4("255.255.255.255", "255.255.255.255")));
    port->write(packet);
    lastDhcpRq = clk;
}

bool PC::hasIP()
{
    return ipSet;
}

void PC::connectTick(EventHandler *eventHandler)
{
    QObject::connect(eventHandler, &EventHandler::tick,
                     this, &PC::tick, Qt::ConnectionType::QueuedConnection);
}

void PC::addPacket(QSharedPointer<Packet> packet)
{
    packets.append(packet);
}

void PC::sendTcpPackets()
{

    //TODO: send appropriate packets
    for (const QSharedPointer<Packet> &packet:packets){
        if (not ip) continue;
        if (ip->ipAddr.addrToNum() == packet->getSource().ipAddr.addrToNum())
        {
            sendPacket(packet);
            break;
        }
    }
}

void PC::tick(int time)
{
    clk = time;
    if (!hasIP()&&time > lastDhcpRq + 100)
        sendDhcpRequest();
    if (hasIP() && time % SEND_LEASE == 0)
        sendLease();

    if (((int)time%MESSAGING_SYSTEM_SEND_PERIOD)==0 && time!=0) {
        sendTcpPackets();
    }
}

void PC::sendLease()
{
    QSharedPointer<Packet> packet(new Packet(QString::number(id), DHCP_LEASE, IPV4, IPv4("255.255.255.255", "255.255.255.255"),
                                             IPv4("255.255.255.255", "255.255.255.255")));
    port->write(packet);
}

void PC::processPacket(QSharedPointer<Packet> packet)
{
    bool isNotReceivedBefore = true;
    for (const auto& p : received_packets) {
        if (p->getFileSeqNum()== packet->getFileSeqNum()) {
            isNotReceivedBefore = false;
            break;
        }
    }

    if (isNotReceivedBefore) {
        // std::cout<<"pc received"<<std::endl;
        received_packets.append(packet);
    }
}

void PC::writePacketsToFile()
{
    if (!received_packets.length()) return;
    std::sort(received_packets.begin(), received_packets.end(),
              [](const QSharedPointer<Packet>& a, const QSharedPointer<Packet>& b) {
                  return a->getFileSeqNum()< b->getFileSeqNum();
              });

    QString filename = QString("%1receiver_output.mp3").arg(OUTPUT_FILE_PATH, id);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Could not open file for writing");
        return;
    }

    for (const auto& packet : received_packets) {
        QByteArray byteArray = packet->getPayload();
        file.write(byteArray);
    }
    cout<<"file saved!\n";
    file.close();
}
