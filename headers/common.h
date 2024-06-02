#ifndef COMMON_H
#define COMMON_H

#include <QString>

const int RIP_TIMEOUT = 1800;
const int RIP_SEND_ROUTING_TABLE = 300;
const int DEFAULT_LAMBDA = 5;
const int DEFAULT_CYCLE_COUNT = 100;
const int DEFAULT_CYCLE_DURATION = 10;
const int DEFAULT_EVENT_HANDLER_STOP = 1000000;
const int MESSAGING_SYSTEM_SEND_PERIOD = 10;
const int RIP_MAX_HOP = 15;

const QString MESH_DELAY_PATH ("../resources/delays/mesh4x4/portDelays.csv");
const QString MESH_ROUTING_TABLE_PATH ("../resources/routingTables/manualMesh4x4/routingTable");
const QString RING_STAR_DELAY_PATH("../resources/delays/ringStar/portDelays.csv");
const QString RING_STAR_ROUTING_TABLE_PATH("../resources/routingTables/manualRingStar/routingTable");

#endif // COMMON_H
