TEMPLATE = app

win32 {
      include(..\wpigale.inc)
      MOC_DIR = moc
      OBJECTS_DIR = opt    
      QMAKE_CXXFLAGS_WARN_ON =  -Wall 
      QMAKE_CXXFLAGS += frtti -fexceptions 
      DEFINES +=  __MINGW32__ _WIN32 -DUNICODE -DQT_LARGEFILE_SUPPORT
      CONFIG += console
      LIBS +=  $$MQTDIR/lib/libQtNetwork4.a $$MQTDIR/lib/libQtCore4.a
      } else {
      include(../pigale.inc)
      MOC_DIR = .moc
      OBJECTS_DIR = .opt   
      QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
#static      LIBS +=  $$MQTDIR/lib/libQtNetwork.a $$MQTDIR/lib/libQtCore.a
      LIBS +=  $$MQTDIR/lib/libQtNetwork.so $$MQTDIR/lib/libQtCore.so
      LIBS += -lz -lm  -ldl
      }

INCLUDEPATH += $$MQTDIR/include/QtCore
INCLUDEPATH += $$MQTDIR/include/QtNetwork
INCLUDEPATH += $$MQTDIR/include/
INCLUDEPATH += ../incl
DEPENDPATH += ../incl

CONFIG =   thread release moc
DEFINES += QT_NO_DEBUG QT_NETWORK_LIB QT_CORE_LIB

SOURCES =  client.cpp\
	pigale.cpp

HEADERS = client.h 

CONFIG =  thread release moc
TARGET=pigale.cgi
unix:DESTDIR=.
#unix:DESTDIR=/var/www/cgi-bin
win32:DESTDIR=.
message(configuring pigale.cgi using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
