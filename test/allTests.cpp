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
    errors += run_routing_table_tests();
    errors += run_router_tests();
    errors += run_cluster_tests();
    errors += run_event_handler_tests();
    errors += run_protocol_tests();
    errors += run_network_tests();

    QMutableVectorIterator<QString> i(errors);
    while (i.hasNext())
    {
        if (i.next() == "")
            i.remove();
    }
    return errors;
}
