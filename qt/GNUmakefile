PIGALEPATH=../../pigale
# L = name of libray
L=qt
# P = prefix name of program
P=pigale_
OPT=opt
 
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
CFLAGSopt += -Wall -Wstrict-prototypes -Wpointer-arith
CFLAGSopt += -fomit-frame-pointer -pipe
CFLAGSopt +=  -DQT

#DEBUGGING OPTIONS
CFLAGSdbg  =  -g -Wall  $(PIC)
CFLAGSdbg +=  -DTDEBUG
CFLAGSdbg +=  -DQT

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

glcontrolwidget.o: glcontrolwidget.h
gprop.o: gprop.h MyWindow.h ../../pigale/incl/TAXI/graphs.h
gprop.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
gprop.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
gprop.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
gprop.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
gprop.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
gprop.o: ../../pigale/incl/TAXI/Tgf.h ../../pigale/incl/TAXI/Tmessage.h
gprop.o: ../../pigale/incl/QT/Misc.h
GraphGL.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
GraphGL.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphGL.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphGL.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
GraphGL.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphGL.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphGL.o: ../../pigale/incl/TAXI/Tgf.h GraphGL.h glcontrolwidget.h
GraphGL.o: mouse_actions.h ../../pigale/incl/QT/GLWindow.h
GraphGL.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/TAXI/color.h
GraphGL.o: ../../pigale/incl/TAXI/netcut.h ../../pigale/incl/TAXI/Tmessage.h
GraphSym.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
GraphSym.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
GraphSym.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
GraphSym.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
GraphSym.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
GraphSym.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
GraphSym.o: ../../pigale/incl/TAXI/Tgf.h GraphSym.h
GraphSym.o: ../../pigale/incl/QT/SymWindow.h
GraphSym.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
GraphSym.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/QT/Misc.h
Handler.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/Tstring.h
Handler.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Handler.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Handler.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Handler.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Handler.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Handler.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Handler.o: ../../pigale/incl/QT/Handler.h
Macro.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
Macro.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Macro.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Macro.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Macro.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Macro.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Macro.o: ../../pigale/incl/TAXI/Tgf.h GraphWidget.h mouse_actions.h
Macro.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/QT/Misc.h
main.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
main.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
main.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
main.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
main.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
main.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
main.o: ../../pigale/incl/TAXI/Tgf.h ../../pigale/incl/QT/Misc.h
MiscQt.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
MiscQt.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
MiscQt.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
MiscQt.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
MiscQt.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
MiscQt.o: ../../pigale/incl/TAXI/Tpoint.h MyWindow.h
MiscQt.o: ../../pigale/incl/TAXI/Tgf.h
mouse_actions.o: mouse_actions.h GraphWidget.h GraphGL.h
MyCanvas.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
MyCanvas.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyCanvas.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyCanvas.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
MyCanvas.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyCanvas.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyCanvas.o: ../../pigale/incl/TAXI/Tgf.h GraphWidget.h mouse_actions.h
MyCanvas.o: gprop.h ../../pigale/incl/TAXI/Tmessage.h
MyCanvas.o: ../../pigale/incl/QT/MyQcolors.h ../../pigale/incl/TAXI/color.h
MyCanvas.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/MyCanvas.h
MyPaint.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
MyPaint.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyPaint.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyPaint.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
MyPaint.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyPaint.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyPaint.o: ../../pigale/incl/TAXI/Tgf.h ../../pigale/incl/QT/MyPaint.h
MyPaint.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/Tmessage.h
MyPaint.o: ../../pigale/incl/QT/MyQcolors.h
MyWindow.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
MyWindow.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MyWindow.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MyWindow.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
MyWindow.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
MyWindow.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
MyWindow.o: ../../pigale/incl/TAXI/Tgf.h GraphWidget.h GraphGL.h GraphSym.h
MyWindow.o: mouse_actions.h gprop.h ../../pigale/incl/TAXI/Tfile.h
MyWindow.o: ../../pigale/incl/TAXI/Tmessage.h
MyWindow.o: ../../pigale/incl/QT/MyWindow_doc.h ../../pigale/incl/QT/Misc.h
MyWindow.o: ../../pigale/incl/QT/Handler.h ../../pigale/incl/QT/MyPaint.h
MyWindow.o: ../../pigale/incl/QT/MyCanvas.h ../../pigale/incl/TAXI/color.h
MyWindow.o: icones/fileopen.xpm icones/filenew.xpm icones/fileprint.xpm
MyWindow.o: icones/filesave.xpm icones/info.xpm icones/sleft.xpm
MyWindow.o: icones/sright.xpm icones/sreload.xpm icones/help.xpm
MyWindow.o: icones/xman.xpm icones/sleftarrow.xpm icones/srightarrow.xpm
MyWindow.o: icones/sfilesave.xpm icones/macroplay.xpm
SchaefferGen.o: ../../pigale/incl/TAXI/Tpoint.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tdebug.h
SchaefferGen.o: ../../pigale/incl/TAXI/graphs.h
SchaefferGen.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tprop.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tsvector.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tbase.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tpropio.h
SchaefferGen.o: ../../pigale/incl/TAXI/Tstring.h
SchaefferGen.o: ../../pigale/incl/TAXI/propdef.h
Settings.o: MyWindow.h ../../pigale/incl/TAXI/graphs.h
Settings.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Settings.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Settings.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Settings.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Settings.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Settings.o: ../../pigale/incl/TAXI/Tgf.h mouse_actions.h gprop.h
Settings.o: ../../pigale/incl/QT/Misc.h ../../pigale/incl/QT/Handler.h
Test.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Test.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Test.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Test.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Test.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Test.o: ../../pigale/incl/TAXI/Tpoint.h
