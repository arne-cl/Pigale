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
#include <qprogressbar.h>
#include <qmenubar.h> 
#include <qtoolbar.h> 
#include <TAXI/Tgf.h> 

static TSArray<int> MacroActions(4),MacroEwidth(4);
static TSArray<short> MacroVcolor(4),MacroEcolor(4);
static int MacroNumActions = 0;
static int  key = 0;

int & pauseDelay()
  {static int delay;
  return delay;
  }

void MyWindow::keyPressEvent(QKeyEvent *k)
  {key = k->key();
  //qDebug("k:%d",k->key());
  QWidget::keyPressEvent(k);
  }
int MyWindow::getKey()
  {int key0 = key;
  //if(key)qDebug("get k=%d",key0);
  if(key0 == Qt::Key_Escape && MacroLooping)MacroLooping = false;
  key = 0;
  return key0;
  }
void MyWindow::blockInput(bool t)
  {menuBar()->setEnabled(!t); tb->setEnabled(!t);
  }
void MyWindow::timer()
  {MacroWait = false;
  }
void AllowAllMenus(QMenuBar *menubar)
  {for(int menu = A_AUGMENT;menu <= A_TEST_END;menu++)
      {if(menubar->text(menu) == QString::null ||menubar->isItemEnabled(menu))continue;
      menubar->setItemEnabled(menu,true);
      }
  }
void MyWindow::macroRecord(int action)
  {if(action > A_SERVER)return;
  MacroActions(++MacroNumActions) = action;
  QString str_action = getActionString(action);
  Tprintf("Recording action (%d):%s",MacroNumActions,(const char *)str_action);
  GeometricGraph G(GetMainGraph());
  short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
  short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
  int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
  }
void MyWindow::macroHandler(int event)
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
	  MacroWait = false;
	  MessageClear();
	  DebugPrintf("PLAY times=%d MacroNumActions:%d",repeat,MacroNumActions);
	  t0.start();
	  DebugPrintf("Macro started at:%s",(const char *)t0.toString(Qt::TextDate)); 
	  t0.restart();
	  MacroLooping = true;
	  blockInput(true);
	  repeat0 = (repeat == 0) ? 1000 : repeat;
	  progressBar->setTotalSteps(repeat0);
	  progressBar->show();
	  j = 0;
	  for(i = 1;i <= repeat0;i++)
	      {if(i == repeat0 && repeat == 0)i = 1;
	      ++j;
	      macroPlay();
	      progressBar->setProgress(i);
	      do
		  {qApp->processEvents();
		  }while(MacroWait);
	      if(!MacroLooping)break; // if an error had occurred
	      if(getKey() == Qt::Key_Escape){gw->update();break;}
	      }
	  progressBar->hide();
	  MacroLooping = MacroExecuting = MacroWait = false;
	  blockInput(false);
	  Time = t0.elapsed()/1000.;
	  DebugPrintf("Ellapsed time:%.3f mean:%f",Time,Time/j);
	  t0.restart();
	  DebugPrintf("Macro stop at:%s",(const char *)t0.toString(Qt::TextDate)); 
	  gw->update();
	  if(!getError())
	      DebugPrintf("END PLAY OK iter:%d",j);
	  else
	      DebugPrintf("END PLAY ERROR iter=%d",j);
	  break;
      case 5:// insert a pause
	  if(MacroRecording)macroRecord(A_PAUSE);
	  break;
      default:
	  break;
      }
  pauseDelay() = macroSpin->value();
  }
void macroDefColors(int record)
  {GeometricGraph G(GetMainGraph());
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
void MyWindow::macroPlay()
  {if(!MacroLooping)
      {MessageClear();
      DebugPrintf("Play macro:%d actions",MacroNumActions);
      }
  MacroExecuting = true;
  MacroRecording = false;
  if(MacroNumActions == 0){load(1);return;}
  int record = 1;
  if(MacroActions[record] < A_GENERATE || MacroActions[record] >  A_GENERATE_END)
      load(1); 
  while(record <= MacroNumActions)
      {macroDefColors(record);
      if(record == MacroNumActions && !MacroLooping)
	  {gw->update();
	  MacroExecuting = false; // so if the action needs a redraw, it will be done
	  }
      if((MacroActions[record] != A_PAUSE && !menuBar()->isItemEnabled(MacroActions[record]))
	 || (MacroActions[record] == A_PAUSE && !MacroLooping))
	  {if(debug())LogPrintf("%s:initial conditons not satisfied\n"
		  ,(const char *)getActionString(MacroActions[record]));
	  ++record;continue;
	  }
      if(debug())LogPrintf("macro action:%d/%d -> %d\n",record,MacroNumActions,MacroActions[record]);
      handler(MacroActions[record++]);
      if(getError()){MacroWait = false;break;}
      else if(debug())LogPrintf("macro action:OK\n");
      }
  if(getError())
      {DebugPrintf("MACRO %s",(const char *)getErrorString());
      setError();
      MacroLooping = false;
      gw->update();
      }

  if(!MacroLooping)information();
  }
