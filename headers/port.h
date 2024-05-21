#ifndef PORT_H
#define PORT_H

#include <iostream>
#include <memory>

#include <QThread>
#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "packet.h"

class Router;

class Port : public QObject
{
    Q_OBJECT
private:
    QMutex mutex;
    QWaitCondition ready;

public:
    Port(QObject *parent = nullptr);
    ~Port();

    Packet packet;

    void write(Packet _packet);
    void read(QString _packet);

signals:
    void getPacket(QString);
};

#endif // PORT_H
