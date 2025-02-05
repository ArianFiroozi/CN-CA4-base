#include "../headers/messagingSystem.h"
#include <QFile>

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

QByteArray MessagingSystem::readFile(QString m_filePath)
{
    QFile file(m_filePath);

    if(!(file.open(QIODevice::ReadOnly)))
    {
        std::cout << "Unable to open file:" << m_filePath.toStdString() <<std::endl;
        // qFatal("Unable to open file");
        return {};
    }

    QByteArray data = file.readAll();
    file.close();

    return data;
}

QVector<QSharedPointer<Packet>> MessagingSystem::generatePackets() {
    packets = QVector<QSharedPointer<Packet>> ();

    QByteArray data = readFile(SONG_FILE_PATH);
    if (!data.length()) {
        std::cout<<"Couldn't read the file, aborting!\n";
        return packets;
    }

    int dest = 0;
    int src = 0;
    int fileSeqNum=0;
    std::cout<<data.length()<<std::endl;
    for(int i = 0; i < int(data.length()/5); i += 1024)
    {
        while (!(receivers[dest]->hasIP() && senders[src]->hasIP()))
            src = ++src%senders.length();

        QByteArray payload = data.mid(i, 1024);
        QSharedPointer<Packet> newPacket (new Packet(QString(payload), MSG_PACKET, IPV4, *(IPv4*)(senders[src]->ip),
                                                    *(IPv4*)(receivers[dest]->ip)));
        newPacket->setFileSeqNum(fileSeqNum++);
        senders[src]->addPacket(newPacket);
        packets.append(newPacket);
        src = ++src%senders.length();
    }
    std::cout<<packets.length();
    return packets;
}
