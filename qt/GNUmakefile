PIGALEPATH=../../pigale
# L = name of libray
L=qt
# P = prefix name of program
P=pigale_
OPT=opt
NVERSION=BETA

MQTDIR=$(QTDIR)
#MQTDIR=/usr/local/qt-2.3.2
MMOC=$(MQTDIR)/bin/moc
IFLAGS =  -I$(MQTDIR)/include 
IFLAGS += -I$(PIGALEPATH)/incl
#One can link with qt or qt-mt (multithread version of qt) 
#UNIXLIB =  -L$(MQTDIR)/lib -lqt-mt -L/usr/X11R6/lib -lglut -lGL -lGLU  
UNIXLIB   =  -L$(MQTDIR)/lib -lqt-mt -L/usr/local/lib -lglut -lGL -lGLU  
PIGALELIB =  -L$(PIGALEPATH)/lib  -ltgraph$(OPT)  -l$(L)$(OPT)
PIC=-fpic
CC=g++

#GENERAL OPTIONS
.SUFFIXES:.cpp .o
#OPTIMIZED OPTIONS
CFLAGSopt = -O2 -m486 -fstrength-reduce $(PIC)
CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith
CFLAGSopt +=  -fomit-frame-pointer -pipe
CFLAGSopt +=  -DQT -DVERSION_$(NVERSION)


#DEBUGGING OPTIONS
CFLAGSdbg  =  -g -Wall  $(PIC)
CFLAGSdbg +=  -Wstrict-prototypes -Wpointer-arith
CFLAGSdbg +=  -fomit-frame-pointer
CFLAGSdbg +=  -DTDEBUG
CFLAGSdbg +=  -DQT -DVERSION_$(NVERSION)


CFLAGS  = $(CFLAGS$(OPT))
VPATH   = $(PIGALEPATH)/tmp.$(OPT)
DLIB    =     $(PIGALEPATH)/lib/lib$L$(OPT).so
FILES  :=  $(wildcard *.cpp)
INCL   := $(wildcard *.h)
MOC_CPP = $(patsubst %.h,moc_%.cpp, $(INCL))
FILES_WO_MOC := $(filter-out $(MOC_CPP), $(FILES))
FILES_W_MOC  := $(FILES_WO_MOC)
FILES_W_MOC  += $(MOC_CPP)
OBJECTS       =  $(patsubst %.cpp,%.o, $(FILES_W_MOC))
OBJ           =  $(patsubst %.cpp,$(VPATH)/%.o, $(FILES_W_MOC))

all :   $(P)$(OPT) 

moc_%.cpp:%.h
	$(MMOC) $< -o $@

.cpp.o: $*.cpp
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $(VPATH)/$*.o   $*.cpp

$(DLIB):$(OBJECTS) 
	@rm -f $(PIGALEPATH)/lib/lib$L$(OPT).so
	@g++ -shared  -o $(PIGALEPATH)/lib/lib$L$(OPT).so  $(OBJ)

$(P)$(OPT) :$(DLIB) $(PIGALEPATH)/lib/libtgraph$(OPT).so 
	g++  $(CFLAGS) -o $(P)$(OPT) $(PIGALELIB)  $(UNIXLIB) 

clean:
	@rm -f $(VPATH)/*.o

test:
	@echo objects=$(OBJECTS)

moc_cpp:    $(MOC_CPP)

depend:
	@makedepend -o.o  -fGNUmakefile -I$(PIGALEPATH)/incl  -Y   $(FILES_WO_MOC)  2> /dev/null
#	@makedepend -o.o  -fGNUmakefile  -Y   *.cpp  2> /dev/null

nodepend:
	@makedepend -o.o  -fGNUmakefile -Y    2> /dev/null

# DO NOT DELETE

CanvasGrid.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasGrid.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
CanvasGrid.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tprop.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tstring.h
CanvasGrid.o: ../../pigale/incl/TAXI/propdef.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
CanvasGrid.o: GraphWidget.h ../../pigale/incl/QT/MyQcolors.h
CanvasGrid.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
CanvasGrid.o: ../../pigale/incl/QT/grid.h
CanvasGrid.o: ../../pigale/incl/QT/GraphWidgetPrivate.h mouse_actions.h
CanvasGrid.o: gprop.h
CanvasItem.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasItem.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasItem.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
CanvasItem.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
CanvasItem.o: ../../pigale/incl/TAXI/Tprop.h
CanvasItem.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasItem.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasItem.o: ../../pigale/incl/TAXI/Tstring.h
CanvasItem.o: ../../pigale/incl/TAXI/propdef.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
CanvasItem.o: GraphWidget.h ../../pigale/incl/QT/MyQcolors.h
CanvasItem.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
CanvasItem.o: ../../pigale/incl/QT/grid.h
CanvasItem.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
glcontrolwidget.o: glcontrolwidget.h
gprop.o: gprop.h MyWindow.h ../../pigale/incl/Pigale.h
gprop.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
gprop.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
gprop.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
gprop.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
gprop.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
gprop.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
gprop.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
gprop.o: ../../pigale/incl/QT/Misc.h
GraphGL.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphGL.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
GraphGL.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
GraphGL.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
GraphGL.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
GraphGL.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
GraphGL.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
GraphGL.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
GraphGL.o: GraphGL.h glcontrolwidget.h mouse_actions.h
GraphGL.o: ../../pigale/incl/QT/GLWindow.h ../../pigale/incl/QT/MyQcolors.h
GraphGL.o: ../../pigale/incl/TAXI/netcut.h
GraphSym.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphSym.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
GraphSym.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
GraphSym.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
GraphSym.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
GraphSym.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
GraphSym.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
GraphSym.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
GraphSym.o: GraphSym.h ../../pigale/incl/QT/SymWindow.h
GraphSym.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
Handler.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
Handler.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Handler.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Handler.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Handler.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Handler.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Handler.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Handler.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/QT/Handler.h
Macro.o: MyWindow.h ../../pigale/incl/Pigale.h
Macro.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Macro.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Macro.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Macro.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Macro.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Macro.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Macro.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Macro.o: GraphWidget.h mouse_actions.h ../../pigale/incl/QT/Misc.h
Macro.o: ../../pigale/incl/TAXI/Tgf.h
main.o: MyWindow.h ../../pigale/incl/Pigale.h
main.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
main.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
main.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
main.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
main.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
main.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
main.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
main.o: ../../pigale/incl/QT/Misc.h
MiscQt.o: MyWindow.h ../../pigale/incl/Pigale.h
MiscQt.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MiscQt.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
MiscQt.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
MiscQt.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
MiscQt.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
MiscQt.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
MiscQt.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
mouse_actions.o: mouse_actions.h GraphWidget.h GraphGL.h
MyCanvas.o: MyWindow.h ../../pigale/incl/Pigale.h
MyCanvas.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyCanvas.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
MyCanvas.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
MyCanvas.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
MyCanvas.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
MyCanvas.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
MyCanvas.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
MyCanvas.o: GraphWidget.h mouse_actions.h gprop.h
MyCanvas.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
MyCanvas.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
MyCanvas.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
MyPaint.o: MyWindow.h ../../pigale/incl/Pigale.h
MyPaint.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyPaint.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
MyPaint.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
MyPaint.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
MyPaint.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
MyPaint.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
MyPaint.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
MyPaint.o: ../../pigale/incl/QT/MyPaint.h ../../pigale/incl/QT/MyQcolors.h
MyWindow.o: MyWindow.h ../../pigale/incl/Pigale.h
MyWindow.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyWindow.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
MyWindow.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
MyWindow.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
MyWindow.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
MyWindow.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
MyWindow.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
MyWindow.o: ../../pigale/incl/TAXI/Tgf.h GraphWidget.h GraphGL.h GraphSym.h
MyWindow.o: mouse_actions.h gprop.h ../../pigale/incl/QT/MyWindow_doc.h
MyWindow.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/Handler.h
MyWindow.o: ../../pigale/incl/QT/MyPaint.h ../../pigale/incl/QT/MyCanvas.h
MyWindow.o: ../../pigale/incl/QT/grid.h icones/fileopen.xpm
MyWindow.o: icones/filenew.xpm icones/fileprint.xpm icones/filesave.xpm
MyWindow.o: icones/info.xpm icones/sleft.xpm icones/sright.xpm
MyWindow.o: icones/sreload.xpm icones/help.xpm icones/xman.xpm
MyWindow.o: icones/sleftarrow.xpm icones/srightarrow.xpm icones/sfilesave.xpm
MyWindow.o: icones/macroplay.xpm
SchaefferGen.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
SchaefferGen.o: ../../pigale/incl/TAXI/color.h
SchaefferGen.o: ../../pigale/incl/TAXI/graphs.h
SchaefferGen.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tprop.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tsvector.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tbase.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tdebug.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tpropio.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tstring.h
SchaefferGen.o: ../../pigale/incl/TAXI/propdef.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tpoint.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tfile.h
Settings.o: MyWindow.h ../../pigale/incl/Pigale.h
Settings.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Settings.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Settings.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Settings.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Settings.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Settings.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Settings.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Settings.o: mouse_actions.h gprop.h ../../pigale/incl/QT/Misc.h
Settings.o: ../../pigale/incl/QT/Handler.h
Test.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
Test.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Test.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Test.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Test.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Test.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Test.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Test.o: ../../pigale/incl/TAXI/Tfile.h
