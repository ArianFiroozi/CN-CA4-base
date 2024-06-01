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
    Q_OBJECT
private:
    QThread* eventThread;

    EventHandler* eventHandler;
    RingStar* ringStar;
    Mesh* mesh;
    MessagingSystem* messagingSystem;
    bool running;
    int packetsSent;
    int packetsReceived;
    int totalWaitCycles, totalQueueWaitCycles;

    void packetReceived(QSharedPointer<Packet> packet);
    void createSenders();
    void createReceivers();
    void addMeshPorts();
    void connectRingStar();
    void connectMesh();
    void packetSent();

public:
    QVector<PC*> senders, receivers;

    Network(EventHandler* _eventHandler, RoutingProtocol protocol, int lambda=0);
    ~Network();

    void start();
    void tick(double time);

    void stop();
    int getPacketsSent() const;
    int getPacketsReceived() const;

    int getTotalWaitCycles() const;
    int getTotalQueueWaitCycles() const;

signals:
    void done();
    void oneCycleFinished(double time);
};

#endif // NETWORK_H
