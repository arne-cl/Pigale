 
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
#include <TAXI/Tsvector.h>
#ifdef _WINDOWS
#define srand48 srand
#define lrand48 rand
#endif

bool & randomEraseMultipleEdges()
// if true the generators generate simple graphs
  {static bool _Erase = false;
  return _Erase;
  }
bool & randomUseGeneratedCir()
  {static bool _UseGeneratedCir = true;
  return _UseGeneratedCir;
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
void randomShuffle(svector<int> &tab)
// randomly permute the elements of tab
 {int n = tab.n();
 if(n < 2)return;
 randomStart();
 for(int i = 0;i < n;i++)
     {int r = i + (int) (lrand48()%(long)(n-i));
     if(i == r)continue;
     tab.SwapIndex(i,r);
     }
 randomEnd();
 }

