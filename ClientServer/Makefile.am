SUBDIRS = .
FILES = client.cpp
MOC_FILES= moc_client.cpp
HEADER_FILES=client.h
BUILT_SOURCES=moc_client.cpp
ALL_FILES=$(FILES) $(HEADER_FILES)
INCLUDES=@QT_INCLUDES@
LRELEASE=@LRELEASE@

bin_PROGRAMS = Client
nodist_Client_SOURCES =  moc_client.cpp
Client_SOURCES = client.cpp
Client_LDFLAGS=@LDFLAGS@

EXTRA_DIST=ClientReadMe.txt Clientdata.txt ex.tgf

OPT=@OPTIM@
DEFS=-DQT_THREAD_SUPPORT  -DQT 
CFLAGS =  @CFLAGS@ -pipe

moc_%.cpp:%.h
	@MOC@ $< -o $@

