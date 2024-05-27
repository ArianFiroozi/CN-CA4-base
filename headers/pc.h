#ifndef PC_H
#define PC_H

#include <QThread>

#include "packet.h"
#include "port.h"
#include "ip.h"

class PC : public QObject
{
    Q_OBJECT
public:
    explicit PC(int _id, IP* _ip, Port* _port,  QObject *parent = nullptr);
    IP* ip;
    int id;
    Port* port;
    QVector<QSharedPointer<Packet>> buffer;

    void recievePacket(QSharedPointer<Packet> packet);
    void sendPacket(QSharedPointer<Packet> packet);

signals:
    void packetReceived();
    void packetSent(Port* newPort);
};

#endif // PC_H
