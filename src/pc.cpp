#include "./headers/pc.h"

using namespace std;

PC::PC(int _id, IP* _ip, Port* _port, QThread *parent)
    : QThread{parent}
{
    id = _id;
    ip = _ip;
    port = _port;
}

void PC::recievePacket(Packet packet)
{
    buffer.append(packet);

    QString path;
    for (auto routerID: packet.getPath())
    {
        path.append(routerID);
        path.append("  ");
    }

    cout<<"pc "<<id<<" recieved msg: "<<packet.getString().toStdString()
         <<" through path: "<< path.toStdString() <<endl;
}

void PC::sendPacket(Packet packet)
{
    port->write(packet);
}
