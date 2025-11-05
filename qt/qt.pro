TEMPLATE = app


VERSION=$$cat(../version)

win32 {
      DEFINES +=  __MINGW32__ _WIN32
      win32-msvc2010:DEFINES += _CRT_SECURE_NO_WARNINGS
      LIBS +=  $$(QTDIR)/lib/libQt5Network.a $$(QTDIR)/lib/libQt5Core.a $$(QTDIR)/lib/libQt5Gui.a $$(QTDIR)/lib/libQt5Widgets.a
      LIBS += $$(QTDIR)/lib/libQt5OpenGl.a $$(QTDIR)/lib/libQt5PrintSupport.a $$(QTDIR)/lib/libQt5Svg.a
      LIBS += $$(QTDIR)/lib/libQt5DBus.a $$(QTDIR)/lib/libQt5Xml.a $$(QTDIR)/lib/libQt5VirtualKeyboard.a
      DEFINES+= STATIC
      } 
DESTDIR=../bin
DISTPATH = $$(HOME)/pigaledist/pigale-$${VERSION}-binaries
win32:DISTPATH = $$(HOME)/pigale-$${VERSION}-binaries
MOC_DIR = moc
INCLUDEPATH += ../incl
DEPENDPATH =  ../incl

#files that need moc
HEADERS = pigaleWindow.h \
    ClientSocket.h \
    glcontrolwidget.h \
    gprop.h \
    GraphGL.h \
    GraphSym.h \
    GraphWidget.h \
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

CONFIG += debug_and_release
CONFIG += qt thread 
CONFIF += static
macx:CONFIG+=sdk_no_version_check

CONFIG(debug, debug|release) {
       LIBTGRAPH=libtgraph_debug.a
       LIBGLUT=libfreeglut_debug.a
       win32-msvc2010:LIBTGRAPH=tgraph_debug.lib 
       win32-msvc2010:LIBGLUT=freeglut_debug.lib 
       }  else  {
       LIBTGRAPH=libtgraph.a
       LIBGLUT=libfreeglut.a
       win32-msvc2010:LIBTGRAPH=tgraph.lib 
       win32-msvc2010:LIBGLUT=freeglut.lib 
       }
win32:CONFIG+= static        
CONFIG(debug, debug|release) {
    TARGET = pigale_debug
    DEFINES += TDEBUG
    OBJECTS_DIR = ./debug
    } else {
    TARGET = pigale
    OBJECTS_DIR = ./release
    }
    
macx {   
     LIBS += -framework GLUT
     LIBS += -dead_strip
     DEFINES += GL_SILENCE_DEPRECATION
     } else {
     DEFINES += FREEGLUT FREEGLUT_STATIC
     unix:LIBS += ../lib//$$LIBGLUT
     win32:LIBS +=../lib/$$LIBGLUT -lopengl32 -lglu32 -lgdi32 -luser32 -lwinmm
     }
     
QT += opengl network xml svg printsupport widgets
LIBS += ../lib/$$LIBTGRAPH    
win32:RC_FILE=pigale.rc

macx {   
          RC_FILE = pigale.icns
          #BUNDLE
          man.path =  Contents/Resources/
          man.files = ../Doc/manual.html ../Doc/style.css  pigale_fr.qm qt_fr.qm
          icon.path = Contents/Resources/icones
          icon.files += ../Doc/icones/misspost.png
          icon.files += ../Doc/icones/sleft.png
          icon.files += ../Doc/icones/sright.png
          icon.files += ../Doc/icones/sleftarrow.png
          icon.files += ../Doc/icones/srightarrow.png
          icon.files += ../Doc/icones/reload.png
          icon.files += ../Doc/icones/sreload.png
          icon.files += ../Doc/icones/filesave.png
          icon.files += ../Doc/icones/xman.png
          icon.files += ../Doc/icones/help.png
          icon.files += ../Doc/icones/macroplay.png
          icon.files += ../Doc/icones/sfilesave.png
          QMAKE_BUNDLE_DATA  += man icon
      	# awk
      	awk.target = ../incl/QT/Action.h
      	awk.depends = ../incl/QT/Action_def.h Action.awk
      	awk.commands = gawk -f Action.awk ../incl/QT/Action_def.h > ../incl/QT/Action.h
      	QMAKE_EXTRA_TARGETS += awk

      	# Translations     
      	#TRANSLATIONS    = $$DESTDIR/pigale_fr.ts     	
      	#translation.target = $$TRANSLATIONS
      	#translation.depends = $$HEADERS $$SOURCES
      	#translation.commands  =  $(QTDIR)/bin/lupdate qt.pro
      	#translationUp.target = pigale_fr.qm 
      	#translationUp.depends = $$TRANSLATIONS
      	#translationUp.commands  = $(QTDIR)/bin/lrelease qt.pro
      	#QMAKE_EXTRA_TARGETS += translation translationUp        
	}

   # Installation
      macx:target.path =  $$DISTPATH
      win32:target.path =  $$DISTPATH/bin
      translations.files = pigale_fr.qm qt_fr.qm
      translations.path = $$DISTPATH/translations
      doc.files = ..\\Doc\\*.html ..\\Doc\\*.css
      doc.path = $$DISTPATH/Doc
      ico.files = ../Doc/icones\\*.png ../Doc/icones\\*.gif
      ico.path  = $$DISTPATH/Doc/icones
      tgf.files = ../tgf\\*.*
      tgf.path = $$DISTPATH/tgf
      macro.files = ../macro/maxPlanar4Connex.mc
      macro.path = $$DISTPATH/macro
      images.files = ../images/*.png
      images.path = $$DISTPATH/images
      lib.path = $$DISTPATH/lib
      lib.files = ../lib/*
      read.path = $$DISTPATH  
      osx:read.files = ../README_OSX.txt 
      win32:read.files = ../README_WINDOWS.txt 
      macx:deploy.path =  $$DISTPATH	
      macx:deploy.commands  = $(QTDIR)/bin/macdeployqt $${DISTPATH}/$${TARGET}.app
      win32:deploy.path =  $$DISTPATH/bin
      win32:deploy.commands  = $(QTDIR)/bin/windeployqt.exe  $${DISTPATH}/bin/$${TARGET}.exe
      INSTALLS += target macro images  doc ico tgf read deploy
 

message($$TARGET QT:$$QT_VERSION dist:$$DISTPATH qtdir:$$QTDIR)
