OPT=opt
PIC=-fpic
AWK=awk
CC=g++

MQTDIR=$(QTDIR)
PIGALEPATH=../../pigale
MMOC=$(MQTDIR)/bin/moc
IFLAGS = -I$(MQTDIR)/include
IFLAGS +=  -I$(PIGALEPATH)/incl
PIGALELIB =  -L$(PIGALEPATH)/lib  -ltgraph$(OPT)  
UNIXLIB   =  -L$(MQTDIR)/lib -lqt-mt  
.SUFFIXES:.cpp .o

#OPTIMIZED OPTIONS
CFLAGSopt = -O3  $(PIC)
CFLAGSopt +=  -Wall -Wstrict-prototypes -Wpointer-arith -Wno-deprecated  -pipe
CFLAGSopt += -DQT_THREAD_SUPPORT 

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


all :Server Client

moc_cpp:    $(MOC_CPP)

moc_%.cpp:%.h
	$(MMOC) $< -o $@

.cpp.o:  $*.cpp 
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $*.o   $*.cpp

$(PIGALEPATH)/incl/QT/Action.h:$(PIGALEPATH)/incl/QT/Action_def.h
	$(AWK) -f define.awk $(PIGALEPATH)/incl/QT/Action_def.h > $(PIGALEPATH)/incl/QT/Action.h
 
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


# DO NOT DELETE

client.o: client.h ../../pigale/incl/QT/Action_def.h
client.o: ../../pigale/incl/QT/Action.h
server.o: server.h ../../pigale/incl/Pigale.h
server.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
server.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
server.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
server.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
server.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
server.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
server.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
server.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/QT/Action_def.h
