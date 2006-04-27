TEMPLATE = app

win32 {
      ENABLE_STATIC=yes
      MQTDIR=C:\Qt\4.1.2\
      MODE = release
      ENABLE_STATIC=yes
      DISTPATH=g:\pigale\Pigale
      DESTDIR=$$DISTPATH/bin
      MOC_DIR = moc
      DEFINES += _WIN32
      } else {
      include(../pigale.inc)
      MOC_DIR = .moc
      QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
      DESTDIR=$$DISTPATH/bin
      }



INCLUDEPATH = ../incl
DEPENDPATH = ../incl

HEADERS = client.h 
SOURCES = client.cpp

CONFIG += qt thread $$MODE

CONFIG(debug, debug|release)  {
    TARGET = client_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    win32:OBJECTS_DIR = ./odb
    #message(client: $$QMAKE_CXXFLAGS_DEBUG)
    }else {
    TARGET = client
    unix:OBJECTS_DIR = ./.opt
    win32:OBJECTS_DIR = ./opt
    #message(client: $$QMAKE_CXXFLAGS_RELEASE)
    }


distdir.commands =
QMAKE_EXTRA_TARGETS += distdir
 
QT += qt3support network 

unix {
      DESTDIR=.
      #Installation
      exec.files=$$TARGET
      exec.path = $$DISTPATH/bin
      INSTALLS += exec
}      

message(configuring $$TARGET using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
