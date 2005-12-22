#include <qapplication.h>
#include "GraphWidget.h"
#include "pigaleWindow.h"
#include <QT/Action_def.h> 
#include <QT/Misc.h> 
#include <QT/pigaleCanvas.h>
#include <QT/GraphWidgetPrivate.h>

#ifndef _WINDOWS
using namespace std;
#endif

int EmbedPolrecLR(TopologicalGraph &G);

// Allowed return values of Test 
// -1:error 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
// 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders

inline double abs(double x) {if(x>=0) return x; else return -x;}
static int Test1(GraphContainer &GC,int &drawing);
static int Test2(GraphContainer &GC,int &drawing);
static int Test3(GraphContainer &GC,int &drawing);
#if VERSION_ALPHA
void pigaleWindow:: initMenuTest()
  {setUserMenu(1,"BFS drawing");
  setUserMenu(2,"test ST");
  setUserMenu(3,"Properties");
  }
#else
void pigaleWindow:: initMenuTest()
  {setUserMenu(1,"1000xTestPlanar");
  setUserMenu(2,"1000xTestPlanar2");
  setUserMenu(3,"Properties");
  }
#endif
int Test(GraphContainer &GC,int action,int &drawing)
  {if(action == 1)return Test1(GC,drawing);
  else if(action == 2)return Test2(GC,drawing);
  return Test3(GC,drawing);
  }
/******************************************************************************************************/
/******************************************************************************************************/
#if VERSION_ALPHA
bool find2ContractibleEdges(TopologicalGraph &G,tbrin in, tbrin &b1,tbrin &b2)
// In a max planar graph find two contractible edges, adjacent to in.
// C4 are not yet checked
  {tvertex v = G.vin[in];
  int degree = G.Degree(v);
  svector<tvertex> voisins(1,degree);   voisins.SetName("voisins");
  svector<tbrin> bv(1,degree);   bv.SetName("bv");// brins adjacent to v
  svector<bool> vAdj(1,G.nv());  vAdj.SetName("vAdj"); vAdj.clear(); // vertices adjacent to v <=> true
  svector<bool> notCon(1,degree);  notCon.SetName("notCon"); notCon.clear(); // contractible <=> false
  // mark vertices adjacent to  G.vin[b]
  tbrin b = in;
  int nv = 0;
  do
      {voisins[++nv] = G.vin[-b];
      bv[nv] = b;
      vAdj[G.vin[-b]] = true;
      }while((b = G.acir[b]) != in);

  // mark  vertices of vAdj which are adjacent
  notCon[1] = true; 
  notCon[nv] = true; 
  for(nv = 1; nv <= degree;nv++)
      {tbrin b0 = G.pbrin[voisins[nv]];
      b = b0;
      do
          {tvertex w = G.vin[-b];
          if( v == w || vAdj[w] == false)continue;
          if(G.acir[b] == -bv[nv] || G.cir[b] == -bv[nv])continue;
          notCon[nv] = true;
          }while((b = G.acir[b]) != b0);
      }

 int numContractible = 0;
 int nv1 = 0,nv2 = 0;
 for(nv = 1; nv <= degree;nv++)
     {if(notCon[nv] == true)continue;
     ++numContractible;
     if(nv1 == 0) nv1 = nv;
     else if(nv1 && nv != nv1+1)nv2 = nv;
     }

 if(nv2 == 0)
     {qDebug("v=%d degree:%d contractible:%d",v(),degree,numContractible);
     for(nv = 1; nv <= degree;nv++)
         qDebug("voisins:%d contractible:%d",voisins[nv](),1-(int)notCon[nv]);
     return false;
     }
 else 
     {if( bv[nv1].GetEdge() > bv[nv2].GetEdge())
         {b1 = bv[nv1];b2 = bv[nv2]; }
     else
         {b1 = bv[nv2];b2 = bv[nv1]; }
     return true;
     }
  }
tbrin findBrinLabel(GeometricGraph &G,tvertex v,long label)
  {tbrin brin = 0;
  tbrin b0 = G.pbrin[v];
  tbrin b = b0;
  do
      {if(G.elabel[b.GetEdge()] == label){brin = b;break;}
      }while((b = G.acir[b]) != b0);
  if(b() && G.vin[b] != v)
      brin = brin.opposite();
  //qDebug(" found at %d label:%ld  -> %d %d",v(),label,G.vin[brin](),G.vin[-brin]());
  return brin;
  }
int Test2(GraphContainer &GC,int &drawing)
  {GeometricGraph G(GC);
  for(tedge e = 1; e <= G.ne();e++)G.elabel[e] = e();
  for(tvertex vv = 1; vv <= G.nv();vv++)G.vlabel[vv] = vv();
  G.ComputeGeometricCir();
  G.extbrin() =  G.acir[G.extbrin()];
  Prop<bool> EdgeToDelete(G.Set(tedge()),PROP_MARK); 
  EdgeToDelete.clear();
  long extEdgeLabel =  G.elabel[G.extbrin().GetEdge()];
  tvertex v = G.vin[ G.acir[G.extbrin()]];
  tbrin brin =  G.extbrin();
  qDebug("ext %ld %ld (%ld)",G.vlabel[G.vin[brin]],G.vlabel[G.vin[-brin]],G.elabel[brin.GetEdge()]);

  tbrin b1,b2;
  bool xx = true;
  // Reverse pour contracter vers le bon sommet
  while(xx &&G.nv() > 4 && find2ContractibleEdges(G,brin,b1,b2))
      {EdgeToDelete[G.acir[-b1].GetEdge()] = true;      EdgeToDelete[G.cir[-b1].GetEdge()] = true;
      EdgeToDelete[G.acir[-b2].GetEdge()] = true;      EdgeToDelete[G.cir[-b2].GetEdge()] = true;
      qDebug("A %ld %ld (%ld) ",G.vlabel[G.vin[b1]],G.vlabel[G.vin[-b1]],G.elabel[b1.GetEdge()]);
      qDebug("B %ld %ld (%ld) ",G.vlabel[G.vin[b2]],G.vlabel[G.vin[-b2]],G.elabel[b2.GetEdge()]);
      tedge je =b1.GetEdge();
      if(G.vin[je] == v)G.ReverseEdge(je);
      v = G.ContractEdge(je); 
      je =b2.GetEdge();
      if(G.vin[je] == v)G.ReverseEdge(je);
      v = G.ContractEdge(je);
      // erase
      Forall_adj_edges(je,v,G)
        {if( EdgeToDelete[je])G.DeleteEdge(je);
        }
      brin = findBrinLabel(G,v,extEdgeLabel);
      qDebug("ext %ld %ld (%ld)",G.vlabel[G.vin[brin]],G.vlabel[G.vin[-brin]],G.elabel[brin.GetEdge()]);
      //xx = false;
      if(!G.CheckSimple())break;
      }
  if(G.nv() > 4)setError(-1,"not reduced");
  G.Set(tedge()).erase(PROP_MARK);
  return 2;
  }

int Test1(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
    int morg=G.ne();
    if(!G.CheckConnected())G.MakeConnected();
    BFSOrientTree(G,tvertex(1));
    Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH,1);
    Prop<bool> istree(G.Set(tedge()),PROP_ISTREE);
    for (tedge e=1; e<=morg; e++)
      if (istree[e]) ewidth[e]++;
    G.RestoreOrientation(); 
    GeometricGraph GG(G);
    ColorPoles(GG);
    tvertex t = G.NewVertex();
    for (tvertex v=1; v<G.nv(); v++)
      if (G.OutDegree(v)==0) G.NewEdge(v,t);
    Vision(G,morg);  
    G.DeleteVertex(t);
    while (G.ne()>morg) G.DeleteEdge(G.ne());
    drawing=2;
  return 3;
  }
#else 
int Test1(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
  for(int i = 0;i < 1000;i++)G.TestPlanar();
  return 0;
  }
int Test2(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
  for(int i = 0;i < 1000;i++)G.TestPlanar2();
  return 0;
  }
#endif
int Test3(GraphContainer &GC,int &drawing)
// display  the properties of the current graph that would be saved in a tgf file.
  {TopologicalGraph G(GC);
  qDebug("Vertices:");
  int i;
  for (i=G.Set(tvertex()).PStart(); i<G.Set(tvertex()).PEnd(); i++)
      if (G.Set(tvertex()).exist(i))
          if (G.Set(tvertex()).defined(i)) 
              qDebug("  %d %s (%s) -> %d bytes",i,PropName(1,i),PropDesc(1,i),
                     (1+G.nv())*G.Set(tvertex())(i)->size_elmt());
          else
              qDebug("  %d %s (%s)",i,PropName(1,i),PropDesc(1,i));
  qDebug("Edges:");
  for (i=G.Set(tedge()).PStart(); i<G.Set(tedge()).PEnd(); i++)
      if (G.Set(tedge()).exist(i))
          if (G.Set(tedge()).defined(i)) 
              qDebug("  %d %s (%s) -> %d bytes",i,PropName(2,i),PropDesc(2,i),
                     (1+G.ne())*G.Set(tedge())(i)->size_elmt());
          else
              qDebug("  %d %s (%s)",i,PropName(2,i),PropDesc(2,i));
  qDebug("Brins (half-edges):");
  for (i=G.Set(tbrin()).PStart(); i<G.Set(tbrin()).PEnd(); i++)
      if (G.Set(tbrin()).exist(i))
          if (G.Set(tbrin()).defined(i)) 
              qDebug("  %d %s (%s) -> %d bytes",i,PropName(3,i),PropDesc(3,i),
                     (1+2*G.ne())*G.Set(tbrin())(i)->size_elmt());
          else
              qDebug("  %d %s (%s)",i,PropName(3,i),PropDesc(3,i));
  qDebug("General:");
  for (i=G.Set().PStart(); i<G.Set().PEnd(); i++)
      if (G.Set().exist(i))
          qDebug("  %d %s (%s)",i,PropName(0,i),PropDesc(0,i));
  drawing = 0;
  return 0;
  }












