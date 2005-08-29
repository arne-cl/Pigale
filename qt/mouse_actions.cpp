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

#include "mouse_actions.h"
#include "GraphWidget.h"
#include "GraphGL.h"

#include <qpalette.h>
#include <qcolordialog.h> 

static int mouse_action_1 = 3;

int GetMouseAction_1()
  {return mouse_action_1;}
// sizes cannot be known at this point by calling methods
// but the width is defined by pigaleEditorMinXsize = pigaleEditorMaxXsize = 280

const int Height = 150; 
const int Width = 280-5; //5 borders
const int dy = 17;//18;
const int oy = 16;//20;                // origin of first radiobutton  
const int dx = 110;               // width of radiobuttons  (80) 
const int ox1 = 6;               // postion of left group
const int dx1 = 125;              // width radio group (90)
const int sliderOx = ox1+dx1+3;
const int sliderOy = 7;
const int sliderWidth = 16;
const int sliderHeight = 6*dy+oy-3; // +-height of the radiogroup
const int sizegrid = 40; //init value of the grid size of the graph
const int oy2 = 8;
const int ox2 =  sliderOx + sliderWidth + 3;    // postion of right group
const int dx2 = Width-ox2-1;             // group of radiobuttons
const int dxb = dx2/2;// width of buttons

Mouse_Actions::Mouse_Actions(QWidget* parent,const char* name
			     ,WFlags fl,GraphWidget* gw)
    : QWidget(parent,name,fl)
  {if(!name)setName("Mouse_Acions");

  setMinimumHeight(Height);  setMaximumHeight(Height); 
  //QFont fnt = this->font();  fnt.setBold(true);  setFont(fnt,true);

  ButtonGroup1 = new QButtonGroup(this,"ButtonGroup1");
  ButtonGroup1->setGeometry(QRect(ox1-1,0,dx1,7*dy+oy+15)); 
  ButtonGroup1->setTitle(tr("Left Button"));
  ButtonGroup1->setRadioButtonExclusive(TRUE);

  ButtonAddV = new QRadioButton(ButtonGroup1,"ButtonAddV");
  ButtonAddV->setGeometry(QRect(ox1,oy,dx,20)); 
  ButtonAddV->setText(tr("Col/Thick"));

  ButtonAddE = new QRadioButton(ButtonGroup1,"ButtonAddE");
  ButtonAddE->setGeometry(QRect(ox1,oy+dy,dx,20)); 
  ButtonAddE->setText(tr("Add/Del"));

  ButtonDel = new QRadioButton(ButtonGroup1,"ButtonDel");
  ButtonDel->setGeometry(QRect(ox1,oy+2*dy,dx,20)); 
  ButtonDel->setText(tr("Orient"));

  ButtonMove = new QRadioButton(ButtonGroup1,"ButtonMove");
  ButtonMove->setGeometry(QRect(ox1,oy+3*dy,dx,20)); 
  ButtonMove->setText(tr("Move"));
  ButtonMove->setChecked(TRUE);

  ButtonBissect = new QRadioButton(ButtonGroup1,"ButtonBisect");
  ButtonBissect->setGeometry(QRect(ox1,oy+4*dy,dx,20)); 
  ButtonBissect->setText(tr("Bis./Cont."));

  ButtonContract = new QRadioButton(ButtonGroup1,"ButtonContract");
  ButtonContract->setGeometry(QRect(ox1,oy+5*dy,dx,20)); 
  ButtonContract->setText(tr("Ext Face/extbrin"));

  ButtonLabel = new QRadioButton(ButtonGroup1,"ButtonLabel");
  ButtonLabel->setGeometry(QRect(ox1,oy+6*dy,dx,20)); 
  ButtonLabel->setText(tr("Label/Reset all"));

    //****************************************************************
  gw->sizegridChanged(sizegrid);
  LCDNumber = new QLCDNumber(this,"LCDNumber");
  LCDNumber->setGeometry(QRect(ox2,oy2,30,30)); 
  QFont LCDNumber_font(LCDNumber->font());
  LCDNumber_font.setBold(TRUE);
  LCDNumber->setFont(LCDNumber_font); 
  LCDNumber->setNumDigits(3);
  LCDNumber->setSegmentStyle(QLCDNumber::Flat);
  LCDNumber->setProperty("intValue",sizegrid);

  LCDNumberX = new QLCDNumber(this,"LCDNumber");
  QColor col;         col.setRgb(0,100,0);
  // 0:background gprop 1:background msg 3:background help 2:background mouse
  QPalette pal = QPalette(QColorDialog::customColor(1));
  pal.setColor(QColorGroup::Foreground,col);
  LCDNumberX->setPalette(pal);
  LCDNumberX->setGeometry(QRect(ox2+35,oy2,30,30)); 
  LCDNumberX->setFont(LCDNumber_font); 
  LCDNumberX->setNumDigits(3);
  LCDNumberX->setSegmentStyle(QLCDNumber::Flat);
  LCDNumberX->setProperty("intValue",sizegrid);

  LCDNumberY = new QLCDNumber(this,"LCDNumber");
  LCDNumberY->setPalette(pal);
  LCDNumberY->setGeometry(QRect(ox2+35+30+2,oy2,30,30)); 
  LCDNumberY->setFont(LCDNumber_font); 
  LCDNumberY->setNumDigits(3);
  LCDNumberY->setSegmentStyle(QLCDNumber::Flat);
  LCDNumberY->setProperty("intValue",sizegrid);

  Slider = new QSlider(this,"Slider");
  Slider->setGeometry(QRect(sliderOx,sliderOy,sliderWidth,sliderHeight)); 
  Slider->setMinValue(2);    Slider->setMaxValue(100);
  Slider->setValue(sizegrid);
  Slider->setOrientation(QSlider::Vertical);

  ButtonFitGrid = new QCheckBox(this,"ButtonFitGrid");
  ButtonFitGrid->setGeometry(QRect(ox2,oy2+35,Width-ox2,20)); 
  ButtonFitGrid->setText(tr("Fit grid"));

  ButtonShowGrid = new QCheckBox(this,"ButtonShowGrid");
  ButtonShowGrid->setGeometry(QRect(ox2,oy2+55,Width-ox2,20)); 
  ButtonShowGrid->setText(tr("Show grid"));
  
  ButtonForceGrid = new QPushButton(this,"ButtonForceGrid");
  ButtonForceGrid->setGeometry(QRect(ox2,oy2+78,dxb,20));
  ButtonForceGrid->setText(tr("Force G"));

  ButtonUndoGrid = new QPushButton(this,"ButtonUndoGrid");
  ButtonUndoGrid->setGeometry(QRect(ox2+dxb,oy2+78,dxb,20));
  ButtonUndoGrid->setText(tr("Undo G"));
  ButtonUndoGrid->setDisabled(true);

  // signals and slots connections
  connect(ButtonGroup1,SIGNAL(clicked(int)),SLOT(valueChanged(int)));
  connect(Slider,SIGNAL(valueChanged(int)),LCDNumber,SLOT(display(int)));
  connect(Slider,SIGNAL(valueChanged(int)),gw,SLOT(sizegridChanged(int)));
  connect(ButtonShowGrid,SIGNAL(toggled(bool)),gw,SLOT(showgridChanged(bool)));
  connect(ButtonFitGrid,SIGNAL(toggled(bool)),gw,SLOT(fitgridChanged(bool)));
  connect(ButtonForceGrid,SIGNAL(clicked()),gw,SLOT(ForceGrid()));
  connect(ButtonUndoGrid,SIGNAL(clicked()),gw,SLOT(UndoGrid()));
  }
Mouse_Actions::~Mouse_Actions()
{}
void Mouse_Actions::valueChanged(int i)
  {mouse_action_1 = i;
  }
