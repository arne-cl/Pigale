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

#include "pigaleWindow.h"
#include <QT/pigalePaint.h>
#include <QT/pigaleQcolors.h>
#include <QT/Misc.h>
#include <qpaintdevicemetrics.h> 

void DrawPolar(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_COORD);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint11(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<Tpoint> Epoint12(G.Set(tedge()),PROP_DRAW_POINT_4);
  Prop<Tpoint> Epoint21(G.Set(tedge()),PROP_DRAW_POINT_5);
  Prop<Tpoint> Epoint22(G.Set(tedge()),PROP_DRAW_POINT_6);
  Prop<double> Erho   (G.Set(tedge()),PROP_DRAW_DBLE_1);
  Prop<double> Etheta1(G.Set(tedge()),PROP_DRAW_DBLE_2);
  Prop<double> Etheta2(G.Set(tedge()),PROP_DRAW_DBLE_3);
  Prop1<double> nw(G.Set(),PROP_DRAW_DBLE_1);

  int m = G.ne(); 
  int ox,oy,nx,ny,theta,dt;
  QPen pn = p->pen();pn.setWidth(1);

  for (tedge ee=1; ee<=m; ee++)
      {if (Erho[ee]==-1)
          {paint->DrawSeg(p,Epoint1[ee],Epoint2[ee],Red);
          continue;
          }
      if (Epoint11[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(p,Epoint1[ee],Epoint11[ee],Blue);
          paint->DrawSeg(p,Epoint11[ee],Epoint12[ee],Blue);
          }
      else if (Epoint12[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint1[ee],Epoint12[ee],Blue);
      if (Epoint21[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(p,Epoint2[ee],Epoint21[ee],Blue);
          paint->DrawSeg(p,Epoint21[ee],Epoint22[ee],Blue);
          }
      else if (Epoint22[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint2[ee],Epoint22[ee],Blue);
	  
      pn.setColor(color[Blue]);pn.setWidth(2);p->setPen(pn);
      ox = paint->to_x(-Erho[ee]);
      oy = paint->to_y(Erho[ee]);
      nx = (int)(2*Erho[ee]*paint->xscale + .5);
      ny = (int)(2*Erho[ee]*paint->yscale + .5);
      theta = (int)(Etheta1[ee]*16*180/PI+.5);
      dt = (int)((Etheta2[ee] - Etheta1[ee])*16*180/PI+.5); 
      p->drawArc(ox,oy,nx,ny,theta,dt);
      }
    // Draw the vertices
  /*
    int n = G.nv();
  Prop<long> label(G.PV(),PROP_LABEL);
  DrawNode(Vcoord[1],label[1],Blue);
  for(tvertex v = 2;v <= n;v++)
      DrawNode(Vcoord[v],label[v()],Yellow);
  */
  }
void DrawPolyline(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> Vcoord(G.Set(tvertex()),PROP_DRAW_COORD);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Ebend(G.Set(tedge()),PROP_DRAW_POINT_3);

  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);

  int m = G.ne(); 
  int n = G.nv(); 
  QPen pn = p->pen();pn.setWidth(2);
  QPointArray bez(4);

  for (tedge ee=1; ee<=m; ee++)
      {if (Ebend[ee] != Tpoint(-1, -1)) {
	  paint->DrawSeg(p, Epoint1[ee], Ebend[ee],ecolor[ee]);
	  paint->DrawSeg(p, Ebend[ee], Epoint2[ee],ecolor[ee]);
      }
      else
	  paint->DrawSeg(p, Epoint1[ee], Epoint2[ee] , ecolor[ee]);
      }
  // Draw the vertices
  p->setFont(QFont("lucida",Min((int)(.45 * Min(paint->xscale,paint->yscale) + .5),13)));
  QString t;
  for(tvertex v = 1;v <= n;v++) 
      {if(ShowVertex() == -1)
	  t.sprintf("%2.2d",v());
      else
 	  t.sprintf("%2.2ld",G.vlabel[v()]);
      paint->DrawText(p,Vcoord[v],t,vcolor[v],1);
      }
  }

void DrawCurves(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> Epoint1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> Epoint2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<Tpoint> Epoint3(G.Set(tedge()),PROP_DRAW_POINT_3);
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  int m = G.ne(); 
  int n = G.nv(); 
  QPen pn = p->pen();pn.setWidth(2);
  QPointArray bez(7);

  for (tedge ee=1; ee<=m; ee++)
    {if (Epoint2[ee]!=Tpoint(0,0)) {
       bez.setPoint(0,paint->to_x(vcoord[G.vin[ee]].x()),paint->to_y(vcoord[G.vin[ee]].y()));
       bez.setPoint(1,paint->to_x(Epoint1[ee].x()),paint->to_y(Epoint1[ee].y()));
       bez.setPoint(2,paint->to_x(Epoint1[ee].x()),paint->to_y(Epoint1[ee].y()));
       bez.setPoint(3,paint->to_x(Epoint2[ee].x()),paint->to_y(Epoint2[ee].y()));
       bez.setPoint(4,paint->to_x(Epoint3[ee].x()),paint->to_y(Epoint3[ee].y()));
       bez.setPoint(5,paint->to_x(Epoint3[ee].x()),paint->to_y(Epoint3[ee].y()));
       bez.setPoint(6,paint->to_x(vcoord[G.vin[-ee]].x()),paint->to_y(vcoord[G.vin[-ee]].y()));
#if QT_VERSION >= 300
       p->drawCubicBezier(bez,0);
       p->drawCubicBezier(bez,3);
#else
       p->drawQuadBezier(bez,0);
       p->drawQuadBezier(bez,3);
       //p->drawPolyline(bez,0,7);
#endif
       //paint->DrawRect(p,Epoint1[ee],3,3,Red);
       //paint->DrawRect(p,Epoint3[ee],3,3,Red);
     }
    else
      paint->DrawSeg(p, vcoord[G.vin[ee]], vcoord[G.vin[-ee]] , ecolor[ee]);
      }
  // Draw the vertices
  p->setFont(QFont("lucida",Min((int)(10*Min(paint->xscale,paint->yscale) + .5),13)));
  QString t;
  for(tvertex v = 1;v <= n;v++) 
      {if(ShowVertex() == -1)
	  t.sprintf("%2.2d",v());
      else
 	  t.sprintf("%2.2ld",G.vlabel[v()]);
      paint->DrawText(p,vcoord[v],t,vcolor[v],1);
      }
  }

void DrawTContact(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<Tpoint> hp1(G.Set(tvertex()),PROP_DRAW_POINT_1);
  Prop<Tpoint> hp2(G.Set(tvertex()),PROP_DRAW_POINT_2);
  Prop<Tpoint> vp1(G.Set(tvertex()),PROP_DRAW_POINT_3);
  Prop<Tpoint> vp2(G.Set(tvertex()),PROP_DRAW_POINT_4);
  Prop<Tpoint> postxt(G.Set(tvertex()),PROP_DRAW_POINT_5);
  Prop1<double> sizetext(G.Set(),PROP_DRAW_DBLE_1);
  tvertex v;	
  // Draw horizontals and verticals
  for(v = 1;v <= G.nv();v++)
      {if(hp1[v].x() > .0)paint->DrawSeg(p,hp1[v],hp2[v],Black);
      if(vp1[v].x() > .0)paint->DrawSeg(p,vp1[v],vp2[v],Black);
      }
  // Draw text
  p->setFont(QFont("lucida",Min((int)(sizetext() * Min(paint->xscale,paint->yscale) + .5),13)));
  QString t;
  for(v=1; v <= G.nv();v++)
      {if(ShowVertex() == -1)
	  t.sprintf("%2.2d",v());
      else
 	  t.sprintf("%2.2ld",G.vlabel[v()]);
      paint->DrawText(p,postxt[v],t,G.vcolor[v],0);
      }
  }
void DrawBipContact(QPainter *p,pigalePaint *paint)
  {GeometricGraph G(paint->GCP);
  Prop<int> h(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> h1(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> h2(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);

  QPen pn = p->pen();pn.setWidth(1);
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  QPoint ps,pt;
  ps = QPoint(paint->to_x(pmin().x()+1),paint->to_y(pmin().y()+1));
  pt = QPoint(paint->to_x(pmax().x()-1),paint->to_y(pmax().y()-1));
  pn.setColor(color[Grey1]); p->setPen(pn);
  p->drawLine(ps,pt);
  QString t;
  QRect rect;
  QSize size;
  int dx,dy;
  dy = Min(12,paint->height()/(pmax().y()+1)-2);
  QFont font = QFont("lucida",dy);
  p->setFont(font);
  tvertex v;
  for(v = 1;v <= G.nv();v++)
      {double delta = (h1[v] < h[v])?.45:-.45;
      if(G.vcolor[v] == Red)
	  {if(h1[v] != h2[v])
	      {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
	      pt  = QPoint(paint->to_x(h2[v]),paint->to_y(h[v]));
	      }
	  else
	      {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
	      pt  = QPoint(paint->to_x(h2[v]+delta),paint->to_y(h[v]));
	      }
	  }
      else
	  {if(h1[v] != h2[v])
	      {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
	      pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]));
	      }
	  else
	      {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
	      pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]+delta));
	      } 
	  }
      pn.setColor(color[G.vcolor[v]]); pn.setWidth(2);p->setPen(pn);
      p->drawLine(ps,pt);
      }
  
  pn.setColor(color[Black]); pn.setWidth(1);p->setPen(pn);
  for(v = 1;v <= G.nv();v++)
      {double delta = (h1[v] < h[v])?.9:-.9;
      if(G.vcolor[v] == Red)
	  {if(h1[v] != h2[v])
	      {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
	      pt  = QPoint(paint->to_x(h2[v]),paint->to_y(h[v]));
	      }
	  else
	      {ps = QPoint(paint->to_x(h1[v]),paint->to_y(h[v]));
	      pt  = QPoint(paint->to_x(h2[v]+delta),paint->to_y(h[v]));
	      }
	  }
      else
	  {if(h1[v] != h2[v])
	      {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
	      pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]));
	      }
	  else
	      {ps = QPoint(paint->to_x(h[v]),paint->to_y(h1[v]));
	      pt  = QPoint(paint->to_x(h[v]),paint->to_y(h2[v]+delta));
	      } 
	  }

      if(ShowVertex() == -1)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",G.vlabel[v()]);
      size = QFontMetrics(font).size(Qt::AlignCenter,t);
      dx =size.width() + 2;   dy =size.height() + 2;
      rect = QRect((ps.x() + pt.x() - dx)/2,(ps.y() + pt.y() - dy)/2,dx,dy);
      pb.setColor(color[G.vcolor[v]]);
      p->setBrush(pb);
      p->drawRect(rect);
      p->drawText(rect,Qt::AlignCenter,t);
      }
  }
void DrawFPPVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<int> xliv(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> xriv(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> xje(G.Set(tedge()),PROP_DRAW_INT_4);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  //Checking if vertices have labels
  svector<long> *vlabel = NULL;
  if(G.Set(tvertex()).exist(PROP_LABEL))
      {Prop<long> label(G.Set(tvertex()),PROP_LABEL);
      vlabel = &label.vector();
      }
  // Draw edges
  int h1,h2,x1,x2;
  double alpha = .5;
  double beta  = .825;
  Tpoint a,b;
  for(tedge e=1; e <= G.ne();e++)
      {h1 = y[G.vin[e]]; 
       h2 = y[G.vin[-e]];
       x1 = xriv[G.vin[e]]; 
       x2 = xliv[G.vin[-e]];
      if(h1 < h2)
	  {a.x() = b.x() = xje[e]; 
	   a.y() = h1;  
	   b.y() = h2 - 2*alpha;
	  paint->DrawSeg(p,a,b,ecolor[e]);
	  }
      else if(h1 > h2)
	  {a.x() = b.x() = xje[e];
	   a.y() = h2;
	   b.y() = h1 - 2*alpha;
	  paint->DrawSeg(p,a,b,ecolor[e]);
	  }
      else if(x1 < x2)
	  {a.x() = x1 + beta;
	   b.x() = x2 - beta;
	   a.y() = b.y() = h1 -alpha;
	  paint->DrawSeg(p,a,b,ecolor[e]);
	  }
      else if(x1 > x2)
	  {a.x() = xriv[G.vin[-e]] + beta;  
	   b.x() = xliv[G.vin[e]]  - beta;
	   a.y() = b.y() = h1 -alpha;
	   paint->DrawSeg(p,a,b,ecolor[e]);
	  }
      }

  // Draw vertices
  double xt = .9*Min(2*alpha*paint->xscale,beta*paint->yscale);
  //p->setFont(QFont("lucida",Min((int)(1.8*alpha * Min(paint->xscale,paint->yscale) + .5),13)));
  p->setFont(QFont("lucida",Min((int)(xt + .5),13)));
  QString t;
  for(tvertex v=1; v <= G.nv();v++) 
      {if(ShowVertex() == -1 || !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);
      paint->DrawText(p,xliv[v]-beta, y[v], xriv[v]-xliv[v]+2.*beta, 2.*alpha,t,vcolor[v]);
      }
  }
void DrawVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> P1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<int> x1(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_5);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  //Checking if vertices have labels
  svector<long> *vlabel = NULL;
  if(G.Set(tvertex()).exist(PROP_LABEL))
      {Prop<long> label(G.Set(tvertex()),PROP_LABEL);
      vlabel = &label.vector();
      }

  double alpha=0.35;
  p->setFont(QFont("lucida",Min((int)(1.8*alpha * Min(paint->xscale,paint->yscale) + .5),13)));
  Tpoint a,b;
  QString t;
  for(tvertex v=1;v<=G.nv();v++)
      {if(ShowVertex() == -1|| !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);
      paint->DrawText(p,x1[v]-alpha,y[v]+alpha, x2[v]-x1[v]+2*alpha,2*alpha,t,vcolor[v]);
      }
  for (tedge e = 1;e <= G.ne();e++)
      {a.x() = P1[e].x(); a.y() = P1[e].y() + alpha;
      b.x() = P1[e].x();  b.y() = P2[e].y() - alpha;
      paint->DrawSeg(p,a,b,ecolor[e]);
      }
  }
void DrawGeneralVisibility(QPainter *p,pigalePaint *paint)
  {TopologicalGraph G(paint->GCP);
  Prop<Tpoint> P1(G.Set(tedge()),PROP_DRAW_POINT_1);
  Prop<Tpoint> P2(G.Set(tedge()),PROP_DRAW_POINT_2);
  Prop<int> x1(G.Set(tvertex()),PROP_DRAW_INT_1);
  Prop<int> x2(G.Set(tvertex()),PROP_DRAW_INT_2);
  Prop<int> x1m(G.Set(tvertex()),PROP_DRAW_INT_3);
  Prop<int> x2m(G.Set(tvertex()),PROP_DRAW_INT_4);
  Prop<int> y(G.Set(tvertex()),PROP_DRAW_INT_5);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  //Checking if vertices have labels
  svector<long> *vlabel = NULL;
  if(G.Set(tvertex()).exist(PROP_LABEL))
      {Prop<long> label(G.Set(tvertex()),PROP_LABEL);
      vlabel = &label.vector();
      }
  double alpha=0.35;
  Tpoint a,b;


  p->setFont(QFont("lucida",Min((int)(1.8*alpha * Min(paint->xscale,paint->yscale) + .5),13)));
  QString t;
  for(tvertex v=1;v<=G.nv();v++)
      {if(x1m[v] != x2m[v]) // always 
	  {a.x() = x1m[v]; a.y() = y[v];
	  b.x() = x2m[v];  b.y() = y[v];
	  paint->DrawSeg(p,a,b,Black);
	  }
      if(ShowVertex() == -1|| !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);
      paint->DrawText(p,x1[v]-alpha,y[v]+alpha, x2[v]-x1[v]+2*alpha,2*alpha,t,vcolor[v]);
      }
  for (tedge e = 1;e <= G.ne();e++)
    {a.x() = P1[e].x(); a.y() = P1[e].y();
    if (a.x()>=x1[G.vin[e]] && a.x()<=x2[G.vin[e]]) a.y()+=alpha;
    b.x() = P1[e].x();  b.y() = P2[e].y();
    if (b.x()>=x1[G.vin[-e]] && b.x()<=x2[G.vin[-e]]) b.y()-=alpha;
    paint->DrawSeg(p,a,b,ecolor[e]);
    }
  }

//**********************************************************************************************
//**********************************************************************************************

typedef void (*draw_func)(QPainter *p,pigalePaint *paint);
struct DrawThing {
    draw_func	 f;
    const char	*name;
};

static DrawThing DrawFunctions[] = 
    {
    {DrawVisibility,QT_TRANSLATE_NOOP("pigalePaint","Visibility")},
    {DrawFPPVisibility,QT_TRANSLATE_NOOP("pigalePaint","FPP Visibility")},
    {DrawGeneralVisibility,QT_TRANSLATE_NOOP("pigalePaint","General Visibility")},
    {DrawBipContact,QT_TRANSLATE_NOOP("pigalePaint","Contact")},
    {DrawPolar,QT_TRANSLATE_NOOP("pigalePaint","Polar")},
    {DrawTContact,QT_TRANSLATE_NOOP("pigalePaint","T Contact")}, 
    {DrawPolyline,QT_TRANSLATE_NOOP("pigalePaint", "Polyline")},
    {DrawCurves,QT_TRANSLATE_NOOP("pigalePaint", "Curves")},
    {0,QT_TRANSLATE_NOOP("pigalePaint","default ")}  
    };
const int border = 20;

pigalePaint::~pigalePaint()
  { }
pigalePaint::pigalePaint(QWidget *parent, const char *name,pigaleWindow *f):
    QWidget(parent,name),father(f),isHidden(true)
  {index = -1;
  setBackgroundColor(Qt::white);
  setFocusPolicy(QWidget::ClickFocus); 
  }
void pigalePaint::print(QPrinter* printer)
  {if(index < 0)return;
  if(printer->setup(this))
      {QPainter pp(printer);
      QPaintDeviceMetrics pdm(printer);
      int nx = width();
      int ny = height();
      double scale = Max((double)nx/(double) pdm.width(),(double)ny/(double)pdm.height());
      printer->setResolution((int)(scale*printer->resolution()+.5));
      drawIt(&pp);
      }
  }
void pigalePaint::png()
  {if(index < 0)return;
  qApp->processEvents();
  QString FileName;
  if(!father->ServerExecuting)
      {FileName = QFileDialog::
      getSaveFileName(father->DirFilePng,"Images(*.png)",this);
      if(FileName.isEmpty())return; 
      if(QFileInfo(FileName).extension(false) != (const char *)"png")
	  FileName += (const char *)".png";
      father->DirFilePng = QFileInfo(FileName).dirPath(true);
      }
  else
      FileName = QString("/tmp/server%1.png").arg(father->ServerClientId);
  QPixmap pixmap = QPixmap::grabWidget (this); 
  pixmap.save(FileName,"PNG",0);
  }
void pigalePaint::drawIt(QPainter *p)
  {if(index < 0)return;
  (*DrawFunctions[index].f)(p,this);
  }
void pigalePaint::update(int i)
  {setBackgroundColor(Qt::white);
  zoom = 1;
  index = i;
  GCP = father->GC;
  TopologicalGraph G(GCP);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  xmin = pmin().x();  ymin = pmin().y();
  xmax = pmax().x();  ymax = pmax().y();
  Wx_max = this->width() - 2*border;  Wy_max = this->height() - 2*border;
  xscale0 = xscale = Wx_max/(xmax - xmin);
  xtr0 = xtr  =  - xmin*xscale + border;
  yscale0 = yscale = Wy_max/(ymax - ymin);
  ytr0 = ytr  =   - ymin*yscale +border;
#if QT_VERSION < 300
  father->tabWidget->changeTab(this,qApp->translate("pigalePaint",DrawFunctions[index].name));
#else
  father->tabWidget->setTabLabel(this,qApp->translate("pigalePaint",DrawFunctions[index].name));
#endif
  father->tabWidget->showPage(this);
  }
void pigalePaint::paintEvent(QPaintEvent * e)
  {if(isHidden)return;
  QWidget::paintEvent(e);
  QPainter p(this);
  drawIt(&p);
  }
void pigalePaint::showEvent(QShowEvent*)
  {isHidden = false;}
void pigalePaint::hideEvent(QHideEvent*)
  {isHidden = true;}
void pigalePaint::resizeEvent(QResizeEvent* e)
  {Wx_max = this->width() - 2*border;  Wy_max = this->height() - 2*border;
  xscale0 = xscale = Wx_max/(xmax - xmin);
  xtr0 = xtr  =  - xmin*xscale + border;
  yscale0 = yscale = Wy_max/(ymax - ymin);
  ytr0 = ytr  =  - ymin*yscale + border;
  QWidget::resizeEvent(e);
  }
void pigalePaint::keyPressEvent(QKeyEvent *k)
  {int key = k->key();

  if(key == Qt::Key_Up)
      zoom = 1.1;
  else if(key == Qt::Key_Down)
      zoom = 1/1.1;
  else if(key == Qt::Key_Home)
      {xtr = xtr0; ytr = ytr0;
      xscale = xscale0; yscale = yscale0;
      }
  else
      return;
  //k->accept();
  if(key !=  Qt::Key_Home)
      {double xx0 = ((double)posClick.x() - xtr)/xscale;
      double yy0 = -((double)posClick.y() + ytr - this->height())/yscale;
      xscale *= zoom ;yscale *= zoom;
      xtr += posClick.x() - to_x(xx0);
      ytr += to_y(yy0) -  posClick.y();
      }
  setBackgroundColor(Qt::white);
  QPainter p(this);
  drawIt(&p);
  }
void pigalePaint::wheelEvent(QWheelEvent *event)
  {//event->accept();
  zoom = (event->delta() > 0) ? 1.1 : 1./1.1;
  double xx0 = ((double)posClick.x() - xtr)/xscale;
  double yy0 = -((double)posClick.y() + ytr - this->height())/yscale;
  xscale *= zoom ;yscale *= zoom;
  xtr += posClick.x() - to_x(xx0);
  ytr += to_y(yy0) -  posClick.y();
  setBackgroundColor(Qt::white);
  QPainter p(this);
  drawIt(&p);
  }
void pigalePaint::mousePressEvent(QMouseEvent *event)
  {posClick = event->pos();
  }
void pigalePaint::mouseMoveEvent(QMouseEvent *event)
  {int dx = event->pos().x() - posClick.x();
  int dy = event->pos().y() - posClick.y();
  posClick = event->pos();
  xtr += dx;  ytr -= dy;
  setBackgroundColor(Qt::white);
  QPainter p(this);
  drawIt(&p);
  }
int pigalePaint::to_x(double x)
  {return (int)(x*xscale + xtr +.5);
  }
int pigalePaint::to_y(double y)
  {return (int)(this->height() - y*yscale -ytr + .5);
  }
void pigalePaint::DrawSeg(QPainter *p,Tpoint &a,Tpoint &b,int col)
  {QPen pn = p->pen();
  QPoint ps = QPoint(to_x(a.x()),to_y(a.y()));
  QPoint pt = QPoint(to_x(b.x()),to_y(b.y()));
  pn.setColor(color[bound(col,1,16)]); pn.setWidth(1);
  p->setPen(pn);
  p->drawLine(ps,pt);
  }
void pigalePaint::DrawRect(QPainter *p,Tpoint &a,double nx,double ny,int col)
// draw a rectangle centered at a
  {QPen pn = p->pen();pn.setWidth(2);pn.setColor(color[Black]);p->setPen(pn);
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,1,16)]);p->setBrush(pb);
  //  nx *= xscale; ny *= yscale;
  //  p->drawRect(QRect(to_x(a.x()-nx/2+.5),to_y(a.y()-ny/2+.5),(int)(nx+.5),(int)(ny+.5)));
  p->drawRect(QRect((int)(to_x(a.x()) - nx*Min(xscale,yscale)/2), (int)(to_y(a.y())- ny*Min(xscale,yscale)/2), (int)(nx*Min(xscale,yscale)), (int)(ny*Min(xscale,yscale))));
  }
void pigalePaint::DrawText(QPainter *p,Tpoint &a,QString &t,int col,int center)
// draw text centered at a, with a surrounding rectangle
// center=1 center
// center=0 horizontal
  {QPen pn = p->pen();pn.setWidth(1);pn.setColor(color[Black]);p->setPen(pn);
  QSize size = QFontMetrics(p->font()).size(Qt::AlignCenter,t);
  double nx = size.width() + 4; double ny = size.height();
  QRect rect;
  //if pn.setWidth() > 1 => rect increase
  if(center)
      rect = QRect((int)(to_x(a.x())-nx/2+.5),(int)(to_y(a.y())-ny/2+.5),(int)(nx+.5),(int)(ny+.5));
  else
      rect = QRect((int)(to_x(a.x())-nx/2+.5),(int)(to_y(a.y())-ny+1.),(int)(nx+.5),(int)(ny+.5));
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,0,16)]);p->setBrush(pb);
  pn.setWidth(1);p->setPen(pn);
  p->drawRect(rect);
  if(ny >= 6){ pn.setWidth(1);p->setPen(pn);p->drawText(rect,Qt::AlignCenter,t);}
  }
void pigalePaint::DrawText(QPainter *p,double x,double y,double nx,double ny,QString &t,int col)
// draw centered text in rectangle left at x,y of size: nx,ny
  {QPen pn = p->pen();pn.setColor(color[Black]);
  nx *= xscale;  ny *= yscale;
  QRect rect = QRect(to_x(x),to_y(y),(int)(nx+.5),(int)(ny+.5));
  QBrush pb = p->brush();pb.setStyle(Qt::SolidPattern);
  pb.setColor(color[bound(col,1,16)]);p->setBrush(pb);
  pn.setWidth(1);p->setPen(pn);
  p->drawRect(rect);
  pn.setWidth(1);p->setPen(pn);
  p->drawText(rect,Qt::AlignCenter,t);
  }
