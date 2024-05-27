#ifndef PORT_H
#define PORT_H

#include <iostream>
#include <memory>

#include <QThread>
#include <QCoreApplication>

#include "packet.h"

class Router;

class Port : public QObject
{
    Q_OBJECT

private:
public:
    Port(int id);
    ~Port();

    QSharedPointer<Packet> packet;
    int id;

    void write(QSharedPointer<Packet> _packet);
    void read(QSharedPointer<Packet> _packet);

signals:
    void getPacket(QSharedPointer<Packet>);
};

#endif // PORT_H
