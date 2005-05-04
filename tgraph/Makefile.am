libtgraph_la_SOURCES= \
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

INCLUDES= -I$(top_srcdir)/incl
lib_LTLIBRARIES = libtgraph.la
libtgraph_la_LDFLAGS = -version-info @LIB_VERSION@
libtgraph_la_DEPENDENCIES =$(top_srcdir)/incl/QT/Action.h

datadir = .
EXTRA_DIST=gnumakefile

PropName.cpp : $(top_srcdir)/incl/TAXI/propdef.h
	$(AWK) -f propname.awk ../incl/TAXI/propdef.h > PropName.cpp

