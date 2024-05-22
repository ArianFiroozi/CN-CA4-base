QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/ip.cpp \
    src/networksimulator.cpp \
    src/packet.cpp \
    src/port.cpp \
    src/router.cpp \
    src/routingtable.cpp \
    test/allTests.cpp \
    test/ipTest.cpp \
    test/packetTest.cpp \
    test/portTest.cpp

HEADERS += \
    headers/ip.h \
    headers/networksimulator.h \
    headers/packet.h \
    headers/port.h \
    headers/router.h \
    headers/routingtable.h \
    test/allTests.h \
    test/errorsStr.h

FORMS += \
    networksimulator.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
