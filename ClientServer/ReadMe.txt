 
This  first implementation of a simple Client-Server program.
These two programs make use of the Qt library  but as the  client reads 
its instructions from stdin and its results  to stdout, 
it should not be difficult for applications to communicate with the server.

To experiment the program  two example files are provided (data1 and data2)
which shows some aspects of the proposed syntax.

The server and client use a TCP connection on port 4242.

To launch the application, first launch the server:
XServer (serves concurrently != clients)
or pigale -server (serves one client at a time)
then the client:
Client or Client < data1
If the Client does not run on the same machine as the server:
Client toto.truc.fr < data1

Two graphs  are given: graph0.txt and ex.tgf. 
The latter one is a binary file and can only be read on x86 compatible machines.
The commands that can be used are nearly all the ones found in QT/Action.h,
that is all the macros plus some other ones:

Commands not in found in  Pigale macros
R_GRAPH   // load a  graph  (now on the server side)
N_GRAPH	  // creates an emty graph
N_V	  // create n vertices
N_E	  // create an edge  
S_DEBUG   // commands are echoed while being executed
PNG       // to get a png image of the current drawing

Commands to retrieve information:
N  
M  
SIMPLE			// simple graph
PLAN			// planar
OPLAN			// outer planar
SR			// serie-parallel
MPLAN			// triangulated planar graph
BI			// Bipartite graph
MBI			// Max planar Bipartite graph
REG			// Regular
C1			// Connected
C2			// 2-Connected	
C3			// 3-Connected
MIN_D			// minimal degree
MAX_D			// maximal degree
COORD			// retrieve coords
VLAB			// retrieve labels (long integers)  

All the other commands could be macros.
To augment the graph:
CONNECT
BICONNECT
BICONNECT_NP
TRIANGULATE_V
TRIANGULATE_ZZ
TRIANGULATE_3C
QUADRANGULATE_V_
BISSECT_ALL_E

Embeddings:
SCHNYDER_E
SCHNYDER_V
FPP_FARY
TUTTE_CIRCLE 
TUTTE 
SPRING 
SPRING_PM 
JACQUARD 
VISION 
CONTATC_BIPARTI 
POLAR
FPP_RECTI 
GVISION 
T_CONTACT 
EMBED-3d 

Duality and angle graphs:
DUAL  
DUAL_G  
ANGLE  
ANGLE_G  

Remove options:
REMOVE_ISOLATED_V  
REMOVE_LOOPS  
REMOVE_MULTIPLE_E  
REMOVE_BRIDGES 

Generators: 
GEN_GRID  
GEN_COMPLETE  
GEN_COMPLETE_BIP  
GEN_RANDOM  
GEN_PLANAR  
GEN_PLANAR_2C  
GEN_PLANAR_3C  
GEN_PLANAR_3R_2C  
GEN_PLANAR_3R_3C  
GEN_PLANAR_3R_D4C  
GEN_PLANAR_4R_C  
GEN_PLANAR_4R_3C  
GEN_PLANAR_4R_BIP  
GEN_PLANAR_P_BIP  
GEN_PLANAR_BIP  
GEN_PLANAR_BIP_2C  
GEN_PLANAR_BIP_3C  

Algorithms:
KURATOWSKI  
COTREE_CRITICAL  
NPSET  
FIND_MAX_PLANAR  
RESET_COLORS  
NETCUT  
USE_GEOMETRIC_CIR  
USE_LRALGO_CIR  
SYMETRIE  
COLOR_BIPARTI  
COLOR_EXT_FACE  
COLOR_NON_CRITIC  

Orientation algorithms:
ORIENT_EDGES  
ORIENT_INF  
ORIENT_TRICON  
ORIENT_BIPARTITE  
ORIENT_SCHNYDER  
ORIENT_BIPOLAR  
ORIENT_BIPOLAR_NON_PLANAR

User defined algorithms:  
TEST_1  
TEST_2  
TEST_3  

Some specific input of the client are:
- lines starting with '#' are comments
- lines starting with ':' which modify the client-server behaviour
For example
:D    // echo comments
:d    // stop echoing
:!    // end of file


