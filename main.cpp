#include "./test/allTests.h"

#include <iostream>
#include <QApplication>
#include <QString>
#include <QVector>

#include "./headers/networkSimulator.h"

using namespace std;

void test()
{
    QVector<QString> errStrs = run_all_tests();
    if(errStrs.size() != 0)
    {
        cout<<"failed tests:"<<endl;
        for (const QString &errStr:errStrs)
            cerr<<errStr.toStdString()<<endl;
        exit(0);
    }
    else
        cout<<"tests passed!"<<endl;
}

int main(int argc, char *argv[])
{
    // test();

    QApplication application(argc, argv);

    // usleep(100000);
    NetworkSimulator networkSimulator;
    networkSimulator.show();

    return application.exec();
}
