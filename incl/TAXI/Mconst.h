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
#ifndef _MCONST_H_
#define _MCONST_H_

// general constants
#define MONDECA_JAVA_DATA                      (0)
#define MONDECA_PIGALE_DATA                    (1)
#define MONDECA_STATUS                         (2) // size 2: ( CODE , ID )
#define MONDECA_ERROR_OK                                 (0)
#define MONDECA_ERROR_MISSING_VALUE                      (1)
#define MONDECA_ERROR_FORMAT_VALUE                       (2)
#define MONDECA_ERROR_BAD_VALUE                          (3)
#define MONDECA_ERROR_ALGO                               (4)
#define MONDECA_ERROR_ON_PROGRESS                        (5)
#define MONDECA_PARAM                          (3)
#define MONDECA_XPARAM1                        (4)
#define MONDECA_XPARAM2                        (5)
#define MONDECA_RESULT                         (6)
#define MONDECA_TOPICMAP                       (7)
#define MONDECA_VERSION                        (8)



// Algo Ids (<=0 implies reset)
#define MONDECA_ALGO_INIT                     (0)
#define MONDECA_ALGO_LOADTGF                  (-1)
#define MONDECA_ALGO_SAVETGF                  (2)
#define MONDECA_ALGO_ADD_ROLE                 (16)
#define MONDECA_ALGO_ADD_ENTITY               (17)
#define MONDECA_ALGO_ADD_RELATION             (18)
#define MONDECA_ALGO_PROMOTE_META             (19)
#define MONDECA_ALGO_ADD_META_EDGE            (20)
#define MONDECA_ALGO_SET_ROLE                 (32)
#define MONDECA_ALGO_SET_ENTITY               (33)
#define MONDECA_ALGO_SET_RELATION             (34)
#define MONDECA_ALGO_INVALIDATE_ROLE          (48)
#define MONDECA_ALGO_INVALIDATE_ENTITY        (49)
#define MONDECA_ALGO_INVALIDATE_RELATION      (50)
#define MONDECA_ALGO_DELETE_META_EDGE         (64)
#define MONDECA_ALGO_DELETE_META_VERTEX       (65)
#define MONDECA_ALGO_POLAR                    (101)
#define MONDECA_ALGO_PATHS                    (102)
#define MONDECA_ALGO_HIERARCHICAL             (103)

#define MONDECA_ALGO_TEST                     (255)


// Main Hypergraph

#define MONDECA_INCID_INCID                    (10)
#define MONDECA_INCID_ROLE                     (11)
#define MONDECA_INCID_ENTITY                   (12)
#define MONDECA_INCID_RELATION                 (13)
#define MONDECA_ROLE_FLAGS                     (14)
#define MONDECA_ROLE_FLAG_ORIENTED                      (0x00000001)
#define MONDECA_ROLE_FLAG_REVERSED                      (0x00000002)
#define MONDECA_ROLE_ROLE                      (15)
#define MONDECA_ROLE_ORDER                     (16)
#define MONDECA_ENTITY_ENTITY                  (17)
#define MONDECA_RELATION_RELATION              (18)
#define MONDECA_ENTITY_COORDX                 (110)
#define MONDECA_ENTITY_COORDY                 (111)
#define MONDECA_RELATION_COORDX               (112)
#define MONDECA_RELATION_COORDY               (113)
















// Main TopicMap

#define XMONDECA_NODE_NODE                      (150)
#define XMONDECA_NODE_PBRIN                     (151)
#define XMONDECA_NODE_PMETA                     (152)
#define XMONDECA_NODE_ISEDGE                    (153)
#define XMONDECA_NODE_BIRTH                     (154)
#define XMONDECA_NODE_DEATH                     (155)
#define XMONDECA_NODE_VALUE                     (156)

#define XMONDECA_INCID_INCID                    (160)
#define XMONDECA_INCID_PMETA                    (161)
#define XMONDECA_INCID_BIRTH                    (162)
#define XMONDECA_INCID_DEATH                    (163)
#define XMONDECA_INCID_VALUE                    (164)

#define XMONDECA_BRIN_VIN                       (170)
#define XMONDECA_BRIN_CIR                       (171)

#define XMONDECA_METAVERTEX_PMETA               (180)
#define XMONDECA_METAVERTEX_FLAGS               (181)

#define XMONDECA_METAEDGE_TYPE                 (185)

#define XMONDECA_METABRIN_VIN                   (190)
#define XMONDECA_METABRIN_CIR                   (191)
#define XMONDECA_METAINCID_TYPE                 (185)
// Additional Parameters

#define XMONDECA_YOUR_ID                        (140)
#define XMONDECA_INSERT_AFTER                   (141)
#define XMONDECA_INSERT_BEFORE                  (142)
#define XMONDECA_LINK_METANODE                  (143)
#define XMONDECA_LINK_METATYPE                  (144)





// Extracted Sub Hypergraph

#define MONDECA_SUB_ENTITY_ENTITY              (20)
#define MONDECA_SUB_ENTITY_CONTAINER_IND       (21)
#define MONDECA_SUB_RELATION_RELATION          (22)
#define MONDECA_SUB_RELATION_CONTAINER_IND     (23)
#define MONDECA_SUB_INCID_INCID                (24)
#define MONDECA_SUB_INCID_CONTAINER_IND        (25)
#define MONDECA_SUB_ENTITY_COORDX             (120)
#define MONDECA_SUB_ENTITY_COORDY             (121)
#define MONDECA_SUB_RELATION_COORDX           (122)
#define MONDECA_SUB_RELATION_COORDY           (123)

// Reduced Sub Hypergraph

#define MONDECA_RED_CONTAINER_TYPE             (30)
#define MONDECA_RED_INCID_ENTITY               (31)
#define MONDECA_RED_INCID_RELATION             (32)
#define MONDECA_RED_INCID_ROLE_IND             (33)
#define MONDECA_RED_ENTITY_COORDX             (130)
#define MONDECA_RED_ENTITY_COORDY             (131)
#define MONDECA_RED_RELATION_COORDX           (132)
#define MONDECA_RED_RELATION_COORDY           (133)

//
// Polar Drawing
//

//   Param indices
#define MONDECA_POLAR_PARAM_ROOT                         (0)
#define MONDECA_POLAR_PARAM_DEPTH                        (1)

// MAids

#define MONDECA_POLAR_INCID_POINT1_X           (200)
#define MONDECA_POLAR_INCID_POINT1_Y           (201)
#define MONDECA_POLAR_INCID_POINT11_X          (202)
#define MONDECA_POLAR_INCID_POINT11_Y          (203)
#define MONDECA_POLAR_INCID_POINT12_X          (204)
#define MONDECA_POLAR_INCID_POINT12_Y          (205)
#define MONDECA_POLAR_INCID_POINT2_X           (206)
#define MONDECA_POLAR_INCID_POINT2_Y           (207)
#define MONDECA_POLAR_INCID_POINT21_X          (208)
#define MONDECA_POLAR_INCID_POINT21_Y          (209)
#define MONDECA_POLAR_INCID_POINT22_X          (210)
#define MONDECA_POLAR_INCID_POINT22_Y          (211)
#define MONDECA_POLAR_INCID_RHO                (212)
#define MONDECA_POLAR_INCID_THETA1             (213)
#define MONDECA_POLAR_INCID_THETA2             (214)
#define MONDECA_POLAR_CONTAINER_X              (215)
#define MONDECA_POLAR_CONTAINER_Y              (216)
#define MONDECA_POLAR_CONTAINER_SIZE           (217)

//
// Path Drawing
//

//   Param indices
#define MONDECA_PATH_PARAM_ROOT1                         (0)
#define MONDECA_PATH_PARAM_ROOT2                         (1)
#define MONDECA_PATH_PARAM_LENGTH                        (2)





// MAids

#define MONDECA_PATH_INCID_POINT1_X            (200)
#define MONDECA_PATH_INCID_POINT1_Y            (201)
#define MONDECA_PATH_INCID_POINT2_X            (202)
#define MONDECA_PATH_INCID_POINT2_Y            (203)
#define MONDECA_PATH_CONTAINER_XL_X            (204)
#define MONDECA_PATH_CONTAINER_L_X             (205)
#define MONDECA_PATH_CONTAINER_XR_X            (206)
#define MONDECA_PATH_CONTAINER_R_X             (207)
#define MONDECA_PATH_CONTAINER_Y               (208)
#define MONDECA_PATH_BOUNDING_BOX              (209)

//
// Hierarchical Drawing
//

// Param indices
#define MONDECA_HIERARCHICAL_PARAM_ROOT                  (0)
#define MONDECA_HIERARCHICAL_PARAM_DEPTH                 (1)

// MAids

#define MONDECA_HIERARCHICAL_INCID_POINT1_X    (200)
#define MONDECA_HIERARCHICAL_INCID_POINT1_Y    (201)
#define MONDECA_HIERARCHICAL_INCID_POINT2_X    (202)
#define MONDECA_HIERARCHICAL_INCID_POINT2_Y    (203)
#define MONDECA_HIERARCHICAL_CONTAINER_XL_X    (204)
#define MONDECA_HIERARCHICAL_CONTAINER_L_X     (205)
#define MONDECA_HIERARCHICAL_CONTAINER_XR_X    (206)
#define MONDECA_HIERARCHICAL_CONTAINER_R_X     (207)
#define MONDECA_HIERARCHICAL_CONTAINER_Y       (208)
#define MONDECA_HIERARCHICAL_BOUNDING_BOX      (209)

#endif
