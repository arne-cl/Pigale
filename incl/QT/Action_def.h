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

//Augment
#define  A_AUGMENT                  (100)
#define  A_AUGMENT_CONNECT          (A_AUGMENT + 1)
#define  A_AUGMENT_BICONNECT        (A_AUGMENT + 2)
#define  A_AUGMENT_BICONNECT_6      (A_AUGMENT + 3)
#define  A_AUGMENT_BICONNECT_NP     (A_AUGMENT + 4)
#define  A_AUGMENT_TRIANGULATE_V    (A_AUGMENT + 5)
#define  A_AUGMENT_TRIANGULATE_ZZ   (A_AUGMENT + 6)
#define  A_AUGMENT_TRIANGULATE_3C   (A_AUGMENT + 7)
#define  A_AUGMENT_QUADRANGULATE_V  (A_AUGMENT + 8)
#define  A_AUGMENT_BISSECT_ALL_E    (A_AUGMENT + 9)
#define  A_AUGMENT_END              (A_AUGMENT + 99)

//Embed
#define  A_EMBED                    (200)
#define  A_EMBED_SCHNYDER_E         (A_EMBED + 1)
#define  A_EMBED_SCHNYDER_V         (A_EMBED + 2)
#define  A_EMBED_FPP                (A_EMBED + 3)
#define  A_EMBED_TUTTE_CIRCLE       (A_EMBED + 4)
#define  A_EMBED_TUTTE              (A_EMBED + 5)
#define  A_EMBED_VISION             (A_EMBED + 50)
#define  A_EMBED_CONTACT_BIP        (A_EMBED + 51)
#define  A_EMBED_POLAR              (A_EMBED + 52)
#define  A_EMBED_FPP_RECTI          (A_EMBED + 53)
#define  A_EMBED_FPP_GVISION        (A_EMBED + 54)
#define  A_EMBED_T_CONTACT          (A_EMBED + 55)
#define  A_EMBED_3d                 (A_EMBED + 98)
#define  A_EMBED_END                (A_EMBED + 99)

//Graph
#define  A_GRAPH                    (300)
#define  A_GRAPH_DUAL               (A_GRAPH + 1)
#define  A_GRAPH_DUAL_G             (A_GRAPH + 2)
#define  A_GRAPH_ANGLE              (A_GRAPH + 3)
#define  A_GRAPH_ANGLE_G            (A_GRAPH + 4)
#define  A_GRAPH_END                (A_GRAPH + 99)

//Remove
#define  A_REMOVE                  (400)
#define  A_REMOVE_ISOLATED_V       (A_REMOVE + 1)
#define  A_REMOVE_LOOPS            (A_REMOVE + 2)
#define  A_REMOVE_MULTIPLE_E       (A_REMOVE + 3)
#define  A_REMOVE_BRIDGES          (A_REMOVE + 4)
#define  A_REMOVE_COLOR_V          (A_REMOVE + 5)
#define  A_REMOVE_COLOR_E          (A_REMOVE + 6)
#define  A_REMOVE_THICK_E          (A_REMOVE + 7)
#define  A_REMOVE_END              (A_REMOVE + 99)

//Generate
#define  A_GENERATE                 (500)
#define  A_GENERATE_GRID            (A_GENERATE + 1)
#define  A_GENERATE_COMPLETE        (A_GENERATE + 2)
#define  A_GENERATE_COMPLETE_BIP    (A_GENERATE + 3)
#define  A_GENERATE_RANDOM          (A_GENERATE + 4)
#define  A_GENERATE_P               (A_GENERATE + 5)
#define  A_GENERATE_P_2C            (A_GENERATE + 6)
#define  A_GENERATE_P_3C            (A_GENERATE + 7)
#define  A_GENERATE_P_3R_2C         (A_GENERATE + 10)
#define  A_GENERATE_P_3R_3C         (A_GENERATE + 11)
#define  A_GENERATE_P_3R_D4C        (A_GENERATE + 12)
#define  A_GENERATE_P_4R_C          (A_GENERATE + 13)
#define  A_GENERATE_P_4R_2C         (A_GENERATE + 14)
#define  A_GENERATE_P_4R_3C         (A_GENERATE + 15)
#define  A_GENERATE_P_4R_BIP        (A_GENERATE + 16)
#define  A_GENERATE_P_BIP           (A_GENERATE + 17)
#define  A_GENERATE_P_BIP_2C        (A_GENERATE + 18)
#define  A_GENERATE_P_BIP_3C        (A_GENERATE + 19)
#define  A_GENERATE_END             (A_GENERATE + 99)

//Algo
#define  A_ALGO                    (600)
#define  A_ALGO_KURATOWSKI         (A_ALGO + 1)
#define  A_ALGO_COTREE_CRITICAL    (A_ALGO + 2)
#define  A_ALGO_NPSET              (A_ALGO + 3)
#define  A_ALGO_MAX_PLANAR         (A_ALGO + 4)
#define  A_ALGO_RESET_COLORS       (A_ALGO + 5)
#define  A_ALGO_NETCUT             (A_ALGO + 6)
#define  A_ALGO_GEOMETRIC_CIR      (A_ALGO + 7)
#define  A_ALGO_LRALGO_CIR         (A_ALGO + 8)
#define  A_ALGO_SYM                (A_ALGO + 9)
#define  A_ALGO_COLOR_BIPARTI      (A_ALGO + 10)
#define  A_ALGO_COLOR_EXT          (A_ALGO + 11)
#define  A_ALGO_COLOR_NON_CRITIC   (A_ALGO + 12)
#define  A_ALGO_END                (A_ALGO + 99)

//Orient
#define  A_ORIENT                  (700)
#define  A_ORIENT_E                (A_ORIENT + 1)
#define  A_ORIENT_INF              (A_ORIENT + 2)
#define  A_ORIENT_TRICON           (A_ORIENT + 3)
#define  A_ORIENT_BIPAR            (A_ORIENT + 4)
#define  A_ORIENT_SCHNYDER         (A_ORIENT + 5)
#define  A_ORIENT_BIPOLAR          (A_ORIENT + 6)
#define  A_ORIENT_END              (A_ORIENT + 99)

#endif
