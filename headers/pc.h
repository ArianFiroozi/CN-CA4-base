#ifndef PC_H
#define PC_H

#include <QThread>

#include "packet.h"
#include "port.h"
#include "ip.h"
#include "common.h"
#include "eventHandler.h"

class PC : public QObject
{
    Q_OBJECT
public:
    explicit PC(int _id, IP* _ip, Port* _port, IPVersion _ipVer=IPV4, QObject *parent = nullptr);
    explicit PC(int _id, Port* _port, IPVersion _ipVer=IPV4, QObject *parent = nullptr);
    IP* ip;
    int id;
    Port* port;
    QVector<QSharedPointer<Packet>> buffer;

    void recievePacket(QSharedPointer<Packet> packet);
    void sendPacket(QSharedPointer<Packet> packet);
    void start();
    bool hasIP();
    void connectTick(EventHandler* eventHandler);
    void addPacket(QSharedPointer<Packet> packet);
    void sendTcpPackets();
    void writePacketsToFile();

signals:
    void packetReceived(QSharedPointer<Packet> packet);
    void packetSent();
private:
    IPVersion ipVer;
    int clk;
    int lastDhcpRq;
    bool dhcp;
    bool ipSet;
    void sendHello();
    void sendDhcpRequest();
    void tick(int time);
    void sendLease();
    void processPacket(QSharedPointer<Packet> packet);
    QVector<QSharedPointer<Packet>> packets;
    QVector<QSharedPointer<Packet>> received_packets;
};

#endif // PC_H
