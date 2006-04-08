MQTDIR = /usr/local/Trolltech/Qt-4.1.2
#version of the library
VERSION= 1.3.0
LIBGLUT = -lglut 
MODE = release
DISTPATH = /home/pigale/pigale
AWK = gawk
ENABLE_STATIC = yes

unix {
     QMAKE_CXXFLAGS_RELEASE += -march=i686 -O3 -fomit-frame-pointer 
}
