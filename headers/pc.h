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
    explicit PC(int _id, IP* _ip, Port* _port,  QObject *parent = nullptr);
    explicit PC(int _id, Port* _port,  QObject *parent = nullptr);
    IP* ip;
    int id;
    Port* port;
    QVector<QSharedPointer<Packet>> buffer;

    void recievePacket(QSharedPointer<Packet> packet);
    void sendPacket(QSharedPointer<Packet> packet);
    void start();
    bool hasIP();
    void connectTick(EventHandler* eventHandler);

signals:
    void packetReceived(QSharedPointer<Packet> packet);
    void packetSent();
private:
    int clk;
    int lastDhcpRq;
    bool dhcp;
    bool ipSet;
    void sendHello();
    void sendDhcpRequest();
    void tick(int time);
    void sendLease();
};

#endif // PC_H
