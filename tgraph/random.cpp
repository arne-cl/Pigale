 
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

#include <TAXI/Tbase.h>

#ifdef _WINDOWS
#define srand48 srand
#define lrand48 rand
#endif

bool & randomEraseMultipleEdges()
// if true the generators generate simple graphs
  {static bool _Erase = false;
  return _Erase;
  }
long & randomSetSeed()
  {static long _seed = 1;
  return _seed;
  }
bool & randomSeed()
// wether program should start with a random seed
  {static bool  _random = false;
  return _random;
  }
void randomInitSeed()
// called once, when program  starts
  {if(randomSeed())
      {time_t time_seed;
      time(&time_seed);
      randomSetSeed() = (long)time_seed; 
      }
  srand48(randomSetSeed());
  }
void randomStart()
  {srand48(randomSetSeed());
  }
void randomEnd()
  {randomSetSeed() = lrand48();
  }
long randomGet(long range) 
//returns an integer >= 1 && <= range
  {return (lrand48()%(long)range) +1;
  }
