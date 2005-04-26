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

#include "pigaleWindow.h"
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
#include <qtextstream.h>

#include <TAXI/Tgf.h> 

static TSArray<int> MacroActions(0,4);
//static TSArray<int> ,MacroEwidth(0,4);
//static TSArray<short> MacroVcolor(0,4),MacroEcolor(0,4);
static int MacroNumActions = 0;
bool EditNeedUpdate;
bool InfoNeedUpdate;
static int key = 0;

class eventEater : public QObject
{public:
  eventEater(){};
  ~eventEater(){};
protected:
  bool eventFilter(QObject *o,QEvent *e);
};

bool eventEater::eventFilter(QObject *o,QEvent *e)
    {if(e->type() == QEvent::MouseButtonPress) return TRUE; // eat event
    return FALSE;
    }
int & pauseDelay()
  {static int delay;
  return delay;
  }
void pigaleWindow::keyPressEvent(QKeyEvent *k)
  {key = k->key();
//   qDebug("pigaleWindow::hasFocus %d",hasFocus());
//   qDebug("pigaleWindow::key_pressed=%d",k->key());
  }
int pigaleWindow::getKey()
  {int key0 = key;
  if(key0 == Qt::Key_Escape && MacroLooping)MacroLooping = false;
  key = 0;
  return key0;
  }
void pigaleWindow::blockInput(bool t)
  {static bool _inputBlocked = false;
  if(!t && (ServerExecuting || MacroLooping))return;
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
#if QT_VERSION >= 300
      qApp->setOverrideCursor(QCursor(Qt::WaitCursor)); 
#else
      qApp->setOverrideCursor(QCursor(WaitCursor));	
#endif
      }
  else 
      {qApp->removeEventFilter(EventEater);
      qApp->restoreOverrideCursor();
      }
  }
void pigaleWindow::timerWait()
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
  {GeometricGraph G(GC);
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
*/
int pigaleWindow::macroLoad(QString FileName)
  {if(FileName.isEmpty())return -1;
  if(QFileInfo(FileName).isReadable() == FALSE)return -1; 
  QFile file( FileName);
  file.open(IO_ReadOnly);
  QTextStream stream(&file);
  QString str = stream.readLine();
  
  if(str == "Macro Version:1")
      stream.readLine();
  else if(str != "Macro Version:2")
      {Tprintf("Wrong Macro File -%s-",(const char *)str);return -1;}
  if(stream.atEnd())return -1;
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
  return 0;
  }

void pigaleWindow::macroRecord(int action)
  {if(action > A_SERVER)return;
  MacroActions(++MacroNumActions) = action;
  QString str_action = getActionString(action);
  Tprintf("Recording action (%d):%s",MacroNumActions,(const char *)str_action);
//   GeometricGraph G(GC);
//   short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
//   short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
//   int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
  }

void pigaleWindow::macroHandler(int event)
  {int repeat = macroLine->getVal();
  int i;
  unsigned j;
  double Time;
  QTime t0;
  QString msg0,msg1;
  int repeat0,record;
  switch(event)
      {case 1://start recording
          if(debug())LogPrintf("\nRecord macro\n");
          AllowAllMenus(menuBar());
          postMessageClear();
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
          MacroLooping = true;
          postMessageClear();
          DebugPrintf("PLAY times=%d MacroNumActions:%d",repeat,MacroNumActions);
          t0.start();
#ifndef _WINDOWS
          msg0 = QString("Macro started at %1").arg(t0.toString(Qt::TextDate));
#else
          msg0 = "Macro started";
#endif
          DebugPrintf("%s",(const char *)msg0); 
          t0.restart();
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
          t0.restart();
#ifndef _WINDOWS
          msg1 = QString("Macro stopped at %1").arg(t0.toString(Qt::TextDate));
#else
          msg1 ="Macro stopped"; 
#endif
          if(EditNeedUpdate)gw->update(-1);
          if(InfoNeedUpdate){information();InfoNeedUpdate = false;}
          DebugPrintf("Ellapsed time:%.3f mean:%f",Time,Time/j);
          Tprintf("%s",(const char *)msg0); 
          DebugPrintf("%s",(const char *)msg1); 
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
          postMessageClear();
          for(record = 1;record <= MacroNumActions;record++)
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
          FILE *out = fopen((const char *)FileName,"wt");
          fprintf(out,"Macro Version:2\n");
          for(record = 1;record <= MacroNumActions;record++)
              fprintf(out,"%s\n",(const char *)getActionString(MacroActions[record]));
          fclose(out);
          }
          break;
      case 8:// read
          {QString FileName = QFileDialog::
          getOpenFileName(DirFileMacro,"Macro files(*.mc)",this);
          postMessageClear();
          if(macroLoad(FileName) == -1)break;
          MacroWait = false;
          }
          break;
      default:
          break;
      }
  pauseDelay() = macroSpin->value();
  }

void pigaleWindow::macroPlay()
//  MacroExecuting = true => handler does not update of the editor
  {if(MacroNumActions == 0)return;
  blockInput(true);
  if(!MacroLooping){postMessageClear();DebugPrintf("Play macro:%d actions",MacroNumActions);}
  int ret_handler = 0,action;
  EditNeedUpdate =  InfoNeedUpdate = true;
  MacroExecuting = true;
  MacroRecording = MacroWait = false;
  
  // Load next graph 
  if(MacroActions[1] < A_GENERATE || MacroActions[1] >  A_GENERATE_END)
      load(1); 

  for(int record = 1;record <= MacroNumActions;++record)
      {action = MacroActions[record];
      //macroDefColors(record);
      if(action != A_PAUSE && !menuBar()->isItemEnabled(action))
          {if(debug()) LogPrintf("%s:initial conditons not satisfied\n",(const char *)getActionString(action));
          ++record;continue;
          }
      if(debug())LogPrintf("macro action:%s\n",(const char *)getActionString(action));
      // Execute the macro
      handler(action);
      ret_handler = getResultHandler();
      if(ret_handler == 1)
          InfoNeedUpdate = true;
      else if(ret_handler == 2)
          InfoNeedUpdate = EditNeedUpdate = true;
      else if(ret_handler == 7 || ret_handler == 8)
          EditNeedUpdate = false;

      // update the editor and information if a pause 
      if(record != MacroNumActions && action == A_PAUSE)
          {if(EditNeedUpdate)
              {gw->update();EditNeedUpdate = false;}
          if(InfoNeedUpdate)
              {MacroExecuting = false; //Otherwise no information displayed
              information();InfoNeedUpdate = false;
              MacroExecuting = true;
              }
          }
      if(getError())
          {DebugPrintf("MACRO %s",(const char *)getErrorString());
          setError();
          MacroWait = MacroLooping = false;
          InfoNeedUpdate = false; // do not hide the error message
          break;
          }
      do
          {qApp->processEvents();
          }while(MacroWait);
      }

  MacroWait = MacroExecuting = false;
  if(!MacroLooping)
      {if(EditNeedUpdate)
          {gw->update(-1);EditNeedUpdate = false;}
      if(InfoNeedUpdate)
          {information();InfoNeedUpdate = false;}
      blockInput(false);
      }
  }
