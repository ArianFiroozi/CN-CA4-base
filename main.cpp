#include "./headers/networksimulator.h"
#include "./test/ipTest.h"

#include <iostream>
#include <QApplication>

using namespace std;

int main(int argc, char *argv[])
{
    if(run_ip_tests() != 0)
    {
        std::cout<<"failed tests!"<<std::endl;
        exit(0);
    }

    // QApplication a(argc, argv);
    // NetworkSimulator w;
    // w.show();
    // return a.exec();
}
