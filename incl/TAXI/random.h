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

#ifndef RANDOM_H
#define RANDOM_H 

bool & randomEraseMultipleEdges(); // if set to true generators generate simple graphs
long & randomSetSeed();            // set the seed of the random generator 
bool & randomSeed();               // wether program should start with a random seed
void randomInitSeed();             // called once, when program  start
void randomStart();                // called when needed randoms number
void randomEnd();                  // save a new seed
long randomGet(long range);        // returns an integer >= 1 && <= range   
#endif
