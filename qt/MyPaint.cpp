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
#include <QT/MyPaint.h>
#include <qtabwidget.h>
#include <TAXI/color.h> 
#include <TAXI/Tmessage.h> 
#include <QT/MyQcolors.h>
#include <iostream.h>
#include <qprinter.h>

//in QtMisc.cpp
int & ShowVertex();

#define Pink Blue
void DrawPolar(QPainter *p,MyPaint *paint)
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
          {paint->DrawSeg(p,Epoint1[ee],Epoint11[ee],Pink);
          paint->DrawSeg(p,Epoint11[ee],Epoint12[ee],Pink);
          }
      else if (Epoint12[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint1[ee],Epoint12[ee],Pink);
      if (Epoint21[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(p,Epoint2[ee],Epoint21[ee],Pink);
          paint->DrawSeg(p,Epoint21[ee],Epoint22[ee],Pink);
          }
      else if (Epoint22[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(p,Epoint2[ee],Epoint22[ee],Pink);
	  
      pn.setColor(color[Blue]);pn.setWidth(2);p->setPen(pn);
      ox = paint->to_x(-Erho[ee]);
      oy = paint->to_y(Erho[ee]);
      nx = (int)(2*Erho[ee]*paint->xscale + .5);
      ny = (int)(2*Erho[ee]*paint->yscale + .5);
      theta = (int)(Etheta1[ee]*16*180/PI+.5);
      dt = (int)((Etheta2[ee] - Etheta1[ee])*16*180/PI+.5); 
      p->drawArc(ox,oy,nx,ny,theta,dt);
      //DrawArc(Erho[ee],Etheta1[ee],Etheta2[ee],Blue);
      }
    // Draw the vertices
  /*
    int n = G.nv();
  Prop<long> label(G.PV(),PROP_LABEL);
  DrawNode(Vcoord[1],label[1],Pink);
  for(tvertex v = 2;v <= n;v++)
      DrawNode(Vcoord[v],label[v()],Yellow);
  */
  }
#undef Pink

void DrawBipContact(QPainter *p,MyPaint *paint)
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
  for(tvertex v = 1;v <= G.nv();v++)
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
  for(tvertex v = 1;v <= G.nv();v++)
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

      if(ShowVertex() == -1 || !G.Set(tvertex()).exist(PROP_LABEL))
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
void DrawFPPVisibility(QPainter *p,MyPaint *paint)
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
  double beta  = .8;
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
  int dx,dy;
  QRect rect;
  QString t;
  QPen pn = p->pen();
  pn.setColor(color[Black]); pn.setWidth(2); p->setPen(pn);
  for(tvertex v=1; v <= G.nv();v++) // draw boxes
      {dx = (int)(paint->to_x(xriv[v]+beta) - paint->to_x(xliv[v]-beta) +.5);
      dy = (int)(2.*alpha*paint->yscale +.5);
      rect = QRect(paint->to_x(xliv[v]-beta),paint->to_y(y[v]),dx,dy);
      p->setBrush(color[vcolor[v]]);p->drawRect(rect);
      if(ShowVertex() == -1 || !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);
      p->drawText(rect,Qt::AlignCenter,t);
      }
  }
void DrawVisibility(QPainter *p,MyPaint *paint)
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
  double alpha=0.3;
  p->setFont(QFont("lucida",12));
  QPen pn = p->pen(); 
  pn.setWidth(2);
  Tpoint a,b;

  pn.setColor(color[Black]); pn.setWidth(2); p->setPen(pn);
  QRect rect;
  int dx,dy;
  QString t;
  for(tvertex v=1;v<=G.nv();v++)
      {p->setBrush(color[vcolor[v]]);
      dx = (int)(paint->to_x(x2[v]+alpha) - paint->to_x(x1[v]-alpha) +.5);
      dy = (int)(2.*alpha*paint->yscale +.5);
      rect = QRect(paint->to_x(x1[v]-alpha),paint->to_y(y[v]+alpha),dx,dy);
      p->drawRect(rect);
      if(ShowVertex() == -1|| !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);

      p->drawText(rect,Qt::AlignCenter,t);
      }

  for (tedge e = 1;e <= G.ne();e++)
      {a.x() = P1[e].x(); a.y() = P1[e].y() + alpha;
      b.x() = P1[e].x();  b.y() = P2[e].y() - alpha;
      paint->DrawSeg(p,a,b,ecolor[e]);
      }
  }
void DrawGeneralVisibility(QPainter *p,MyPaint *paint)
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
  double alpha=0.3;
  p->setFont(QFont("lucida",12));
  QPen pn = p->pen(); 
  pn.setWidth(2);
  Tpoint a,b;
  for (tedge e = 1;e <= G.ne();e++)
      {a.x() = P1[e].x(); a.y() = P1[e].y();
      b.x() = P1[e].x();  b.y() = P2[e].y();
      paint->DrawSeg(p,a,b,ecolor[e]);
      }
  pn.setColor(color[Black]); pn.setWidth(2); p->setPen(pn);
  QRect rect;
  int dx,dy;
  QString t;
  for(tvertex v=1;v<=G.nv();v++)
      {if(x1m[v] != x2m[v]) // always 
	  {a.x() = x1m[v]; a.y() = y[v];
	  b.x() = x2m[v];  b.y() = y[v];
	  paint->DrawSeg(p,a,b,Black);
	  }
      p->setBrush(color[vcolor[v]]);
      dx = (int)(paint->to_x(x2[v]+alpha) - paint->to_x(x1[v]-alpha) +.5);
      dy = (int)(2.*alpha*paint->yscale +.5);
      rect = QRect(paint->to_x(x1[v]-alpha),paint->to_y(y[v]+alpha),dx,dy);
      p->drawRect(rect);
      if(ShowVertex() == -1|| !vlabel)
	  t.sprintf("%2.2d",v());
      else
	  t.sprintf("%2.2ld",(*vlabel)[v()]);

      p->drawText(rect,Qt::AlignCenter,t);
      }
  }

typedef void (*draw_func)(QPainter *p,MyPaint *paint);
struct DrawThing {
    draw_func	 f;
    const char	*name;
};

DrawThing DrawFunctions[] = 
    {
    {DrawVisibility,"&Visibility"},
    {DrawFPPVisibility,"&FPP Visibility"},
    {DrawGeneralVisibility,"&General Visibility"},
    {DrawBipContact,"&Contact"},
    {DrawPolar,"&Polar"},
    {0,0} 
    };

MyPaint::~MyPaint()
  { }
MyPaint::MyPaint(QWidget *parent, const char *name,MyWindow *f):
    QWidget(parent,name),father(f),isHidden(true)
  {index = -1;
  setBackgroundColor(Qt::white);
  }
void MyPaint::print(QPrinter* printer)
  {if(index < 0)return;
  if(printer->setup(this))
      {QPainter pp(printer);
      drawIt(&pp);
      }
  }
void MyPaint::drawIt(QPainter *p)
  {if(index < 0)return;
  (*DrawFunctions[index].f)(p,this);
  }
void MyPaint::update(int i)
  {setBackgroundColor(Qt::white);
  index = i;
  GCP = father->GC;
  TopologicalGraph G(GCP);
  Prop1<Tpoint> pmin(G.Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(G.Set(),PROP_POINT_MAX);
  xmin = pmin().x();  ymin = pmin().y();
  xmax = pmax().x();  ymax = pmax().y();
  Wx_max = this->width();  Wy_max = this->height();
  xscale = Wx_max/(xmax - xmin);
  xtr  =  - xmin*xscale;
  yscale = Wy_max/(ymax - ymin);
  ytr  =   - ymin*yscale;
#if QT_VERSION < 300
  father->tabWidget->changeTab(this,DrawFunctions[index].name);
#else
  father->tabWidget->setTabLabel(this,DrawFunctions[index].name);
#endif
  father->tabWidget->showPage(this);
  }
void MyPaint::paintEvent(QPaintEvent * e)
  {if(isHidden)return;
  QWidget::paintEvent(e);
  QPainter p(this);
  drawIt(&p);
  }
void MyPaint::showEvent(QShowEvent*)
  {isHidden = false;}
void MyPaint::hideEvent(QHideEvent*)
  {isHidden = true;}
void MyPaint::resizeEvent(QResizeEvent* e)
  {Wx_max = this->width();  Wy_max = this->height();
  xscale = Wx_max/(xmax - xmin);
  xtr  =  - xmin*xscale;
  yscale = Wy_max/(ymax - ymin);
  ytr  =  - ymin*yscale;
  QWidget::resizeEvent(e);
  }
void MyPaint::mousePressEvent(QMouseEvent * event)
  {Tprintf("%d %d",event->x(),event->y());}
int MyPaint::to_x(double x)
  {return (int)(x*xscale + xtr +.5);
  }
int MyPaint::to_y(double y)
  {return (int)(this->height() - y*yscale -ytr + .5);
  }
void MyPaint::DrawSeg(QPainter *p,Tpoint &a,Tpoint &b,int col)
  {QPen pn = p->pen();
  QPoint ps = QPoint(to_x(a.x()),to_y(a.y()));
  QPoint pt = QPoint(to_x(b.x()),to_y(b.y()));
  pn.setColor(color[col]); pn.setWidth(2);
  p->setPen(pn);
  p->drawLine(ps,pt);
  }
