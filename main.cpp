#include "networksimulator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkSimulator w;
    w.show();
    return a.exec();
}
