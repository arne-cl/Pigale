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
static int Test1(GraphContainer &GC,int &drawing);
static int Test2(GraphContainer &GC,int &drawing);
static int Test3(GraphContainer &GC,int &drawing);

void pigaleWindow:: initMenuTest()
  {setUserMenu(1,"TestPlanar x1000");
  setUserMenu(2,"TestPlanar2 x1000");
  setUserMenu(3,"Properties");
  }
int Test(GraphContainer &GC,int action,int &drawing)
  {if(action == 1)return Test1(GC,drawing);
  else if(action == 2)return Test2(GC,drawing);
  return Test3(GC,drawing);
  }
int Test2(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
   for (int i=1; i<=1000; i++)
       G.TestPlanar2();
   return 0;
  }
int Test1(GraphContainer &GC,int &drawing)
  {GeometricGraph GG(GC);
  for (int i=1; i<=1000; i++)
      GG.TestPlanar();
  drawing = 0;
  return 0;
  }
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












