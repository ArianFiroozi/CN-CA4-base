#ifndef ALLTESTS_H
#define ALLTESTS_H

#include <QString>

QVector<QString> run_all_tests();
QVector<QString> run_ip_tests();
QVector<QString> run_packet_tests();
QVector<QString> run_port_tests();
QVector<QString> run_routing_table_tests();
QVector<QString> run_router_tests();
QVector<QString> run_cluster_tests();
QVector<QString> run_event_handler_tests();

#endif // ALLTESTS_H
