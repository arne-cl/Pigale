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
UNIXLIB =  -L$(MQTDIR)/lib -lqt-mt -L/usr/X11R6/lib -lGL -lGLU -L/usr/local -lglut
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
CFLAGSdbg +=  -DDEBUG
CFLAGSdbg +=  -DQT

CFLAGS  = $(CFLAGS$(OPT))
VPATH = $(PIGALEPATH)/tmp.$(OPT)
DLIB   =     $(PIGALEPATH)/lib/lib$L$(OPT).so
FILES :=  $(wildcard *.cpp)
INCL := $(wildcard *.h)
OBJECTS =  $(patsubst %.cpp,%.o, $(FILES))
OBJ =  $(patsubst %.cpp,$(VPATH)/%.o, $(FILES))
MOC = $(patsubst %.h,moc_%.o, $(INCL))
MOC_CPP = $(patsubst %.h,moc_%.cpp, $(INCL))
OBJECTS += $(MOC)

all :   $(P)$(OPT) 

moc_%.cpp:%.h
	$(MMOC) $< -o $@

.cpp.o: $*.cpp
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $(VPATH)/$*.o   $*.cpp

$(DLIB):$(OBJECTS) 
	@rm -f $(PIGALEPATH)/lib/lib$L$(OPT).so
	@g++ -shared  -o $(PIGALEPATH)/lib/lib$L$(OPT).so  $(OBJ)

$(P)$(OPT) :$(DLIB) $(PIGALEPATH)/lib/libtgraph$(OPT).so 
	g++  $(CFLAGS) -o $(P)$(OPT)  $(UNIXLIB) $(PIGALELIB) 

clean:
	@rm -f $(VPATH)/*.o

test:
	@echo objects=$(OBJECTS)

moc_cpp:    $(MOC_CPP)

depend:
#	@Makedepend -o.o  -fGNUmakefile -I$(PIGALEPATH)/incl  -Y   *.cpp  2> /dev/null
	@makedepend -o.o  -fGNUmakefile  -Y   *.cpp  2> /dev/null

nodepend:
	@makedepend -o.o  -fGNUmakefile -Y    2> /dev/null

# DO NOT DELETE

glcontrolwidget.o: glcontrolwidget.h
gprop.o: gprop.h MyWindow.h
GraphGL.o: MyWindow.h GraphGL.h glcontrolwidget.h mouse_actions.h
GraphSym.o: MyWindow.h GraphSym.h
main.o: MyWindow.h
MiscQt.o: MyWindow.h mouse_actions.h
moc_glcontrolwidget.o: glcontrolwidget.h
moc_gprop.o: gprop.h
moc_GraphGL.o: GraphGL.h
moc_GraphSym.o: GraphSym.h
moc_GraphWidget.o: GraphWidget.h
moc_mouse_actions.o: mouse_actions.h
moc_MyWindow.o: MyWindow.h
mouse_actions.o: mouse_actions.h GraphWidget.h GraphGL.h
MyCanvas.o: MyWindow.h GraphWidget.h mouse_actions.h gprop.h
MyPaint.o: MyWindow.h
MyWindow.o: MyWindow.h GraphWidget.h GraphGL.h GraphSym.h mouse_actions.h
MyWindow.o: gprop.h icones/fileopen.xpm icones/filenew.xpm
MyWindow.o: icones/fileprint.xpm icones/filesave.xpm icones/info.xpm
MyWindow.o: icones/sleft.xpm icones/sright.xpm icones/sreload.xpm
MyWindow.o: icones/help.xpm icones/xman.xpm
