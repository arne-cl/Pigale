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
#include "LineEditNum.h"
#include <QT/Misc.h> 
#include <QT/Action_def.h> 
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
void AllowAllMenus(QMenuBar *menubar)
  {for(int menu = A_AUGMENT;menu <= A_TEST_END;menu++)
      {if(menubar->text(menu) == QString::null ||menubar->isItemEnabled(menu))continue;
      menubar->setItemEnabled(menu,true);
      }
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
//{int repeat = spinMacro->value();
  {int repeat = macroLine->getVal();
  int i;
  unsigned j;
  double Time;
  QTime t0;
  int repeat0;
  switch(event)
      {case 1://start recording
	   if(debug())LogPrintf("\nRecord macro\n");
	   AllowAllMenus(menuBar());
	   MessageClear();
	   Tprintf("No info while recording");
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
	  MacroRecording = false;
	  DebugPrintf("PLAY times=%d MacroNumActions:%d",repeat,MacroNumActions);
	  t0.start();
	  DebugPrintf("Macro start at:%s",(const char *)t0.toString(Qt::TextDate)); 
	  t0.restart();
	  MacroLooping = true;
	  blockInput(true);
	  repeat0 = (repeat == 0) ? 10000 : repeat;
	  j = 0;
	  for(i = 1;i <= repeat0;i++)
	      {if(i == repeat0 && repeat == 0)i = 1;
	      if(i == repeat0 || escape)MacroLooping = false;
	      ++j;
	      macroPlay();
	      mouse_actions->LCDNumber->display((int)(i*100./repeat0));
	      mouse_actions->Slider->setValue((int)(i*100./repeat0));
	      if(escape || !MacroLooping)break;
	      qApp->processEvents(5);
	      }
	  escape = 0;
	  MacroLooping = MacroExecuting = false;
	  blockInput(false);
	  Time = t0.elapsed()/1000.;
	  DebugPrintf("Ellapsed time:%.3f mean:%f",Time,Time/j);
	  t0.restart();
	  DebugPrintf("Macro stop at:%s",(const char *)t0.toString(Qt::TextDate)); 
	  if(!Error())
	      DebugPrintf("END PLAY OK times:%d MacroNumActions:%d",j,MacroNumActions);
	  else
	      {gw->update();
	      DebugPrintf("END PLAY ERROR times=%d MacroNumActions:%d",j,MacroNumActions);
	      Error()=0;
	      }
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
  {if(debug())LogPrintf("\nPlay macro:%d actions\n",MacroNumActions);
  MacroRecording = false;
  if(MacroNumActions == 0){load(1);return;}
  MacroExecuting = true;
  int record = 1;
  if(MacroActions[record] < A_GENERATE || MacroActions[record] >  A_GENERATE_END)
      load(1); 
  while(record <= MacroNumActions)
      {macroDefColors(record);
      if(MacroExecuting && record == MacroNumActions && !MacroLooping)
	  {MacroExecuting = false;
	  // Call editor if last action is an embedding not in graph editor
	  //if(MacroActions[record] > A_EMBED_TUTTE && MacroActions[record] < A_EMBED_END)
	      gw->update();
	  }
      handler(MacroActions[record++]);
      }
  
  if(!MacroLooping)
      {information();
      Tprintf("MacroNumActions:%d",MacroNumActions);
      }
  if(Error())
      {DebugPrintf("MACRO error=%d",Error());
      MacroLooping = false;
      }
  }
