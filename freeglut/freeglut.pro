!macx0 {
TEMPLATE = lib
#cache()

win32 {
      DESTDIR="..\\lib"
      DEFINES = FREEGLUT_STATIC WIN32 _LIB _CRT_SECURE_NO_WARNINGS
      }else{
      DESTDIR = ../lib
      DEFINES = FREEGLUT_STATIC _LIB 
}

CONFIG(release) {
    TARGET = freeglut
    OBJECTS_DIR = ./release
    DEFINES += NDEBUG
}    
DESTDIR=../lib
CONFIG += thread create_prl static $$MODE
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-const-variable -Wno-implicit-fallthrough 
QMAKE_CFLAGS += -Wno-dangling-else -Wno-maybe-uninitialized 
INCLUDEPATH = ./include ./srv/util
unix::INCLUDEPATH += /usr/X11R6/include
DEPENDPATH = ./include/GL
QT += opengl
QT -= qt

SOURCES =\
src/fg_callbacks.c\
src/fg_cursor.c\
src/fg_display.c\
src/fg_ext.c\
src/fg_font.c\
src/fg_font_data.c\
src/fg_gamemode.c\
src/fg_geometry.c\
src/fg_gl2.c\
src/fg_init.c\
src/fg_input_devices.c\
src/fg_joystick.c\
src/fg_main.c\
src/fg_menu.c\
src/fg_misc.c\
src/fg_overlay.c\
src/fg_spaceball.c\
src/fg_state.c\
src/fg_stroke_mono_roman.c\
src/fg_stroke_roman.c\
src/fg_structure.c\
src/fg_teapot.c\
src/fg_videoresize.c\
src/fg_window.c\
src/util/xparsegeometry_repl.c\
src/mswin/fg_cursor_mswin.c\
src/mswin/fg_display_mswin.c\
src/mswin/fg_ext_mswin.c\
src/mswin/fg_gamemode_mswin.c\
src/mswin/fg_init_mswin.c\
src/mswin/fg_input_devices_mswin.c\
src/mswin/fg_joystick_mswin.c\
src/mswin/fg_main_mswin.c\
src/mswin/fg_menu_mswin.c\
src/mswin/fg_spaceball_mswin.c\
src/mswin/fg_state_mswin.c\
src/mswin/fg_structure_mswin.c\
src/mswin/fg_window_mswin.c


unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
}


message(configuring the library $$TARGET)
} else {
TEMPLATE = subdirs
message(FREEGLUT:Nothing to be done under MAC OSX)
}
