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
  // hw something like optimal mean distances
  double mhw = Min(gwp->canvas->width(),gwp->canvas->height()) - 2*BORDER;
  Tpoint center((w - space - sizerect)/2.,gwp->canvas->height()/2.); 
  int n = G.nv();
  double hw = .5*(mhw*mhw)/(n*n);
  int iter,niter = 1000;
  double dist2,strength;
  Tpoint p0,p;

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
	  for(tedge e = 1; e <= G.ne();e++)
	      {tvertex v = G.vin[e], w = G.vin[-e];
	      if(v0 == v || v0 == w)continue;
	      p = (G.vcoord[v]+G.vcoord[w])/2.;
	      dist2 = Max(Distance2(p0,p),1.);
	      strength = (hw/dist2);
	      translate[v0] += (p0 - p)*strength;
	      }
	  }
      // adjacent vertices are attracted
      for(tedge e = 1; e <= G.ne();e++)
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
	  strength = Min(sqrt(hw/dist2),.5);// if min too big -> unstability
	  translate[v0] -= (p0 - center)*strength;
	  }

      // update the drawing
      double dx,dy,dep = .0;
      for(tvertex v = 1;v <= n;v++)
	  {G.vcoord[v] += translate[v];
	  dx = Abs(translate[v].x()); dy = Abs(translate[v].y());
	  dep = Max(dep,dx);  dep = Max(dep,dy);
	  if(dx > 1. || dy > 1.) 
	      nodeitem[v]->moveBy(translate[v].x(),-translate[v].y());
	  }
      if(dep < .5)break;
      canvas()->update(); 
      qApp->processEvents(5);
      if(gwp->mywindow->getKey() == Qt::Key_Escape)break;
      }
  Tprintf("Spring iter=%d",iter);
  DoNormalise = true;
  Normalise();load(false);
  }


