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
#ifndef _MHELPERS_H
#define _MHELPERS_H
#include <TAXI/graphs.h>
#include <TAXI/Tsmap.h>
// should only be included through "Mondeca.h"
#define MONDECA_DOUBLE_MULTIPLIER (1.0E6)


const char *Status(MondecaArea &MA, long &cause);

void Paste(svector<double> &value, MondecaArea &MA, int mid);
void Paste(svector<Tpoint> &coord, MondecaArea &MA, int mid);
void Paste(svector<long> &value, MondecaArea &MA, int mid);
void Paste(svector<bool>&value, MondecaArea &MA, int mid);
void Paste(svector<tvertex> &value, MondecaArea &MA, int mid);
void Paste(svector<int> &value, MondecaArea &MA, int mid);

void PasteDouble(PSet &PS, MondecaArea &MA, int prop_id, int mid);
void PastePoint(PSet &PS, MondecaArea &MA, int prop_id, int mid);
void PasteLong(PSet &PS, MondecaArea &MA, int prop_id, int mid);
void PasteInt(PSet &PS, MondecaArea &MA, int prop_id, int mid);
void PasteBool(PSet &PS, MondecaArea &MA, int prop_id, int mid);

void Retrieve(svector<bool> &value, MondecaArea &MA, int mid);
void Retrieve(svector<long> &value, MondecaArea &MA, int mid);
void Retrieve(svector<int> &value, MondecaArea &MA, int mid);
void Retrieve(svector<double> &value, MondecaArea &MA, int mid);
void Retrieve(svector<Tpoint> &coord, MondecaArea &MA, int mid);
void Retrieve(double &d, MondecaArea &MA,int mid, int msid);
void Retrieve(int &d, MondecaArea &MA,int mid, int msid);

int ReadGraph(GraphContainer &GC,PSet &RSet, MondecaArea &MA,
              smap<int> &Vertex,smap<int> &Edge,smap<int> &Role);
void WriteGraph(TopologicalGraph &G, MondecaArea &MA, tvertex v0);
void WriteESG(MondecaArea &MA,TopologicalGraph &ESG);
void WriteRSG(MondecaArea &MA,TopologicalGraph &RSG);
void OrientFromRoles(TopologicalGraph &G, PSet &RSet);




#endif
