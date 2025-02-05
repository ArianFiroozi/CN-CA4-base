#ifndef NETWORK_H
#define NETWORK_H

#include "./headers/eventHandler.h"
#include "./headers/cluster.h"
#include "./headers/pc.h"

#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "../headers/eventHandler.h"
#include "../headers/messagingSystem.h"
#include "dhcp.h"

class Network : public QObject
{
    Q_OBJECT
private:
    QThread* eventThread;

    EventHandler* eventHandler;
    RingStar* ringStar;
    Mesh* mesh;
    Torus* torus;
    MessagingSystem* messagingSystem;

    bool running;
    int packetsSent, packetsReceived, packetsDropped;
    int highestQueueWait, highestWait, leastWait, leastQueueWait;
    int totalWaitCycles, totalQueueWaitCycles;

    void packetReceived(QSharedPointer<Packet> packet);
    void createSenders();
    void createReceivers();
    void addMeshPorts();
    void addRingStarPorts();
    void addTorusPorts();
    void connectRingStar();
    void connectMesh();
    void connectTorus();
    void packetSent();
    void packetDropped();
    
    void connectPcTick();
    QVector<QSharedPointer<Packet>> packets;
    
public:
    QVector<PC*> senders, receivers;

    Network(EventHandler* _eventHandler, RoutingProtocol protocol, int lambda=DEFAULT_LAMBDA);
    ~Network();

    void start();
    void tick(double time);
    void stop();
    void printRoutingTables();

    // getters
    int getPacketsSent() const;
    int getPacketsReceived() const;
    int getTotalWaitCycles() const;
    int getTotalQueueWaitCycles() const;
    int getPacketsDropped() const;
    int getHighestQueueWait() const;
    int getHighestWait() const;
    int getLeastWait() const;
    int getLeastQueueWait() const;

signals:
    void done();
    void oneCycleFinished(double time);
};

#endif // NETWORK_H
