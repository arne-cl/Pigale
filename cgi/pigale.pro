TEMPLATE = app

win32 {
      MQTDIR = c:/Qt/4.1.4
      MOC_DIR = moc
      QMAKE_CXXFLAGS_WARN_ON =  -Wall 
      QMAKE_CXXFLAGS += frtti -fexceptions 
      DEFINES +=  __MINGW32__ _WIN32 -DUNICODE -DQT_LARGEFILE_SUPPORT
      CONFIG += console
      INCLUDEPATH = c:/pigale/1.3.4/incl
      INCLUDEPATH += $$MQTDIR/include/QtCore
      INCLUDEPATH += $$MQTDIR/include/QtNetwork
      INCLUDEPATH += $$MQTDIR/include/
      LIBS +=  $$MQTDIR/lib/libQtNetwork4.a $$MQTDIR/lib/libQtCore4.a
      DESTDIR=/www/cgi-bin
      } else {
      MQTDIR = /usr/local/Trolltech/Qt-4.1.4
      MOC_DIR = .moc
      QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
      LIBS +=  $$MQTDIR/lib/libQtNetwork.a $$MQTDIR/lib/libQtCore.a
      LIBS += -lz -lm  -ldl
      INCLUDEPATH = /home/pigale/pigale/incl
      INCLUDEPATH += $$MQTDIR/include/QtCore
      INCLUDEPATH += $$MQTDIR/include/QtNetwork
      INCLUDEPATH += $$MQTDIR/include/
      DESTDIR=/var/www/cgi-bin
      }

CONFIG =  thread release moc
DEFINES += QT_NO_DEBUG QT_NETWORK_LIB QT_CORE_LIB

SOURCES =  client.cpp\
	pigale.cpp

HEADERS = client.h 

TARGET=pigale.cgi
MOC_DIR = ./.moc
OBJECTS_DIR = ./.opt
unix:DESTDIR=/var/www/cgi-bin
win32:DESTDIR=/www/cgi-bin