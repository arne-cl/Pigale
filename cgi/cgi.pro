TEMPLATE  = app

VERSION=$$cat(../version)

DESTDIR=../bin
DISTPATH = $$(HOME)/pigaledist/pigale-$${VERSION}-binaries

win32 {             
      DEFINES +=  __MINGW32__ _WIN32 
      LIBS +=  $$(QTDIR)/lib/libQt5Network.a $$(QTDIR)/lib/libQt5Core.a
      }
QT       += core network sql
QT       -= gui
TARGET    = pigale.cgi
CONFIG   += console
CONFIG   -= app_bundle

SOURCES  += main.cpp \
    client.cpp
INCLUDEPATH = ../incl
DEPENDPATH = ../incl

HEADERS += \
    client.h

macx:CONFIG+=sdk_no_version_check

#Installation
target.path =  $$DISTPATH
INSTALLS += target

message(Release:$$VERSION pigale.cgi)
