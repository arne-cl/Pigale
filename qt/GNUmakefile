####################### MAY NEED EDITING ########################
#Compiler options
CC=g++
CPU=
#CPU="-mcpu=i686"
#CPU=-m686 
#OPT: if set to dbg > debug version
#OPT: if set to opt > optimised version
OPT=opt
#PIC if wanted shared dynamic libraries
PIC=-fpic

#libray paths
MQTDIR=$(QTDIR)
#MQTDIR=/usr/local/qt-2.3.2
#One can link with qt or qt-mt (multithread version of qt) 
#UNIXLIB =  -L$(MQTDIR)/lib -lqt-mt -L/usr/X11R6/lib -lglut -lGL -lGLU  
UNIXLIB   =  -L$(MQTDIR)/lib -lqt-mt -L/usr/local/lib -lglut -lGL -lGLU 

#include paths
IFLAGS =  -I$(MQTDIR)/include 
IFLAGS += -I$(PIGALEPATH)/incl

#awk 
AWK=awk
####################### END EDITING #########################
PIGALEPATH=../../pigale
NVERSION=BETA
# P = prefix name of program
P=pigale_
# L = name of libray
L=qt
MMOC=$(MQTDIR)/bin/moc
PIGALELIB =  -L$(PIGALEPATH)/lib    -l$(L)$(OPT) -ltgraph$(OPT)

#GENERAL OPTIONS
.SUFFIXES:.cpp .o
#OPTIMIZED OPTIONS
CFLAGSopt = -O3  $(PIC) $(CPU)
#CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith -Wno-deprecated  -pipe
CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith -Werror  -pipe
CFLAGSopt +=  -fomit-frame-pointer 
CFLAGSopt +=  -DQT -DVERSION_$(NVERSION) 
#CFLAGSopt +=  -DQT_THREAD_SUPPORT 

#DEBUGGING OPTIONS
CFLAGSdbg  =  -g -Wall $(PIC) $(CPU)
CFLAGSdbg +=  -Wstrict-prototypes -Wpointer-arith -Werror -pipe
CFLAGSdbg +=  -DTDEBUG
CFLAGSdbg +=  -DQT -DVERSION_$(NVERSION)
#CFLAGSopt +=  -DQT_THREAD_SUPPORT 

CFLAGS  = $(CFLAGS$(OPT))
VPATH   = $(PIGALEPATH)/tmp.$(OPT)
DLIB    = $(PIGALEPATH)/lib/lib$L$(OPT).so
FILES  := $(wildcard *.cpp)
INCL   := $(wildcard *.h)
MOC_CPP = $(patsubst %.h,moc_%.cpp, $(INCL))
FILES_WO_MOC := $(filter-out $(MOC_CPP), $(FILES))
FILES_W_MOC  := $(FILES_WO_MOC)
FILES_W_MOC  += $(MOC_CPP)
OBJECTS       =  $(patsubst %.cpp,%.o, $(FILES_W_MOC))
OBJ           =  $(patsubst %.cpp,$(VPATH)/%.o, $(FILES_W_MOC))
ALLOBJ        = $(wildcard $(VPATH)/*.o)

all :   $(P)$(OPT) 

$(PIGALEPATH)/incl/QT/Action.h:$(PIGALEPATH)/incl/QT/Action_def.h
	$(AWK) -f Action.awk $(PIGALEPATH)/incl/QT/Action_def.h > $(PIGALEPATH)/incl/QT/Action.h
 
moc_%.cpp:%.h
	$(MMOC) $< -o $@

.cpp.o: $*.cpp
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $(VPATH)/$*.o   $*.cpp

$(DLIB):$(OBJECTS) 
	@rm -f $(PIGALEPATH)/lib/lib$L$(OPT).so
	@$(CC) -shared  -o $(PIGALEPATH)/lib/lib$L$(OPT).so  $(OBJ)

$(P)$(OPT) :$(DLIB) $(PIGALEPATH)/lib/libtgraph$(OPT).so 
	$(CC)  $(CFLAGS) -o $(P)$(OPT) $(PIGALELIB)  $(UNIXLIB) 

clean:
	@rm -f *~ *bak *.flc  moc_*.cpp  $(OBJ) $(DLIB)

test:
	@echo obj=$(OBJ)

static:$(DLIB) $(PIGALEPATH)/lib/libtgraph$(OPT).so 
	g++  $(CFLAGS) -o s$(P)$(OPT) $(ALLOBJ)  $(UNIXLIB) 

moc_cpp:    $(MOC_CPP)

depend:
	@makedepend -o.o  -fGNUmakefile -I$(PIGALEPATH)/incl  -Y   $(FILES_WO_MOC)  2> /dev/null

nodepend:
	@makedepend -o.o  -fGNUmakefile -Y    2> /dev/null

# DO NOT DELETE

CanvasGrid.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tbase.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasGrid.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
CanvasGrid.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tprop.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tstring.h
CanvasGrid.o: ../../pigale/incl/TAXI/propdef.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
CanvasGrid.o: ../../pigale/incl/TAXI/random.h GraphWidget.h
CanvasGrid.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
CanvasGrid.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
CanvasGrid.o: ../../pigale/incl/QT/GraphWidgetPrivate.h mouse_actions.h
CanvasGrid.o: gprop.h
CanvasItem.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasItem.o: ../../pigale/incl/TAXI/Tbase.h
CanvasItem.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasItem.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
CanvasItem.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
CanvasItem.o: ../../pigale/incl/TAXI/Tprop.h
CanvasItem.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasItem.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasItem.o: ../../pigale/incl/TAXI/Tstring.h
CanvasItem.o: ../../pigale/incl/TAXI/propdef.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
CanvasItem.o: ../../pigale/incl/TAXI/random.h GraphWidget.h mouse_actions.h
CanvasItem.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
CanvasItem.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
CanvasItem.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
CanvasSpring.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tbase.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasSpring.o: ../../pigale/incl/TAXI/color.h
CanvasSpring.o: ../../pigale/incl/TAXI/graphs.h
CanvasSpring.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tprop.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tdebug.h
CanvasSpring.o: ../../pigale/incl/TAXI/Errors.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tstring.h
CanvasSpring.o: ../../pigale/incl/TAXI/propdef.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tpoint.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tfile.h
CanvasSpring.o: ../../pigale/incl/TAXI/random.h GraphWidget.h mouse_actions.h
CanvasSpring.o: gprop.h ../../pigale/incl/QT/MyQcolors.h
CanvasSpring.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
CanvasSpring.o: ../../pigale/incl/QT/grid.h
CanvasSpring.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
ClientSocket.o: ClientSocket.h ../../pigale/incl/Pigale.h
ClientSocket.o: ../../pigale/incl/TAXI/Tbase.h
ClientSocket.o: ../../pigale/incl/TAXI/Tmessage.h
ClientSocket.o: ../../pigale/incl/TAXI/color.h
ClientSocket.o: ../../pigale/incl/TAXI/graphs.h
ClientSocket.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
ClientSocket.o: ../../pigale/incl/TAXI/Tprop.h
ClientSocket.o: ../../pigale/incl/TAXI/Tsvector.h
ClientSocket.o: ../../pigale/incl/TAXI/Tdebug.h
ClientSocket.o: ../../pigale/incl/TAXI/Errors.h
ClientSocket.o: ../../pigale/incl/TAXI/Tpropio.h
ClientSocket.o: ../../pigale/incl/TAXI/Tstring.h
ClientSocket.o: ../../pigale/incl/TAXI/propdef.h
ClientSocket.o: ../../pigale/incl/TAXI/Tpoint.h
ClientSocket.o: ../../pigale/incl/TAXI/Tfile.h
ClientSocket.o: ../../pigale/incl/TAXI/random.h MyWindow.h
ClientSocket.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/Action_def.h
ClientSocket.o: GraphWidget.h GraphGL.h GraphSym.h mouse_actions.h gprop.h
ClientSocket.o: LineEditNum.h ../../pigale/incl/QT/Handler.h
ClientSocket.o: ../../pigale/incl/QT/MyPaint.h
ClientSocket.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
glcontrolwidget.o: ../../pigale/incl/TAXI/Tbase.h glcontrolwidget.h
gprop.o: gprop.h MyWindow.h ../../pigale/incl/Pigale.h
gprop.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
gprop.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
gprop.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
gprop.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
gprop.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
gprop.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
gprop.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
gprop.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
gprop.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/Action_def.h
GraphGL.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphGL.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
GraphGL.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
GraphGL.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphGL.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphGL.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
GraphGL.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphGL.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphGL.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
GraphGL.o: GraphGL.h glcontrolwidget.h mouse_actions.h
GraphGL.o: ../../pigale/incl/QT/GLWindow.h ../../pigale/incl/QT/MyQcolors.h
GraphGL.o: ../../pigale/incl/TAXI/netcut.h
GraphSym.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphSym.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
GraphSym.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
GraphSym.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphSym.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphSym.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
GraphSym.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphSym.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphSym.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
GraphSym.o: GraphSym.h ../../pigale/incl/QT/SymWindow.h
GraphSym.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
Handler.o: ../../pigale/incl/QT/Handler.h ../../pigale/incl/QT/Action_def.h
Handler.o: MyWindow.h ../../pigale/incl/Pigale.h
Handler.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
Handler.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Handler.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Handler.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Handler.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Handler.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Handler.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Handler.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
Handler.o: ../../pigale/incl/QT/Misc.h
LineEditNum.o: LineEditNum.h
Macro.o: MyWindow.h ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tbase.h
Macro.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Macro.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Macro.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Macro.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Macro.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Macro.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Macro.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Macro.o: ../../pigale/incl/TAXI/random.h GraphWidget.h mouse_actions.h
Macro.o: LineEditNum.h ../../pigale/incl/QT/Misc.h
Macro.o: ../../pigale/incl/QT/Action_def.h ../../pigale/incl/TAXI/Tgf.h
main.o: MyWindow.h ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tbase.h
main.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
main.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
main.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
main.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
main.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
main.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
main.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
main.o: ../../pigale/incl/TAXI/random.h ../../pigale/incl/QT/Misc.h
MiscQt.o: MyWindow.h ../../pigale/incl/Pigale.h
MiscQt.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
MiscQt.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
MiscQt.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MiscQt.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MiscQt.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MiscQt.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MiscQt.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MiscQt.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
MiscQt.o: GraphWidget.h
mouse_actions.o: mouse_actions.h GraphWidget.h GraphGL.h
MyCanvas.o: MyWindow.h ../../pigale/incl/Pigale.h
MyCanvas.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
MyCanvas.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
MyCanvas.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyCanvas.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyCanvas.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyCanvas.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyCanvas.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyCanvas.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
MyCanvas.o: GraphWidget.h mouse_actions.h gprop.h
MyCanvas.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
MyCanvas.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
MyCanvas.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
MyPaint.o: MyWindow.h ../../pigale/incl/Pigale.h
MyPaint.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
MyPaint.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
MyPaint.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyPaint.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyPaint.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyPaint.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyPaint.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyPaint.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
MyPaint.o: ../../pigale/incl/QT/MyPaint.h ../../pigale/incl/QT/MyQcolors.h
MyPaint.o: ../../pigale/incl/QT/Misc.h
MyWindow.o: MyWindow.h ../../pigale/incl/Pigale.h
MyWindow.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
MyWindow.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
MyWindow.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyWindow.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyWindow.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyWindow.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyWindow.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyWindow.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
MyWindow.o: ../../pigale/incl/TAXI/Tgf.h GraphWidget.h GraphGL.h GraphSym.h
MyWindow.o: mouse_actions.h gprop.h LineEditNum.h
MyWindow.o: ../../pigale/incl/QT/MyWindow_doc.h ../../pigale/incl/QT/Misc.h
MyWindow.o: ../../pigale/incl/QT/Handler.h ../../pigale/incl/QT/Action_def.h
MyWindow.o: ../../pigale/incl/QT/Action.h ../../pigale/incl/QT/MyPaint.h
MyWindow.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/QT/grid.h
MyWindow.o: icones/fileopen.xpm icones/filenew.xpm icones/fileprint.xpm
MyWindow.o: icones/filesave.xpm icones/info.xpm icones/sleft.xpm
MyWindow.o: icones/sright.xpm icones/sreload.xpm icones/help.xpm
MyWindow.o: icones/xman.xpm icones/sleftarrow.xpm icones/srightarrow.xpm
MyWindow.o: icones/sfilesave.xpm icones/macroplay.xpm icones/film.xpm
Settings.o: MyWindow.h ../../pigale/incl/Pigale.h
Settings.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
Settings.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Settings.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Settings.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Settings.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Settings.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Settings.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Settings.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
Settings.o: mouse_actions.h LineEditNum.h gprop.h ../../pigale/incl/QT/Misc.h
Settings.o: ../../pigale/incl/QT/Handler.h
Test.o: MyWindow.h ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tbase.h
Test.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Test.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Test.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Test.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Test.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Test.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Test.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Test.o: ../../pigale/incl/TAXI/random.h ../../pigale/incl/QT/Misc.h
