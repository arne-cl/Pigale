 
This is the very first implementation of a simple Client-Server program.
These two programs make use of the Qt library  but as the  client reads 
its instructions from stdin and its results  to stdout 
it should not be difficult for applications to communicate with the server.
To experiment the program  two example files are provided (data1 and data2   which shows some 
aspects of the proposed syntax.
To launch the application:
./Server &
./XClient < data1
Two graphs  are given: graph0.txt and ex.tgf. 
The latter one is a binary file and can only be read on x86 compatible machines.
The server and client use a TCP connection on port 4242.
The commands that can be used are nearly all the ones found in QT/Action.h:
CONNECT
BICONNECT
BICONNECT_NP
TRIANGULATE_V
TRIANGULATE_ZZ
TRIANGULATE_3C
QUADRANGULATE_V_
BISSECT_ALL_E
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
DUAL  
DUAL_G  
ANGLE  
ANGLE_G  
REMOVE_ISOLATED_V  
REMOVE_LOOPS  
REMOVE_MULTIPLE_E  
REMOVE_BRIDGES  
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
ORIENT_EDGES  
ORIENT_INF  
ORIENT_TRICON  
ORIENT_BIPARTITE  
ORIENT_SCHNYDER  
ORIENT_BIPOLAR  
ORIENT_BIPOLAR_NON_PLANAR  
TEST_1  
TEST_2  
TEST_3  
Informations that can be retrieved:
N  
M  
SIMPLE  
PLAN  
OPLAN  
SR  
MPLAN  
BI  
MBI  
REG  
C1  
C2  
C3  
MIN_D  
MAX_D  
COORD  
VLAB  

Commands not in Pigale
R_GRAPH  
N_GRAPH  
N_V  
N_E  
S_DEBUG  

