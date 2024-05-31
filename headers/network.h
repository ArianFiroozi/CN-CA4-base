#ifndef NETWORK_H
#define NETWORK_H

#include "./headers/eventHandler.h"
#include "./headers/cluster.h"
#include "./headers/pc.h"

#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include <../headers/eventHandler.h>
#include <../headers/messagingSystem.h>

class Network : public QObject
{
private:
    QThread* eventThread;

    EventHandler* eventHandler;
    RingStar* ringStar;
    Mesh* mesh;
    MessagingSystem* messagingSystem;
    bool running;

    void createSenders();
    void createReceivers();
    void addMeshPorts();
    void connectRingStar();
    void connectMesh();

public:
    QVector<PC*> senders, receivers;

    Network(EventHandler* _eventHandler, RoutingProtocol protocol, int lambda=0);

    void start();
    void tick(double time);

    void stop();
signals:
    void done();
};

#endif // NETWORK_H
