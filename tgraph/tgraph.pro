TEMPLATE = lib

win32 {
MQTDIR=C:\Qt\4.1.2\
MODE = release
ENABLE_STATIC=yes
DISTPATH=g:\pigale4\Pigale
ENABLE_STATIC=yes
DEFINES += MINGW _WINDOWS
DESTDIR=.
 }else{
include(../pigale.inc)
}


INCLUDEPATH = ../incl;../incl/TAXI
DEPENDPATH = ../incl/TAXI
SOURCES =\
    3-ConOrientTriang.cpp\
    3-ConShelling1.cpp\
    3-ConShelling2.cpp\
    Barycenter.cpp\
    BFS.cpp\
    Biconnect.cpp\
    Bipolar.cpp\
    BipPlanarize.cpp\
    CotreeCritical.cpp\
    Debug.cpp\
    DFS.cpp\
    DFSGraph.cpp\
    DFSMap.cpp\
    Diagonalise.cpp\
    Embed2Pages.cpp\
    EmbedPolrec.cpp\
    EmbedFPP.cpp\
    EmbedCCD.cpp\
    EmbedPolyline.cpp\
    EmbedTutteCircle.cpp\
    EmbedVision.cpp\
    FastTwit.cpp\
    File.cpp\
    Generate.cpp\
    GeoAlgs.cpp\
    Graph.cpp\
    HeapSort.cpp\
    k-InfOrient.cpp\
    LR-Algo.cpp\
    LR-Map.cpp\
    mark.cpp\
    MaxPlanar.cpp\
    more.cpp\
    netcut.cpp\
    NewPolar.cpp\
    NpBiconnect.cpp\
    Planar.cpp\
    PropName.cpp\
    PropTgf.cpp\
    PSet.cpp\
    random.cpp\
    reduce.cpp\
    SchaefferGen.cpp\
    Schnyder.cpp\
    SchnyderWood.cpp\
    SWShelling.cpp\
    STList.cpp\
    Tgf.cpp\
    TopoAlgs.cpp\
    Twit.cpp\
    VertexPacking.cpp\
    Vision.cpp


CONFIG += qt thread $$MODE
contains(ENABLE_STATIC,"yes") {
 CONFIG += static
}


CONFIG(debug, debug|release)  {
    TARGET = tgraph_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    win32:OBJECTS_DIR = ./odb
    }else {
    TARGET = tgraph
    unix:OBJECTS_DIR = ./.opt
    win32:OBJECTS_DIR = ./opt
    }
unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
     DESTDIR=$$DISTPATH/lib
}

contains(ENABLE_STATIC,"yes") {
  message(creating the static library $$TARGET version:$$VERSION ($$OBJECTS_DIR))
}else{
  message(creating the library $$TARGET version:$$VERSION ($$OBJECTS_DIR))
}
