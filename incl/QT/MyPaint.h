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

class MyWindow;

class MyPaint : public QWidget 
{
  //Q_OBJECT

public:
  MyPaint(QWidget *parent=0,const char *name=0,MyWindow *father = 0);
  ~MyPaint();
  int to_x(double x);   
  int to_y(double y);   
  void DrawSeg(Tpoint &a,Tpoint &b,int col);
  //public slots:
  void update(int index);

private:
  void drawIt(QPainter *);
  void paintEvent(QPaintEvent *);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent* e);
  void mousePressEvent(QMouseEvent * event);
  int index;

public:


private:
  MyWindow *father;
  bool isHidden;

public:
  GraphContainer GCP;
  double Wx_min,Wx_max,Wy_min,Wy_max;
  double xscale,xtr,yscale,ytr;
  double xmin,xmax,ymin,ymax;
};
#endif
