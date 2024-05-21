#include "./headers/networksimulator.h"
#include "./test/ipTest.h"

#include <iostream>
#include <QApplication>

#include "./headers/port.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(run_all_tests() != 0)
    {
        cout<<"failed tests!"<<endl;
        exit(0);
    }
    else
        cout<<"tests passed!"<<endl;
    // QApplication a(argc, argv);
    // NetworkSimulator w;
    // w.show();
    // return a.exec();
}
