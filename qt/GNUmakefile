PIGALEPATH=../../pigale
# L = name of libray
L=qt
# P = prefix name of program
P=pigale_
OPT=opt
NVERSION=BETA
AWK=awk
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
CFLAGSopt = -O3  $(PIC)
CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith -Wno-deprecated  -pipe
#CFLAGSopt +=  -fomit-frame-pointer (does not work with GCC 3)
CFLAGSopt +=  -DQT -DVERSION_$(NVERSION)


#DEBUGGING OPTIONS
CFLAGSdbg  =  -g -Wall $(PIC)
CFLAGSdbg +=  -Wstrict-prototypes -Wpointer-arith -Werror -pipe
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
	@rm -f $(VPATH)/*.o
	@rm -f moc_*.cpp

test:
	@echo objects=$(ALLOBJ)

static:$(DLIB) $(PIGALEPATH)/lib/libtgraph$(OPT).so 
	g++  $(CFLAGS) -o s$(P)$(OPT) $(ALLOBJ)  $(UNIXLIB) 

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
CanvasGrid.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
CanvasGrid.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasGrid.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
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
CanvasItem.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
CanvasItem.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
CanvasItem.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasItem.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasItem.o: ../../pigale/incl/TAXI/Tstring.h
CanvasItem.o: ../../pigale/incl/TAXI/propdef.h
CanvasItem.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
CanvasItem.o: GraphWidget.h mouse_actions.h ../../pigale/incl/QT/MyQcolors.h
CanvasItem.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
CanvasItem.o: ../../pigale/incl/QT/grid.h
CanvasItem.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
CanvasSpring.o: MyWindow.h ../../pigale/incl/Pigale.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tmessage.h
CanvasSpring.o: ../../pigale/incl/TAXI/color.h
CanvasSpring.o: ../../pigale/incl/TAXI/graphs.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
CanvasSpring.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tsvector.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tdebug.h
CanvasSpring.o: ../../pigale/incl/TAXI/Errors.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tpropio.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tstring.h
CanvasSpring.o: ../../pigale/incl/TAXI/propdef.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tpoint.h
CanvasSpring.o: ../../pigale/incl/TAXI/Tfile.h GraphWidget.h mouse_actions.h
CanvasSpring.o: gprop.h ../../pigale/incl/QT/MyQcolors.h
CanvasSpring.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
CanvasSpring.o: ../../pigale/incl/QT/grid.h
CanvasSpring.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
glcontrolwidget.o: glcontrolwidget.h
gprop.o: gprop.h MyWindow.h ../../pigale/incl/Pigale.h
gprop.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
gprop.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
gprop.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
gprop.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
gprop.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
gprop.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
gprop.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
gprop.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/QT/Misc.h
gprop.o: ../../pigale/incl/QT/Action_def.h
GraphGL.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphGL.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
GraphGL.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
GraphGL.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphGL.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphGL.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
GraphGL.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphGL.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphGL.o: ../../pigale/incl/TAXI/Tfile.h GraphGL.h glcontrolwidget.h
GraphGL.o: mouse_actions.h ../../pigale/incl/QT/GLWindow.h
GraphGL.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/TAXI/netcut.h
GraphSym.o: MyWindow.h ../../pigale/incl/Pigale.h
GraphSym.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
GraphSym.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
GraphSym.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphSym.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphSym.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
GraphSym.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphSym.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphSym.o: ../../pigale/incl/TAXI/Tfile.h GraphSym.h
GraphSym.o: ../../pigale/incl/QT/SymWindow.h ../../pigale/incl/QT/MyQcolors.h
GraphSym.o: ../../pigale/incl/QT/Misc.h
Handler.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
Handler.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Handler.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
Handler.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Handler.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Handler.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Handler.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Handler.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Handler.o: ../../pigale/incl/QT/Handler.h ../../pigale/incl/QT/Action_def.h
LineEditNum.o: LineEditNum.h
Macro.o: MyWindow.h ../../pigale/incl/Pigale.h
Macro.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Macro.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
Macro.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Macro.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Macro.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Macro.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Macro.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Macro.o: ../../pigale/incl/TAXI/Tfile.h GraphWidget.h mouse_actions.h
Macro.o: LineEditNum.h ../../pigale/incl/QT/Misc.h
Macro.o: ../../pigale/incl/QT/Action_def.h ../../pigale/incl/TAXI/Tgf.h
main.o: MyWindow.h ../../pigale/incl/Pigale.h
main.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
main.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
main.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
main.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
main.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
main.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
main.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
main.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/QT/Misc.h
MiscQt.o: MyWindow.h ../../pigale/incl/Pigale.h
MiscQt.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MiscQt.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
MiscQt.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MiscQt.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MiscQt.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MiscQt.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MiscQt.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MiscQt.o: ../../pigale/incl/TAXI/Tfile.h GraphWidget.h
mouse_actions.o: mouse_actions.h GraphWidget.h GraphGL.h
MyCanvas.o: MyWindow.h ../../pigale/incl/Pigale.h
MyCanvas.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyCanvas.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
MyCanvas.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyCanvas.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyCanvas.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyCanvas.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyCanvas.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyCanvas.o: ../../pigale/incl/TAXI/Tfile.h GraphWidget.h mouse_actions.h
MyCanvas.o: gprop.h ../../pigale/incl/QT/MyQcolors.h
MyCanvas.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
MyCanvas.o: ../../pigale/incl/QT/grid.h
MyCanvas.o: ../../pigale/incl/QT/GraphWidgetPrivate.h
MyPaint.o: MyWindow.h ../../pigale/incl/Pigale.h
MyPaint.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyPaint.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
MyPaint.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyPaint.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyPaint.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyPaint.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyPaint.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyPaint.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/QT/MyPaint.h
MyPaint.o: ../../pigale/incl/QT/MyQcolors.h
MyWindow.o: MyWindow.h ../../pigale/incl/Pigale.h
MyWindow.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
MyWindow.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
MyWindow.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyWindow.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyWindow.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MyWindow.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyWindow.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyWindow.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/Tgf.h
MyWindow.o: GraphWidget.h GraphGL.h GraphSym.h mouse_actions.h gprop.h
MyWindow.o: LineEditNum.h ../../pigale/incl/QT/MyWindow_doc.h
MyWindow.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/Handler.h
MyWindow.o: ../../pigale/incl/QT/Action_def.h ../../pigale/incl/QT/Action.h
MyWindow.o: ../../pigale/incl/QT/MyPaint.h ../../pigale/incl/QT/MyCanvas.h
MyWindow.o: ../../pigale/incl/QT/grid.h icones/fileopen.xpm
MyWindow.o: icones/filenew.xpm icones/fileprint.xpm icones/filesave.xpm
MyWindow.o: icones/info.xpm icones/sleft.xpm icones/sright.xpm
MyWindow.o: icones/sreload.xpm icones/help.xpm icones/xman.xpm
MyWindow.o: icones/sleftarrow.xpm icones/srightarrow.xpm icones/sfilesave.xpm
MyWindow.o: icones/macroplay.xpm icones/film.xpm
SchaefferGen.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
SchaefferGen.o: ../../pigale/incl/TAXI/color.h
SchaefferGen.o: ../../pigale/incl/TAXI/graphs.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
SchaefferGen.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tsvector.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tdebug.h
SchaefferGen.o: ../../pigale/incl/TAXI/Errors.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tpropio.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tstring.h
SchaefferGen.o: ../../pigale/incl/TAXI/propdef.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tpoint.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tfile.h
Settings.o: MyWindow.h ../../pigale/incl/Pigale.h
Settings.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Settings.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
Settings.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Settings.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Settings.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Settings.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Settings.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Settings.o: ../../pigale/incl/TAXI/Tfile.h mouse_actions.h LineEditNum.h
Settings.o: gprop.h ../../pigale/incl/QT/Misc.h
Settings.o: ../../pigale/incl/QT/Handler.h
Test.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tmessage.h
Test.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
Test.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
Test.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Test.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Test.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Test.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Test.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
Test.o: MyWindow.h ../../pigale/incl/QT/Misc.h
