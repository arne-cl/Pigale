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

  QLabel* TextLabel3;
  QLabel* TextLabel1;
  QLineEdit* LE_N;
  QLabel* TextLabel1_2;
  QLineEdit* LE_M;
  RoRadioButton* RBConnected;
  RoRadioButton* RB2Connected;
  RoRadioButton* RB3Connected;
  RoRadioButton* RBPlanar;
  RoRadioButton* RBMxPlanar;
  RoRadioButton* RBOuPlanar;
  RoRadioButton* RBSerie;
  RoRadioButton* RBSimple;
  RoRadioButton* RBBipartite;
  RoRadioButton* RBRegular;
  int MaxNSlow;
  int MaxNDisplay;
public slots:
  void MaxNSlowChanged(int i);
  void MaxNDisplayChanged(int i);
protected:
  QVBoxLayout* MainLayout;
  QHBoxLayout* Layout_NM;
  QHBoxLayout* Layout_CP;
  QVBoxLayout* Layout_C;
  QVBoxLayout* LayoutP;
  QMenuBar *menu;

};

#endif // GRAPH_PROPERTIES_H
