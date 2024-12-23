#include "../headers/messagingSystem.h"

int MessagingSystem::numOfPackets() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::poisson_distribution<int> poisson(lambda);

    return poisson(generator);
}

MessagingSystem::MessagingSystem(int lambda, const QVector<PC *> &receivers, const QVector<PC *> &senders) : lambda(lambda),
    receivers(receivers),
    senders(senders)
{}

QVector<QSharedPointer<Packet>> MessagingSystem::generatePackets() {
    packets = QVector<QSharedPointer<Packet>> ();
    int messageNum = numOfPackets();

    for (int i=0;i<messageNum;i++)
    {
        int src = rand() % senders.size();
        int dest = rand() % receivers.size();
        if (receivers[dest]->hasIP() && senders[src]->hasIP())
            packets.append(QSharedPointer<Packet> (new Packet("test", MSG_PACKET, IPV4, *(IPv4*)(senders[src]->ip), *(IPv4*)(receivers[dest]->ip))));
    }

    return packets;
}
