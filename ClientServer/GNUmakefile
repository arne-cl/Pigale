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


all :Client XServer

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
	@makedepend -o.o  -fGNUmakefile $(IFLAGS) -Y   $(FILES_WO_MOC)  2> /dev/null

nodepend:
	@makedepend -o.o  -fGNUmakefile -Y    2> /dev/null

test:
	@echo moc: $(MOC_CPP)
	@echo without moc: $(FILES_WO_MOC)
	@echo all: $(FILES_W_MOC)
	@echo  $(PIGALELIBS)

# DO NOT DELETE

client.o: client.h ../../pigale/incl/QT/Action_def.h
client.o: /usr/local/qt/include/qsocket.h /usr/local/qt/include/qobject.h
client.o: /usr/local/qt/include/qobjectdefs.h /usr/local/qt/include/qglobal.h
client.o: /usr/local/qt/include/qwindowdefs.h /usr/local/qt/include/qstring.h
client.o: /usr/local/qt/include/qcstring.h /usr/local/qt/include/qmemarray.h
client.o: /usr/local/qt/include/qgarray.h /usr/local/qt/include/qshared.h
client.o: /usr/local/qt/include/qnamespace.h /usr/local/qt/include/qevent.h
client.o: /usr/local/qt/include/qregion.h /usr/local/qt/include/qrect.h
client.o: /usr/local/qt/include/qsize.h /usr/local/qt/include/qpoint.h
client.o: /usr/local/qt/include/qmime.h /usr/local/qt/include/qmap.h
client.o: /usr/local/qt/include/qdatastream.h
client.o: /usr/local/qt/include/qiodevice.h /usr/local/qt/include/qpair.h
client.o: /usr/local/qt/include/qvaluelist.h /usr/local/qt/include/qtl.h
client.o: /usr/local/qt/include/qtextstream.h
client.o: /usr/local/qt/include/qhostaddress.h
client.o: /usr/local/qt/include/qapplication.h
client.o: /usr/local/qt/include/qdesktopwidget.h
client.o: /usr/local/qt/include/qwidget.h
client.o: /usr/local/qt/include/qpaintdevice.h
client.o: /usr/local/qt/include/qpalette.h /usr/local/qt/include/qcolor.h
client.o: /usr/local/qt/include/qbrush.h /usr/local/qt/include/qfont.h
client.o: /usr/local/qt/include/qfontmetrics.h
client.o: /usr/local/qt/include/qfontinfo.h
client.o: /usr/local/qt/include/qsizepolicy.h
client.o: /usr/local/qt/include/qasciidict.h /usr/local/qt/include/qgdict.h
client.o: /usr/local/qt/include/qptrcollection.h
client.o: /usr/local/qt/include/qtranslator.h
client.o: /usr/local/qt/include/qstrlist.h /usr/local/qt/include/qptrlist.h
client.o: /usr/local/qt/include/qglist.h /usr/local/qt/include/qstringlist.h
client.o: /usr/local/qt/include/qvbox.h /usr/local/qt/include/qhbox.h
client.o: /usr/local/qt/include/qframe.h /usr/local/qt/include/qtextview.h
client.o: /usr/local/qt/include/qtextedit.h
client.o: /usr/local/qt/include/qscrollview.h
client.o: /usr/local/qt/include/qscrollbar.h
client.o: /usr/local/qt/include/qrangecontrol.h
client.o: /usr/local/qt/include/qstylesheet.h
client.o: /usr/local/qt/include/qptrvector.h /usr/local/qt/include/qgvector.h
client.o: /usr/local/qt/include/qdict.h /usr/local/qt/include/qlineedit.h
client.o: /usr/local/qt/include/qlabel.h /usr/local/qt/include/qpushbutton.h
client.o: /usr/local/qt/include/qbutton.h
client.o: /usr/local/qt/include/qkeysequence.h
client.o: /usr/local/qt/include/qiconset.h /usr/local/qt/include/qpixmap.h
client.o: /usr/local/qt/include/qradiobutton.h /usr/local/qt/include/qfile.h
client.o: /usr/local/qt/include/qfileinfo.h /usr/local/qt/include/qdatetime.h
client.o: /usr/local/qt/include/qthread.h /usr/local/qt/include/qmutex.h
server.o: server.h /usr/local/qt/include/qsocket.h
server.o: /usr/local/qt/include/qobject.h /usr/local/qt/include/qobjectdefs.h
server.o: /usr/local/qt/include/qglobal.h /usr/local/qt/include/qwindowdefs.h
server.o: /usr/local/qt/include/qstring.h /usr/local/qt/include/qcstring.h
server.o: /usr/local/qt/include/qmemarray.h /usr/local/qt/include/qgarray.h
server.o: /usr/local/qt/include/qshared.h /usr/local/qt/include/qnamespace.h
server.o: /usr/local/qt/include/qevent.h /usr/local/qt/include/qregion.h
server.o: /usr/local/qt/include/qrect.h /usr/local/qt/include/qsize.h
server.o: /usr/local/qt/include/qpoint.h /usr/local/qt/include/qmime.h
server.o: /usr/local/qt/include/qmap.h /usr/local/qt/include/qdatastream.h
server.o: /usr/local/qt/include/qiodevice.h /usr/local/qt/include/qpair.h
server.o: /usr/local/qt/include/qvaluelist.h /usr/local/qt/include/qtl.h
server.o: /usr/local/qt/include/qtextstream.h
server.o: /usr/local/qt/include/qhostaddress.h
server.o: /usr/local/qt/include/qserversocket.h
server.o: /usr/local/qt/include/qsocketdevice.h
server.o: /usr/local/qt/include/qapplication.h
server.o: /usr/local/qt/include/qdesktopwidget.h
server.o: /usr/local/qt/include/qwidget.h
server.o: /usr/local/qt/include/qpaintdevice.h
server.o: /usr/local/qt/include/qpalette.h /usr/local/qt/include/qcolor.h
server.o: /usr/local/qt/include/qbrush.h /usr/local/qt/include/qfont.h
server.o: /usr/local/qt/include/qfontmetrics.h
server.o: /usr/local/qt/include/qfontinfo.h
server.o: /usr/local/qt/include/qsizepolicy.h
server.o: /usr/local/qt/include/qasciidict.h /usr/local/qt/include/qgdict.h
server.o: /usr/local/qt/include/qptrcollection.h
server.o: /usr/local/qt/include/qtranslator.h
server.o: /usr/local/qt/include/qstrlist.h /usr/local/qt/include/qptrlist.h
server.o: /usr/local/qt/include/qglist.h /usr/local/qt/include/qstringlist.h
server.o: /usr/local/qt/include/qvbox.h /usr/local/qt/include/qhbox.h
server.o: /usr/local/qt/include/qframe.h /usr/local/qt/include/qtextview.h
server.o: /usr/local/qt/include/qtextedit.h
server.o: /usr/local/qt/include/qscrollview.h
server.o: /usr/local/qt/include/qscrollbar.h
server.o: /usr/local/qt/include/qrangecontrol.h
server.o: /usr/local/qt/include/qstylesheet.h
server.o: /usr/local/qt/include/qptrvector.h /usr/local/qt/include/qgvector.h
server.o: /usr/local/qt/include/qdict.h /usr/local/qt/include/qlabel.h
server.o: /usr/local/qt/include/qpushbutton.h /usr/local/qt/include/qbutton.h
server.o: /usr/local/qt/include/qkeysequence.h
server.o: /usr/local/qt/include/qiconset.h /usr/local/qt/include/qpixmap.h
server.o: ../../pigale/incl/Pigale.h ../../pigale/incl/TAXI/Tbase.h
server.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
server.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
server.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
server.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
server.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
server.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
server.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tfile.h
server.o: ../../pigale/incl/TAXI/random.h ../../pigale/incl/QT/Action_def.h
xserver.o: xserver.h /usr/local/qt/include/qapplication.h
xserver.o: /usr/local/qt/include/qdesktopwidget.h
xserver.o: /usr/local/qt/include/qwidget.h
xserver.o: /usr/local/qt/include/qwindowdefs.h
xserver.o: /usr/local/qt/include/qobjectdefs.h
xserver.o: /usr/local/qt/include/qglobal.h /usr/local/qt/include/qstring.h
xserver.o: /usr/local/qt/include/qcstring.h /usr/local/qt/include/qmemarray.h
xserver.o: /usr/local/qt/include/qgarray.h /usr/local/qt/include/qshared.h
xserver.o: /usr/local/qt/include/qnamespace.h /usr/local/qt/include/qobject.h
xserver.o: /usr/local/qt/include/qevent.h /usr/local/qt/include/qregion.h
xserver.o: /usr/local/qt/include/qrect.h /usr/local/qt/include/qsize.h
xserver.o: /usr/local/qt/include/qpoint.h /usr/local/qt/include/qmime.h
xserver.o: /usr/local/qt/include/qmap.h /usr/local/qt/include/qdatastream.h
xserver.o: /usr/local/qt/include/qiodevice.h /usr/local/qt/include/qpair.h
xserver.o: /usr/local/qt/include/qvaluelist.h /usr/local/qt/include/qtl.h
xserver.o: /usr/local/qt/include/qtextstream.h
xserver.o: /usr/local/qt/include/qpaintdevice.h
xserver.o: /usr/local/qt/include/qpalette.h /usr/local/qt/include/qcolor.h
xserver.o: /usr/local/qt/include/qbrush.h /usr/local/qt/include/qfont.h
xserver.o: /usr/local/qt/include/qfontmetrics.h
xserver.o: /usr/local/qt/include/qfontinfo.h
xserver.o: /usr/local/qt/include/qsizepolicy.h
xserver.o: /usr/local/qt/include/qasciidict.h /usr/local/qt/include/qgdict.h
xserver.o: /usr/local/qt/include/qptrcollection.h
xserver.o: /usr/local/qt/include/qtranslator.h
xserver.o: /usr/local/qt/include/qstrlist.h /usr/local/qt/include/qptrlist.h
xserver.o: /usr/local/qt/include/qglist.h /usr/local/qt/include/qstringlist.h
xserver.o: /usr/local/qt/include/qvbox.h /usr/local/qt/include/qhbox.h
xserver.o: /usr/local/qt/include/qframe.h ../../pigale/qt/ClientSocket.h
xserver.o: /usr/local/qt/include/qsocket.h
xserver.o: /usr/local/qt/include/qhostaddress.h
xserver.o: /usr/local/qt/include/qserversocket.h
xserver.o: /usr/local/qt/include/qsocketdevice.h
xserver.o: /usr/local/qt/include/qtextview.h
xserver.o: /usr/local/qt/include/qtextedit.h
xserver.o: /usr/local/qt/include/qscrollview.h
xserver.o: /usr/local/qt/include/qscrollbar.h
xserver.o: /usr/local/qt/include/qrangecontrol.h
xserver.o: /usr/local/qt/include/qstylesheet.h
xserver.o: /usr/local/qt/include/qptrvector.h
xserver.o: /usr/local/qt/include/qgvector.h /usr/local/qt/include/qdict.h
xserver.o: /usr/local/qt/include/qlabel.h /usr/local/qt/include/qpushbutton.h
xserver.o: /usr/local/qt/include/qbutton.h
xserver.o: /usr/local/qt/include/qkeysequence.h
xserver.o: /usr/local/qt/include/qiconset.h /usr/local/qt/include/qpixmap.h
xserver.o: /usr/local/qt/include/qradiobutton.h ../../pigale/incl/Pigale.h
xserver.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tmessage.h
xserver.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/graphs.h
xserver.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
xserver.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
xserver.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
xserver.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
xserver.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
xserver.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/random.h
xserver.o: ../../pigale/qt/MyWindow.h /usr/local/qt/include/qmainwindow.h
xserver.o: /usr/local/qt/include/qtoolbar.h
xserver.o: /usr/local/qt/include/qdockwindow.h
xserver.o: /usr/local/qt/include/qdatetime.h ../../pigale/incl/QT/Misc.h
xserver.o: ../../pigale/incl/QT/Action_def.h /usr/local/qt/include/qmenubar.h
xserver.o: /usr/local/qt/include/qpopupmenu.h
xserver.o: /usr/local/qt/include/qmenudata.h /usr/local/qt/include/qsignal.h
xserver.o: /usr/local/qt/include/qvariant.h
