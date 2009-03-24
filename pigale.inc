SYSTEM=xxxx
contains(SYSTEM,"ubuntu") {
        QTINCLUDE=/usr/include/qt4
        GLUTLIB=/usr/lib/libglut.a
        QTLRELEASE=/usr/share/qt4/bin/lrelease
        QTLUPDATE=/usr/share/qt4/bin/lupdate
        } else {
        MQTDIR = /usr/local/Trolltech/Qt-4.5.0
        QTINCLUDE= /usr/local/Trolltech/Qt-4.5.0/include/Qt
        QTLRELEASE= /usr/local/Trolltech/Qt-4.5.0/bin/lrelease
        QTLUPDATE= /usr/local/Trolltech/Qt-4.5.0/bin/lupdate
        GLUTLIB=/home/pigale/pigale/lib/libglut.a
        }

###################### mac ##############################################
#QTINCLUDE=/opt/local/libexec/qt4-mac/include/
#GLUTLIB=
#QTLRELEASE=
#QTLUPDATE=/usr/share/qt4/bin/lupdate
#in runqmake.sh modify QMAKE=$QT4DIR/bin/qmake -> QMAKE=/usr/bin/qmake-qt4
####################### Nothing to edit after this line ##################
#version of the library
VERSION= 1.3.3
MODE = release
DISTPATH = /home/pigale/pigale
AWK = gawk
ENABLE_STATIC = yes
QMAKE_CXXFLAGS_RELEASE =  -march=i686
#debug_level 1-normal (used for profiler) 2-check arrays (slow)
DEBUG_LEVEL = 2


