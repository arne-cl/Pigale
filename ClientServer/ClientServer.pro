TEMPLATE = app

VERSION=$$cat(../version)
DESTDIR=../bin
DISTPATH = $$(HOME)/pigaledist/pigale-$${VERSION}-binaries
win32:DISTPATH = $$(HOME)/pigale-$${VERSION}-binaries

MOC_DIR = moc
INCLUDEPATH = ../incl
DEPENDPATH = ../incl
HEADERS = client.h 
SOURCES = client.cpp
CONFIG += debug_and_release
CONFIG += qt thread 
macx:CONFIG+=sdk_no_version_check

QT += network widgets
   
CONFIG(debug, debug|release)  {
    TARGET = client_debug
    DEFINES += TDEBUG
    OBJECTS_DIR = ./debug
    }else {
    TARGET = client
    unix:OBJECTS_DIR = ./release
    }
 
#Installation
macx:target.path =  $$DISTPATH
win32:target.path =  $$DISTPATH/bin
data.files =ClientData.txt
data.path = $$DISTPATH
macx:deploy.path =  $$DISTPATH	
macx:deploy.commands  = $(QTDIR)/bin/macdeployqt $$DISTPATH/$${TARGET}.app
win32:deploy.path =  $$DISTPATH/bin	
win32:deploy.commands  = $(QTDIR)/bin/windeployqt.exe $${DISTPATH}/bin/$${TARGET}.exe
INSTALLS += target data deploy

message($$TARGET QT:$$QT_VERSION dist:$$DISTPATH )
