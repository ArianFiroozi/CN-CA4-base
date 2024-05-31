#ifndef NETWORK_H
#define NETWORK_H

#include "./headers/eventHandler.h"
#include "./headers/cluster.h"
#include "./headers/pc.h"

#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

class Network
{
private:
    QThread* eventThread;

    EventHandler* eventHandler;
    RingStar* ringStar;
    Mesh* mesh;

    void createSenders();
    void createReceivers();

    void addMeshPorts();

    void connectRingStar();

    void connectMesh();

public:
    QVector<PC*> senders, receivers;

    Network(EventHandler* _eventHandler, RoutingProtocol protocol);

    void start();

signals:
    void done();
};

#endif // NETWORK_H
