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
#ifndef PIGALE_H
#define PIGALE_H
#include <TAXI/Tbase.h> 
#include <TAXI/Tmessage.h> 
#include <TAXI/color.h> 
#include <TAXI/graphs.h>
#include <TAXI/Tfile.h>

int NPBipolar(TopologicalGraph &G, tbrin bst);
int EmbedGVision(TopologicalGraph &G);
int TestOuterPlanar(TopologicalGraph &G);
// Drawings
bool & SchnyderRect();
bool & SchnyderLongestFace();
bool & SchnyderColor();
int EmbedTContact(TopologicalGraph &G);
int EmbedFPPRecti(TopologicalGraph &G);
int EmbedFPP(TopologicalGraph &G);
int EmbedTutteCircle(TopologicalGraph &G);
int FindNPSet(TopologicalGraph &);
int split(Graph &G0,int &NumberOfClasses);
int EmbedVision(TopologicalGraph &G);
int Vision(TopologicalGraph &G);
int EmbedContactBip(GeometricGraph &G);
int DecompMaxBip(TopologicalGraph &G);
int Polar(TopologicalGraph &G);
int Embed3d(TopologicalGraph &G0);

// Generators
//in Generate.cpp
GraphContainer *GenerateGrid(int a, int b);
GraphContainer *GenerateCompleteGraph(int a);
GraphContainer *GenerateCompleteBiGraph(int a,int b);
GraphContainer *GenerateRandomGraph(int a,int b);
bool & EraseMultipleEdges(); // if set to true generators generate simple graphs
//in SchaefferGen.cpp
GraphContainer *GenerateSchaeffer(int n_ask,int type,int e_connectivity);
long & setSeed();  // set the seed of the random generator (1 if not randomSeed)
bool & randomSeed(); 
void randomInit();
#endif
