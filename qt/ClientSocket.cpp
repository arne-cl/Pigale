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

#include "ClientSocket.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include "LineEditNum.h"
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <QT/MyPaint.h>
#include <QT/MyCanvas.h> 

#include <qmenubar.h>

void InitPigaleColors();
void InitPigaleFont();
int InitPigaleServer(MyWindow *w);

int InitPigaleServer(MyWindow *w)
  {PigaleServer  *server = new PigaleServer(w,qApp);
  if(!server->ok())
      {Tprintf("Server: Init failed");
      return -1;
      }
  else
      Tprintf("Server: Init");
  return 0;
  }

//PigaleServer:public QServerSocket 
PigaleServer::PigaleServer(MyWindow *w,QObject* parent):
    QServerSocket(4242,1,parent),mw(w),nconnections(0)
    { }
void PigaleServer::newConnection(int socket)
  {if(nconnections == 0)
      {clientsocket = new ClientSocket(socket,mw,this,qApp);
      nconnections = 1;
      Tprintf("Server: New connection");
      mw->ServerExecuting = true;
      mw->blockInput(true);
      }
  else
      Tprintf("Server: already a connection");
  }
void PigaleServer::OneClientClosed()
  {--nconnections;
   Tprintf("Server: The client disconnects");
   mw->ServerExecuting = false;
   mw->blockInput(false);
   
  }

//ClientSocket: public QSocket
ClientSocket::ClientSocket(int sock,const int id,bool display,QObject *parent,const char *name) :
    QSocket(parent,name),line(0),prId(id),sdebug(0),cli(this)
  {connect(this, SIGNAL(readyRead()),SLOT(readClient()));
  connect(this, SIGNAL(connectionClosed()),SLOT(deleteLater()));
  
  setSocket(sock);
  InitPigaleFont(); 
  InitPigaleColors();
  mw = new MyWindow();
  mw->show();
  if(!display)mw->showMinimized(); 
  connect(this, SIGNAL(connectionClosed()),SLOT(ClientClosed()));
  }
ClientSocket::ClientSocket(int sock,MyWindow *p,PigaleServer *server,QObject *parent,const char *name) :
    QSocket(parent,name),sdebug(0),cli(this),mw(p)
  {connect(this, SIGNAL(readyRead()),SLOT(readClient()));
  connect(this, SIGNAL(connectionClosed()),SLOT(deleteLater()));
  connect(this, SIGNAL(connectionClosed()),server,SLOT(OneClientClosed()));
  setSocket(sock);
  prId = 1;
  line = 1;
  }
void ClientSocket::ClientClosed()
  {mw->close();
  }
void ClientSocket::writeServer(QString& msg)
  {emit logText(QString("  %1->%2: %3\n").arg(prId).arg(line).arg(msg));
  }
void ClientSocket::readClient()
  {while (canReadLine())
      {QString str = cli.readLine();
      //qDebug("R:%s",(const char *)str);
      if(++line == 10000)line = 0;
      if(str.at(0) == '#')
	  cli << str << endl;
      else 
	  //parseSplitAction(str);  
	  xhandler(str); // now the server split actions  
      }
  }
int ClientSocket::parseSplitAction(const QString& data)
  {QStringList fields = QStringList::split(ACTION_SEP,data);
  for(int i = 0;i < (int)fields.count();i++)
      xhandler(fields[i]);
  cli <<"!" << endl; // tells the client that processing is over
  return 0;
  }
int ClientSocket::xhandler(const QString& dataAction)
  {int pos = dataAction.find(PARAM_SEP);
  QString beg = dataAction.left(pos);
  QString dataParam = dataAction.mid(pos+1);
  if(sdebug)cli <<"# '"<<dataAction<<"'"<<endl;
  bool ok;
  int action = beg.toInt(&ok);
  int err = 0;
  if(!ok)
      {err = ACTION_NOT_INT;
      cli <<"# ERREUR '"<<dataAction <<"' -> "<<err << endl;
      cli <<"# data param:" <<  beg << endl;
      return err;
      }
  
  // call the right handler
  if(action > A_INFO && action < A_INFO_END)
      err =  handlerInfo(action);
  else if(action > A_INPUT && action < A_INPUT_END)
      err =  handlerInput(action,dataParam);
  else if(action > A_AUGMENT && action < A_TEST_END)
      {if(mw->menuBar()->isItemEnabled(action))
	  mw->handler(action);
      else 
	  cli <<"#Action not allowed:"<<mw->getActionString(action)<< endl;
      }
  else if(action == SERVER_DEBUG)
      sdebug = 1;
  else
      err = UNKNOWN_COMMAND;
  if(err)
      {cli <<"# ERREUR '"<<mw->getActionString(action)<<"' -> "
	   << mw->getActionString(err)<<endl;
      cli <<"# " <<dataAction<< endl;
      }
  return err; 
  }
int ClientSocket::handlerInput(int action,const QString& dataParam)
  {QStringList fields = QStringList::split(PARAM_SEP,dataParam);
  QString msg;
  int nfield = (int)fields.count();
  bool ok;
  switch(action)
      {case A_INPUT_READ_GRAPH:
	   {if(nfield == 0)return WRONG_PARAMETERS;
	   mw->InputFileName = fields[0];
	   int num = 1;
	   if(nfield > 1)num = fields[1].toInt(&ok);
	   if(!ok)return WRONG_PARAMETERS;
	   if(mw->publicLoad(num) < 0)return READ_ERROR;
	   //msg = QString("R: '%1':%2").arg(mw->InputFileName).arg(num);
	   //writeServer(msg);
	   }
	   break;
      case  A_INPUT_NEW_GRAPH:
	  {Graph G(mw->GC);
	  G.StrictReset();
	  }
	  break;
      case A_INPUT_NEW_VERTEX:
	  {int n = 1;
	  if(nfield > 0)
	      {n = fields[0].toInt(&ok);
	      if(!ok)return WRONG_PARAMETERS; 
	      }
	  TopologicalGraph G(mw->GC);
	  for(int i = 0;i < n;i++)G.NewVertex();
	  }
	  break;
      case A_INPUT_NEW_EDGE:
	  {if(nfield < 2)return WRONG_PARAMETERS; 
	  int v1 = fields[0].toInt(&ok);
	  if(!ok)return WRONG_PARAMETERS; 
	  int v2 = fields[1].toInt(&ok);
	  if(!ok)return WRONG_PARAMETERS;
	  TopologicalGraph G(mw->GC);
	  if(v1 > G.nv() || v2 > G.nv() || v1 == v2)return WRONG_PARAMETERS;
	  G.NewEdge((tvertex)v1,(tvertex)v2);
	  mw->gw->update();
	  }
	  break;
      default:
	  return  UNKNOWN_COMMAND;
	  break;
      }
  return 0;
  }
int ClientSocket::handlerInfo(int action)
  {TopologicalGraph G(mw->GC);
  mw->information();
  Graph_Properties *inf = mw->graph_properties;
  cli << mw->getActionString(action) << ":";
  switch(action)
      {case A_INFO_N:
	   cli << G.nv() << endl;
	   break;
      case A_INFO_M:
	    cli << G.ne() << endl;
	    break;
      case A_INFO_SIMPLE:
	  cli << inf->Simple() << endl;
	  break;
      case A_INFO_PLANAR:
	  cli << inf->Planar() << endl;
	  break;
      case A_INFO_OUTER_PLANAR:
	  cli << inf->OuterPlanar() << endl;
	  break;
      case A_INFO_SERIE_PAR:
	  cli << inf->SeriePlanar() << endl;
	  break;
      case A_INFO_MAX_PLANAR:
	  cli << inf->Triangulation() << endl;
	  break;
      case A_INFO_BIPAR:
	  cli << inf->Biparti() << endl;
	  break;
      case A_INFO_MAX_BIPAR:
	  cli << inf->MaxBiparti() << endl;
	  break;
      case A_INFO_REGULIER:
	  cli << inf->Regular() << endl;
	  break;
      case A_INFO_CON1:
	  cli << inf->Con1() << endl;
	  break;
      case A_INFO_CON2:
	  cli << inf->Con2() << endl;
	  break;
      case A_INFO_CON3:
	  cli << inf->Con3() << endl;
	  break;
      case A_INFO_MIN_D:
	  cli << inf->DegreeMin() << endl;
	  break;
      case A_INFO_MAX_D:
	  cli << inf->DegreeMax() << endl;
	  break;
      case A_INFO_COORD:
	  {if(!G.Set(tvertex()).exist(PROP_COORD)) return PROP_NOT_DEFINED;
	  Prop<Tpoint> coord(G.Set(tvertex()),PROP_COORD);
	  cli << endl;
	  for(tvertex v = 1;v <= G.nv();v++)
	      cli << coord[v].x() <<" "<< coord[v].y() << endl;
	  }
	  break;
      case A_INFO_VLABEL:
	  {if(!G.Set(tvertex()).exist(PROP_LABEL)) return PROP_NOT_DEFINED;
	  Prop<long> label(G.Set(tvertex()),PROP_LABEL);
	  cli << endl;
	  for(tvertex v = 1;v <= G.nv();v++)
	      cli << label[v] << endl;
	  }
	  break;
      default:
	  return  UNKNOWN_COMMAND;
	  break;
      }
  return 0;
  }
