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

#ifndef GRAPH_PROPERTIES_H
#define GRAPH_PROPERTIES_H

#include <qvariant.h>
#include <qwidget.h>
#include <qradiobutton.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QRadioButton;
class QMenuBar;
 

class RoRadioButton : public QRadioButton
{ 
  Q_OBJECT
public:
  RoRadioButton(QWidget * parent, const char * name = 0);
  ~RoRadioButton(){};
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
};

class Graph_Properties : public QWidget
{ 
  Q_OBJECT

public:
  Graph_Properties( QWidget* parent = 0,QMenuBar *menu = 0,
		    const char* name = 0,WFlags fl = 0 );
  ~Graph_Properties();
  void update(bool print = true);
  bool Simple(){return S;}
  bool Planar(){return P;}
  bool Triangulation(){return T;}
  bool Acyclic(){return A;}
  bool Biparti(){return B;}
  bool MaxBiparti(){return MaxBi;}
  bool Con1(){return C1;}
  bool Con2(){return C2;}
  bool Con3(){return C3;}
  bool OuterPlanar(){return Outer;}
  bool SeriePlanar(){return Serie;}
  bool Regular(){return R;}
  int DegreeMin(){return dmin;}
  int DegreeMax(){return dmax;}
  int NumSources(){return ns;}
  int NumSinks(){return nt;}

  QLineEdit* LE_N,*LE_M,*LE_Min,*LE_Max;
  RoRadioButton* RBConnected;
  RoRadioButton* RB2Connected;
  RoRadioButton* RB3Connected;
  RoRadioButton* RBPlanar;
  RoRadioButton* RBMxPlanar;
  RoRadioButton* RBOuPlanar;
  RoRadioButton* RBSerie;
  RoRadioButton* RBSimple;
  RoRadioButton* RBBipartite;
  RoRadioButton* RBAcyclic;
  int MaxNSlow;
  int MaxNDisplay;

public slots:
  void MaxNSlowChanged(int i);
  void MaxNDisplayChanged(int i);

private:
  QMenuBar *menu;
  int ns,nt,dmin,dmax;
  bool S,P,A,B,R,C1,C2,C3,T,Outer,Serie,MaxBi;
};

#endif // GRAPH_PROPERTIES_H
