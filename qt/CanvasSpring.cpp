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

#include "MyWindow.h"
#include "GraphWidget.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <TAXI/Tbase.h> 
#include <QT/MyQcolors.h> 
#include <QT/Misc.h> 
#include <QT/MyCanvas.h>
#include <QT/GraphWidgetPrivate.h>
#include <qapplication.h> 


void GraphEditor::Spring()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  svector<Tpoint> translate(1,G.nv()); //translate.clear();
  double w = gwp->canvas->width();
  double mhw = Min(gwp->canvas->width(),gwp->canvas->height()) - 2*BORDER;
  Tpoint center((w - space - sizerect)/2.,gwp->canvas->height()/2.); 
  int n = G.nv(),m =G.ne();
  // during iteration keeep the drawing size
  double hw = .8*(mhw*mhw)/(n*m); 
  int iter,niter = 2000;
  double dist2,strength;
  Tpoint p0,p;
  gwp->mywindow->blockInput(true);
  for(iter = 1;iter <= niter;iter++)
      {for(tvertex v0 = 1;v0 <= n;v0++)
	  {p0 = G.vcoord[v0];
	  // vertices repulse each other, more when they are close to each other
	  for(tvertex v = 1;v <= n;v++)
	      {if(v == v0)continue;
	      p = G.vcoord[v];
	      dist2 = Max(Distance2(p0,p),1.);
	      strength = (hw/dist2);
	      translate[v0]  = (p0 - p)*strength; 
	      }
	  // edges repulse non adjacent vertices
	  // now too simple solution
	  for(tedge e = 1; e <= m;e++)
	      {tvertex v = G.vin[e], w = G.vin[-e];
	      if(v0 == v || v0 == w)continue;
	      p = (G.vcoord[v]+G.vcoord[w])/2.;
	      dist2 = Max(Distance2(p0,p),1.);
	      strength = (hw/dist2);
	      translate[v0] += (p0 - p)*strength;
	      }
	  }

      // adjacent vertices are attracted
      for(tedge e = 1; e <= m;e++)
	  {p0 = G.vcoord[G.vin[e]]; p = G.vcoord[G.vin[-e]];
	  dist2 = Max(Distance2(p0,p),1.);
	  strength = Min(sqrt(hw/dist2),.1);
	  translate[G.vin[e]]  -= (p0-p)*strength;
	  translate[G.vin[-e]] += (p0-p)*strength;
	  }

      // vertices are attracted by the center
      for(tvertex v0 = 1;v0 <= n;v0++)
	  {p0 = G.vcoord[v0];
	  dist2 = Max(Distance2(p0,center),1.);
	  strength = Min(sqrt(hw/dist2),.25);
	  translate[v0] -= (p0 - center)*strength;
	  }

      // update the drawing
      double dx,dy,dep = .0;
      for(tvertex v = 1;v <= n;v++)
	  {G.vcoord[v] += translate[v];
	  dx = Abs(translate[v].x()); dy = Abs(translate[v].y());
	  dep = Max(dep,dx);  dep = Max(dep,dy);
	  if(dx > 1. || dy > 1.) 
	      {nodeitem[v]->moveBy(translate[v].x(),-translate[v].y());
	      nodeitem[v]->SetColor(color[G.vcolor[v]]);
	      }
	  else
	      nodeitem[v]->SetColor(red);
	  }
      if(dep < .5)break;
      canvas()->update(); 
      qApp->processEvents(1);
      if(gwp->mywindow->getKey() == Qt::Key_Escape)break;
      }
  gwp->mywindow->blockInput(false);
  Tprintf("Spring iter=%d",iter-1);
  DoNormalise = true;
  Normalise();load(false);
  }

//******************** JACQUARD SPRING EMBEDDER 
//  ****************** VARIABLES D'EQUILIBRAGE
int isinf (double x)
  {if ( x <= 0.0000001 )return 1;
  return 0;
  }


// l'ordre minimum de deux sommets d'une arete pour qu'un sommet
// virtuel soit ajouter (si l'option est selectionnee i.e
// si addVirtualVertex est vrai)
double coefDiff = 0.707;
int DEFAULT_ADD_VIRTUAL_DEGRE = 4;
int MAX_GENERATIONS = 1000;
double K_ANGLE = 10;
double K_MINDIST = 100;
double K_ELECTRO = 10;
double K_SPRING = 0.1;
bool K_FRONTIER = false;

double _4PI2 = 4*PI*PI;
struct tk {double angle,spring,mindist,electro;bool frontier;};



Tpoint ZERO = Tpoint();

/**************************************************************
**                Equilibrage de la Carte                    **
**************************************************************/

/* pour les pottentiels
** on somme les interactions entre les sommets
** et les aretes chargees lineiquement
** l'entier est pour la ponderation
*/

/* isCrossing
** verifie que deux edges, donnés chacun par un de leurs brins, se coupent
*/

static bool isIntersecting(Tpoint &p1, Tpoint &p2, Tpoint &p3, Tpoint &p4)
  { double d1,d2,d3,d4;
  d1 = Determinant(p2 - p1, p4 - p1);
  d2 = Determinant(p2 - p1, p3 - p1);
  d3 = Determinant(p1 - p3, p4 - p3);
  d4 = Determinant(p2 - p3, p4 - p3);
  if ( (d1 == 0) && ((p4.x()-p2.x())*(p4.x()-p1.x())<=0) && ((p4.y()-p2.y())*(p4.y()-p1.y())<=0) )
      {return true;}
  if ( (d2 == 0) && ((p3.x()-p2.x())*(p3.x()-p1.x())<=0) && ((p3.y()-p2.y())*(p3.y()-p1.y())<=0) )
      {return true;}
  if ( (d3 == 0) && ((p1.x()-p4.x())*(p1.x()-p3.x())<=0) && ((p1.y()-p4.y())*(p1.y()-p3.y())<=0) )
      {return true;}
  if ( (d4 == 0) && ((p2.x()-p4.x())*(p2.x()-p3.x())<=0) && ((p2.y()-p4.y())*(p2.y()-p3.y())<=0) )
      {return true;}
  if (( ((d1<0) && (d2>0)) || ((d1>0) && (d2<0)) ) &&
      ( ((d3<0) && (d4>0)) || ((d3>0) && (d4<0)) ) )
      {return true;}
  else
      {return false;}
  }
static int isCrossing(tbrin e, tbrin f, GeometricGraph &G)
  {return isIntersecting(G.vcoord[G.vin[e]], G.vcoord[G.vin[-e]],G.vcoord[G.vin[f]], G.vcoord[G.vin[-f]]);}


/* getAngle brinOrderOk
** getAngle rend l'angle d'un brin entre 0 et 2PI
** brinOrderOk verifie que l'orde de trois brins successifs
** est le meme en coordonnees que celui de la permutation
*/

static double getAngle(tbrin b,GeometricGraph &G)
  {double tan_angle;
  double angle;
  double x1= (G.vcoord[G.vin[b]]).x();
  double y1= (G.vcoord[G.vin[b]]).y();
  double x2= (G.vcoord[G.vin[-b]]).x();
  double y2= (G.vcoord[G.vin[-b]]).y();
    
  if(x1 == x2) 
      {if (y1>y2)
          {return 3*PI/2;} 
      else
          {return PI/2;}
      }
  else
      {tan_angle= (y2-y1)/(x2-x1);
      angle= atan(tan_angle);
      if (x1>x2)
          {return angle+PI;}
      else
          {if (y1>y2) 
              {return angle+2*PI;}
          else
              {return angle;}
          }
      }
  }

/* brinOrderOk
** teste que le brin, son predecesseur et son successeur
** on conserve un ordre normal
*/

static bool brinOrderOk(GeometricGraph &G,tbrin b)
  {int deg = G.Degree(G.vin[b]);
  // b est le brin du milieu
  if (deg == 1)
      {return true;}
  if (deg == 2)
      {double i = getAngle(G.acir[b], G);
      double j = getAngle(b, G);
      if (i == j)
          {return false;}
      else
          {return true;}
      }
  // on a au moins trois brins
  double i = getAngle(G.acir[b],G);
  double j = getAngle(b,G);
  double k = getAngle(G.cir[b],G);
    
  if ( ((i<j) && (j<k)) || ((j<k) && (k<i)) || ((k<i) && (i<j)) )
      {return true;}
  else
      {return false;}
  }


/* CrossingEdgesGraph
** Graphe reliant les aretes faisant partie d'une
** meme face dans le graphe de référence
*/
GraphContainer * TopologicalGraph::CrossingEdgesGraph()
  {if(FindPlanarMap() !=0)
      {DebugPrintf("Error Computing the CEG:no planar map");
      return (GraphContainer *)0;
      }
    
  int m = ne();
  tedge e,ee;
  tbrin b,fb;
  tvertex v;
  GraphContainer &CEG(*new GraphContainer);
  CEG.setsize(m,0);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleCEG(CEG.Set(),PROP_TITRE);
  titleCEG() = "CEG-" + title();
  Prop<tvertex> evin(CEG.PB(),PROP_VIN);    evin.clear();
  svector<tedge> voisin(0,m);             voisin.clear();

  ForAllEdgesOfG(e)
      {v = tvertex(e());
      fb = FirstBrin(vin[e]);
      b = fb;
      do
          {voisin[b.GetEdge()]=e;
          b=cir[b];
          }
      while (b!=fb);
      fb = FirstBrin(vin[-e]);
      b = fb;
      do
          {voisin[b.GetEdge()]=e;
          b=cir[b];
          }
      while (b!=fb);
      b=-cir(e);
      ee=b.GetEdge();
      while (ee!=e)
          {if ((ee>e) && (voisin[ee]!=e))
              {CEG.incsize(tedge());
              tbrin eb = CEG.ne(); 
              evin[eb] = v;
              evin[-eb] = tvertex(ee());
              voisin[ee]=e;
              }
          b=-cir(b);
          ee=b.GetEdge();
          }
      b=-cir(-e);
      ee=b.GetEdge();
      while (ee!=e)
          {if ((ee>e) && (voisin[ee]!=e))
              {CEG.incsize(tedge());
              tbrin eb = CEG.ne(); 
              evin[eb] = v;
              evin[-eb] = tvertex(ee());
              voisin[ee]=e;
              }
          b=-cir(b);
          ee=b.GetEdge();
          }
      }
  return &CEG;
  }

/* canMove
** verifie que le deplacement d'un vertex est valide
** pas d'intersection ou d'intervertion de edges
*/
static bool canMove(tvertex v, Tpoint Offset, TopologicalGraph &CEG, GeometricGraph &G)
  {
  /* on verifie d'abord que le vertex n'est pas
  ** superpose a un autre ou qu'il n'y a pas
  ** d'intervertion
  */

  tbrin premierbrin = G.FirstBrin(v);
  G.vcoord[v] += Offset;
  tbrin b,b2,fb;
  b = premierbrin;
  do
      {if (!brinOrderOk(G,-b))
          {G.vcoord[v] -= Offset;
          return false;
          }
      b = G.cir[b];
      }
  while (b!=premierbrin);

  b = premierbrin;
  // et on verifie l'ordre des aretes de la face
  double angle = getAngle(b,G);
  double angleNew;
  double angle0 = angle;
  bool desc = false; // si on a plus d'une descente -> inversion
  b= G.cir[b];

  while (b != premierbrin)
      {angleNew = getAngle(b,G);
      if (angle == angleNew)
          {G.vcoord[v]  -= Offset;
          return false;
          }
      if (!desc)
          {if (angleNew < angle)
              {desc = true;
              if (angleNew >= angle0)
                  {G.vcoord[v] -= Offset;
                  return false;
                  }
              }
          }
      else
          {if ( (angleNew < angle) || (angleNew >= angle0) )
              {G.vcoord[v] -= Offset;
              return false;
              }
    
          }
      angle = angleNew;
      b= G.cir[b];
      }


  // puis les intersections des edges precalcules
  b = premierbrin;
  do
      {fb = CEG.FirstBrin(tvertex(Abs(b())));
      b2 = fb;
      do
          {if (isCrossing(b,tbrin(CEG.vin[-b2]()),G))
              {G.vcoord[v] -= Offset;
              return false;
              }
          b2 = CEG.cir[b2];
          }
      while (b2!=fb);
      b=G.cir[b];
      }
  while (b!=premierbrin);

  G.vcoord[v] -= Offset;
  return true;
  }


/* potentiel
** calcul le potentiel d'un sommet qui viens de bouger
** pour chaque arete incidente, on somme les potentiels des aretes 
** avec les sommets de sa face
*/
static double potEdge(tvertex const &v, tbrin const &brin, tk &k, GeometricGraph &G)
  {Tpoint a,b,c;
  c = (G.vcoord[v]);
  a = (G.vcoord[G.vin[brin]]);
  b = (G.vcoord[G.vin[-brin]]);
    
  double ab = Distance(a,b);
  double h = fabs(Determinant(c-a,b-a))/ab;
  // on donne a h une valeur minimum
  if ( h < 0.0001 )
      {h = 0.0001;}
  double la = ((b-a)*(c-a))/h/ab;
  double lb = ((b-a)*(c-b))/h/ab;
  // minDist vaut la distance minimal de l'arete au sommet
  double minDist;
  if (la*lb < 0)
      {minDist = h;}
  else
      {double bc = Distance(b,c);
      double ac = Distance(a,c);
      minDist = bc<ac? bc: ac;
      }
  if ( minDist < 0.001)
      {minDist = 0.001;}

  double electro = log((la+sqrt(1+la*la))/(lb+sqrt(1+lb*lb)));
  return   (k.mindist/minDist + k.electro*electro);
  }

/* Vis_a_vis_Graph
** Graphe reliant les sommets aux aretes non incidentes mais faisant partie d'une
** meme face dans le graphe de référence
*/


GraphContainer * TopologicalGraph::Vis_a_vis_Graph()
  {if(FindPlanarMap() !=0)
      {DebugPrintf("Error Computing the adj:no planar map");
      return (GraphContainer *)0;
      }
    
  int n = nv();
  int m = ne();
  int nn = n+m;
    
  tedge e;
  tbrin b;
  tbrin Vavb;
  tvertex v;
  tvertex vv;
  GraphContainer &Vav(*new GraphContainer);
  Vav.setsize(nn,0);
  Prop1<tstring> title(Set(),PROP_TITRE);
  Prop1<tstring> titleVav(Vav.Set(),PROP_TITRE);
  titleVav() = "Vis_a_vis-" + title();
  Prop<tvertex> vvin(Vav.PB(),PROP_VIN);    vvin.clear();
  svector<tedge> LastEdge(0,nv());        LastEdge.clear();

  ForAllEdgesOfG(e)
      {LastEdge[vin[e]]=e;
      LastEdge[vin[-e]]=e;
      vv = tvertex(e()+ n);
      b = -cir(e);
      v= vin[b];
      while (b.GetEdge()!=e)
          {if (LastEdge[v]!=e)
              {Vav.incsize(tedge());
              tbrin vb = Vav.ne(); 
              vvin[vb] = v;
              vvin[-vb] = vv;
              LastEdge[v]=e;
              }
          b = -cir(b);
          v= vin[b];
          }
      b = -cir(-e);
      v = vin[b];
      while (b.GetEdge()!=e)
          {if (LastEdge[v]!=e)
              {Vav.incsize(tedge());
              tbrin vb = Vav.ne(); 
              vvin[vb] = v;
              vvin[-vb] = vv;
              LastEdge[v]=e;
              }
          b = -cir(b);
          v= vin[b];
          }
      }
  return &Vav;
  }
static int init_extvertex(GeometricGraph &G, svector<int> &ev)
  {tbrin b,fb;
  int nb = 0;
  G.ComputeGeometricCir();
  ev.clear();
  fb = G.FindExteriorFace();
  b = fb;
  do
      {tvertex v = G.vin[b];
      ev[v]++;
      if (ev[v] == 1)nb++;
      b= G.cir[-b];
      }while (b!=fb);
  return nb;
  }

static double potentiel(tvertex v, Tpoint Offset, tk &k, GeometricGraph &G,TopologicalGraph &Vav,svector<int> extvertex)
  {tbrin b,b2,fb,fb2;
  tedge e;
  int n = G.nv();
  G.vcoord[v] += Offset;
  double pot = 0;

  // on somme les potentiels des edges
  fb = Vav.FirstBrin(v);
  b = fb;
  do
      {pot += potEdge(v,(tbrin)(Vav.vin[-b]()-n),k,G);
      b = Vav.cir[b];
      }
  while (b!=fb);
  fb2 = G.FirstBrin(v);
  b2 = fb2;
  do
      {tvertex w = tvertex(Abs(b2())+n);
      fb = Vav.FirstBrin(w);
      b = fb;
      do
          {pot += potEdge(Vav.vin[-b],b2,k,G);
          b = Vav.cir[b];
          }
      while (b!=fb);
      b2 = G.cir[b2];
      }
  while (b2!=fb2);
    
  if (isnan(pot)) {DebugPrintf("potentiel arete-sommet =%f pour v=%d",pot,v());}
    
  // potentiel de type ressort
  if(extvertex[v] != 1)
      {double carre=0;
      double lastAngle = getAngle(G.FirstBrin(v),G);
      double newAngle,diffAngle,angle;
      Forall_adj_brins(b,v,G)
          {Tpoint p1=G.vcoord[v];
          Tpoint p2=G.vcoord[G.vin[-b]];
          carre += Distance2(p1,p2);
          if (G.cir[-b]!=-b)
              {// le premier angle en face
              angle = getAngle(-b,G);
              diffAngle = getAngle(G.cir[-b],G)-angle;
              if ( diffAngle < 0 )
                  {diffAngle += 2*PI;}
              if ( diffAngle != 0 )            
                  // si -b = cir(-b), diffAngle = 0
                  {pot += k.angle/diffAngle;
                  // le second angle en face
                  diffAngle = angle-getAngle(G.acir[-b],G);
                  if ( diffAngle < 0 )
                      {diffAngle += 2*PI;}
                  pot += k.angle/diffAngle;
                  }
              // termine. Calcul du sommet actuel
              }
          newAngle = getAngle(G.cir[b],G);
          diffAngle = newAngle-lastAngle;
          if ( diffAngle < 0 )
              {diffAngle += 2*PI;}
          if ( diffAngle != 0 )
              {pot += k.angle/diffAngle;}
          lastAngle = newAngle;
          }
      if (isnan(pot)) {DebugPrintf("potentiel repulsif des angles=%f pour v=%d",pot,v());}
      pot +=  k.spring*carre;
      if (isnan(pot)) {DebugPrintf("potentiel attractif des sommets=%f pour v=%d",pot,v());}
      }
	  
  G.vcoord[v] -= Offset;
  return pot;
  }

/* tryMove
** rend le carre de la distance parcourue par le sommet 
** en suivant le potentiel
** si ce nombre est nul le sommet est bloqué
*/

static double tryMove(tvertex v, svector<Tpoint> &sumDep, tk &k, TopologicalGraph &Vav, TopologicalGraph &CEG, svector<int> extvertex, GeometricGraph &G)
  {Tpoint dMin;
  Tpoint dTotal = ZERO;
  bool moved;
  double MinPot;
  double tmp,tmp1;
  // on réduit sumDx sumDy par un coef
  sumDep[v] *= coefDiff;
  Tpoint diff;
  Tpoint mov;
  double scale;
  Tpoint xplusun = Tpoint(0.1,0);
  Tpoint yplusun = Tpoint(0,0.1);

  while (true)
      {MinPot = potentiel(v,ZERO,k,G,Vav,extvertex);
      if (canMove(v,xplusun,CEG,G) )
          {diff.x() = potentiel(v,xplusun,k,G,Vav,extvertex) - MinPot;}
      else
          {if( canMove(v,-xplusun,CEG,G))
              {diff.x() = MinPot - potentiel(v,-xplusun,k,G,Vav,extvertex);}
          else
              {diff.x() = 0;}
          }
      if(canMove(v,yplusun,CEG,G))
          {diff.y() = potentiel(v,yplusun,k,G,Vav,extvertex) - MinPot;}
      else
          {if(canMove(v,-yplusun,CEG,G))
              {diff.y() = MinPot - potentiel(v,-yplusun,k,G,Vav,extvertex);}
          else
              {diff.y() = 0;}
          }
      // simplement une normalisation exacte (le max vaut 1)
      if (fabs(diff.x())<1E-6) diff.x()=0;
      if (fabs(diff.y())<1E-6) diff.y()=0;      
      tmp1 = Distance(diff,ZERO);
      if (tmp1 < 1E-6) {return 0;}
      if (isinf(diff.x())) {mov = -(double)isinf(diff.x()) * xplusun;}
      else if (isinf(diff.y())) {mov = -(double)isinf(diff.y()) * yplusun;}
      else mov = ( -diff / tmp1);
      scale = 1;
      tmp = MinPot;
      moved = false;
      while ( canMove(v,(mov * scale),CEG,G) && ( ( tmp1 = potentiel(v,(mov * scale),k,G,Vav,extvertex) ) < tmp ) )
          {tmp = tmp1;
          moved = true;
          scale *= 2;
          }
      if (moved)
          {scale /= 2;
          G.vcoord[v] += scale*mov;
          dTotal += scale*mov;
          }
      else
          {break;}
      }

  // methode classique de mouvement a la fin
  moved = true;
  while (moved)
      {dMin = ZERO;
      if (canMove(v,xplusun,CEG,G) && (MinPot>(tmp = potentiel(v,xplusun,k,G,Vav,extvertex))) )
          {dMin = xplusun;
          MinPot = tmp;
          }
      if(canMove(v,-xplusun,CEG,G) && (MinPot>(tmp = potentiel(v,-xplusun,k,G,Vav,extvertex))) )
          {dMin = -xplusun;
          MinPot = tmp;
          }
      if  ( canMove(v,yplusun,CEG,G) && (MinPot>(tmp = potentiel(v,yplusun,k,G,Vav,extvertex))) )
          {dMin = yplusun;
          MinPot = tmp;
          }
      if  ( canMove(v,-yplusun,CEG,G) && (MinPot>(tmp = potentiel(v,yplusun,k,G,Vav,extvertex))) )
          {dMin = -yplusun;
          MinPot = tmp;
          }
      if (dMin == ZERO)
          {moved = false;}
      else
          {G.vcoord[v] += dMin;
          dTotal += dMin;
          // on avance dans la meme direction jusqu'a ce qu'on remonte ou bloque
          while ( canMove(v,dMin,CEG,G) && (MinPot>(tmp = potentiel(v,dMin,k,G,Vav,extvertex))) )
              {G.vcoord[v] += dMin;
              dTotal += dMin;
              MinPot = tmp;
              }
          }
      }
  sumDep[v] += dTotal;

  return dTotal.x()*dTotal.x()+dTotal.y()*dTotal.y();
  }

int GraphEditor::SpringJacquard()			
  {int maxgen      = MAX_GENERATIONS;
  double k_angle   = K_ANGLE;
  double k_mindist = K_MINDIST;
  double k_electro =  K_ELECTRO;
  double k_spring  = .1;
  bool k_frontier  = false;
  GeometricGraph & G = *(gwp->pGG);
  if(G.ComputeGeometricCir() == 0)
      {G.extbrin() = G.FindExteriorFace();
      Prop1<int> map(G.Set(),PROP_PLANARMAP);
      Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
      maptype() = PROP_MAPTYPE_GEOMETRIC;
      }
  else
      {Tprintf("NOT A PLANAR EMBEDDING");return -1;}
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  svector<Tpoint> SumDep(1,G.nv());       
  svector<int> extvertex(1,G.nv());      extvertex.clear();
  svector<bool> blockedvertex(1,G.nv());  blockedvertex.clear();
  TopologicalGraph VAV(*G.Vis_a_vis_Graph());
  TopologicalGraph CEG(*G.CrossingEdgesGraph());
  tvertex v;
  int generations = 0;
  tk k;
  k.angle = k_angle;
  k.mindist = k_mindist;
  k.electro = k_electro;
  k.spring = k_spring;
  k.frontier = k_frontier;
  // initialisation
  int nonblocked = init_extvertex(G,extvertex);
  for(v = 1;v <= G.nv();v++)blockedvertex[v]=false;
  double deplacement = 0;

  //Redimentionnement de la carte
  DoNormalise = true;Normalise();
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
  // Boucle de calcul
  gwp->mywindow->blockInput(true);
  for(;nonblocked && (generations<maxgen);generations++)
      {scoord = G.vcoord;
      for(v = 1;v <= G.nv();v++)// calcul d'une generation
          {if ((k.frontier && extvertex[v]) || (blockedvertex[v]))
              {continue;}
          if (isnan(G.vcoord[v].x()) || isnan(G.vcoord[v].y())) 
	      {DebugPrintf("Error in Jacquard: Emergency exit at v=%d before moving",v());
	      gwp->mywindow->blockInput(false);
	      return generations;
	      }
          deplacement = tryMove(v,SumDep,k,VAV,CEG,extvertex,G);
          if (isnan(G.vcoord[v].x()) || isnan(G.vcoord[v].y())) 
	      {DebugPrintf("Error in Jacquard: Emergency exit at v=%d",v());
	      gwp->mywindow->blockInput(false);
	      return generations;
	      }
          if (extvertex[v] && deplacement<0.01)
              {blockedvertex[v] = true;
              nonblocked--;
	      nodeitem[v]->SetColor(red);
              }
          }
      // size_and_center(centre,*this);
      DoNormalise = true;Normalise();
      // Affichage du graphe
      double dx,dy;
      for(tvertex v = 1; v <= G.nv();v++)
	  {dx = G.vcoord[v].x()- scoord[v].x();
	  dy = G.vcoord[v].y()- scoord[v].y();
	  if(Abs(dx) > 1. || Abs(dy) > 1.)
	      {nodeitem[v]->moveBy(dx,-dy);
	      nodeitem[v]->SetColor(color[G.vcolor[v]]);
	      }
	  else
	      nodeitem[v]->SetColor(red);
	  }      
      canvas()->update();
      qApp->processEvents(1);
      if(gwp->mywindow->getKey() == Qt::Key_Escape)break;
      }
  gwp->mywindow->blockInput(false);
  Tprintf("Spring iter=%d",generations);
  DoNormalise = true;
  Normalise();load(false);
  return generations;
  }
void GraphEditor::keyPressEvent(QKeyEvent *k)
  {if(k->key() == Qt::Key_Escape)
      Tprintf("ESC");
  qDebug("Ackey pressed:%d",k->key());
  }

