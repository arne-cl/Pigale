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
#include <TAXI/Tmessage.h> 
#include <TAXI/color.h> 
#include <TAXI/graphs.h>
#include <TAXI/Tfile.h> 

int TestOuterPlanar(TopologicalGraph &G);
//Drawings
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


#endif
