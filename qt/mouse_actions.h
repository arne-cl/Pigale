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

#ifndef MOUSE_ACIONS_H
#define MOUSE_ACIONS_H

#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>   // first for gcc 2.7.2
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlcdnumber.h>
#include <qslider.h>

class GraphWidget;
class GraphGL;

int GetMouseAction_1();


class Mouse_Actions : public QWidget
{ 
    Q_OBJECT

public:
  Mouse_Actions(QWidget* parent = 0,const char* name = 0,WFlags fl = 0
		,GraphWidget* gw=0);
  ~Mouse_Actions();
  
  void contentsMousePressEvent(QMouseEvent* e);

  QButtonGroup* ButtonGroup1;
  QRadioButton* ButtonAddV;
  QRadioButton* ButtonAddE;
  QRadioButton* ButtonDel;
  QRadioButton* ButtonMove;
  QRadioButton* ButtonBissect;
  QRadioButton* ButtonContract;
  QRadioButton* ButtonLabel;

  QCheckBox* ButtonFitGrid;
  QCheckBox* ButtonShowGrid;
  QPushButton* ButtonForceGrid;
  QPushButton* ButtonUndoGrid;

  QLCDNumber* LCDNumber;
  QLCDNumber* LCDNumberX;
  QLCDNumber* LCDNumberY;
  QSlider* Slider;


private slots:
  void valueChanged(int i);
};
int GetMouseAction_1();
#endif 
