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
#include <qfile.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qinputdialog.h> 
#include <qtabwidget.h> 
#include <qtabbar.h> 
#include <qcursor.h> 
#include <qmessagebox.h> 

#include <TAXI/Tgf.h> 

static TSArray<int> MacroActions(4);
//static TSArray<int> ,MacroEwidth(4);
//static TSArray<short> MacroVcolor(4),MacroEcolor(4);
static int MacroNumActions = 0;
bool EditNeedUpdate;
static int key = 0;

class eventEater : public QObject
{public:
  eventEater(){};
  ~eventEater(){};
protected:
  bool eventFilter(QObject *o,QEvent *e);
};

bool eventEater::eventFilter(QObject *o,QEvent *e)
    {if(e->type() == QEvent::MouseButtonPress) 
	return TRUE; // eat event
//     if(e->type() == QEvent::KeyPress) 
// 	{QKeyEvent *k = (QKeyEvent *)e;
// 	if(k->key() != Qt::Key_Escape)return TRUE; 
// 	}
    return FALSE;
    }
int & pauseDelay()
  {static int delay;
  return delay;
  }
void MyWindow::keyPressEvent(QKeyEvent *k)
  {key = k->key();
  QWidget::keyPressEvent(k);
  }
int MyWindow::getKey()
  {int key0 = key;
  if(key0 == Qt::Key_Escape && MacroLooping)MacroLooping = false;
  key = 0;
  return key0;
  }
void MyWindow::blockInput(bool t)
  {static bool _inputBlocked = false;
  if(_inputBlocked == t)return;
  _inputBlocked = t;
  menuBar()->setDisabled(t); 
//   tb->setDisabled(t);
//   mouse_actions->setDisabled(t);
//   tabWidget->setDisabled(t); 
  static eventEater *EventEater = 0;
  if(!EventEater)EventEater = new eventEater();
  if(t)
      {qApp->installEventFilter(EventEater);
      qApp->setOverrideCursor(QCursor(Qt::WaitCursor)); 
      }
  else 
      {qApp->removeEventFilter(EventEater);
      qApp->restoreOverrideCursor();
      }
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
/*
void macroDefColors(int record)
  {GeometricGraph G(GetMainGraph());
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
*/

void MyWindow::macroRecord(int action)
  {if(action > A_SERVER)return;
  MacroActions(++MacroNumActions) = action;
  QString str_action = getActionString(action);
  Tprintf("Recording action (%d):%s",MacroNumActions,(const char *)str_action);
//   GeometricGraph G(GetMainGraph());
//   short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
//   short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
//   int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
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
	   MacroWait = false;
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
	  //blockInput(true);
	  repeat0 = (repeat == 0) ? 1000 : repeat;
	  progressBar->setTotalSteps(repeat0);
	  progressBar->setProgress(0);
	  progressBar->show();
	  j = 0;
	  for(i = 1;i <= repeat0;i++)
	      {if(i == repeat0 && repeat == 0)i = 1;
	      ++j;
	      macroPlay();
	      progressBar->setProgress(i);
	      qApp->processEvents();
	      // !MacroLooping if an error had occurred
	      if(!MacroLooping || getKey() == Qt::Key_Escape)break;
	      }
	  progressBar->hide();
	  MacroLooping = MacroExecuting = MacroWait = false;
	  Time = t0.elapsed()/1000.;
	  DebugPrintf("Ellapsed time:%.3f mean:%f",Time,Time/j);
	  t0.restart();
	  DebugPrintf("Macro stop at:%s",(const char *)t0.toString(Qt::TextDate)); 
	  if(EditNeedUpdate)gw->update(-1);
	  if(!getError())
	      DebugPrintf("END PLAY OK iter:%d",j);
	  else
	      DebugPrintf("END PLAY ERROR iter=%d",j);
	  blockInput(false);
	  break;
      case 5:// insert a pause
	  if(MacroRecording)macroRecord(A_PAUSE);
	  break;
      case 6:// display
	  MessageClear();
	  for(int record = 1;record <= MacroNumActions;record++)
	      {Tprintf("Action (%d/%d):%s",record,MacroNumActions
		       ,(const char *)getActionString(MacroActions[record]));
	      }
	  break;
      case 7://save
	  {QString FileName = QFileDialog::getSaveFileName(DirFileMacro,"Macros (*.mc)",this);
	  if(FileName.isEmpty())break;
	  if(QFileInfo(FileName).extension(false) != (const char *)"mc")
	      FileName += (const char *)".mc";
	  DirFileMacro = QFileInfo(FileName).dirPath(true);
	  QFileInfo fi = QFileInfo(FileName);
	  if(fi.exists())
	      {int rep = QMessageBox::warning(this,"Pigale Editor"
			    ,"This file already exixts.<br>"
			    "Overwrite ?"
			    ,QMessageBox::Ok 
			    ,QMessageBox::Cancel);
	      if(rep == 2)break;
	      } 
	  bool ok = TRUE;
	  QString titre("name");
	  titre = QInputDialog::getText("Pigale","Enter the macro name",
						QLineEdit::Normal,titre, &ok, this );
	  if(!ok)break;
	  FILE *out = fopen((const char *)FileName,"wt");
	  fprintf(out,"Macro Version:1\n");
	  fprintf(out,"%s\n",(const char *)titre);
	  for(int record = 1;record <= MacroNumActions;record++)
	      fprintf(out,"%s\n",(const char *)getActionString(MacroActions[record]));
	  fclose(out);
	  }
	  break;
      case 8:// read
	  {QString FileName = QFileDialog::getOpenFileName(DirFileMacro,"Macro files(*.mc)",this);
	  if(FileName.isEmpty())break;
	  if(QFileInfo(FileName).isReadable() == FALSE)return; 
	  QFile file( FileName);
	  file.open(IO_ReadOnly);
	  QTextStream stream(&file);
	  QString str = stream.readLine();
	  if(str != "Macro Version:1")
	      {Tprintf("Wrong Macro File -%s-",(const char *)str);break;}
	  MessageClear();
	  if(stream.atEnd())break;
	  Tprintf("%s",(const char *)stream.readLine());
	  MacroNumActions = 0;
	  int action;
	  while(!stream.atEnd())
	      {str = stream.readLine();
	      action = getActionInt(str);
	      if(action < 99 || action > A_TEST_END)
		  {Tprintf("Unknown action:%s",(const char *)str);continue;}
	      Tprintf("Action (%d):%s",MacroNumActions,(const char *)str);
	      MacroActions(++MacroNumActions) = action;
	      }
	  }
	  MacroWait = false;
	  break;
      default:
	  break;
      }
  pauseDelay() = macroSpin->value();
  }

void MyWindow::macroPlay()
//  MacroExecuting = true => handler does not update of the editor
  {if(MacroNumActions == 0)return;
  blockInput(true);
  if(!MacroLooping){MessageClear();DebugPrintf("Play macro:%d actions",MacroNumActions);}
  int ret_handler = 0,action;
  EditNeedUpdate = MacroExecuting = true;
  MacroRecording = MacroWait = false;
  
  // Load next graph 
  if(MacroActions[1] < A_GENERATE || MacroActions[1] >  A_GENERATE_END)
      load(1); 

  for(int record = 1;record <= MacroNumActions;++record)
      {action = MacroActions[record];
      //macroDefColors(record);
      if(action != A_PAUSE && !menuBar()->isItemEnabled(action))
	  {if(debug())
	      LogPrintf("%s:initial conditons not satisfied\n"
			,(const char *)getActionString(action));
	  ++record;continue;
	  }
      if(debug())LogPrintf("macro action:%s\n",(const char *)getActionString(action));
      // Execute the macro
      ret_handler = handler(action);
      if(ret_handler == 1 || ret_handler == 2)EditNeedUpdate = true;
      else if(ret_handler >= 7 && ret_handler <= 8)EditNeedUpdate = false;
      // update the editor if a pause 
      if(record != MacroNumActions && action == A_PAUSE && EditNeedUpdate)
	  {gw->update();EditNeedUpdate = false;}
      if(getError())
	  {DebugPrintf("MACRO %s",(const char *)getErrorString());
	  setError();
	  MacroWait = MacroLooping = false;
	  break;
	  }
      do
	  {qApp->processEvents();
	  }while(MacroWait);
      }

  MacroWait = MacroExecuting = false;
  if(!MacroLooping)
      {if(EditNeedUpdate){gw->update(-1);EditNeedUpdate = false;}
      blockInput(false);
      }
  }
