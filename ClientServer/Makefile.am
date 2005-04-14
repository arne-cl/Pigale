SUBDIRS = .
FILES = client.cpp
MOC_FILES= moc_client.cpp
HEADER_FILES=client.h
BUILT_SOURCES=moc_client.cpp
ALL_FILES=$(FILES) $(HEADER_FILES)
INCLUDES=@QT_INCLUDES@
LRELEASE=@LRELEASE@

bin_PROGRAMS =client
nodist_client_SOURCES =moc_client.cpp
client_SOURCES = client.cpp client.h
client_LDFLAGS=@LDFLAGS@

EXTRA_DIST=Clientdata.txt ex.tgf symm.tgf graph.txt gnumakefile 

OPT=@OPTIM@
DEFS=-DQT_THREAD_SUPPORT  -DQT 
CFLAGS =  @CFLAGS@ -pipe

moc_%.cpp:%.h
	@MOC@ $< -o $@

