#include "./test/allTests.h"

#include <iostream>
#include <QApplication>
#include <QString>
#include <QVector>

#include "./headers/port.h"

using namespace std;

int main(int argc, char *argv[])
{
    QVector<QString> errStrs = run_all_tests();
    if(errStrs.size() != 0)
    {
        cout<<"failed tests:"<<endl;
        for (QString errStr:errStrs)
            cerr<<errStr.toStdString()<<endl;
        exit(0);
    }
    else
        cout<<"tests passed!"<<endl;
    // QApplication a(argc, argv);
    // NetworkSimulator w;
    // w.show();
    // return a.exec();
}
