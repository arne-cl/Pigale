
FILES = \
    CanvasGrid.cpp \
    CanvasItem.cpp \
    CanvasSpring.cpp \
    ClientSocket.cpp \
    EmbedCurves.cpp \
    glcontrolwidget.cpp \
    gprop.cpp \
    GraphGL.cpp \
    graphml.cpp \
    graphmlparser.cpp \
    GraphSym.cpp \
    Handler.cpp \
    LineEditNum.cpp \
    Macro.cpp \
    MiscQt.cpp \
    mouse_actions.cpp \
    pigaleCanvas.cpp \
    pigaleFile.cpp \
    pigaleMenus.cpp \
    pigalePaint.cpp \
    pigaleUndo.cpp \
    pigaleWindow.cpp \
    Settings.cpp \
    Test.cpp

MOC_FILES= \
    moc_ClientSocket.cpp \
    moc_glcontrolwidget.cpp \
    moc_gprop.cpp \
    moc_GraphGL.cpp \
    moc_GraphSym.cpp \
    moc_GraphWidget.cpp \
    moc_LineEditNum.cpp \
    moc_mouse_actions.cpp \
    moc_pigaleWindow.cpp

HEADER_FILES:= $(wildcard *.h)

BUILT_SOURCES=$(MOC_FILES) $(top_srcdir)/incl/QT/Action.h

ALL_FILES=$(FILES) $(HEADER_FILES)
INCLUDES=@QT_INCLUDES@
LRELEASE=@LRELEASE@

lib_LTLIBRARIES = libpigaleQt.la
nodist_libpigaleQt_la_SOURCES=$(MOC_FILES)
libpigaleQt_la_SOURCES=$(ALL_FILES)
libpigaleQt_la_LDFLAGS = -version-info @LIB_VERSION@

bin_PROGRAMS = pigale 
pigale_SOURCES=main.cpp
pigale_LDADD = -L$(prefix)/lib -ltgraph -lpigaleQt
pigale_DEPENDENCIES =tgraph pigale_fr.qm 


datadir = $(prefix)/translations
data_DATA = pigale_fr.qm qt_fr.qm
EXTRA_DIST = pigale_fr.ts pigale.pro qt_fr.ts gnumakefile Action.awk

tgraph:
	cd ../tgraph && make install

$(top_srcdir)/incl/QT/Action.h : $(top_srcdir)/incl/QT/Action_def.h
	@AWK@ -f Action.awk $(top_srcdir)/incl/QT/Action_def.h > $(top_srcdir)/incl/QT/Action.h

moc_%.cpp:%.h
	@MOC@ $< -o $@

pigale_fr.qm:
	$(LRELEASE) pigale_fr.ts
	$(LRELEASE) qt_fr.ts


