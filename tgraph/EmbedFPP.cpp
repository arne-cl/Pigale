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


#include <TAXI/graphs.h>
 
#define CONE 0
#define HOR_LEFT 1
#define HOR_RIGHT 2

#define RED (char)1
#define BLUE (char)2
#define GREEN (char)3
#define UNVISITED (char)0
#define VISITED (char)4


bool & SchnyderLongestFace();

//***************************************************
/*    mark[iv] = VISITED adjacent a un sommet traite.
        color[je] = BLUE  si incidente a un sommet traité.
        top_saturated,next,previous definissent la pile des sommets satures.
        n_cones[iv] nombre de cones d'un sommet.
        iv1,iv2,iv3 definissent la face exterieure.
        last_left brin de la face exterieure du cone de iv3.
        nfind nombre de sommets traites +1.
*/

class ShellingOrder: public TopologicalGraph
{public:
  ShellingOrder(Graph &G);
  ~ShellingOrder();
  void init(tbrin ee);
  int findver(tvertex &iv,tbrin &left,tbrin &right);

private:
  svector<char> mark;
  svector<char> couleur;
  svector<tvertex>previous,next;
  svector<int>n_cones;
  tvertex top_saturated;
  tvertex iv3;
  tbrin last_left,last_right;
  int nfind;

  void mod_iv(tvertex iv,tbrin &left,tbrin &right);
  tbrin left_c(tvertex iv);
  void mod_ee(tbrin ee);
  int inpile(tvertex iv);
  void push(tvertex iv);
  void pop(tvertex iv);
};

ShellingOrder::~ShellingOrder()
  {}
ShellingOrder::ShellingOrder(Graph &G) :
    TopologicalGraph(G),top_saturated(0),nfind(3)
  {mark.resize(1,nv());  mark.clear(); mark.SetName("Mark");
  couleur.resize(1,ne()); //couleur.fill(1,ne(),RED);
  for(tedge e = 1;e <= ne();e++)couleur[e] = RED;
  next.resize(0,nv()); next.clear(); next.SetName("next");
  previous.resize(0,nv()); previous.clear(); previous.SetName("previous");
  n_cones.resize(1,nv()); n_cones.clear(); n_cones.SetName("n_cones");
  }
void ShellingOrder::init(tbrin ee)
  {iv3 = vin[-acir[ee]];
  last_left = acir[ee];
  last_right = cir[-ee];
  couleur[ee.GetEdge()] = BLUE;
  tbrin left,right;
  mod_iv(vin[ee],left,right);
  mod_iv(vin[-ee],left,right);
  }
void ShellingOrder::mod_iv(tvertex iv,tbrin &left,tbrin &right)
// les aretas rouges incidentes a iv modifient la pile des sommets satures.
  {tbrin ee = left_c(iv);
  left = -ee;
  ee = acir[ee];
  while(couleur[ee.GetEdge()] == RED)
      {mod_ee(-ee);ee = acir[ee];}
  right = -ee;
  }
tbrin ShellingOrder::left_c(tvertex iv) 
  {tbrin ee;
  ee = pbrin[iv];
  while(couleur[ee.GetEdge()] != RED)  // recherche d'une arete rouge 
      ee = acir[ee];
  while(couleur[ee.GetEdge()] != BLUE) // recherche de la premiere arete bleue 
      ee = cir[ee];
  return(ee);
  }
void ShellingOrder::mod_ee(tbrin ee)
// le brin ee en changeant de couleur modifie la pile des sommets satures
  {// int inpile(); !!!!!
  char lje_c,rje_c;  // lje_c couleur de l'arete a gauche de ee (rje_c droite)
  tvertex iv;
  iv = vin[ee];
  couleur[ee.GetEdge()] = BLUE;
  if(mark[iv] == UNVISITED)
      {mark[iv] = VISITED;
      n_cones[iv] = 1;
      return;
      }
  // calcul du nouveau nombre de cones de iv
  lje_c = couleur[cir[ee].GetEdge()];
  rje_c = couleur[acir[ee].GetEdge()];
  if(lje_c != rje_c)
      ;
  else if(lje_c == BLUE)
      --n_cones[iv];
  else
      ++n_cones[iv];
  // insertion ou effacement de iv de la pile des satures 
  if((n_cones[iv] == 1) && (inpile(iv) == 0))
      push(iv);
  else if((n_cones[iv] != 1) && inpile(iv))
      pop(iv);
  }
int ShellingOrder::inpile(tvertex iv)
// indique si iv est dans la pile
  {return((iv == top_saturated) || (previous[iv] != 0));
  }
void ShellingOrder::push(tvertex iv)
// met le sommet iv dans la pile s'il n'y etait pas 
  {if(iv == iv3)return;
  if(top_saturated != 0)previous[top_saturated] = iv;
  next[iv] =top_saturated;
  top_saturated = iv;
  }
void ShellingOrder::pop(tvertex iv)
// sort le sommet de la pile 
  {if(iv == top_saturated)
      top_saturated = next[iv];
  else
      {previous[next[iv]] = previous[iv];
      next[previous[iv]] = next[iv];
      }
  previous[iv] = 0;
  }
int ShellingOrder::findver(tvertex &iv,tbrin &left,tbrin &right)
  {if(top_saturated != 0)
      {iv = top_saturated;
      pop(iv);
      mod_iv(iv,left,right);
      ++nfind;
      }
  else if(nfind == nv())
      {iv = iv3;
      left = last_left;
      right = last_right;
      }
  else return -1;
  return 0;
  }

//*********************************************************************
//*********************************************************************
class Constraints: public TopologicalGraph
{public:
  Constraints(Graph &G);
  ~Constraints();
  void init(tbrin ee);
  void add(tbrin ee);
  void deplace(tvertex start,int length,tvertex iv);
  tvertex up(tvertex vi,tvertex iv);
  int monot_up(tbrin lee,tbrin ree);
  tbrin monot_dwn(tbrin lee,tbrin ree);
  void ComputeCoord(tvertex iv,tvertex iv1,tvertex iv2,int forme);

  svector<int>x,y;
  bool ok;
private:
  svector<tvertex>push1,push2,push3,push_from,push_by;
};
Constraints:: ~Constraints()
  {}
Constraints:: Constraints(Graph &G) :
    TopologicalGraph(G),ok(true)
  {push1.resize(0,nv());         push1.clear();       push1.SetName("push1");
  push2.resize(0,nv());          push2.clear();       push2.SetName("push2");
  push3.resize(0,nv());          push3.clear();       push3.SetName("push3");
  push_from.resize(1,nv());      push_from.clear();   push_from.SetName("push_from"); 
  push_by.resize(1,nv());        push_by.clear();     push_by.SetName("push_by"); 
  x.resize(1,nv());              x.clear();           x.SetName("x");
  y.resize(1,nv());              y.clear();           y.SetName("y");  
  }
void Constraints::init(tbrin ee)
  {tvertex iv1,iv2,iv3;
  iv1 = vin[ee];iv2 = vin[-ee];iv3 = vin[-acir[ee]];
  x[iv1] = 1;x[iv2] = 3;
  push1[iv3] = iv2;push_from[iv2] = iv3;push_by[iv2] = 0;
  }
void Constraints::add(tbrin ee)
//  iv2=G.vin[-ee] poussera le sommet iv1=G.vin[ee]
  {tvertex iv1,iv2,iv3;
  iv1 = vin[ee];iv2 = vin[-ee];
  iv3 = push_from[iv1];
  push_from[iv1] = iv2;
  if(push1[iv3] == iv1)push1[iv3]=0;
  if(push2[iv3] == iv1)push2[iv3]=0;
  if(push3[iv3] == iv1)push3[iv3]=0;
  if(push1[iv2] != 0  && push2[iv2] !=0 && push3[iv2] !=0)
      {ok = false;}
  if(push1[iv2] == 0)
      push1[iv2] = iv1;
  else if(push2[iv2] == 0)
      push2[iv2] = iv1;
  else
      push3[iv2] = iv1;
  }
void Constraints::deplace(tvertex start,int length,tvertex iv)
  {tvertex vi,vii;
  vi = start;
  if(push_by[vi] != iv)
      {push_by[vi] = iv;x[vi] += length;}
  for(;;)
      {if((vii = up(vi,iv)) != 0)
	  {vi = vii;x[vi] += length;}
      else if(vi == start)
	  return;
      else
	  vi = push_from[vi];
      }
  }
tvertex Constraints::up(tvertex vi,tvertex iv)
  {tvertex vi_up;

  if(((vi_up = push1[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  if(((vi_up = push2[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  if(((vi_up = push3[vi]) != 0) && (push_by[vi_up] != iv))
      {push_by[vi_up] = iv;return(vi_up);}
  return 0;
  }
int Constraints::monot_up(tbrin lee,tbrin ree)
  {for(tbrin ee = lee;ee != ree;ee = acir[-acir[ee]])
      if(y[vin[ee]] > y[vin[acir[-acir[ee]]]])return 0;
  return 1;
  }
tbrin Constraints::monot_dwn(tbrin lee,tbrin ree)
  {tbrin  ee = acir[-acir[lee]];
  for(;;)
      {if(ee == ree)
	  return(ee);
      else if(y[vin[ee]] <= y[vin[lee]])
	  {lee = ee;ee = acir[-acir[ee]];}
      else
	  return lee;
      }
  }
void Constraints::ComputeCoord(tvertex iv,tvertex iv1,tvertex iv2,int forme)
 {int x1,x2,y1,y2;
 x1 = x[iv1];y1 = y[iv1];x2 = x[iv2];y2 = y[iv2];
 if(forme == HOR_LEFT)
     {y[iv] = y2;
     x[iv] = x1 + y2 - y1;
     }
 else if(forme == HOR_RIGHT)
     {y[iv] = y1;
     x[iv] = x2 + y2 -y1;
     }
 else
     {x[iv] = (x1 + x2 + y2 -y1)/2;
     y[iv] = (y1 + y2 + x2 -x1)/2;
     }
 }

//***********************************************************************
/*    x[iv],y[iv] tableau des coordonnees
        push_by[iv] indique le sommet que l'on place et qui entraine que l'on
                doive pousser iv
        push1[iv], push2[iv], push3[iv] indiquent quels sommets iv pousse
        push_from[iv] est le sommet qui pousse iv
*/

int EmbedFPP(TopologicalGraph &G)
  {int OldNumEdge = G.ne();
  G.MakeConnected();
  G.Simplify();
  if(!G.CheckPlanar())return -1;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin ee;
  if(SchnyderLongestFace() && !MaxPlanar)
      G.LongestFace(ee,len);
  else
      ee = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate())return -2;
  tbrin left,right,new_push;
  tvertex iv,iv1,iv2,iv3;
  tvertex ivl,nivl,ivr,pivr;
  int move_l,move_r,forme;

  ShellingOrder shell(G);   shell.init(ee);
  Constraints cs(G);           cs.init(ee);

  iv1 = G.vin[ee];iv2 = G.vin[-ee];iv3 = G.vin[-G.acir[ee]];
  for(tvertex ivn = 3;ivn <= G.nv();ivn++)
      {if(shell.findver(iv,left,right))return -2;
      // sur le front on trouve dans l'ordre: ivl,nivl,pivr,ivr
      ivl = G.vin[left];nivl = G.vin[G.acir[-G.acir[left]]];
      ivr = G.vin[right];pivr = G.vin[G.cir[-G.cir[right]]];
      move_l = move_r = 0;
      forme = CONE;
      /* determination de la forme du front */
      if(cs.y[nivl] > cs.y[ivl])
	  {new_push = G.acir[-G.acir[left]];
	  move_l = 1;
	  if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	      forme = HOR_LEFT;
	  else if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if(cs.y[nivl] < cs.y[ivl])
	  {if((new_push = cs.monot_dwn(left,right)) == right)
	      forme = HOR_RIGHT;
	  if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  }
      else if((cs.y[pivr] < cs.y[ivr]) && cs.monot_up(left,right))
	  {forme = HOR_LEFT;new_push = cs.monot_dwn(left,right);
	  }
      else
	  {if(cs.y[pivr] > cs.y[ivr])
	      move_r = 1;
	  new_push = cs.monot_dwn(left,right);
	  }

      /* calcul du deplacement des sommets */
      //if(!minimisation)forme = CONE;
      if(forme == CONE)
	  {int dx = cs.x[ivl]+cs.x[ivr]+cs.y[ivl]+cs.y[ivr]+move_l+move_r;
	  if(dx%2 != 0)++move_r;
	  }
      move_r += move_l;
      if(move_r)cs.deplace(ivr,move_r,iv);
      if(move_l)cs.deplace(nivl,move_l,iv);
      /* modification des pousseurs (cet ordre est capital) */
      cs.add(new_push);cs.add(-left);
      cs.ComputeCoord(iv,G.vin[left],G.vin[right],forme);
      }
  if(!cs.ok)return -1;
  Prop<Tpoint> Coord(G.Set(tvertex()),PROP_COORD);
  for(tvertex v = 1;v <= G.nv();v++)
      {Coord[v].x() = cs.x[v];
       Coord[v].y() = cs.y[v];
      }
  // Erase added edges
  for(tedge e = G.ne();e > OldNumEdge;e--)G.DeleteEdge(e);
  return 0;
  }


//***************************************************************************************
//***************************************************************************************
class Recti: public TopologicalGraph
{public:
  Recti(Graph &G);
  ~Recti();
  int  hauteur(tbrin lee,tbrin ree);
  void con_iv(tbrin lee,tbrin ree);
  void con1_ee(tbrin ee,tbrin ree);
  void con2_ee(tbrin ee);
  void addrecti(tedge je,tedge jf,int len);
  void resolve(void);
  void solve(tedge je);
  int  lrver(void);

  svector<int> length,xliv,xriv,y,xje;
  tvertex iv1;
  int m_origin;

private:
  svector<tedge> push0_by,push1_by,push2_by,back;
 
};
Recti::~Recti()
  {}
Recti::Recti(Graph &G) :
    TopologicalGraph(G)
  {push0_by.resize(0,ne()); push0_by.clear(); push0_by.SetName("push0_by");
  push1_by.resize(0,ne());  push1_by.clear(); push1_by.SetName("push1_by");
  push2_by.resize(0,ne());  push2_by.clear(); push2_by.SetName("push2_by");
  length.resize(0,ne());    length.clear();   length.SetName("length");
  back.resize(0,ne());      back.clear();     back.SetName("length");
  xje.resize(0,ne());       xje.clear();      xje.SetName("xje");
  xliv.resize(0,nv());      xliv.clear();     xliv.SetName("xliv");
  xriv.resize(0,nv());      xriv.clear();     xriv.SetName("xriv");
  y.resize(0,nv());         y.clear();        y.SetName("y");
  }
int Recti::hauteur(tbrin lee,tbrin ree)
  {int h = 0;
  while((lee = -cir[-lee]) != ree)
      {h = Max(h,y[vin[lee]]);
      }
  return h;
  }
void Recti::con_iv(tbrin lee,tbrin ree)
  {tbrin ee = lee;
  while(ee != ree)
      {con1_ee(lee,ree);
      ee = lee;lee = -cir[-lee];
      }
  }
void Recti::con1_ee(tbrin ee,tbrin ree)
  {tbrin gg;
  int h,hee,hff,hgg;
  tbrin ff = cir[ee];
  h = y[vin[-ee]];hee = y[vin[ee]];hff = y[vin[-ff]];
  
  if(hee == h && ee == ree)
      {gg = acir[ee];
      if(y[vin[-gg]])addrecti(ff.GetEdge(),gg.GetEdge(),0);
      addrecti(acir[-ee].GetEdge(),cir[ee].GetEdge(),2);
      return;
      }
  else if(hee == h)
      {addrecti(acir[ee].GetEdge(),cir[-ee].GetEdge(),2);
      return;
      }
  else if(hff && (hff < hee))
      {addrecti(ff.GetEdge(),ee.GetEdge(),0);
      }
  else if(hff && (hff ==  hee) && (vin[ee] != iv1))
      {gg = cir[-ff];hgg = y[vin[-gg]];
      if(hgg > hee){addrecti(gg.GetEdge(),ee.GetEdge(),2);}

      ff = cir[ff];
      hff = y[vin[-ff]];
      if(hff && (hff < hee))addrecti(ff.GetEdge(),ee.GetEdge(),0);
      }
  con2_ee(ee);
  }

void Recti::con2_ee(tbrin ee)
  {tbrin ff,gg;
  int hee,hff,hgg;
  hee = y[vin[ee]];
  ff = acir[ee];hff = y[vin[-ff]];
  if(hff == 0)return;
  if((hff > hee) && (hff != y[vin[ff]]) && (hff != y[vin[-ee]]))
      {addrecti(ee.GetEdge(),ff.GetEdge(),1);return;}
  gg = acir[-ff];hgg = y[vin[-gg]];
  if((hgg > hff) && hff != hee)
      {addrecti(ee.GetEdge(),gg.GetEdge(),1);
      addrecti(ff.GetEdge(),gg.GetEdge(),1);
      }
  gg = cir[-ff];hgg = y[vin[-gg]];
  if(hgg && (hff == hee) && (hgg < hee))
      {addrecti(ee.GetEdge(),gg.GetEdge(),2);}
  }
void Recti::addrecti(tedge je,tedge jf,int len)
  {if(len)
      {if(push1_by[jf] == 0)
	  {push1_by[jf] = je;length[je] = len;}
      else
	  {push2_by[jf] = je;length[je] = len;}
      }
  else
      push0_by[jf] = je;
  }
void Recti::resolve(void)
  {for(tedge je = 1;je <= ne();je++)
      {if(vin[je] == 0)continue;
      if(xje[je] != 0)continue;
      solve(je);
      }
  }
void Recti::solve(tedge je)
  {tedge jje;
  for(;;)
      {if((jje = push1_by[je]) != 0)
	  {back[jje] =je;push1_by[je] = 0;
	  je = jje;
	  }
      else if((jje = push2_by[je]) != 0)
	  {back[jje] =je;push2_by[je] = 0;
	  je = jje;
	  }
      else if((jje = push0_by[je]) != 0)
	  {back[jje] =je;
	  je = jje;
	  }
      else if((jje = back[je]) != 0)
	  {if(push0_by[jje] == je)
	      {push0_by[jje] = 0;
	      xje[jje] = Max(xje[jje],xje[je]);
	      je = jje;
	      }
	  else
	      {xje[jje] = Max(xje[jje],xje[je] + length[je]);
	      je = jje;
	      }
	  }
      else
	  return;
      }
  }
int Recti::lrver(void)
  {tbrin ref,ee;
  int xx,xmax = 0;
  bool vertical;
  for(tvertex iv = 1;iv <= nv();iv++)
      {ref = ee = pbrin[iv];
      // Check exist vertical edges around iv from original graph
      vertical = false;
      do    
	  {if(y[vin[ee]] == y[vin[-ee]])continue;
	  if(ee.GetEdge() <= m_origin)
	      vertical = true;
	  }while(!vertical && (ee = acir[ee]) != ref);

      ee = ref;
      xliv[iv] = 0x7FFF;
      do    
	  {if(y[vin[ee]] == y[vin[-ee]])continue;
	  if(vertical && ee.GetEdge() > m_origin)continue;
	  xx = xje[ee.GetEdge()];
	  xliv[iv] = Min(xliv[iv],xx);
	  xriv[iv] = Max(xriv[iv],xx);
	  xmax = Max(xmax,xriv[iv]);
	  }while((ee = acir[ee]) != ref);
      }
  return(xmax);
  }


int EmbedFPPRecti(TopologicalGraph &G)
  {int OldNumEdge = G.ne();
  G.MakeConnected();
  G.Simplify();
  if(!G.CheckPlanar())return -1;
  bool MaxPlanar = (G.ne() != 3 * G.nv() - 6) ? false : true;
  int len;
  tbrin ee;
  if(SchnyderLongestFace() && !MaxPlanar)
      G.LongestFace(ee,len);
  else
      ee = G.extbrin();
  if(!MaxPlanar && G.ZigZagTriangulate())return -2;
  tvertex iv,iv1,iv2,iv3;

  ShellingOrder shell(G);   shell.init(ee);
  Recti recti(G);              
  recti.m_origin = OldNumEdge;
  recti.iv1 = iv1 = G.vin[ee];iv2 = G.vin[-ee];iv3 = G.vin[-G.acir[ee]];
  recti.y[iv1] = recti.y[iv2] = 1;recti.xje[G.acir[ee].GetEdge()] = 1;
  int h,h0;
  tbrin left,nleft,pright,right;
  tvertex ivl,nivl,pivr,ivr;
  for(int i = 3;i <= G.nv();i++)
      {if(shell.findver(iv,left,right) != 0)return -2;
      ivl = G.vin[left];nleft = G.acir[-G.acir[left]];nivl = G.vin[nleft];
      ivr = G.vin[right];pright = G.cir[-G.cir[right]];pivr = G.vin[pright];
      // calcul de la hauteur du nouveau sommet iv 
      h0 = Max(recti.y[ivl],recti.y[ivr]);
      if((h = recti.hauteur(left,right)) != 0)
	  {h += 2;h = Max(h,h0);}
      else if(recti.y[ivl] != recti.y[ivr])
	  h = h0;
      else
	  h = recti.y[ivl] + 2;
      recti.y[iv] = h;
      // calcul des pousseurs introduits sur les aretes 
      recti.con_iv(left,right);
      }
  // resolution des contraintes
  recti.resolve();
  int xmax = recti.lrver();
  
  // Erase added edges
  for(tedge e = G.ne();e > OldNumEdge;e--)
      G.DeleteEdge(e);
  // Defines properties for the drawing
  Prop<int> pxliv(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> pxriv(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> py(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> pxje(G.Set(tedge()),PROP_DRAW_INT_4);
  pxliv.vector() = recti.xliv;  pxriv.vector() = recti.xriv;  
  py.vector() = recti.y;  pxje.vector() = recti.xje;
  // define the boundaries
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  pmin() = Tpoint(.0,-1.);
  pmax() = Tpoint((double)xmax+1.,(double)recti.y[iv3]+ 1.);
  return 0;
  }
