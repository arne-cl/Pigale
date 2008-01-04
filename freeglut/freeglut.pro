TEMPLATE = lib

win32 {
      include(..\wpigale.inc)
      DESTDIR=.
      QMAKE_CFLAGS_WARN_ON =  -Wall -Wextra
      DEFINES += FREEGLUT_STATIC __MINGW32__ _WIN32
      DEFINES -= UNICODE
      }else{
      include(../pigale.inc)
      QMAKE_CFLAGS_WARN_ON = -pedantic  -Wall -Werror
      QMAKE_CFLAGS_RELEASE += -O3 -fomit-frame-pointer
}

CONFIG = thread static
INCLUDEPATH = ../incl
unix::INCLUDEPATH += /usr/X11R6/include
DEPENDPATH = ../incl

SOURCES =\
freeglut_callbacks.c\
freeglut_display.c\
freeglut_ext.c\
freeglut_font.c\
freeglut_font_data.c\
freeglut_gamemode.c\
freeglut_geometry.c\
freeglut_glutfont_definitions.c\
freeglut_cursor.c\
freeglut_init.c\
freeglut_main.c\
freeglut_menu.c\
freeglut_misc.c\
freeglut_overlay.c\
freeglut_state.c\
freeglut_stroke_mono_roman.c\
freeglut_stroke_roman.c\
freeglut_structure.c\
freeglut_teapot.c\
freeglut_videoresize.c\
freeglut_joystick.c\
freeglut_window.c


TARGET = glut
unix:OBJECTS_DIR = ./.opt
win32:OBJECTS_DIR = ./opt

unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
     DESTDIR=$$DISTPATH/lib
}

message(configuring the static library freeglut ($$OBJECTS_DIR))
