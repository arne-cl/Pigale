/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

#ifndef  ACTIONS_H_INCLUDED
#define  ACTIONS_H_INCLUDED 

//DO NOT ADD WHITES

//Augment
#define  A_AUGMENT                  (100)
#define  A_AUGMENT_CONNECT          (A_AUGMENT+1)
#define  A_AUGMENT_BICONNECT        (A_AUGMENT+2)
#define  A_AUGMENT_BICONNECT_6      (A_AUGMENT+3)
#define  A_AUGMENT_BICONNECT_NP     (A_AUGMENT+4)
#define  A_AUGMENT_TRIANGULATE_V    (A_AUGMENT+5)
#define  A_AUGMENT_TRIANGULATE_ZZ   (A_AUGMENT+6)
#define  A_AUGMENT_TRIANGULATE_3C   (A_AUGMENT+7)
#define  A_AUGMENT_QUADRANGULATE_V  (A_AUGMENT+8)
#define  A_AUGMENT_BISSECT_ALL_E    (A_AUGMENT+9)
#define  A_AUGMENT_END              (A_AUGMENT+99)

//Embed
#define  A_EMBED                    (200)
#define  A_EMBED_SCHNYDER_E         (A_EMBED+1)         // EMB_SCHNYDER_E
#define  A_EMBED_SCHNYDER_V         (A_EMBED+2)         // EMB_SCHNYDER_V 
#define  A_EMBED_FPP                (A_EMBED+3)         // EMB_FPP
#define  A_EMBED_TUTTE_CIRCLE       (A_EMBED+4)         // EMB_TUTTE_C
#define  A_EMBED_TUTTE              (A_EMBED+5)
#define  A_EMBED_VISION             (A_EMBED+50)
#define  A_EMBED_CONTACT_BIP        (A_EMBED+51)
#define  A_EMBED_POLAR              (A_EMBED+52)
#define  A_EMBED_FPP_RECTI          (A_EMBED+53)         // EMB_FPP_R
#define  A_EMBED_FPP_GVISION        (A_EMBED+54)
#define  A_EMBED_T_CONTACT          (A_EMBED+55)
#define  A_EMBED_3d                 (A_EMBED+98)
#define  A_EMBED_END                (A_EMBED+99)

//Graph
#define  A_GRAPH                    (300)
#define  A_GRAPH_DUAL               (A_GRAPH+1)
#define  A_GRAPH_DUAL_G             (A_GRAPH+2)
#define  A_GRAPH_ANGLE              (A_GRAPH+3)
#define  A_GRAPH_ANGLE_G            (A_GRAPH+4)
#define  A_GRAPH_END                (A_GRAPH+99)

//Remove
#define  A_REMOVE                  (400)
#define  A_REMOVE_ISOLATED_V       (A_REMOVE+1)
#define  A_REMOVE_LOOPS            (A_REMOVE+2)
#define  A_REMOVE_MULTIPLE_E       (A_REMOVE+3)
#define  A_REMOVE_BRIDGES          (A_REMOVE+4)
#define  A_REMOVE_COLOR_V          (A_REMOVE+5)
#define  A_REMOVE_COLOR_E          (A_REMOVE+6)
#define  A_REMOVE_THICK_E          (A_REMOVE+7)
#define  A_REMOVE_END              (A_REMOVE+99)

//Generate
#define  A_GENERATE                 (500)
#define  A_GENERATE_GRID            (A_GENERATE+1)
#define  A_GENERATE_COMPLETE        (A_GENERATE+2)
#define  A_GENERATE_COMPLETE_BIP    (A_GENERATE+3)
#define  A_GENERATE_RANDOM          (A_GENERATE+4)
#define  A_GENERATE_P               (A_GENERATE+5)
#define  A_GENERATE_P_2C            (A_GENERATE+6)
#define  A_GENERATE_P_3C            (A_GENERATE+7)
#define  A_GENERATE_P_3R_2C         (A_GENERATE+10)
#define  A_GENERATE_P_3R_3C         (A_GENERATE+11)
#define  A_GENERATE_P_3R_D4C        (A_GENERATE+12)
#define  A_GENERATE_P_4R_C          (A_GENERATE+13)
#define  A_GENERATE_P_4R_2C         (A_GENERATE+14)
#define  A_GENERATE_P_4R_3C         (A_GENERATE+15)
#define  A_GENERATE_P_4R_BIP        (A_GENERATE+16)
#define  A_GENERATE_P_BIP           (A_GENERATE+17)
#define  A_GENERATE_P_BIP_2C        (A_GENERATE+18)
#define  A_GENERATE_P_BIP_3C        (A_GENERATE+19)
#define  A_GENERATE_END             (A_GENERATE+99)

//Algo
#define  A_ALGO                    (600)
#define  A_ALGO_KURATOWSKI         (A_ALGO+1)
#define  A_ALGO_COTREE_CRITICAL    (A_ALGO+2)
#define  A_ALGO_NPSET              (A_ALGO+3)
#define  A_ALGO_MAX_PLANAR         (A_ALGO+4)
#define  A_ALGO_RESET_COLORS       (A_ALGO+5)
#define  A_ALGO_NETCUT             (A_ALGO+6)
#define  A_ALGO_GEOMETRIC_CIR      (A_ALGO+7)
#define  A_ALGO_LRALGO_CIR         (A_ALGO+8)
#define  A_ALGO_SYM                (A_ALGO+9)
#define  A_ALGO_COLOR_BIPARTI      (A_ALGO+10)
#define  A_ALGO_COLOR_EXT          (A_ALGO+11)
#define  A_ALGO_COLOR_NON_CRITIC   (A_ALGO+12)
#define  A_ALGO_END                (A_ALGO+99)

//Orient
#define  A_ORIENT                  (700)
#define  A_ORIENT_E                (A_ORIENT+1)
#define  A_ORIENT_INF              (A_ORIENT+2)
#define  A_ORIENT_TRICON           (A_ORIENT+3)
#define  A_ORIENT_BIPAR            (A_ORIENT+4)
#define  A_ORIENT_SCHNYDER         (A_ORIENT+5)
#define  A_ORIENT_BIPOLAR          (A_ORIENT+6)
#define  A_ORIENT_END              (A_ORIENT+99)


// Defines used by the server

#define A_SERVER               (2000)

#define A_INFO                 (A_SERVER)     
#define A_INFO_N               (A_INFO+1)     // N
#define A_INFO_M               (A_INFO+2)     // M
#define A_INFO_SIMPLE          (A_INFO+3)     // SIMPLE
#define A_INFO_PLANAR          (A_INFO+4)     // PLAN
#define A_INFO_OUTER_PLANAR    (A_INFO+5)     // OPLAN 
#define A_INFO_SERIE_PAR       (A_INFO+6)     // SR
#define A_INFO_MAX_PLANAR      (A_INFO+7)     // MPLAN
#define A_INFO_BIPAR           (A_INFO+8)     // BI
#define A_INFO_MAX_BIPAR       (A_INFO+9)     // MBI
#define A_INFO_REGULIER        (A_INFO+10)    // REG
#define A_INFO_CON1            (A_INFO+11)    // C1
#define A_INFO_CON2            (A_INFO+12)    // C2
#define A_INFO_CON3            (A_INFO+13)    // C3
#define A_INFO_MIN_D           (A_INFO+14)    // MIN_D 
#define A_INFO_MAX_D           (A_INFO+15)    // MAX_D
#define A_INFO_COORD           (A_INFO+16)    // COORD
#define A_INFO_VLABEL          (A_INFO+17)    // VLAB
#define A_INFO_END             (A_INFO+99)     

#define A_INPUT                (A_SERVER+100)    
#define A_INPUT_READ_GRAPH     (A_INPUT+1)    //  R_GRAPH
#define A_INPUT_NEW_GRAPH      (A_INPUT+2)    //  N_GRAPH
#define A_INPUT_NEW_VERTEX     (A_INPUT+3)    //  N_V
#define A_INPUT_NEW_EDGE       (A_INPUT+4)    //  N_E
#define A_INPUT_END            (A_INPUT+99)    

#define SERVER_DEBUG           (A_SERVER+999)  //  S_DEBUG  

// server message errors
#define A_ERRORS               (-10000) 
#define UNKNOWN_COMMAND        (A_ERRORS-1) 
#define WRONG_PARAMETERS       (A_ERRORS-2) 
#define PROP_NOT_DEFINED       (A_ERRORS-3) 
#define ACTION_NOT_APPLICABLE  (A_ERRORS-4) 
#define ACTION_NOT_INT         (A_ERRORS-5) 
#define READ_ERROR             (A_ERRORS-6) 

// server separators
const char ACTION_SEP = ':';  //to separate actions
const char PARAM_SEP  = ';';  //to separate parameters
const char TYPE_SEP   = ',';  //to separate the parameter from its type if any

#endif
