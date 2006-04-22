TEMPLATE = app

win32 {
      ENABLE_STATIC=yes
      MQTDIR=C:\Qt\4.1.2\
      MODE = release
      ENABLE_STATIC=yes
      DISTPATH=g:\pigale4\Pigale
      DESTDIR=$$DISTPATH/bin
      MOC_DIR = moc
      DEFINES += _WIN32
      win32:LIBS += -Wl,-rpath ../glut -L../glut -l glut32 
      message(configuring for Windows in $$DISTPATH)
      } else {
      include(../pigale.inc)
      unix:LIBS +=$$LIBGLUT
      MOC_DIR = .moc
      DESTDIR=$$DISTPATH/bin
      message(configuring for Linux in $$DESTDIR)
      }


QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
INCLUDEPATH = ../incl
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

CONFIG += qt thread $$MODE

CONFIG(debug, debug|release) {
    TARGET = pigale_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    win32:OBJECTS_DIR = ./odb
    message(qt: $$QMAKE_CXXFLAGS_DEBUG)
    contains(ENABLE_STATIC,"yes")  {
          unix:LIBS += $$DISTPATH/lib/libtgraph_debug.a
          win32:LIBS +=../tgraph/release/libtgraph.a 
          }  else  {
          unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph_debug
          win32:LIBS += -Wl,-rpath ../tgraph/release -L../tgraph/$$Mode -l tgraph 
          }
    } else {
    TARGET = pigale
    unix:OBJECTS_DIR = ./.opt
    win32:OBJECTS_DIR = ./opt
    message(qt: $$QMAKE_CXXFLAGS_RELEASE)
    contains(ENABLE_STATIC,"yes")  {
        unix:LIBS += $$DISTPATH/lib/libtgraph.a
        win32:LIBS +=../tgraph/release/libtgraph.a 
        } else  {
        unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph
        win32:LIBS += -Wl,-rpath ../tgraph/$$release -L../tgraph/release -l tgraph 
        }
    }

QT += qt3support opengl network xml

unix {
      DESTDIR=.
      # awk
      awk.depends = ../incl/QT/Action_def.h FORCE
      awk.commands = $$AWK -f Action.awk ../incl/QT/Action_def.h > ../incl/QT/Action.h
      QMAKE_EXTRA_TARGETS += awk
      # Translations
      TRANSLATIONS    = pigale_fr.ts 
      translation.commands  =  $$MQTDIR/bin/lrelease pigale.pro
      QMAKE_EXTRA_TARGETS += translation
      # needed by ../makefile
      distdir.commands =
      QMAKE_EXTRA_TARGETS += distdir
      # Installation
      translations.files = pigale_fr.qm qt_fr.qm
      translations.path = $$DISTPATH/translations
      INSTALLS += translations
      # Distribution
      DISTDIR=..
      DISTFILES += gnumakefile
      depends += awk
      message(extra target $$QMAKE_EXTRA_TARGETS) 
}


contains(ENABLE_STATIC,"yes") {
  message(creating $$TARGET (static) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
  #message(using libraries:$$LIBS)
} else {
  message(creating $$TARGET (dynamic) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
  #message(using libraries:$$LIBS)
}
