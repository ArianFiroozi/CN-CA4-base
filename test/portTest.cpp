#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/port.h"

using namespace std;

QString string_transfer_between_ports_correct()
{
    int dummy_argc = 0;
    char x = 'b';
    char *dummy_argv[1] = {&x};
    QCoreApplication dummy(dummy_argc, dummy_argv);

    Packet myPack("hello world");

    Port a(1), b(2);
    QThread bthread;
    b.moveToThread(&bthread);
    bthread.start();

    QObject::connect(&a, &Port::getPacket, &b, &Port::read);
    a.write(myPack);
    bthread.wait(10);

    Packet bPack(b.packet.getString());

    bthread.exit();
    bthread.wait(10);
    bthread.deleteLater();

    if (bPack.getString() != myPack.getString())
        return "string does not transfer between ports correctly";
    return "";
}

QVector<QString> run_port_tests()
{
    QVector<QString> errors;

    errors += string_transfer_between_ports_correct();

    return errors;
}
