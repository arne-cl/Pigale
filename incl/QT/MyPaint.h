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

#ifndef MYPAINT_H
#define MYPAINT_H
#include <TAXI/graphs.h> 
#include <qpainter.h>
#include <qwidget.h>
#include <qstring.h>

class MyWindow;
class QPrinter;

class MyPaint : public QWidget 
{
  //Q_OBJECT

public:
  MyPaint(QWidget *parent=0,const char *name=0,MyWindow *father = 0);
  ~MyPaint();
  int to_x(double x);   
  int to_y(double y);   
  void DrawSeg(QPainter *p,Tpoint &a,Tpoint &b,int col);
  void DrawRect(QPainter *p,Tpoint &a,double nx,double ny,int col);
  void DrawText(QPainter *p,Tpoint &a,QString &t,int col,int center);
  void DrawText(QPainter *p,double x,double y,double nx,double ny,QString &t,int color);
  //public slots:
  void update(int index);
  void print(QPrinter *printer);

private:
  void drawIt(QPainter *);
  void paintEvent(QPaintEvent *);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent* e);
  void mousePressEvent(QMouseEvent * event);

private:
  MyWindow *father;
  bool isHidden;
  QPrinter* printer;
  int index;

public:
  GraphContainer GCP;
  double Wx_min,Wx_max,Wy_min,Wy_max;
  double xscale,xtr,yscale,ytr;
  double xmin,xmax,ymin,ymax;
};
#endif
