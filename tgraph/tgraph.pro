TEMPLATE = lib

      
INCLUDEPATH = ../incl
DEPENDPATH = ../incl

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


CONFIG += static
CONFIG -=qt
CONFIG += debug_and_release
macx:DEFINES += OSX
macx:CONFIG+=sdk_no_version_check
#QMAKE_CXXFLAGS += -Wnull-dereference

CONFIG(debug, debug|release)  {
    TARGET = tgraph_debug
    OBJECTS_DIR = ./debug
    #to debug every thing
    #DEFINES += TDEBUG
    }else {
    TARGET = tgraph
    OBJECTS_DIR = ./release
    }
    
DESTDIR=../lib

unix {
     distdir.commands =
     QMAKE_EXTRA_TARGETS += distdir
      # awk
      #awk.target = PropName.cpp
      #awk.depends = propname.awk ../incl/TAXI/propdef.h
      #awk.commands = gawk -f propname.awk ../incl/TAXI/propdef.h > PropName.cpp
      #QMAKE_EXTRA_TARGETS += awk
      #PRE_TARGETDEPS =  PropName.cpp
}

message($$TARGET)

