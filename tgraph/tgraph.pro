include(../pigale.inc)
TEMPLATE = lib
INCLUDEPATH = ../incl;../incl/TAXI
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
#CONFIG += static
CONFIG(debug, debug|release)  {
    TARGET = tgraph_debug
    DEFINES += TDEBUG
    unix:OBJECTS_DIR = ./.odb
    }else {
    TARGET = tgraph
    unix:OBJECTS_DIR = ./.opt
    }

distdir.commands =
QMAKE_EXTRA_TARGETS += distdir
DESTDIR=$$DISTPATH/lib

message(creating the librayry $$TARGET version:$$VERSION ($$OBJECTS_DIR))
