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

#ifndef MISC_H
#define MISC_H

#include <qstring.h> 

// in Misc.cpp 
void DefineGraphContainer(GraphContainer *GC);
void DefinepigaleWindow(pigaleWindow *father);
void DefinepigalePaintWindow(pigalePaint *p);
pigaleWindow* GetpigaleWindow();
pigalePaint* GetpigalePaintWindow();
void DrawGraph(void);
void DrawGraph(Graph &G);
GraphContainer& GetMainGraph();
int GraphInformation();
QString  getErrorString();
void Normalise();
void Normalise(GeometricGraph &G); 
void Normalise(TopologicalGraph &G);
int & pauseDelay();
int & useDistance();
bool & ShowOrientation();
int & ShowVertex();
QColor OppCol(QColor & col);
QColor Desaturate(QColor & col);
bool & ShowIndex();
void ColorPoles(GeometricGraph &G);
bool CheckCoordNotOverlap(GeometricGraph & G);
void ComputeGeometricCir(GeometricGraph &G,svector<tbrin> &cir);
//in Test.cpp
void  initMenuTest();
#endif
