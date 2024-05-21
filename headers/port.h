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
    Port(QObject *parent = nullptr);
    ~Port();

    Packet packet;

    void write(Packet _packet);
    void read(Packet _packet);

signals:
    void getPacket(Packet);
};

#endif // PORT_H
