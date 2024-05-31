#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>
#include <unistd.h>

#include "./headers/messagingSystem.h"
#include "../headers/network.h"

using namespace std;

QString generate_random_poisson()
{
    Network net(new EventHandler(10), OSPF);
    MessagingSystem ms(5, net.receivers, net.senders);

    for (int i=0;i<10;i++)
    {
        ms.generatePackets();
        // for (Packet* packet: ms.generatePackets())
        //     qDebug() << packet->toStr();
    }

    return "";
}

QVector<QString> run_messaging_system_tests()
{
    QVector<QString> errors;

    errors += generate_random_poisson(); // tested manually

    return errors;
}
