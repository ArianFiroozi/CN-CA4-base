#ifndef PC_H
#define PC_H

#include "packet.h"
#include "port.h"
#include "ip.h"

class PC : public QObject
{
    Q_OBJECT
public:
    PC(int _id, IP* _ip, Port* _port);
    IP* ip;
    int id;
    Port* port;
    QVector<Packet> buffer;

    void recievePacket(Packet packet);
    void sendPacket(Packet packet);

signals:
    void packetSent(Port* newPort);
};

#endif // PC_H
