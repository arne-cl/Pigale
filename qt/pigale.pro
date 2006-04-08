include(../pigale.inc)
TEMPLATE = app
INCLUDEPATH = ../incl;../incl/TAXI;../incl/QT 
DEPENDPATH = ../incl
HEADERS = pigaleWindow.h \
    ClientSocket.h \
    glcontrolwidget.h \
    gprop.h \
    GraphGL.h \
    GraphSym.h \
    GraphWidget.h \
    LineEditNum.h \
    mouse_actions.h 

SOURCES = main.cpp \
    CanvasGrid.cpp \
    CanvasItem.cpp \
    CanvasSpring.cpp \
    ClientSocket.cpp \
    EmbedCurves.cpp \
    glcontrolwidget.cpp \
    gprop.cpp \
    GraphGL.cpp \
    graphml.cpp \
    graphmlparser.cpp \
    GraphSym.cpp \
    Handler.cpp \
    LineEditNum.cpp \
    Macro.cpp \
    MiscQt.cpp \
    mouse_actions.cpp \
    pigaleCanvas.cpp \
    pigaleFile.cpp \
    pigaleMenus.cpp \
    pigalePaint.cpp \
    pigaleUndo.cpp \
    pigaleWindow.cpp \
    Settings.cpp \
    Test.cpp


CONFIG += qt thread $$MODE warn_off
CONFIG(debug, debug|release)  {
    TARGET = pigale_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    contains(ENABLE_STATIC,"yes")   {
      unix:LIBS += $$DISTPATH/lib/libtgraph_debug.a
      }else {
      unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph_debug
      }
    }else {
    TARGET = pigale
    unix:OBJECTS_DIR = ./.opt
    contains(ENABLE_STATIC,"yes") 
      {
      unix:LIBS += $$DISTPATH/lib/libtgraph.a
      }else {
      unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph
      }
    }
unix:LIBS +=$$LIBGLUT
QT += qt3support opengl network xml
MOC_DIR = .moc

#awk
awk.depends = ../incl/QT/Action_def.h FORCE
awk.commands = $$AWK -f Action.awk ../incl/QT/Action_def.h > ../incl/QT/Action.h
QMAKE_EXTRA_TARGETS += awk


#Translations
TRANSLATIONS    = pigale_fr.ts 
translation.commands  =  $$MQTDIR/bin/lrelease pigale.pro
QMAKE_EXTRA_TARGETS += translation

#needed by ../makefile
distdir.commands =
QMAKE_EXTRA_TARGETS += distdir

#Installation
DESTDIR=$$DISTPATH/bin
translations.files = pigale_fr.qm qt_fr.qm
translations.path = $$DISTPATH/translations
#INSTALLS += target translations
INSTALLS += translations

#Distribution
DISTDIR=..
DISTFILES += gnumakefile


depends += awk
contains(ENABLE_STATIC,"yes") {
  message(creating $$TARGET (static) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
}else {
  message(creating $$TARGET (dynamic) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
}