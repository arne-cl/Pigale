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

#ifndef TFILE_H
#define TFILE_H

#include  <TAXI/graphs.h>

int IsFileTgf(char const *name);
int SaveGraphTgf(GraphAccess& G,tstring filename,int tag=2);
int SaveGraphAscii(GraphAccess& G,tstring filename);
int ReadGraphAscii(GraphContainer& G,tstring filename);
int DeleteTgfRecord(tstring filename,int index);
int GetNumRecords(tstring fname);
int ReadGeometricGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
int ReadGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
int ReadTgfGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
#endif

