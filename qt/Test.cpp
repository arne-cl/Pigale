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

// Allowed return values of Test 
//-1:Error 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 

inline double abs(double x) {if(x>=0) return x; else return -x;}
static int Test1(GraphContainer &GC);
static int Test2(GraphContainer &GC);
static int Test3(GraphContainer &GC);

void initMenuTest()
  {pigaleWindow *mw =  GetpigaleWindow();
  mw->setUserMenu(1,"TestPlanar x1000");
  mw->setUserMenu(2,"3-Con Triangulate");
  mw->setUserMenu(3,"Properties");
  }
int Test(GraphContainer &GC,int action)
  {if(action == 1)return Test1(GC);
  else if(action == 2)return Test2(GC);
  return Test3(GC);
  }
int Test3(GraphContainer &GC)
  {GeometricGraph GG(GC);
  for (int i=1; i<=1000; i++)
      GG.TestPlanar();
  return 0;
  }
int Test1(GraphContainer &GC)
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
  return 0;
  }

//#include <TAXI/KantShel.h>
int Test2(GraphContainer &GC)
  {TopologicalGraph G(GC);
  //int m0 =  G.ne();
  int err = G.TriconTriangulate();
  int diff = G.ne() - 3*G.nv() +6;
  //qDebug("err:%d diff:%d     %d -> %d",err,diff,m0,G.ne());
  if(diff)err = -2;
  if(err){setError(-1,"TriconTriangulate Error");return -1;}
  return 2;
  }
//#include <TAXI/KantShel.h>
//   tbrin fb = G.extbrin();
//  KantShelling KS(G,fb);
//  tbrin left,right;
//  while (KS.FindNext(left,right) != 0)
//      ;
//  int err = KS.Error();
//  if(err)
//      {qDebug("KS ERROR:%d",err);
//      setError(-1,"KS Error");
//      }
//   return err;
