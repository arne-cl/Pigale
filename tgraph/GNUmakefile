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
AWK=awk

#OPTIMIZED OPTIONS
CFLAGSopt = -O3   $(PIC)
CFLAGSopt += -Wall -Wstrict-prototypes -Wpointer-arith -Wno-deprecated -pipe
#CFLAGSopt +=  -fomit-frame-pointer (does not work with GCC 3)

#DEBUGGING OPTIONS (-pg -a for sampling)
CFLAGSdbg  =  -g -Wall -Wstrict-prototypes -Wpointer-arith -Werror -pipe $(PIC)
CFLAGSdbg +=  -DTDEBUG

DLIB    = $(PIGALEPATH)/lib/lib$L$(OPT).so
LIBS    = -L$(PIGALEPATH)/lib -ltgraph$(OPT)
CFLAGS  = $(CFLAGS$(OPT))
VPATH   = $(PIGALEPATH)/tmp.$(OPT)
FILES   :=  $(wildcard *.cpp)
SPECIAL := PropName.cpp
FILES   := $(filter-out $(SPECIAL), $(FILES))
FILES2  := $(FILES)
FILES2  += $(SPECIAL)
OBJECTS =  $(patsubst %.cpp,%.o, $(FILES2))
OBJ =  $(patsubst %.o,$(VPATH)/%.o, $(OBJECTS))

all :  $(DLIB)

PropName.cpp : $(PIGALEPATH)/incl/TAXI/propdef.h
	$(AWK) -f propname.awk $(PIGALEPATH)/incl/TAXI/propdef.h > PropName.cpp

.cpp.o: $*.cpp
	$(CC) -c $(CFLAGS) $(IFLAGS) -o  $(VPATH)/$*.o   $*.cpp

$(DLIB):	$(OBJECTS)
	@rm -f $(PIGALEPATH)/lib/lib$L$(OPT).so
	@$(CC) -shared  -o $(PIGALEPATH)/lib/lib$L$(OPT).so  $(OBJ)

clean:
	rm -f *.$(OPT) $(DLIB) 
	rm -f *~ *bak $(OBJ) *.flc

depend:
	@makedepend -o.o -fGNUmakefile $(IFLAGS)  -Y $(FILES) 2> /dev/null

nodepend:
	@makedepend -o.o -fGNUmakefile  -Y  2> /dev/null

list:
	../makelist *.cpp >Make.lst


# DO NOT DELETE

3-ConOrientTriang.o: ../../pigale/incl/TAXI/KantShel.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/graphs.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tbase.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/graph.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/elmt.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tprop.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConOrientTriang.o: ../../pigale/incl/TAXI/Errors.h
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
3-ConShelling1.o: ../../pigale/incl/TAXI/Tbase.h
3-ConShelling1.o: ../../pigale/incl/TAXI/graph.h
3-ConShelling1.o: ../../pigale/incl/TAXI/elmt.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tprop.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Errors.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tpropio.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tstring.h
3-ConShelling1.o: ../../pigale/incl/TAXI/propdef.h
3-ConShelling1.o: ../../pigale/incl/TAXI/Tpoint.h
3-ConShelling1.o: ../../pigale/incl/TAXI/IntList.h
3-ConShelling2.o: ../../pigale/incl/TAXI/KantShel.h
3-ConShelling2.o: ../../pigale/incl/TAXI/graphs.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tbase.h
3-ConShelling2.o: ../../pigale/incl/TAXI/graph.h
3-ConShelling2.o: ../../pigale/incl/TAXI/elmt.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tprop.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tsvector.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tdebug.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Errors.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tpropio.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tstring.h
3-ConShelling2.o: ../../pigale/incl/TAXI/propdef.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tpoint.h
3-ConShelling2.o: ../../pigale/incl/TAXI/IntList.h
3-ConShelling2.o: ../../pigale/incl/TAXI/color.h
3-ConShelling2.o: ../../pigale/incl/TAXI/Tmessage.h
Barycenter.o: ../../pigale/incl/TAXI/smatrix.h ../../pigale/incl/TAXI/Tbase.h
Barycenter.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/elmt.h
Barycenter.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Barycenter.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/Tprop.h
Barycenter.o: ../../pigale/incl/TAXI/Tpropio.h
Barycenter.o: ../../pigale/incl/TAXI/Tstring.h
Barycenter.o: ../../pigale/incl/TAXI/propdef.h
Barycenter.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
Barycenter.o: ../../pigale/incl/TAXI/Tmessage.h
BFS.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
BFS.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
BFS.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
BFS.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
BFS.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
BFS.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
BFS.o: ../../pigale/incl/TAXI/Tsmap.h
Biconnect.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/color.h
Biconnect.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Biconnect.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Biconnect.o: ../../pigale/incl/TAXI/Tsvector.h
Biconnect.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Biconnect.o: ../../pigale/incl/TAXI/Tpropio.h
Biconnect.o: ../../pigale/incl/TAXI/Tstring.h
Biconnect.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Biconnect.o: ../../pigale/incl/TAXI/IntList.h
Bipolar.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
Bipolar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Bipolar.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Bipolar.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Bipolar.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Bipolar.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Bipolar.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Bipolar.o: ../../pigale/incl/TAXI/DFSGraph.h
BipPlanarize.o: ../../pigale/incl/TAXI/graphs.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
BipPlanarize.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tsvector.h
BipPlanarize.o: ../../pigale/incl/TAXI/Tdebug.h
BipPlanarize.o: ../../pigale/incl/TAXI/Errors.h
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
CotreeCritical.o: ../../pigale/incl/TAXI/Errors.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tpropio.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tstring.h
CotreeCritical.o: ../../pigale/incl/TAXI/propdef.h
CotreeCritical.o: ../../pigale/incl/TAXI/Tpoint.h
CotreeCritical.o: ../../pigale/incl/TAXI/DFSLow.h
CotreeCritical.o: ../../pigale/incl/TAXI/lralgo.h
CotreeCritical.o: ../../pigale/incl/TAXI/bicon.h
Debug.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Debug.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tmessage.h
DFS.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
DFS.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
DFS.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
DFS.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
DFS.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
DFS.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
DFS.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/color.h
DFSGraph.o: ../../pigale/incl/TAXI/DFSLow.h ../../pigale/incl/TAXI/Tbase.h
DFSGraph.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/DFSGraph.h
DFSGraph.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
DFSGraph.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
DFSGraph.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
DFSGraph.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
DFSGraph.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
DFSGraph.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/lralgo.h
DFSGraph.o: ../../pigale/incl/TAXI/bicon.h
DFSMap.o: ../../pigale/incl/TAXI/DFSLow.h ../../pigale/incl/TAXI/Tbase.h
DFSMap.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/DFSGraph.h
DFSMap.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
DFSMap.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
DFSMap.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
DFSMap.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
DFSMap.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
DFSMap.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/lralgo.h
DFSMap.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/embed.h
Diagonalise.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tdebug.h
Diagonalise.o: ../../pigale/incl/TAXI/Errors.h
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
Embed2Pages.o: ../../pigale/incl/TAXI/Errors.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tpropio.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tstring.h
Embed2Pages.o: ../../pigale/incl/TAXI/propdef.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tpoint.h
Embed2Pages.o: ../../pigale/incl/TAXI/IntList.h
Embed2Pages.o: ../../pigale/incl/TAXI/Tmessage.h
Embed2Pages.o: ../../pigale/incl/TAXI/color.h
EmbedFPP.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
EmbedFPP.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
EmbedFPP.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
EmbedFPP.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
EmbedFPP.o: ../../pigale/incl/TAXI/SchPack.h ../../pigale/incl/TAXI/IntList.h
EmbedFPP.o: ../../pigale/incl/TAXI/MaxPath.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/graphs.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tbase.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/graph.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/elmt.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tprop.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tsvector.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Tdebug.h
EmbedTutteCircle.o: ../../pigale/incl/TAXI/Errors.h
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
EmbedVision.o: ../../pigale/incl/TAXI/Errors.h
EmbedVision.o: ../../pigale/incl/TAXI/Tpropio.h
EmbedVision.o: ../../pigale/incl/TAXI/Tstring.h
EmbedVision.o: ../../pigale/incl/TAXI/propdef.h
EmbedVision.o: ../../pigale/incl/TAXI/Tpoint.h
EmbedVision.o: ../../pigale/incl/TAXI/Tmessage.h
EmbedVision.o: ../../pigale/incl/TAXI/MaxPath.h
EmbedVision.o: ../../pigale/incl/TAXI/color.h
FastTwit.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/Tbase.h
FastTwit.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
FastTwit.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
FastTwit.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
FastTwit.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
FastTwit.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/graphs.h
FastTwit.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/DFSGraph.h
FastTwit.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/bicon.h
File.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tstring.h
File.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
File.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
File.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
File.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/propdef.h
File.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tgf.h
File.o: ../../pigale/incl/TAXI/Tproptgf.h ../../pigale/incl/TAXI/Tfile.h
File.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tsmap.h
File.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/Tmessage.h
Generate.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tbase.h
Generate.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Generate.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Generate.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Generate.o: ../../pigale/incl/TAXI/Tsvector.h
Generate.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Generate.o: ../../pigale/incl/TAXI/propdef.h
GeoAlgs.o: ../../pigale/incl/TAXI/smatrix.h ../../pigale/incl/TAXI/Tbase.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/elmt.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/Tpoint.h
GeoAlgs.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
GeoAlgs.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tpropio.h
GeoAlgs.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/color.h
Graph.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/Tbase.h
Graph.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
Graph.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Graph.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Graph.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Graph.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Graph.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/Tmessage.h
Graph.o: ../../pigale/incl/TAXI/color.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
k-InfOrient.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tprop.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tsvector.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tdebug.h
k-InfOrient.o: ../../pigale/incl/TAXI/Errors.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tpropio.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tstring.h
k-InfOrient.o: ../../pigale/incl/TAXI/propdef.h
k-InfOrient.o: ../../pigale/incl/TAXI/Tpoint.h
k-InfOrient.o: ../../pigale/incl/TAXI/IntList.h
LR-Algo.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
LR-Algo.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tprop.h
LR-Algo.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
LR-Algo.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
LR-Algo.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
LR-Algo.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
LR-Algo.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/lralgo.h
LR-Algo.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/color.h
LR-Map.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
LR-Map.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tprop.h
LR-Map.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
LR-Map.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
LR-Map.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
LR-Map.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
LR-Map.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/lralgo.h
LR-Map.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/color.h
LR-Map.o: ../../pigale/incl/TAXI/embed.h ../../pigale/incl/TAXI/DFSLow.h
LR-Map.o: ../../pigale/incl/TAXI/Tmessage.h
mark.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
mark.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
mark.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
mark.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
mark.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
mark.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
mark.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/Tmessage.h
MaxPlanar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tprop.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tsvector.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tpropio.h
MaxPlanar.o: ../../pigale/incl/TAXI/Tstring.h
MaxPlanar.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/color.h
MaxPlanar.o: ../../pigale/incl/TAXI/DFSGraph.h
MaxPlanar.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
more.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tbase.h
more.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tdebug.h
more.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tprop.h
more.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
more.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/propdef.h
more.o: ../../pigale/incl/TAXI/Tpoint.h
netcut.o: ../../pigale/incl/TAXI/netcut.h ../../pigale/incl/TAXI/graphs.h
netcut.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
netcut.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
netcut.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
netcut.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
netcut.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
netcut.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/color.h
netcut.o: ../../pigale/incl/TAXI/Tmessage.h
NewPolar.o: ../../pigale/incl/TAXI/Dos.h ../../pigale/incl/TAXI/graphs.h
NewPolar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
NewPolar.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
NewPolar.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
NewPolar.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
NewPolar.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
NewPolar.o: ../../pigale/incl/TAXI/Tpoint.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
NpBiconnect.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tprop.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tsvector.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tdebug.h
NpBiconnect.o: ../../pigale/incl/TAXI/Errors.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tpropio.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tstring.h
NpBiconnect.o: ../../pigale/incl/TAXI/propdef.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tpoint.h
NpBiconnect.o: ../../pigale/incl/TAXI/Tmessage.h
NpBiconnect.o: ../../pigale/incl/TAXI/color.h
Planar.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Planar.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tprop.h
Planar.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Planar.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Planar.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Planar.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/lralgo.h
Planar.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tpoint.h
Planar.o: ../../pigale/incl/TAXI/DFSGraph.h ../../pigale/incl/TAXI/color.h
Planar.o: ../../pigale/incl/TAXI/embed.h ../../pigale/incl/TAXI/DFSLow.h
Planar.o: ../../pigale/incl/TAXI/ndfs.h ../../pigale/incl/TAXI/Tmessage.h
PropTgf.o: ../../pigale/incl/TAXI/Tproptgf.h ../../pigale/incl/TAXI/Tprop.h
PropTgf.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tsvector.h
PropTgf.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tdebug.h
PropTgf.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
PropTgf.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/Tgf.h
PropTgf.o: ../../pigale/incl/TAXI/Tpoint.h
PSet.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tbase.h
PSet.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/elmt.h
PSet.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
PSet.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
reduce.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/Tbase.h
reduce.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
reduce.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
reduce.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
reduce.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
reduce.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
reduce.o: ../../pigale/incl/TAXI/Tmessage.h
Schnyder.o: ../../pigale/incl/TAXI/SchPack.h ../../pigale/incl/TAXI/graphs.h
Schnyder.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graph.h
Schnyder.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
Schnyder.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
Schnyder.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
Schnyder.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/propdef.h
Schnyder.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/IntList.h
Schnyder.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
STList.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/Tbase.h
STList.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
STList.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
STList.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
STList.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
STList.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
STList.o: ../../pigale/incl/TAXI/Tmessage.h
Tgf.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/Tgf.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tstring.h ../../pigale/incl/TAXI/Tbase.h
TopoAlgs.o: ../../pigale/incl/TAXI/graphs.h ../../pigale/incl/TAXI/graph.h
TopoAlgs.o: ../../pigale/incl/TAXI/elmt.h ../../pigale/incl/TAXI/Tprop.h
TopoAlgs.o: ../../pigale/incl/TAXI/Tsvector.h ../../pigale/incl/TAXI/Tdebug.h
TopoAlgs.o: ../../pigale/incl/TAXI/Errors.h ../../pigale/incl/TAXI/Tpropio.h
TopoAlgs.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
TopoAlgs.o: ../../pigale/incl/TAXI/bicon.h ../../pigale/incl/TAXI/Tmessage.h
TopoAlgs.o: ../../pigale/incl/TAXI/color.h
Twit.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Twit.o: ../../pigale/incl/TAXI/lralgo.h ../../pigale/incl/TAXI/Tbase.h
Twit.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Twit.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Twit.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Twit.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/graphs.h
Twit.o: ../../pigale/incl/TAXI/Tpoint.h ../../pigale/incl/TAXI/DFSGraph.h
Twit.o: ../../pigale/incl/TAXI/color.h ../../pigale/incl/TAXI/bicon.h
VertexPacking.o: ../../pigale/incl/TAXI/SchPack.h
VertexPacking.o: ../../pigale/incl/TAXI/graphs.h
VertexPacking.o: ../../pigale/incl/TAXI/Tbase.h
VertexPacking.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
VertexPacking.o: ../../pigale/incl/TAXI/Tprop.h
VertexPacking.o: ../../pigale/incl/TAXI/Tsvector.h
VertexPacking.o: ../../pigale/incl/TAXI/Tdebug.h
VertexPacking.o: ../../pigale/incl/TAXI/Errors.h
VertexPacking.o: ../../pigale/incl/TAXI/Tpropio.h
VertexPacking.o: ../../pigale/incl/TAXI/Tstring.h
VertexPacking.o: ../../pigale/incl/TAXI/propdef.h
VertexPacking.o: ../../pigale/incl/TAXI/Tpoint.h
VertexPacking.o: ../../pigale/incl/TAXI/IntList.h
Vision.o: ../../pigale/incl/TAXI/Tbase.h ../../pigale/incl/TAXI/graphs.h
Vision.o: ../../pigale/incl/TAXI/graph.h ../../pigale/incl/TAXI/elmt.h
Vision.o: ../../pigale/incl/TAXI/Tprop.h ../../pigale/incl/TAXI/Tsvector.h
Vision.o: ../../pigale/incl/TAXI/Tdebug.h ../../pigale/incl/TAXI/Errors.h
Vision.o: ../../pigale/incl/TAXI/Tpropio.h ../../pigale/incl/TAXI/Tstring.h
Vision.o: ../../pigale/incl/TAXI/propdef.h ../../pigale/incl/TAXI/Tpoint.h
Vision.o: ../../pigale/incl/TAXI/Tmessage.h ../../pigale/incl/TAXI/color.h
Vision.o: ../../pigale/incl/TAXI/MaxPath.h ../../pigale/incl/TAXI/Channel.h
