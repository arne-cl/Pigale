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
class QColor;

// in Misc.cpp 
void DefineGraphContainer(GraphContainer *GC);
void DefineMyWindow(MyWindow *father);
void DefineMyPaintWindow(MyPaint *p);
MyWindow* GetMyWindow();
MyPaint* GetMyPaintWindow();
void DrawGraph(void);
void DrawGraph(Graph &G);
GraphContainer& GetMainGraph();
int GraphInformation();
void Normalise();
void Normalise(GeometricGraph &G); 
void Normalise(TopologicalGraph &G);

int & useDistance();
bool & ShowOrientation();
bool & ShowIndex();
int & ShowVertex();
QColor OppCol(QColor & col);
bool & ShowIndex();
void SetPigaleColors();
int  GetPigaleColors();
bool CheckCoordNotOverlap(GeometricGraph & G);
#endif
