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

#ifndef HANDLER_H
#define HANDLER_H_
bool & SchnyderRect();
bool & SchnyderLongestFace();
bool & SchnyderColor();

int EmbedHandler(int action,int &drawing); 
int OrientHandler(int action); 
int AlgoHandler(int action); 
int DualHandler(int action); 
int RemoveHandler(int action); 
int AugmentHandler(int action); 
int AlgoHandler(int action,int nn); 
int GenerateHandler(int action,int n1_gen,int n2_gen,int m_gen);
#endif
