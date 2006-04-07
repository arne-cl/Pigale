include(../pigale.inc)
TEMPLATE = app
INCLUDEPATH = ../incl;../incl/TAXI;../incl/QT 
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
    unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph_debug
#    unix:LIBS += $$DISTPATH/lib/libtgraph_debug.a
    }else {
    TARGET = pigale
    unix:OBJECTS_DIR = ./.opt
    unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph
#    unix:LIBS += $$DISTPATH/lib/libtgraph.a
    }
unix:LIBS +=$$LIBGLUT
QT += qt3support opengl network xml
MOC_DIR = .moc

#Translations
TRANSLATIONS    = pigale_fr.ts 
translation.commands  =  $$MQTDIR/bin/lrelease pigale.pro
QMAKE_EXTRA_TARGETS += translation
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

message(creating $$TARGET using QT version $$[QT_VERSION] ($$OBJECTS_DIR))
