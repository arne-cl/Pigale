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
#include "GraphWidget.h"
#include "mouse_actions.h"
#include <QT/Misc.h> 
#include <qapplication.h> 
#include <qspinbox.h> 
#include <qslider.h> 
#include <qmenubar.h> 
#include <qtoolbar.h> 
#include <TAXI/Tgf.h> 

static TSArray<int> MacroActions(4),MacroEwidth(4);
static TSArray<short> MacroVcolor(4),MacroEcolor(4);
static int MacroNumActions = 0;
static int volatile escape = 0;
static int volatile key = 0;

void MyWindow::keyPressEvent(QKeyEvent *k)
  {if(k->key() == Qt::Key_Escape)
      escape = 1;
  key = k->key();
  //qDebug("k=%d",key);
  //QWidget::keyPressEvent(k);
  }
int MyWindow::getKey()
  {int key0 = key;
  key = 0;
  return key0;
  }
void MyWindow::blockInput(bool t)
  {menuBar()->setEnabled(!t); tb->setEnabled(!t);
  }
void macroRecord(int action)
  {if(action > 10000)return;
  MacroActions(++MacroNumActions) = action;
  GeometricGraph G(GetMainGraph());
  short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
  short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
  int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
  }
void MyWindow::macroHandler(int event)
  { int repeat = spinMacro->value();
  switch(event)
      {case 1://start recording
	   if(debug())LogPrintf("\nRecord macro\n");
	   MacroRecording = true;
	   MacroNumActions = 0;
	   break;
      case 2://stop recording
	  MacroRecording = false;
	  break;
      case 3://continue recording
	  MacroRecording = true;
	  break; 
      case 4:// play repeat times
	  MacroLooping = true;
	  blockInput(true);
	  for(int i = 1;i <= repeat;i++)
	      {if(i == repeat || escape)MacroLooping = false;
	      macroPlay();
	      mouse_actions->LCDNumber->display((int)(i*100./repeat));
	      mouse_actions->Slider->setValue((int)(i*100./repeat));
	      if(escape)break;
	      qApp->processEvents(5);
	      }
	  escape = 0;
	  MacroLooping = false;
	  blockInput(false);
	  break;
      default:
	  break;
      }
  }
void macroDefColors(int record)
  {GeometricGraph G(GetMainGraph());
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
void MyWindow::macroPlay()
  {if(debug())LogPrintf("\nPlay macro:%d records",MacroNumActions);
  MacroRecording = false;
  if(MacroNumActions == 0){load(1);return;}
  MacroExecuting = true;
  int record = 1;
  if(MacroActions[record] < 500 || MacroActions[record] > 600)
      load(1); 
  while(record <= MacroNumActions)
      {macroDefColors(record);
      if(MacroExecuting && record == MacroNumActions && !MacroLooping)
	  {MacroExecuting = false;
	  // Call editor if last action is an embedding not in graph editor
	  if(MacroActions[record] > 205 && MacroActions[record] < 300)
	      gw->update();
	  }
      handler(MacroActions[record++]);
      }
  if(!MacroLooping)information();
  }
