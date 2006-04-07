MQTDIR = /usr/local/Trolltech/Qt-4.1.2
#version of the library
VERSION= 1.3.0
LIBGLUT = -lglut 
MODE = debug
DISTPATH = /home/pigale/PigaleDist

unix {
     QMAKE_CXXFLAGS_RELEASE += -march=i686 -O3 -fomit-frame-pointer 
}
