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

//in MyCanvas.cpp
bool & ShowIndex();

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
          {paint->DrawSeg(Epoint1[ee],Epoint2[ee],Red);
          continue;
          }
      if (Epoint11[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(Epoint1[ee],Epoint11[ee],Pink);
          paint->DrawSeg(Epoint11[ee],Epoint12[ee],Pink);
          }
      else if (Epoint12[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(Epoint1[ee],Epoint12[ee],Pink);
      if (Epoint21[ee]!=Tpoint(-1,-1))
          {paint->DrawSeg(Epoint2[ee],Epoint21[ee],Pink);
          paint->DrawSeg(Epoint21[ee],Epoint22[ee],Pink);
          }
      else if (Epoint22[ee]!=Tpoint(-1,-1))
          paint->DrawSeg(Epoint2[ee],Epoint22[ee],Pink);
	  
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
      if(ShowIndex())
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

void DrawVision(QPainter *p,MyPaint *paint)
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
  QPoint ps,pt;
  for (tedge e = 1;e <= G.ne();e++)
      {ps = QPoint(paint->to_x(P1[e].x()),paint->to_y(P1[e].y()));
      pt = QPoint(paint->to_x(P1[e].x()),paint->to_y(P2[e].y()));
      pn.setColor(color[ecolor[e]]); p->setPen(pn);
      p->drawLine(ps,pt);
      }
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
      if(ShowIndex() || !vlabel)
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
    {DrawVision,"&Vision"},
    {DrawBipContact,"&Contact"},
    {DrawPolar,"&Polair"},
    {0,0} 
    };

MyPaint::~MyPaint(){}
MyPaint::MyPaint(QWidget *parent, const char *name,MyWindow *f):
    QWidget(parent,name),father(f),isHidden(true)
  {index = -1;
  setBackgroundColor(Qt::white);
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
void MyPaint::DrawSeg(Tpoint &a,Tpoint &b,int col)
  {QPainter p(this);QPen pn = p.pen();
  QPoint ps = QPoint(to_x(a.x()),to_y(a.y()));
  QPoint pt = QPoint(to_x(b.x()),to_y(b.y()));
  pn.setColor(color[col]); pn.setWidth(2);p.setPen(pn);
  p.drawLine(ps,pt);
  }
