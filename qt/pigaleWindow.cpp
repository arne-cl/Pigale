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

#include <config.h>
#include "pigaleWindow.h"
#include <TAXI/Tgf.h>
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include "LineEditNum.h"

#include <QT/Misc.h> 
#include <QT/Handler.h>
#include <QT/Action_def.h>
#include <QT/Action.h>
#include <QT/pigalePaint.h> 
#include <QT/pigaleCanvas.h>
#include <QT/clientEvent.h>

#include <qstatusbar.h>
#include <qfontdialog.h> 
#include <qmessagebox.h>
#include <qprogressbar.h>


int Test(GraphContainer &GC,int action,int & drawing);
int InitPigaleServer(pigaleWindow *w);



void pigaleWindow::whenReady()
  {if(MacroPlay && macroLoad(MacroFileName) != -1)  macroPlay();
  ServerClientId = 0;
  if(Server)InitPigaleServer(this); 
  }
void pigaleWindow::initServer()
  {ServerClientId = 0;
  NewGraph(); 
  InitPigaleServer(this); 
  }
void pigaleWindow::customEvent( QCustomEvent * ev)
  {switch( ev->type())
      {case CLIENT_EVENT:
          {clientEvent *event  =  (clientEvent  *)ev;
          int action  = event->getAction();
          QString msg = event->getParamString();
          if(action > A_AUGMENT && action < A_TEST_END)
              handler(action);
          ServerBusy = false;
          }
          break;
      case  TEXT_EVENT:
          {textEvent *event  =  (textEvent  *)ev;
          Message(event->getString());
          }
          break;
      case CLEARTEXT_EVENT:
          messages->setText("");
          break;
      case BANNER_EVENT:
          {bannerEvent *event  =  (bannerEvent  *)ev;
          statusBar()->message(event->getString());
          }
          break;
      case WAIT_EVENT:
          {waitEvent *event  =  (waitEvent  *)ev;
          QString msg = event->getString();
          int rep = QMessageBox::information (this,"Wait",msg,"","CANCEL","EXIT",0,0);
          if(rep == 2)close();
          }
      case DRAWG_EVENT:
          gw->update();
          break;
      case HANDLER_EVENT:
           postHandler(ev);    
          break;
      case PROGRESS_EVENT:
          {progressEvent *event  =  (progressEvent  *)ev;
          int action  = event->getAction();
          if(action == -1)
              progressBar->hide();
          else if(action == 1)
              {progressBar->setTotalSteps(event->getStep());
              progressBar->setProgress(0);
              progressBar->show();
              }
          else
              progressBar->setProgress(event->getStep());
          }
          break;
      default:
          qDebug("UNKNOWN EVENT");
          break;
      }
  }
void pigaleWindow::postMessage(const QString &msg)
  {textEvent *e = new textEvent(msg);
  QApplication::postEvent(this,e);
  //qApp->processEvents();  would block the application
  }
void pigaleWindow::postMessageClear()
  {QCustomEvent *e = new QCustomEvent(CLEARTEXT_EVENT);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postWait(const QString &msg)
  {waitEvent *e = new waitEvent(msg);
  QApplication::postEvent(this,e);
  }
 void pigaleWindow::postDrawG()
  {QCustomEvent *e = new QCustomEvent(DRAWG_EVENT);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::Message(QString s)
  {messages->append(s);
#if QT_VERSION >= 300
  messages->scrollToBottom ();
#endif
  } 
void pigaleWindow::mapActionsInit()
  {int na = (int)(sizeof(actions)/sizeof(_Action));
  for(int i = 0;i < na;i++)
      {mapActionsString[actions[i].num] = actions[i].name;
      mapActionsInt[actions[i].name] = actions[i].num;
      }
  }
QString pigaleWindow::getActionString(int action)
  {return mapActionsString[action];
  }
int pigaleWindow::getActionInt(QString action_str)
  {return mapActionsInt[action_str];
  }
void pigaleWindow::information()
  {if(!getError() && !MacroExecuting && !ServerExecuting)postMessageClear();
  graph_properties->update(!MacroExecuting && !ServerExecuting);
  }
void pigaleWindow::settingsHandler(int action)
  {if(action == A_SET_COLOR)
      {SetPigaleColors(); return;}
  else if(action == A_SET_DOC)
      {QString StartDico = (DirFileDoc.isEmpty()) ? "." : DirFileDoc;
      DirFileDoc = QFileDialog::
      getExistingDirectory(StartDico,this,"find",
                           tr("Choose the documentation directory <em>Doc</em>"),TRUE);
      if(!DirFileDoc.isEmpty())
          {QFileInfo fi =  QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
          if(fi.exists())
              {browser->mimeSourceFactory()->setFilePath(DirFileDoc);
              browser->setSource("manual.html");
              SaveSettings();
              QMessageBox::information(this,"Pigale Editor"
                                       ,tr("You should restart Pigale"
                                           " to get the manual loaded")
                                       ,QMessageBox::Ok);
              }
          else 
              QMessageBox::information(this,"Pigale Editor"
                                       ,tr("I cannot find the inline manual <em>manual.html</em>")
                                       ,QMessageBox::Ok);
          }
          return;
      }
  else if(action == A_SET_PAUSE_DELAY) 
      {pauseDelay() = macroSpin->value();
      SaveSettings();
      return;
      }
  else if(action == A_SET_RANDOM_SEED_CHANGE)
      {randomSetSeed() = atol((const char *)seedEdit->text()); return; 
      }
  
// Change one parameter
  menuBar()->setItemChecked(action,!menuBar()->isItemChecked(action));
  // Read parameters when one is changed
  debug()               =  menuBar()->isItemChecked(A_SET_DEBUG);
  SchnyderRect()        =  menuBar()->isItemChecked(A_SET_SCH_RECT);
  SchnyderLongestFace() =  menuBar()->isItemChecked(A_SET_LFACE);
  SchnyderColor()       =  menuBar()->isItemChecked(A_SET_SCH_COLOR);
  ShowOrientation()     =  menuBar()->isItemChecked(A_SET_ORIENT);
  ShowArrow()     =  menuBar()->isItemChecked(A_SET_ARROW);
  randomEraseMultipleEdges()  =  menuBar()->isItemChecked(A_SET_ERASE_MULT);
  randomSeed()          =  menuBar()->isItemChecked(A_SET_RANDOM_SEED);
  pauseDelay() = macroSpin->value();
  UndoEnable(menuBar()->isItemChecked(A_SET_UNDO));

  if(action == A_SET_ORIENT || action == A_SET_ARROW )
      // update the editor
      {handlerEvent *e = new handlerEvent(1,0,0);
      QApplication::postEvent(this,e);
      }
  return;
  }
void PigaleThread:: run(int _action,int  _N,int _N1,int _N2,int _M,int _delay)
  {action = _action;
  N = _N;
  N1 = _N1;
  N2 = _N2;
  M = _M;
  delay = _delay;
  start (QThread::HighPriority);
  }
void PigaleThread::run()
  {if(!action)return;
  mw->timer.start();
  int ret = 0;
  int saveType = 0;
  int drawingType = 0;
  if(action < A_AUGMENT_END)
      ret = AugmentHandler(action);
  else if(action < A_EMBED_END)
      ret = EmbedHandler(action,drawingType);
  else if(action < A_GRAPH_END)
      {ret = DualHandler(action); 
      saveType = 1;
      }
  else if(action < A_REMOVE_END)
      {ret = RemoveHandler(action);
      saveType = 2;
      }
  else if(action < A_GENERATE_END)
      {ret = GenerateHandler(action,N1,N2,M);
      saveType = 1;
      }
  else if(action < A_ALGO_END)
      ret = AlgoHandler(action,N);
  else if(action < A_ORIENT_END)
      {ret = OrientHandler(action);
      ShowOrientation() = true;
      }
//   else if(action < A_TEST_END)
//       ret  = Test(mw->GC,action - A_TEST,drawingType);
  
  handlerEvent *e = new handlerEvent(ret,drawingType,saveType);
  QApplication::postEvent(mw,e);
  }
int  pigaleWindow::getResultHandler(int value)
  {static int _value = 0;
  int value0 = _value;
  _value = value;
  return value0;
  }
int pigaleWindow::handler(int action)
// Should not have any graphic action (so can be called from a thread)
  {if(action > A_SET){settingsHandler(action);return 0;}
  if(MacroRecording)macroRecord(action);
  if(action == A_PAUSE)
      {pauseDelay() = macroSpin->value();
      qApp->processEvents();
      MacroWait = true;
      QTimer::singleShot(1000*pauseDelay(),this,SLOT(timerWait()));
      return 0;
      }
  if(action > A_TEST_END)return 0;

  blockInput(true);
  if(action < A_AUGMENT_END)
      {UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_EMBED_END)
      pigaleThread.run(action);
  else if(action < A_GRAPH_END)
      {UndoClear();UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_REMOVE_END)
      {UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_GENERATE_END)
      {UndoClear();UndoSave();
      pigaleThread.run(action,0,spin_N1->value(),spin_N2->value(),spin_M->value());
      }
  else if(action < A_ALGO_END)
      pigaleThread.run(action,spin_N->value());
  else if(action < A_ORIENT_END)
      pigaleThread.run(action);
  else if(action < A_TEST_END)
      {int drawing;
      timer.start();
      int ret = Test(GC,action - A_TEST,drawing);
      handlerEvent *e = new handlerEvent(ret,drawing,0);
      QApplication::postEvent(this,e);
      return 0;
      }
      //pigaleThread.run(action);
  else
      return 0;
  while( pigaleThread.running())
      {pigaleThread.wait(50);
      qApp->processEvents (); 
      }
  return 0;
  }

int pigaleWindow::postHandler(QCustomEvent *ev)
  {// 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
  // 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders
  handlerEvent *event = (handlerEvent  *)ev;  
  int ret = event->getAction();
   getResultHandler(ret);    // set the result
  if(ret < 0){blockInput(false);return ret;}
  int saveType = event->getSaveType();
  if(saveType == 1)
      UndoSave();
  else if(saveType == 2)
      UndoTouch(false);

  // In case we called the orienthandler
  menuBar()->setItemChecked(A_SET_ORIENT,ShowOrientation()); 
  double Time = timer.elapsed()/1000.;
  if(ret == 1)
      {if(!MacroExecuting )gw->update();}
  else if(ret == 2)
      {if(!MacroRecording)information();
      if(!MacroExecuting )gw->update();
      }
  else if(ret == 20) // Remove handler
      {if(!MacroRecording)information();
      if(!MacroExecuting ) gw->update(0);
      }
  else if(ret == 3)
      mypaint->update(event->getDrawingType()); 
  else if(ret == 4) //3d drawings
      graphgl->update(); 
  else if(ret == 5) //symetrie
      {graphgl->update(); 
      graphsym->update();
      }
  // cases 6-7-8 we need a canvas with the current graph loaded
  else if(ret == 6)
      {gw->update();gw->Spring();}
  else if(ret == 7)
      {gw->update();gw->SpringPreservingMap();}
  else if(ret == 8)
      {gw->update();gw->SpringJacquard();}

  blockInput(false);
  double TimeG = timer.elapsed()/1000.;
  if(!MacroLooping && !MacroRecording && !ServerExecuting)
      {Tprintf("Used time:%3.3f (G+I:%3.3f)",Time,TimeG);
      if(getError())
          {Tprintf("Handler Error:%s",(const char *)getErrorString());
          if(debug())Twait((const char *)getErrorString()); 
          }
      }
  return ret;
  }
void pigaleWindow::banner()
  {QString msg;  
    int NumRecords =IO_GetNumRecords(0,(const char *)InputFileName);
    int NumRecordsOut =IO_GetNumRecords(0,(const char *)OutputFileName);
  msg.sprintf("Input: %s %d/%d  Output: %s %d Undo:%d/%d"
	    ,(const char *)InputFileName
	    ,*pGraphIndex,NumRecords
	    ,(const char *)OutputFileName
	    ,NumRecordsOut
	    ,UndoIndex,UndoMax);
  bannerEvent *e = new bannerEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::about()
  {QMessageBox::about(this,tr("Pigale Editor"), 
                      "<b>"+tr("Pigale Editor")+"</b> (version:  "+PACKAGE_VERSION+")"
                      "<br><b>Copyright (C) 2001</b>"
                      +"<br>Hubert de Fraysseix"
	    +"<br>Patrice Ossona de Mendez "
	    +"<br> See <em>license.html</em>");
  }
void pigaleWindow::aboutQt()
  {QMessageBox::aboutQt(this,"Qt Toolkit");
  }
void pigaleWindow::showLabel(int show)
  {int _show = ShowVertex();
  ShowVertex() = show -3;
  if(ShowVertex() != _show && GC.nv())
      {switch(tabWidget->currentPageIndex())
          {case 0:
              gw->update();
              break;
          case 1:
              mypaint->update();
              break;
          }
      }
  }
void  pigaleWindow::distOption(int use)
  {useDistance() = use;
  }
void  pigaleWindow::setShowOrientation(bool val)
  {ShowOrientation() = val;
  menuBar()->setItemChecked(A_SET_ORIENT,val);
  }
void pigaleWindow::print()
  {switch(tabWidget->currentPageIndex())
      {case 0:
          gw->print(printer);
          break;
      case 1:
          mypaint->print(printer);
          break;
      case 3:
          graphsym->print(printer);
          break;
      default:
          break;
      }
  }
void pigaleWindow::png()
  {switch(tabWidget->currentPageIndex())
      {case 0:
          gw->png();
          break;
      case 1:
          mypaint->png();
          break;
      case 2:
          graphgl->png();
          break;
      case 3:
          graphsym->png();
          break;
      default:
          break;
      }
  }
void pigaleWindow::SetPigaleFont()
 {bool ok;
  QFont font = QFontDialog::getFont( &ok, this->font(), this );
  if(!ok)return;
  QApplication::setFont(font,true);
 }
void  pigaleWindow::setUserMenu(int i, const QString &txt)
 {userMenu->changeItem ( A_TEST+i,txt);
 }


