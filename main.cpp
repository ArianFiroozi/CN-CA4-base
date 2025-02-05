#include <iostream>
#include <QApplication>
#include <QString>
#include <QVector>

#include "./headers/networkSimulator.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    NetworkSimulator networkSimulator;
    networkSimulator.show();

    return application.exec();
}
