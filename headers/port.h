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
    QMutex mutex;
public:
    Port(int id, int _delay=0);
    ~Port();

    QSharedPointer<Packet> packet;
    int id;
    int delay;

    void write(QSharedPointer<Packet> _packet);
    void read(QSharedPointer<Packet> _packet);

    void setDelay(int newDelay);

signals:
    void getPacket(QSharedPointer<Packet>);
};

#endif // PORT_H
