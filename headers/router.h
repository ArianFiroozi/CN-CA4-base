#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QSharedPointer>

#include "packet.h"
#include "port.h"
#include "ip.h"
#include "routingtable.h"
#include "common.h"

enum RoutingProtocol
{
    RIP,
    OSPF,
    MANUAL
};

struct WaitingQueueLine
{
    Port* port;
    QSharedPointer<Packet> packet;
    int delay, queueTime;
    WaitingQueueLine(Port *port, QSharedPointer<Packet> packet, int delay, int queueTime);
};

class Router : public QThread
{
    Q_OBJECT
private:
    IPVersion ipVer;
    bool sendTable;
    void forwardTable();
    int clk;
    QVector<WaitingQueueLine> waitingQueue;
    int bufferSize;

    void getHello(QSharedPointer<Packet> packet);

public:
    explicit Router(int _id, IPv4* _ip, RoutingProtocol _protocol = MANUAL, IPVersion=IPV4, int _bufferSize=DEFAULT_BUFFER_SIZE, QThread *parent = nullptr);
    IPv4* ip;
    IPv6* ip6;
    int id;
    QVector<Port*> ports;
    QVector<QSharedPointer<Packet>> buffer;
    RoutingTable routingTable;
    RoutingProtocol protocol;

    void recievePacket(QSharedPointer<Packet> packet);
    bool sendPacket(QSharedPointer<Packet> packet);
    void forward();
    void sendWaiting();
    void tick(int _time);

    void start();
    void stop();
    void queuePacket(Packet packet);
    void addPort(Port* port);

    Port* getPortWithID(int portID);
signals:
    void packetSent(Port* newPort);
    void packetDropped();
};

#endif // ROUTER_H
