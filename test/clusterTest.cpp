#include <iostream>
#include <stdio.h>
#include <QVector>
#include <QThread>

#include "./headers/cluster.h"

using namespace std;

QString mesh_cluster_static_init_correct()
{
    Mesh cluster(4, 4, IPv4("255.255.255.252", "20.0.0.0"));
    cluster.printRoutingTables();
    return "";
}

QVector<QString> run_cluster_tests()
{
    QVector<QString> errors;

    errors += mesh_cluster_static_init_correct();

    return errors;
}
