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


const int dy = 18;
const int oy = 15;
const int dx = 80;
const int ox_grid = 100;
//const int oy_grid = 10;
const int oy_grid = 8;
const int sizegrid = 40;

Mouse_Actions::Mouse_Actions(QWidget* parent,const char* name
			     ,WFlags fl,GraphWidget* gw,GraphGL* gp)
    : QWidget(parent,name,fl)
{
    if(!name)setName("Mouse_Acions");
    resize(200,126); 
    setMinimumSize(QSize(80,140));
    setMaximumSize(QSize(227,140));
    //QFont fnt = QFont("helvetica",12);
    QFont fnt = this->font();
    fnt.setBold(true);
    setFont(fnt,true);

    ButtonGroup1 = new QButtonGroup(this,"ButtonGroup1");
    ButtonGroup1->setGeometry(QRect(5,0,90,6*dy+oy+5)); 
    ButtonGroup1->setTitle("Left Button");
    ButtonGroup1->setRadioButtonExclusive(TRUE);

    ButtonAddV = new QRadioButton(ButtonGroup1,"ButtonAddV");
    ButtonAddV->setGeometry(QRect(6,oy,dx,20)); 
    ButtonAddV->setText("Col/Thick");

    ButtonAddE = new QRadioButton(ButtonGroup1,"ButtonAddE");
    ButtonAddE->setGeometry(QRect(6,oy+dy,dx,20)); 
    ButtonAddE->setText("Add/Del");

    ButtonDel = new QRadioButton(ButtonGroup1,"ButtonDel");
    ButtonDel->setGeometry(QRect(6,oy+2*dy,dx,20)); 
    ButtonDel->setText("Orient");

    ButtonMove = new QRadioButton(ButtonGroup1,"ButtonMove");
    ButtonMove->setGeometry(QRect(6,oy+3*dy,dx,20)); 
    ButtonMove->setText("Move");
    ButtonMove->setChecked(TRUE);

    ButtonBissect = new QRadioButton(ButtonGroup1,"ButtonBisect");
    ButtonBissect->setGeometry(QRect(6,oy+4*dy,dx,20)); 
    ButtonBissect->setText("Bis./Cont.");

    ButtonContract = new QRadioButton(ButtonGroup1,"ButtonContract");
    ButtonContract->setGeometry(QRect(6,oy+5*dy,dx,20)); 
    ButtonContract->setText("Ext Face");

    //****************************************************************
    gw->sizegridChanged(sizegrid);
    LCDNumber = new QLCDNumber(this,"LCDNumber");
    LCDNumber->setGeometry(QRect(ox_grid+18,oy_grid,30,30)); 
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
    LCDNumberX->setGeometry(QRect(ox_grid+18+35,oy_grid,30,30)); 
    LCDNumberX->setFont(LCDNumber_font); 
    LCDNumberX->setNumDigits(3);
    LCDNumberX->setSegmentStyle(QLCDNumber::Flat);
    LCDNumberX->setProperty("intValue",sizegrid);

    LCDNumberY = new QLCDNumber(this,"LCDNumber");
    LCDNumberY->setPalette(pal);
    LCDNumberY->setGeometry(QRect(ox_grid+18+35+30+2,oy_grid,30,30)); 
    LCDNumberY->setFont(LCDNumber_font); 
    LCDNumberY->setNumDigits(3);
    LCDNumberY->setSegmentStyle(QLCDNumber::Flat);
    LCDNumberY->setProperty("intValue",sizegrid);

    Slider = new QSlider(this,"Slider");
    Slider->setGeometry(QRect(ox_grid,oy_grid,15,115)); 
    Slider->setMinValue(2);    Slider->setMaxValue(100);
    Slider->setValue(sizegrid);
    Slider->setOrientation(QSlider::Vertical);

    ButtonFitGrid = new QCheckBox(this,"ButtonFitGrid");
    ButtonFitGrid->setGeometry(QRect(ox_grid+18,oy_grid+35,85,20)); 
    ButtonFitGrid->setText("Fit grid");

    ButtonShowGrid = new QCheckBox(this,"ButtonShowGrid");
    ButtonShowGrid->setGeometry(QRect(ox_grid+18,oy_grid+55,85,20)); 
    ButtonShowGrid->setText("Show grid");

    ButtonForceGrid = new QPushButton(this,"ButtonForceGrid");
    ButtonForceGrid->setGeometry(QRect(ox_grid+18,oy_grid+78,48,20));
    ButtonForceGrid->setText("Force G");

    ButtonUndoGrid = new QPushButton(this,"ButtonUndoGrid");
    ButtonUndoGrid->setGeometry(QRect(ox_grid+18+48,oy_grid+78,48,20));
    ButtonUndoGrid->setText("Undo G");
    ButtonUndoGrid->setDisabled(true);

    ButtonZoom = new QPushButton(this,"ButtonZoom");
    ButtonZoom->setGeometry(QRect(ox_grid+18,oy_grid+98,32,20));
    ButtonZoom->setText("Z +");

    ButtonOZoom = new QPushButton(this,"ButtonOZoom");
    ButtonOZoom->setGeometry(QRect(ox_grid+18+32,oy_grid+98,32,20));
    ButtonOZoom->setText("Z 0");

    ButtonUZoom = new QPushButton(this,"ButtonUZoom");
    ButtonUZoom->setGeometry(QRect(ox_grid+18+64,oy_grid+98,32,20));
    ButtonUZoom->setText("Z -");

    // signals and slots connections
    connect(ButtonGroup1,SIGNAL(clicked(int)),SLOT(valueChanged(int)));
    connect(Slider,SIGNAL(valueChanged(int)),LCDNumber,SLOT(display(int)));
    connect(Slider,SIGNAL(valueChanged(int)),gw,SLOT(sizegridChanged(int)));
    connect(Slider,SIGNAL(valueChanged(int)),gp,SLOT(delayChanged(int)));
    connect(ButtonShowGrid,SIGNAL(toggled(bool)),gw,SLOT(showgridChanged(bool)));
    connect(ButtonFitGrid,SIGNAL(toggled(bool)),gw,SLOT(fitgridChanged(bool)));
    connect(ButtonForceGrid,SIGNAL(clicked()),gw,SLOT(ForceGrid()));
    connect(ButtonUndoGrid,SIGNAL(clicked()),gw,SLOT(UndoGrid()));
    connect(ButtonZoom,SIGNAL(clicked()),gw,SLOT(zoom()));
    connect(ButtonOZoom,SIGNAL(clicked()),gw,SLOT(ozoom()));
    connect(ButtonUZoom,SIGNAL(clicked()),gw,SLOT(uzoom()));
    
}
Mouse_Actions::~Mouse_Actions()
{}
void Mouse_Actions::valueChanged(int i)
  {mouse_action_1 = i;}
