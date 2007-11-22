TEMPLATE = app
PIGALEPATH = ..
INCLUDEPATH = $$PIGALEPATH/incl
SOURCES = main.cpp 
CONFIG -= qt 
LIBS += $$PIGALEPATH/lib/libtgraph.a 
target = ex