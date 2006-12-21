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

#ifdef QT3_SUPPORT
#undef QT3_SUPPORT
#endif
#include <config.h>

#include <QProgressBar>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QMessageBox>
#include <QStatusBar>


#include "ClientSocket.h"
#include "pigaleWindow.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"

#include <TAXI/Tgf.h>
#include <QT/Misc.h> 
#include <QT/Handler.h>
#include <QT/Action_def.h>
#include <QT/Action.h>
#include <QT/pigalePaint.h> 
#include <QT/pigaleCanvas.h> 
#include <QT/clientEvent.h>

int Test(GraphContainer &GC,int action,int & drawing);
void UndoErase();

void pigaleWindow::initServer()
  {ServerClientId = 0;
  NewGraph(); 
  
  server = new PigaleServer(this);
  server->setProxy(QNetworkProxy::NoProxy);
  threadServer = 0;
  if(!server->isListening ())
      {Tprintf("Server: Init failed");
      cout <<"Server: Init failed"<<endl;
      }
  else
      {Tprintf("Server using port%d",server->serverPort());
      menuIntAction[A_SERVER_INIT]-> setEnabled(false);
      }
  showMinimized();
  }
bool pigaleWindow::event(QEvent * ev)
  {
  if(ev->type() >=  QEvent::User)
      {customEvent(ev);
      return TRUE;
      }
  if(ev->type() == QEvent::Close)
      {QCloseEvent *ec = new QCloseEvent::QCloseEvent();
      closeEvent(ec);
      return TRUE;
      }
  if(ev->type() >=  QEvent::KeyPress)
      {QKeyEvent *k = (QKeyEvent *)ev;
      keyPressEvent(k);
      return TRUE;
      }
  return FALSE;
  }
void pigaleWindow::customEvent(QEvent * ev)
  {ev->accept();
  switch( ev->type())
      {case  TEXT_EVENT:
          {textEvent *event  =  (textEvent  *)ev;
          Message(event->getString());
          }
          break;
      case CLEARTEXT_EVENT:
          messages->setPlainText("");
          break;
      case BANNER_EVENT:
          {bannerEvent *event  =  (bannerEvent  *)ev;
          statusBar()->showMessage(event->getString());
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
           blockInput(true);qApp->processEvents();
           postHandler(ev);    
          break;
      case READY_EVENT:
          whenReady();
          break;
      case PROGRESS_EVENT:
          {progressEvent *event  =  (progressEvent  *)ev;
          int action  = event->getAction();
          if(action == -1)
              progressBar->hide();
          else if(action == 1)
              {progressBar->setRange(0,event->getStep());
              progressBar->setValue(0);
              progressBar->show();
              }
          else
              progressBar->setValue(event->getStep());
          }
          break;
      default:
          qDebug("UNKNOWN EVENT");
          break;
      }
  }
void pigaleWindow::banner()
  {QString msg;  
  int NumRecords =IO_GetNumRecords(0,(const char *)InputFileName.toAscii());
    int NumRecordsOut =IO_GetNumRecords(0,(const char *)OutputFileName.toAscii());
    msg.sprintf("Input: %s %d/%d  Output: %s %d Undo:%d/%d"
                ,(const char *)InputFileName.toAscii()
                ,*pGraphIndex,NumRecords
                ,(const char *)OutputFileName.toAscii()
                ,NumRecordsOut
                ,UndoIndex,UndoMax);
  bannerEvent *e = new bannerEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postMessage(const QString &msg)
  {if(++numMessages > 500|| MacroLooping)return;
  textEvent *e = new textEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postMessageClear()
  {clearTextEvent *e = new clearTextEvent();
  numMessages = 0;
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postWait(const QString &msg)
  {waitEvent *e = new waitEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postDrawG()
  {drawgEvent *e = new drawgEvent();
  QApplication::postEvent(this,e);
  }
void pigaleWindow::Message(QString s)
  {messages->append(s);
  messages->ensureCursorVisible();
  } 
void pigaleWindow::showInfoTab()
  {// index is 0
  rtabWidget->setCurrentIndex(rtabWidget->indexOf(gInfo));
  }
void pigaleWindow::mapActionsInit()
  {int na = (int)(sizeof(Actions)/sizeof(_Action));
  for(int i = 0;i < na;i++)
      {mapActionsString[Actions[i].num] = Actions[i].name;
      mapActionsInt[Actions[i].name] = Actions[i].num;
      }
  }
QString pigaleWindow::getActionString(int action)
  {return mapActionsString[action];
  }
int pigaleWindow::getActionInt(QString action_str)
  {return mapActionsInt[action_str];
  }
void pigaleWindow::computeInformation()
// only called manually from the menu
  {TopologicalGraph G(GC);
  if(G. Set(tvertex()).exist(PROP_NLOOPS))  G. Set(tvertex()).erase(PROP_NLOOPS);
  if(G.Set(tedge()).exist(PROP_MULTIPLICITY))  G.Set(tedge()).erase(PROP_MULTIPLICITY);
  if(G.Set(tedge()).exist(PROP_ORIENTED))  G.Set(tedge()).erase(PROP_ORIENTED);
  if(G.Set(tedge()).exist(PROP_REORIENTED))  G.Set(tedge()).erase(PROP_REORIENTED);
  if(G.Set().exist(PROP_SIMPLE))      G.Set().erase(PROP_SIMPLE);
  if(G.Set().exist(PROP_CONNECTED))  G.Set().erase(PROP_CONNECTED);
  if(G.Set().exist(PROP_BICONNECTED))  G.Set().erase(PROP_BICONNECTED);
  if(G.Set().exist(PROP_TRICONNECTED))  G.Set().erase(PROP_TRICONNECTED);
  if(G.Set().exist(PROP_BIPARTITE))  G.Set().erase(PROP_BIPARTITE);
  if(G.Set().exist(PROP_PLANAR))  G.Set().erase(PROP_PLANAR);
  if(G. Set().exist(PROP_NLOOPS))  G. Set().erase(PROP_NLOOPS);
  Prop1<int> maptype(G.Set(),PROP_MAPTYPE,PROP_MAPTYPE_UNKNOWN);
  G.planarMap() = 0;
  postMessageClear();setError(0);
  graph_properties->update(GC,true);
  }
void pigaleWindow::information(bool erase)
  {if(!getError() && erase &&  !MacroLooping  && !ServerExecuting)postMessageClear();
  graph_properties->update(GC,!MacroExecuting && !MacroLooping && !ServerExecuting);
  }
void pigaleWindow::settingsHandler(int action)
  {// only called when a checkbox is clicked
  // but could be called from the server
  switch(action)
      {case A_SET_DEBUG:
          debug() = !debug();
          return;
      case  A_SET_SCH_RECT:
          staticData::SchnyderRect() = !staticData::SchnyderRect();
          return;
      case A_SET_LFACE:
          staticData::SchnyderLongestFace() = !staticData::SchnyderLongestFace();
          return;
      case A_SET_SCH_COLOR:
          staticData::SchnyderColor() =!staticData::SchnyderColor();
          return;
      case A_SET_ERASE_MULT:
          staticData::RandomEraseMultipleEdges() = !staticData::RandomEraseMultipleEdges();
          return;
      case A_SET_GEN_CIR:
          staticData::RandomUseGeneratedCir() = !staticData::RandomUseGeneratedCir();
          return;
      case A_SET_RANDOM_SEED:
          staticData::RandomSeed() = !staticData::RandomSeed();
          return;
      case A_SET_UNDO:
          staticData::IsUndoEnable = !staticData::IsUndoEnable;
          UndoEnable(staticData::IsUndoEnable);
          return;
      case A_SET_ORIENT:
          staticData::ShowOrientation() = !staticData::ShowOrientation();
          break;
      case A_SET_ARROW:
          staticData::ShowArrow() = !staticData::ShowArrow();
          break;
      case A_SET_EXTBRIN:
          staticData::ShowExtTbrin() = !staticData::ShowExtTbrin();
          break;
      }
  // update the editor
  handlerEvent *e = new handlerEvent(1,0,0);
  QApplication::postEvent(this,e);
  return;
  }
void pigaleWindow::handler(QAction *qaction)
  {int Id = getId(qaction);
  if(Id)handler(Id);
  }
int pigaleWindow::handler(int action)
  {if(action < A_PAUSE)return 0;
  getResultHandler() = -999;
  if(MacroRecording)macroRecord(action);
  if(action == A_PAUSE)
      {qApp->processEvents();
      MacroWait = true;
      QTimer::singleShot(100*staticData::macroDelay,this,SLOT(timerWait()));
      return 0;
      }
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
      pigaleThread.run(action,0,staticData::Gen_N1,staticData::Gen_N2,staticData::Gen_M);
      }
  else if(action < A_ALGO_END)
      pigaleThread.run(action,staticData::nCut);
  else if(action < A_ORIENT_END)
      pigaleThread.run(action);
  else if(action < A_TEST_END)
      pigaleThread.run(action);
//       {int drawing;
//       timer.start();
//       int ret = Test(GC,action - A_TEST,drawing);
//       handlerEvent *e = new handlerEvent(ret,drawing,0);
//       QApplication::postEvent(this,e);
//       return 0;
//       }
   else if(action > A_INPUT && action < A_INPUT_END)
       pigaleThread.run(action);
   else if(action > A_TRANS && action < A_TRANS_END)
       pigaleThread.run(action);
  else if(action > A_SET)
      {settingsHandler(action);return 0;}
  else
      return 0;
  return 0;
  }
int pigaleWindow::postHandler(QEvent *ev)
  {handlerEvent *event = (handlerEvent  *)ev;  
  int action = event->getAction();
  postHandler(action,event->getDrawingType(),event->getSaveType());
  pigaleThread.run(0); //to unlock the thread
  return 0;
  }
int pigaleWindow::postHandler(int action,int drawingType,int saveType)
  {//action   0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
  // 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders
  //10:png to client
  //cout<<"action:"<<action<<" drawingType:"<<drawingType<<" saveType:"<<saveType<<endl;
  double Time = timer.elapsed()/1000.;
  if(action <= 0) // error
      {blockInput(false);
      if(getError())
          {Tprintf("Handler Error:%s",(const char *)getErrorString().toAscii());
          setError(0);
          }
      return action;
      }
 
  if(saveType == 1)
      UndoSave();
  else if(saveType == 2)
      UndoTouch(false);

  // In case we called the orienthandler
  chkOrient->setCheckState(staticData::ShowOrientation() ? Qt::Checked : Qt::Unchecked);
  if(action == 1)
      //{if(!MacroExecuting )gw->update();}
      {gw->update();}
  else if(action == 2)
      {if(MacroExecuting)information(false);
      else if(!MacroRecording)information();
      if(!MacroExecuting )gw->update();
      }
  else if(action == 20) // Remove handler
      {if(!MacroRecording)information();
      if(!MacroExecuting ) gw->update(0);
      }
  else if(action == 3)
      mypaint->update(drawingType); 
  else if(action == 4) //3d drawings
      graphgl->update(); 
  else if(action == 5) //symetrie
      {graphgl->update(); 
      graphsym->update();
      }
  // cases 6-7-8 we need a canvas with the current graph loaded
  else if(action == 6)
      {gw->update();gw->Spring();}
  else if(action == 7)
      {gw->update();gw->SpringPreservingMap();}
  else if(action == 8)
      {gw->update();gw->SpringJacquard();}

  //cases 10 create a png 11 create a ps
  else if(action == 10)
      png();
  else if(action == 11)
      print();
 
  blockInput(false);
  double TimeG = timer.elapsed()/1000.;
  if(!MacroLooping && !MacroRecording && !ServerExecuting)
      {Tprintf("Used time:%3.3f (G+I:%3.3f)",Time,TimeG);
      if(getError())
          {Tprintf("Handler Error:%s",(const char *)getErrorString().toAscii());
          setError(0);
          if(debug())Twait((const char *)getErrorString().toAscii()); 
          }
      }
  return action;
  }
int & pigaleWindow::getResultHandler()
  {static int _value = 0;
  return _value;
  }
/************************************************************/
PigaleThread::PigaleThread(QObject *parent) 
    : QThread (parent)
  {restart = false;
  abort = false;
  }
PigaleThread::~PigaleThread()
  {stop();
  }
void PigaleThread::stop()
  {mutex.lock();
  abort = true;
  condition.wakeOne();
  mutex.unlock();
  wait();
  //cout<<"stop computing thread"<<endl;
  }
void PigaleThread:: run(int action,int N,int N1,int N2,int M,int delay)
  {QMutexLocker locker(&mutex);
  this->action   =  action;
  this->N  = N;
  this->N1 = N1;
  this->N2 = N2;
  this->M  = M;
  this->delay = delay;
  if(!isRunning()) 
      start();
  else 
      {restart = true;
      condition.wakeOne();
      }
  }
void PigaleThread::run()
  {int ret,saveType,drawingType;
  ret = saveType = drawingType = 0;
  for(;;)
      {mutex.lock();
      int action = this->action;
      int N      = this->N;
      int N1     = this->N1;
      int N2     = this->N2;
      int M      = this->M;
      delay      = this->delay; 
      mutex.unlock();

      if(abort)return;
      if(action)
          {//cout <<"thread:"<<(const char*)mw->getActionString(action)<<endl;
          mw->timer.start();
          
          if(action < A_AUGMENT_END)
              ret = AugmentHandler(mw->GC,action);
          else if(action < A_EMBED_END)
              ret = EmbedHandler(mw->GC,action,drawingType);
          else if(action < A_GRAPH_END)
              {ret = DualHandler(mw->GC,action); 
              saveType = 1;
              }
          else if(action < A_REMOVE_END)
              {ret = RemoveHandler(mw->GC,action);
              saveType = 2;
              }
          else if(action < A_GENERATE_END)
              {ret = GenerateHandler(mw->GC,action,N1,N2,M);
              saveType = 1;
              }
          else if(action < A_ALGO_END)
              ret = AlgoHandler(mw->GC,action,N);
          else if(action < A_ORIENT_END)
              {ret = OrientHandler(mw->GC,action);
              staticData::ShowOrientation() = true;
              }
          else if(action < A_TEST_END)
              ret = Test(mw->GC,action - A_TEST,drawingType);
          else if(action == A_TRANS_GET_CGRAPH || action == A_INPUT_READ_GRAPH)
              {mw->publicLoad(mw->GraphIndex1);
              ret = 2;
              }
           else if(action == A_TRANS_SEND_PNG)
               ret = 10;
           else if(action == A_TRANS_SEND_PS)
               ret = 11;
          // post an event to execute the graphics
          mw->pigaleThreadRet = ret;
          handlerEvent *e = new handlerEvent(ret,drawingType,saveType);
          mutex.lock();
          QApplication::postEvent(mw,e);
          }
      else
          mutex.lock();
      if(!restart) condition.wait(&mutex);
      restart = false;
      mutex.unlock();
      // ne s'execute que quand: condition.wakeOne();
      if(action)
          mw->getResultHandler() = mw->pigaleThreadRet;
      else
          continue;
      
      if(mw->ServerExecuting)// will execute only when posthandler has finished
          {if(getError())
              {mw->threadServer->writeClientEvent(":ERROR "+getErrorString()+" : "+mw->getActionString(action));
              setError();
              mw->threadServer->writeClientEvent("!!");
              mw->threadServer->serverReady();
              }
          else
              {if(action ==  A_TRANS_SEND_PNG)
                  mw->threadServer->Png();
              else if(action ==  A_TRANS_SEND_PS)
                  mw->threadServer->Ps();
              else
                  {mw->threadServer->writeClientEvent(QString("!%1 S").arg(mw->getActionString(action)));
                  //cout <<"      -> end thread:"<<(const char*)mw->getActionString(action)<<endl;
                  mw->threadServer->serverReady();
                  }
              }
          }
      }
  }
