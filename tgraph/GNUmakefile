####################### TO BE EDITED ########################
#OPT: if set to dbg > debug version
#OPT: if set to opt > optimised version
OPT      =opt
#PIC if wanted shared dynamic libraries
PIC=-fpic
#Unix libraries to be linked (system dependent)
UNIXLIB = -L/usr/X11R6/lib -lX11
####################### END EDITING #########################
PIGALEPATH  = ../../pigale
SAMPLING =
RAN = ranlib
#SAMPLING = -lc_p
L = tgraph
#GENERAL OPTIONS
.SUFFIXES:
.SUFFIXES:.cpp .c .opt .dbg .o
IFLAGS = -I$(PIGALEPATH)/incl 

CC=g++

#OPTIMIZED OPTIONS
CFLAGSopt = -O2 -m486 -fstrength-reduce $(PIC)
CFLAGSopt += -Wall -Wstrict-prototypes -Wpointer-arith
# -Winline
CFLAGSopt += -fomit-frame-pointer

#DEBUGGING OPTIONS (-pg -a for sampling)
CFLAGSdbg  =  -g -Wall  $(PIC)
CFLAGSdbg +=  -DTDEBUG

DLIB   = $(PIGALEPATH)/lib/lib$L$(OPT).so
LIBS    = -L$(PIGALEPATH)/lib -lleda$(OPT) -ltgraph$(OPT)
CFLAGS  = $(CFLAGS$(OPT))
VPATH = $(PIGALEPATH)/tmp.$(OPT)
FILES :=  $(wildcard *.cpp)
OBJECTS =  $(patsubst %.cpp,%.o, $(FILES))
#include  Make.lst
OBJ =  $(patsubst %.o,$(VPATH)/%.o, $(OBJECTS))

all :  $(DLIB)


.cpp.o: $*.cpp
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $(VPATH)/$*.o   $*.cpp


$(DLIB):	$(OBJECTS)
	@rm -f $(PIGALEPATH)/lib/lib$L$(OPT).so
	@g++ -shared  -o $(PIGALEPATH)/lib/lib$L$(OPT).so  $(OBJ)

clean:
	rm -f *.$(OPT) $(DLIB) 
	rm -f *~ *bak $(OBJ) *.flc

depend:
	@makedepend -o.o -fGNUmakefile $(IFLAGS)  -Y *.cpp 2> /dev/null

nodepend:
	@makedepend -o.o -fGNUmakefile  -Y  2> /dev/null

list:
	../makelist *.cpp >Make.lst


# DO NOT DELETE

3-ConOrientTriang.o: ../../pigale/incl/TAXI/KantShel.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/graphs.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/graph.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/elmt.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tprop.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tbase.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tpropio.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tstring.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/propdef.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tpoint.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/IntList.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tflist.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tstack.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tarray.h
3-ConShelling1.o: ../../pigale/incl/TAXI/KantShel.h
3-ConShelling1.o: ../../pigale/incl/TAXI/graphs.h
3-ConShelling1.o: ../../pigale/incl/TAXI/graph.h
3-ConShelling1.o: ../../pigale/incl/TAXI/elmt.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tprop.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tbase.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tpropio.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tstring.h
3-ConShelling1.o: ../../pigale/incl/TAXI/propdef.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tpoint.h
3-ConShelling1.o: ../../pigale/incl/TAXI/IntList.h
3-ConShelling2.o: ../../pigale/incl/TAXI/KantShel.h
3-ConShelling2.o: ../../pigale/incl/TAXI/graphs.h
3-ConShelling2.o: ../../pigale/incl/TAXI/graph.h
3-ConShelling2.o: ../../pigale/incl/TAXI/elmt.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tprop.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tbase.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tpropio.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tstring.h
3-ConShelling2.o: ../../pigale/incl/TAXI/propdef.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tpoint.h
3-ConShelling2.o: ../../pigale/incl/TAXI/IntList.h
3-ConShelling2.o: ../../pigale/incl/TAXI/color.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tmessage.h
Barycenter.o: ../../pigale/incl/TAXI/smatrix.h
Barycenter.o: ../../pigale/incl/TAXI/Tsvector.h
Barycenter.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Barycenter.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/graph.h
Barycenter.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tpropio.h
Barycenter.o: ../../pigale/incl/TAXI/Tstring.h
Barycenter.o: ../../pigale/incl/TAXI/propdef.h
Barycenter.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
Barycenter.o: ../../pigale/incl/TAXI/Tmessage.h
BFS.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
BFS.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
BFS.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
BFS.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
BFS.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
BFS.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tsmap.h
Biconnect.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/color.h
Biconnect.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Biconnect.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Biconnect.o: ../../pigale/incl/TAXI/Tsvector.h
Biconnect.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Biconnect.o: ../../pigale/incl/TAXI/Tstring.h
Biconnect.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Biconnect.o: ../../pigale/incl/TAXI/IntList.h
Bipolar.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Bipolar.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Bipolar.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Bipolar.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Bipolar.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Bipolar.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tmessage.h
Bipolar.o: ../../pigale/incl/TAXI/color.h
BipPlanarize.o: ../../pigale/incl/TAXI/graphs.h
BipPlanarize.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tprop.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tsvector.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tbase.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tdebug.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tpropio.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tstring.h
BipPlanarize.o: ../../pigale/incl/TAXI/propdef.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tpoint.h
BipPlanarize.o: ../../pigale/incl/TAXI/color.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tmessage.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tbase.h
CotreeCritical.o: ../../pigale/incl/TAXI/CotreeCritical.h
CotreeCritical.o: ../../pigale/incl/TAXI/DFSGraph.h
CotreeCritical.o: ../../pigale/incl/TAXI/color.h
CotreeCritical.o: ../../pigale/incl/TAXI/graphs.h
CotreeCritical.o: ../../pigale/incl/TAXI/graph.h
CotreeCritical.o: ../../pigale/incl/TAXI/elmt.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tprop.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tsvector.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tdebug.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tpropio.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tstring.h
CotreeCritical.o: ../../pigale/incl/TAXI/propdef.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tpoint.h
CotreeCritical.o: ../../pigale/incl/TAXI/DFSLow.h
CotreeCritical.o: ../../pigale/incl/TAXI/lralgo.h
CotreeCritical.o: ../../pigale/incl/TAXI/bicon.h
Debug.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tmessage.h
DFS.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
DFS.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
DFS.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
DFS.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
DFS.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
DFS.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/bicon.h
DFS.o: ../../pigale/incl/TAXI/color.h
DFSGraph.o: ../../pigale/incl/TAXI/DFSLow.h ../../pigale/incl/TAXI/color.h
DFSGraph.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/graphs.h
DFSGraph.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
DFSGraph.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
DFSGraph.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
DFSGraph.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
DFSGraph.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
DFSGraph.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/bicon.h
DFSMap.o: ../../pigale/incl/TAXI/DFSLow.h ../../pigale/incl/TAXI/color.h
DFSMap.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/graphs.h
DFSMap.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
DFSMap.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
DFSMap.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
DFSMap.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
DFSMap.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
DFSMap.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/bicon.h
DFSMap.o: ../../pigale/incl/TAXI/embed.h
Diagonalise.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Diagonalise.o: ../../pigale/incl/TAXI/Tmessage.h
Diagonalise.o: ../../pigale/incl/TAXI/Tsvector.h
Diagonalise.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/netcut.h
Diagonalise.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Diagonalise.o: ../../pigale/incl/TAXI/Tprop.h
Diagonalise.o: ../../pigale/incl/TAXI/Tpropio.h
Diagonalise.o: ../../pigale/incl/TAXI/Tstring.h
Diagonalise.o: ../../pigale/incl/TAXI/propdef.h
Diagonalise.o: ../../pigale/incl/TAXI/Tpoint.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
Embed2Pages.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tprop.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tsvector.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tdebug.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tpropio.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tstring.h
Embed2Pages.o: ../../pigale/incl/TAXI/propdef.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tpoint.h
Embed2Pages.o: ../../pigale/incl/TAXI/IntList.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tmessage.h
Embed2Pages.o: ../../pigale/incl/TAXI/color.h
EmbedFPP.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
EmbedFPP.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/SchPack.h
EmbedFPP.o: ../../pigale/incl/TAXI/IntList.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/graphs.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/graph.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/elmt.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tprop.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tsvector.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tbase.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tdebug.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tpropio.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tstring.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/propdef.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tpoint.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tmessage.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/color.h
EmbedVision.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
EmbedVision.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
EmbedVision.o: ../../pigale/incl/TAXI/Tprop.h
EmbedVision.o: ../../pigale/incl/TAXI/Tsvector.h
EmbedVision.o: ../../pigale/incl/TAXI/Tdebug.h
EmbedVision.o: ../../pigale/incl/TAXI/Tpropio.h
EmbedVision.o: ../../pigale/incl/TAXI/Tstring.h
EmbedVision.o: ../../pigale/incl/TAXI/propdef.h
EmbedVision.o: ../../pigale/incl/TAXI/Tpoint.h
EmbedVision.o: ../../pigale/incl/TAXI/Tmessage.h
EmbedVision.o: ../../pigale/incl/TAXI/MaxPath.h
EmbedVision.o: ../../pigale/incl/TAXI/color.h
FastTwit.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/graph.h
FastTwit.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
FastTwit.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
FastTwit.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
FastTwit.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
FastTwit.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
FastTwit.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/color.h
FastTwit.o: ../../pigale/incl/TAXI/bicon.h
File.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tstring.h
File.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
File.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
File.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
File.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
File.o: ../../pigale/incl/TAXI/Tgf.h ../../pigale/incl/TAXI/Tproptgf.h
File.o: ../../pigale/incl/TAXI/Tfile.h ../../pigale/incl/TAXI/graphs.h
File.o: ../../pigale/incl/TAXI/Tsmap.h ../../pigale/incl/TAXI/color.h
File.o: ../../pigale/incl/TAXI/Tmessage.h
Generate.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tdebug.h
Generate.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Generate.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Generate.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
Generate.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Generate.o: ../../pigale/incl/TAXI/propdef.h
GeoAlgs.o: ../../pigale/incl/TAXI/smatrix.h ../../pigale/incl/TAXI/Tsvector.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
GeoAlgs.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tstring.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/graphs.h
GeoAlgs.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/Tprop.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/propdef.h
GeoAlgs.o: ../../pigale/incl/TAXI/color.h
Graph.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tbase.h
Graph.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Graph.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Graph.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Graph.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Graph.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/bicon.h
Graph.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
k-InfOrient.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tprop.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tsvector.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tdebug.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tpropio.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tstring.h
k-InfOrient.o: ../../pigale/incl/TAXI/propdef.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tpoint.h
k-InfOrient.o: ../../pigale/incl/TAXI/IntList.h
LR-Algo.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
LR-Algo.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
LR-Algo.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
LR-Algo.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
LR-Algo.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/graphs.h
LR-Algo.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/bicon.h
LR-Algo.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/DFSGraph.h
LR-Algo.o: ../../pigale/incl/TAXI/color.h
LR-Map.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
LR-Map.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
LR-Map.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
LR-Map.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
LR-Map.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/graphs.h
LR-Map.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/bicon.h
LR-Map.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/DFSGraph.h
LR-Map.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/embed.h
LR-Map.o: ../../pigale/incl/TAXI/DFSLow.h
mark.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
mark.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
mark.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
mark.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
mark.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
mark.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/bicon.h
mark.o: ../../pigale/incl/TAXI/Tmessage.h
MaxPlanar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tpropio.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tstring.h
MaxPlanar.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/color.h
MaxPlanar.o: ../../pigale/incl/TAXI/DFSGraph.h
MaxPlanar.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
more.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
more.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/elmt.h
more.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tpropio.h
more.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/graph.h
more.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
netcut.o: ../../pigale/incl/TAXI/netcut.h ../../pigale/incl/TAXI/graphs.h
netcut.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
netcut.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
netcut.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
netcut.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
netcut.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
netcut.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/Tmessage.h
NewPolar.o: ../../pigale/incl/TAXI/Dos.h ../../pigale/incl/TAXI/graphs.h
NewPolar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
NewPolar.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
NewPolar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
NewPolar.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
NewPolar.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
NpBiconnect.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tprop.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tsvector.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tdebug.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tpropio.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tstring.h
NpBiconnect.o: ../../pigale/incl/TAXI/propdef.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tpoint.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tmessage.h
NpBiconnect.o: ../../pigale/incl/TAXI/color.h
Planar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Planar.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Planar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Planar.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Planar.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/bicon.h
Planar.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/graphs.h
Planar.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/DFSGraph.h
Planar.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/embed.h
Planar.o: ../../pigale/incl/TAXI/DFSLow.h ../../pigale/incl/TAXI/ndfs.h
PropTgf.o: ../../pigale/incl/TAXI/Tproptgf.h ../../pigale/incl/TAXI/Tprop.h
PropTgf.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
PropTgf.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/elmt.h
PropTgf.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
PropTgf.o: ../../pigale/incl/TAXI/Tgf.h ../../pigale/incl/TAXI/Tpoint.h
PSet.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
PSet.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
PSet.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tpropio.h
PSet.o: ../../pigale/incl/TAXI/Tstring.h
reduce.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/graphs.h
reduce.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
reduce.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
reduce.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
reduce.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/propdef.h
reduce.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tmessage.h
Schnyder.o: ../../pigale/incl/TAXI/SchPack.h ../../pigale/incl/TAXI/graphs.h
Schnyder.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Schnyder.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Schnyder.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Schnyder.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Schnyder.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Schnyder.o: ../../pigale/incl/TAXI/IntList.h
Schnyder.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
STList.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
STList.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
STList.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
STList.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
STList.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
STList.o: ../../pigale/incl/TAXI/Tpoint.h
Tgf.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tgf.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/graphs.h
TopoAlgs.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/propdef.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/bicon.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Twit.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/lralgo.h
Twit.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Twit.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Twit.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tpropio.h
Twit.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Twit.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
Twit.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/color.h
Twit.o: ../../pigale/incl/TAXI/bicon.h
VertexPacking.o: ../../pigale/incl/TAXI/SchPack.h
VertexPacking.o: ../../pigale/incl/TAXI/graphs.h
VertexPacking.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
VertexPacking.o: ../../pigale/incl/TAXI/Tprop.h
VertexPacking.o: ../../pigale/incl/TAXI/Tsvector.h
VertexPacking.o: ../../pigale/incl/TAXI/Tbase.h
VertexPacking.o: ../../pigale/incl/TAXI/Tdebug.h
VertexPacking.o: ../../pigale/incl/TAXI/Tpropio.h
VertexPacking.o: ../../pigale/incl/TAXI/Tstring.h
VertexPacking.o: ../../pigale/incl/TAXI/propdef.h
VertexPacking.o: ../../pigale/incl/TAXI/Tpoint.h
VertexPacking.o: ../../pigale/incl/TAXI/IntList.h
Vision.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
Vision.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Vision.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Vision.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Tpropio.h
Vision.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Vision.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tmessage.h
Vision.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/MaxPath.h
Vision.o: ../../pigale/incl/TAXI/Channel.h
