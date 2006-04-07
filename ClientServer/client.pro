include(../pigale.inc)
TEMPLATE = app
INCLUDEPATH = ../incl;../incl/TAXI;../incl/QT 
HEADERS = client.h 
SOURCES = client.cpp

CONFIG += qt thread $$MODE
CONFIG(debug, debug|release)  {
    TARGET = client_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    }else {
    TARGET = client
    unix:OBJECTS_DIR = ./.opt
    }
MOC_DIR = .moc
distdir.commands =
QMAKE_EXTRA_TARGETS += distdir
 
QT += qt3support network 
DESTDIR=$$DISTPATH/bin
#Installation
#target.files=$$TARGET
#target.path = $$DISTPATH/bin
#INSTALLS += target 

message(creating $$TARGET using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
