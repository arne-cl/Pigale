TEMPLATE = app

VERSION=$$cat(../version)
INCLUDEPATH = ../incl
SOURCES = main.cpp

win32 {
      win32-msvc2010:DEFINES += _CRT_SECURE_NO_WARNINGS
      }
      
CONFIG += debug_and_release
CONFIG -= qt 
CONFIG += console
macx:CONFIG+=sdk_no_version_check
macx:CONFIG-=app_bundle   

CONFIG(debug, debug|release)  {
       LIBTGRAPH=libtgraph_debug.a
       win32-msvc2010:LIBTGRAPH=tgraph_debug.lib 
       TARGET=usingtgraph_debug
       OBJECTS_DIR = ./debug
    }else { 
       LIBTGRAPH=libtgraph.a
       win32-msvc2010:LIBTGRAPH=tgraph.lib 
       TARGET=usingtgraph    
       OBJECTS_DIR = ./release
}

LIBS += ../lib/$$LIBTGRAPH



message($$TARGET QT:$$QT_VERSION)


