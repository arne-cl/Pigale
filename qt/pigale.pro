TEMPLATE = app

win32 {
      include(../wpigale.inc)
      MOC_DIR = moc
      QMAKE_CXXFLAGS_WARN_ON =  -Wall 
      DEFINES +=  __MINGW32__ _WIN32
      DEFINES += GLUT_NO_LIB_PRAGMA	
      } else {
      include(../pigale.inc)
      MOC_DIR = .moc
      QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer
      DESTDIR=$$DISTPATH/bin
      }

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
    #message(qt: $$QMAKE_CXXFLAGS_DEBUG)
    contains(ENABLE_STATIC,"yes")  {
          unix:LIBS += $$DISTPATH/lib/libtgraph_debug.a
          win32:LIBS +=../tgraph/libtgraph_debug.a 
          }  else  {
          unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph_debug
          win32:LIBS += -Wl,-rpath ../tgraph -L../tgraph -l tgraph_debug 
          }
    } else {
    TARGET = pigale
    unix:OBJECTS_DIR = ./.opt
    win32:OBJECTS_DIR = ./opt
    #message(qt: $$QMAKE_CXXFLAGS_RELEASE)
    contains(ENABLE_STATIC,"yes")  {
        unix:LIBS += $$DISTPATH/lib/libtgraph.a
        win32:LIBS +=../tgraph/libtgraph.a 
        } else  {
        unix:LIBS += -Wl,-rpath $$DISTPATH/lib -L$$DISTPATH/lib -l tgraph
        win32:LIBS += -Wl,-rpath ../tgraph/ -L../tgraph/ -l tgraph 
        }
    }


DEFINES += FREEGLUT FREEGLUT_STATIC
unix:LIBS += $$DISTPATH/lib/libglut.a
win32:LIBS +=../freeglut/libglut.a -lopengl32 -lglu32 -lgdi32 -luser32 -lwinmm

QT += qt3support opengl network xml
DESTDIR = .
unix {
      # awk
      awk.depends = ../incl/QT/Action_def.h
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
      target.path =  $$DISTPATH/bin
      INSTALLS += target translations
      # Distribution
      DISTDIR=..
      DISTFILES += gnumakefile
      PRE_TARGETDEPS = awk
}
win32 {
      # Installation
      target.path =  $$DISTPATH/bin
      translations.files = pigale_fr.qm qt_fr.qm
      translations.path = $$DISTPATH/translations
      doc.files = ../Doc/*.html ../Doc/*.css 
      doc.path = $$DISTPATH/Doc
      ico.files = ../Doc/icones/*.png ../Doc/icones/*.gif
      ico.path  = $$DISTPATH/Doc/icones
      tgf.files = ../tgf/*.*
      tgf.path = $$DISTPATH/tgf
      macro.files = ../macro/*.*
      macro.path = $$DISTPATH/macro
      INSTALLS += target translations doc ico tgf macro
      contains(DLL,"yes") {
           dll.files = $$MQTDIR/bin/QtCore4.dll $$MQTDIR/bin/QtGui4.dll $$MQTDIR/bin/Qt3Support4.dll 
           dll.files += $$MQTDIR/bin/QtNetwork4.dll $$MQTDIR/bin/QtOpenGL4.dll $$MQTDIR/bin/QtXml4.dll
           dll.files += $$MQTDIR/bin/QtSql4.dll $$MQTDIR/bin/QtSvg4.dll
           dll.files += $$MINGW//bin/mingwm10.dll
           dll.path = $$DISTPATH/bin  
           INSTALLS += dll
      }
 
}

contains(ENABLE_STATIC,"yes") {
  message(configuring $$TARGET (static) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
} else {
  message(configuring $$TARGET (dynamic) using QT version $$[QT_VERSION] ($$OBJECTS_DIR ))
}
