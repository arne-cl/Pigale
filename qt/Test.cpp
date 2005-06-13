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
  setUserMenu(2,"Polrec drawing");
  setUserMenu(3,"Properties");
  }
#else
void pigaleWindow:: initMenuTest()
  {setUserMenu(1,"1000xTestPlanar");
  setUserMenu(2,"Polrec drawing");
  setUserMenu(3,"Properties");
  }
#endif
int Test(GraphContainer &GC,int action,int &drawing)
  {if(action == 1)return Test1(GC,drawing);
  else if(action == 2)return Test2(GC,drawing);
  return Test3(GC,drawing);
  }
int Test2(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
  if(EmbedPolrecLR(G) != 0)return -1;
  drawing = 8;
  return 3;
  }

#if VERSION_ALPHA
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












