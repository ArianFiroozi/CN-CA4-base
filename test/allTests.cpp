#include <iostream>
#include <stdio.h>
#include <QVector>

#include "allTests.h"

using namespace std;

QVector<QString> run_all_tests()
{
    QVector<QString> errors;

    errors += run_ip_tests();
    errors += run_packet_tests();
    errors += run_port_tests();

    QMutableVectorIterator<QString> i(errors);
    while (i.hasNext()) {
        if (i.next() == "")
            i.remove();
    }
    return errors;
}
