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
void initMenuTest()
  {pigaleWindow *mw =  GetpigaleWindow();
  mw->setUserMenu(1,"TestPlanar x1000");
  mw->setUserMenu(2,"Nothing");
  mw->setUserMenu(3,"Properties");
  }
int Test(GraphContainer &GC,int action)
  {TopologicalGraph G(GC);
  GeometricGraph GG(GC);
  
  if(action == 1)
      {for (int i=1; i<=1000; i++)
          GG.TestPlanar();
      return 0;
      }
  if(action == 3) 
      // display  the properties of the current graph that would be saved in a tgf file.
      {qDebug("Vertices:");
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
      }
  return 0;
  }

