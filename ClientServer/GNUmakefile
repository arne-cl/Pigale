OPT=opt
PIC=-fpic
AWK=awk
CC=g++
#CPU="-mcpu=i686"
#CPU=-m686 //

MQTDIR=$(QTDIR)
PIGALEPATH=../../pigale
MMOC=$(MQTDIR)/bin/moc
IFLAGS = -I$(MQTDIR)/include
IFLAGS +=  -I$(PIGALEPATH)/incl
IFLAGS +=  -I$(PIGALEPATH)/qt

PIGALELIB =  -L$(PIGALEPATH)/lib  -ltgraph$(OPT)  
PIGALELIBS =  -L$(PIGALEPATH)/lib  -ltgraph$(OPT)  -lqt$(OPT)
UNIXLIB   =  -L$(MQTDIR)/lib -lqt-mt  
UNIXLIBS   =  -L$(MQTDIR)/lib -lqt-mt  -lqt-mt -L/usr/local/lib -lglut -lGL -lGLU
.SUFFIXES:.cpp .o

#OPTIMIZED OPTIONS
CFLAGSopt = -O3  $(PIC)
CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith  -pipe
CFLAGSopt += -DQT_THREAD_SUPPORT $(CPU)

#DEBUGGING OPTIONS
CFLAGSdbg  =  -g -Wall  $(PIC) 
CFLAGSdbg +=  -Wstrict-prototypes -Wpointer-arith -Werror -pipe
CFLAGSdbg +=  -DTDEBUG
CFLAGSdbg += -DQT_THREAD_SUPPORT 

FILES  :=  $(wildcard *.cpp)
INCL   := $(wildcard *.h)
MOC_CPP = $(patsubst %.h,moc_%.cpp, $(INCL))
FILES_WO_MOC := $(filter-out $(MOC_CPP), $(FILES))
FILES_W_MOC  := $(FILES_WO_MOC)
FILES_W_MOC  += $(MOC_CPP)


CFLAGS  = $(CFLAGS$(OPT))


all :Server Client XServer

moc_cpp:    $(MOC_CPP)

moc_%.cpp:%.h
	$(MMOC) $< -o $@

.cpp.o:  $*.cpp 
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $*.o   $*.cpp

$(PIGALEPATH)/incl/QT/Action.h:$(PIGALEPATH)/incl/QT/Action_def.h
	$(AWK) -f define.awk $(PIGALEPATH)/incl/QT/Action_def.h > $(PIGALEPATH)/incl/QT/Action.h

XServer: xserver.o  moc_xserver.o 
	$(CC)   -o XServer xserver.o moc_xserver.o $(PIGALELIBS)   $(UNIXLIBS)  

Server: server.o  moc_server.o 
	$(CC)   -o Server server.o moc_server.o  $(PIGALEPATH)/lib/libtgraph$(OPT).so  $(UNIXLIB) 
#	$(CC)   -o Server server.o moc_server.o  $(UNIXLIB) $(PIGALELIB)



Client: $(PIGALEPATH)/incl/QT/Action.h client.o  moc_client.o
	$(CC)   -o Client client.o moc_client.o $(UNIXLIB)

clean:
	@rm -f *.o
	@rm -f moc_*.cpp
	@rm -f Client Server
	@rm -f *~ *bak  *.flc	

depend:
	@makedepend -o.o  -fGNUmakefile -I$(PIGALEPATH)/incl -Y   $(FILES_WO_MOC)  2> /dev/null

nodepend:
	@makedepend -o.o  -fGNUmakefile -Y    2> /dev/null

test:
	@echo moc: $(MOC_CPP)
	@echo without moc: $(FILES_WO_MOC)
	@echo all: $(FILES_W_MOC)
	@echo  $(PIGALELIBS)

# DO NOT DELETE

client.o: client.h ../../pigale/incl/QT/Action_def.h
client.o: ../../pigale/incl/QT/Action.h
server.o: server.h ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tbase.h
server.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
server.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
server.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
server.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
server.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
server.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
server.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
server.o: ../../pigale/incl/TAXI/random.h ../../pigale/incl/QT/Action_def.h
