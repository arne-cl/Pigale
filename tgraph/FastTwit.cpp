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
#define PRIVATE
#include <TAXI/lralgo.h>
#undef PRIVATE

int _FastTwit::DeleteLeft(tvertex vi)
  {while(vi <= vin[Twin().ltop()])
      {if(Twin().MoreThanOne(TW_LEFT)) // droite a > 1 arete
          {Twin().ltop() = Hist.Link[Twin().ltop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_RIGHT)) // gauche non vide
          {Hist.Link[Twin().ltop()] = Twin().rbot();
          Twin().ltop() = Twin().lbot() = 0;
          break;
          }
      else
          {if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }
int _FastTwit::DeleteRight(tvertex vi)
  {while(vi <= vin[Twin().rtop()])
      {if(Twin().MoreThanOne(TW_RIGHT)) // droite a > 1 arete
          {Twin().rtop() = Hist.Link[Twin().rtop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_LEFT)) // gauche non vide
          { Hist.Link[Twin().rtop()] = Twin().lbot();
          Twin().rtop() = Twin().rbot() = 0;
          break;
          }
      else
          {if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }

void _FastTwit::Deletion(tvertex vi)
  {int c;
  if (_current==_fork) return;

  //printf("Deletion on %d\n",vi);//POM
  while(1)
      {c=DeleteRight(vi);
      if (c==AGAIN) continue;
      else if (c==FIN) return;
      if (DeleteLeft(vi)!=AGAIN) return;
      }
  }

void _FastTwit::Fuse(tedge ej)
  {if (PrevTwin().Empty(TW_LEFT))
      PrevTwin().lbot() = ej;

  Hist.Link[ej] = PrevTwin().ltop();
  PrevTwin().ltop() = Twin().ltop();

  if (Twin().NotEmpty(TW_RIGHT))
      {Hist.Link[Twin().rbot()] = PrevTwin().rtop();
      PrevTwin().rtop() = Twin().rtop();
      }
  _current--;
  }

void _FastTwit::Fusion(tedge ej)
  {//printf("Fusion with %d\n",ej); //POM
  do
      {if (vin[ej] >= vin[PrevTwin().ltop()])
          {if (vin[ej] >= vin[PrevTwin().rtop()])
              break; // Stop !
          }
      else if (vin[ej] >= vin[PrevTwin().rtop()])
          PrevTwin().Permute();
      else
          Flipin(ej);
      Fuse(ej);
      } while(_current>_fork+1);
  }

void _FastTwit::Flipin(tedge ej)
  {tedge cv = Couve();
  int l1 = (vin[PrevTwin().ltop()] <= vin[PrevTwin().rbot()]);
  int l2 = (vin[PrevTwin().rtop()] <= vin[PrevTwin().lbot()]);
  if (l2)
      {if(l1 && !Cover(cv))
          Flipe(ej);
      else
          {PrevTwin().Permute();
          if(Cover(cv))   //NotPlanar
              {Hist.Cove = cv;
              Hist.LeftTop  = PrevTwin().ltop();   
              Hist.LeftBot  = PrevTwin().lbot();
              Hist.RightTop = PrevTwin().rtop();   
              Hist.RightBot = PrevTwin().rbot();
              Hist.NP = ej;
              isnotplanar = 1;
              Hist.Thick = 5;
              longjmp(env,1);
              }
          else
              Flipe(ej);
          }
      }
  else
      {if (l1)
          {if(Cover(cv))
              {Hist.Cove = cv;
              Hist.LeftTop  = PrevTwin().ltop();     
              Hist.LeftBot = PrevTwin().lbot();
              Hist.RightTop = PrevTwin().rtop();    
              Hist.RightBot = PrevTwin().rbot();
              Hist.NP = ej;
              isnotplanar = 1;
              Hist.Thick = 3;
              longjmp(env,1);
              }
          else
              Flipe(ej);
          }
      else
          {Hist.Cove = cv; //cv ????
          Hist.LeftTop  = PrevTwin().ltop();    
          Hist.LeftBot  = PrevTwin().lbot();
          Hist.RightTop = PrevTwin().rtop();    
          Hist.RightBot = PrevTwin().rbot();
          Hist.NP = ej;
          isnotplanar = 1;
          Hist.Thick = 4;
          longjmp(env,1);
          }
      }
  }

void _FastTwit::Flipe(tedge ej) // toujours k=pile   bj=brin correspondant … ej
// Flip left under right
  {do
      {Hist.Link[PrevTwin().rbot()] = PrevTwin().ltop();
      PrevTwin().rbot() = PrevTwin().ltop();
      PrevTwin().ltop()=Hist.Link[PrevTwin().ltop()];
      Hist.Link[PrevTwin().rbot()] = 0;
      }while(vin[ej] < vin[PrevTwin().ltop()]);

  if(PrevTwin().ltop() == tedge(0)) PrevTwin().lbot() = tedge(0);
  }
void _FastTwit::Thin(tedge ej)
  { _current = _fork;
  NewTwin(ej);
  }

void _FastTwit::Thick()
  {int _SauvCurrent = _current;

  for(;_current > _fork ;--_current)
      {if(vin[Twin().ltop()] < vin[Twin().rtop()])
          Twin().Permute();
      }
  _current = _SauvCurrent;
  Flick();
  FirstTwin().rbot() = 0;
  FirstTwin().ltop() = Twin().ltop();
  GotoFirstTwin();
  }
void _FastTwit::Mflip()
  {tedge lbt,rtp;

  lbt = Twin().lbot();
  rtp = Twin().rtop();
  if(vin[rtp] == vin[lbt])                                /* effacement en cas d'egalite */
      {
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[rtp] = lbt;
      }
  else                                                    /* flipping de rtp sous lbt  */
      {
      Twin().lbot() = rtp;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[lbt] = rtp;
      Hist.Link[rtp] = 0;
      }
  }

void _FastTwit::Flick()
  {int SauvCurrent=_current;
  tedge cv = Couve();

  for(;_current > _fork;--_current)
      {while(Twin().rtop()!=0)
          {if(side_flip(Twin().rtop(),Twin().lbot(),cv))
              // flip rtop under lbot
              Mflip();
          else
              {if(side_flip(Twin().ltop(),Twin().rbot(),cv))
                  {Twin().Permute();Mflip();}
              else //NotPlanar
                  {Hist.Cove = cv;
                  Hist.LeftTop  = Twin().ltop();    Hist.LeftBot  = Twin().lbot();
                  Hist.RightTop =  Twin().rtop();    Hist.RightBot = Twin().rbot();
                  if(Twin1().lbot() != 0 && Twin1().rbot() != 0)
                      Hist.NP = (Twin1().lbot() <  Twin1().rbot()) ?
                          Twin1().lbot() :Twin1().rbot();
                  else if(Twin1().lbot() != 0)
                      Hist.NP = Twin1().lbot(); 
                  else
                      Hist.NP = Twin1().rbot();
                  isnotplanar = 1;
                  Hist.Thick = 6;
                  longjmp(env,1);
                  }
              }
          }
      if(_current < SauvCurrent)
          Hist.Link[NextTwin().lbot()] = Twin().ltop();
      }
  _current = SauvCurrent;
  }
