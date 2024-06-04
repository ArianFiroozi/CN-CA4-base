QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/cluster.cpp \
    src/dhcp.cpp \
    src/eventHandler.cpp \
    src/ip.cpp \
    src/messagingSystem.cpp \
    src/network.cpp \
    src/networkSimulator.cpp \
    src/packet.cpp \
    src/pc.cpp \
    src/port.cpp \
    src/router.cpp \
    src/routingtable.cpp \
    test/allTests.cpp \
    test/clusterTest.cpp \
    test/eventHandlerTest.cpp \
    test/ipTest.cpp \
    test/messagingSystemTest.cpp \
    test/networkTest.cpp \
    test/packetTest.cpp \
    test/portTest.cpp \
    test/protocolTest.cpp \
    test/routerTest.cpp \
    test/routingTableTest.cpp

HEADERS += \
    headers/cluster.h \
    headers/common.h \
    headers/dhcp.h \
    headers/eventHandler.h \
    headers/ip.h \
    headers/messagingSystem.h \
    headers/network.h \
    headers/networkSimulator.h \
    headers/packet.h \
    headers/pc.h \
    headers/port.h \
    headers/router.h \
    headers/routingtable.h \
    test/allTests.h

FORMS += \
    networksimulator.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
        resources/delays/mesh4x4/portDelays.csv \
        resources/delays/ringStar/portDelays.csv \
        resources/delays/torus/portDelays.csv \
        resources/routingTables/manualMesh4x4/routingTable1.csv \
        resources/routingTables/manualMesh4x4/routingTable10.csv \
        resources/routingTables/manualMesh4x4/routingTable11.csv \
        resources/routingTables/manualMesh4x4/routingTable12.csv \
        resources/routingTables/manualMesh4x4/routingTable13.csv \
        resources/routingTables/manualMesh4x4/routingTable14.csv \
        resources/routingTables/manualMesh4x4/routingTable15.csv \
        resources/routingTables/manualMesh4x4/routingTable16.csv \
        resources/routingTables/manualMesh4x4/routingTable2.csv \
        resources/routingTables/manualMesh4x4/routingTable3.csv \
        resources/routingTables/manualMesh4x4/routingTable4.csv \
        resources/routingTables/manualMesh4x4/routingTable5.csv \
        resources/routingTables/manualMesh4x4/routingTable6.csv \
        resources/routingTables/manualMesh4x4/routingTable7.csv \
        resources/routingTables/manualMesh4x4/routingTable8.csv \
        resources/routingTables/manualMesh4x4/routingTable9.csv \
        resources/routingTables/manualRingStar/routingTable1.csv \
        resources/routingTables/manualRingStar/routingTable2.csv \
        resources/routingTables/manualRingStar/routingTable3.csv \
        resources/routingTables/manualRingStar/routingTable4.csv \
        resources/routingTables/manualRingStar/routingTable5.csv \
        resources/routingTables/manualRingStar/routingTable6.csv \
        resources/routingTables/manualRingStar/routingTable7.csv \
        resources/routingTables/manualRingStar/routingTable8.csv \
        resources/routingTables/routingTable1.csv \
        routingTable1.csv
