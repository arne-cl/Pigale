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
#include <QT/pigalePaint.h>
#include <QT/pigaleCanvas.h> 
#include <QT/clientEvent.h> 

#include <qmenubar.h>
#include <qfile.h>
#include <qfileinfo.h>

int InitPigaleServer(pigaleWindow *w)
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
PigaleServer::PigaleServer(pigaleWindow *w,QObject* parent):
    QServerSocket(4242,1,parent),mw(w),nconnections(0)
    { }
void PigaleServer::newConnection(int socket)
  {if(nconnections == 0)
      {clientsocket = new ClientSocket(socket,mw,this,qApp);
      nconnections = 1;
      mw->postMessageClear();
      Tprintf("Server: New connection");
      mw->ServerExecuting = true;      mw->blockInput(true);
      }
  else
      {QSocket *so =  new QSocket(qApp);
      so->setSocket(socket);
      QTextStream cli;
      cli.setDevice(so);
      cli << ":Server Busy"<<endl;
      so->close();
      }
  }
void PigaleServer::OneClientClosed()
  {Tprintf("Server: Client disconnects");
  mw->ServerClientId = 0;
  nconnections = 0;
  mw->ServerExecuting = false;   mw->blockInput(false);
  }

//ClientSocket: thread reading and writing on a socket
ClientSocket::ClientSocket(int sock,pigaleWindow *p,PigaleServer *server,QObject *parent,const char *name) :
    QSocket(parent,name),sdebug(0),getRemoteGraph(false),mw(p)
  {connect(this,SIGNAL(readyRead()),SLOT(readClient()));
  connect(this,SIGNAL(connectionClosed()),SLOT(deleteLater()));
  connect(this,SIGNAL(connectionClosed()),server,SLOT(OneClientClosed()));
  connect(this,SIGNAL(logText(const QString&)),SLOT(writeServer(const QString&)));
  setSocket(sock);
  cli.setDevice(this);
  clo.setDevice(this);
  line = 1;
  start();
  mw->ServerClientId = prId = sock;
  cli << ":Server Ready"<<endl;
  cli << "!" << endl;
  }
void ClientSocket::ClientClosed()
  {mw->ServerExecuting = false;
  mw->blockInput(false);
  mw->close();
#ifndef _WINDOWS
  terminate();
#endif
  wait();
  }
void ClientSocket::writeServer(const QString& msg)
  {T_STD cout << msg;  
  }
void ClientSocket::readClient()
  {run();
  }
void ClientSocket::run()
  {while (canReadLine())
      {QString str = cli.readLine();
      if(++line == 10000)line = 0;
      if(str.at(0) == '#')
          {cli << str << endl;
          cli << "!" << endl;
          }
      else if(str.at(0) == '!')
          {cli << ":END OF FILE" << endl;
          cli << "!" << endl;
          }
      else 
          xhandler(str);
      }
  }
void ClientSocket::customEvent( QCustomEvent * e )
  {if(e->type() == CLIENT_EVENT) 
    {clientEvent  *event  =  (clientEvent  *)e;
    handlerInput(event->getAction(),event-> getParamString());
    }
  }

int ClientSocket::xhandler(const QString& dataAction)
  {int pos = dataAction.find(PARAM_SEP);
  QString beg = dataAction.left(pos);
  QString dataParam = dataAction.mid(pos+1);
  int action = mw->getActionInt(beg);
  Tprintf("%s",(const char *)dataAction);
 // call the right handler
  if(action == 0)
      setError( UNKNOWN_COMMAND,"unknown command");
  else if(action > A_INFO && action < A_INFO_END)
      handlerInfo(action);
  else if(action > A_INPUT && action < A_INPUT_END)
      {clientEvent *event = new clientEvent(action,dataParam);
      QApplication::postEvent(this,event);
      }
  else if(action > A_AUGMENT && action < A_TEST_END)
      {if(mw->graph_properties->actionAllowed(action))
          mw->handler(action);
      else 
          cli <<":ACTION NOT ALLOWED:"<<mw->getActionString(action)<< endl;
      }
  else if(action > A_TRANS && action < A_TRANS_END)
      {if(action == A_TRANS_PNG) // send a png
          Png();
      else if(action == A_TRANS_GRAPH_GET) // get a graph
          {QStringList fields = QStringList::split(PARAM_SEP,dataParam);
          indexRemoteGraph = 1;
          bool ok =true;
          if(fields.count() > 1)indexRemoteGraph = fields[1].toInt(&ok);
          if(!ok)setError(WRONG_PARAMETERS,"Wrong parameters");
          else   GetRemoteGraph();
          }
      }
  else if(action == SERVER_DEBUG)
      sdebug = 1;
  else
      setError( UNKNOWN_COMMAND,"unknown command");

  int err = 0;
  if(getError())
      {err = getError();
      if(strlen(getErrorMsg()))
          cli << ":ERROR "<< getErrorMsg() <<endl;
      else
          {cli <<":ERROR '"<<mw->getActionString(action)<<"' -> " << mw->getActionString(err)<<endl;
          cli <<": " <<dataAction<< endl;
          }
      }

  cli << "!" << endl;
  return err; 
  }
uint ClientSocket::readBuffer(char  *  &buffer)
  {uint size = 0;
  uint nb = bytesAvailable();
  while(bytesAvailable() < 4)waitForMore(10);
  clo >>  size;
  if(size <= 0)return 0;
  uint size0 = 0;
  buffer  = new char[size+1];
  int retry = 0;
  uint nread = 0;
  char *pbuff = buffer;
  while(nread  < size)
      {waitForMore(10);  // in millisec
      nb = bytesAvailable();
      if(nb == 0)
          {if(++retry > 1000) {setError(-1,"Timeout");  cli << ":Server: receiving graph TIMEOUT" << endl; return 0; }
          continue;
          }
      retry = 0;
      if(nb > size-nread)nb = size-nread;
      nread += nb;
      clo.readRawBytes(pbuff,nb);
      pbuff += nb;
      if(nread >= size0 && sdebug)
          {int percent = (int)(nread*100./size + .5);
          size0 = nread + size/10; // we write when at least 10% more  is read
           if(sdebug)cli << QString(":Server:%1 % (%2 / %3)").arg(percent).arg(nread).arg(size) << endl;
          Tprintf(" %d % (%d)",percent,size);
          }
      }
  return size;
  }
int ClientSocket::GetRemoteGraph()
  {if(sdebug)cli << ":Server: receiving graph" << endl;
  Tprintf("Receiving graph");
  char *buffer = NULL;
  uint size = readBuffer(buffer);
  if(size == 0) {delete [] buffer;setError(READ_ERROR,"empty file");return READ_ERROR;}
  QString GraphFileName = QString("/tmp/graph%1.tmp").arg(mw->ServerClientId);
  QFile file(GraphFileName);
  file.remove();
  file.open(IO_ReadWrite);
  QDataStream stream(&file);
  stream.writeRawBytes(buffer,size);
  file.close();
  delete [] buffer;
  if(sdebug)cli << QString(":GOT %1:%2 bytes").arg(GraphFileName).arg(size) << endl;
  mw->InputFileName = GraphFileName;
  if(mw->publicLoad(indexRemoteGraph) < 0)setError(READ_ERROR,"could not read file");
  return 0;
  }
int ClientSocket::ReadRemoteGraph(QString &dataParam)
  {// Read a graph on the server side
  QStringList fields = QStringList::split(PARAM_SEP,dataParam);
  int num = 1;
  bool ok;
  if(fields.count() > 1)num = fields[1].toInt(&ok);
  if(!ok)return WRONG_PARAMETERS;
  if(mw->publicLoad(num) < 0)setError(READ_ERROR,"coud not read file");
  return 0;
  }

int ClientSocket::Png()
  {mw->png();
  QString PngFileName =  QString("/tmp/server%1.png").arg(mw->ServerClientId);
  QFileInfo fi = QFileInfo(PngFileName);
  if(PngFileName.isEmpty())
      {setError(-1,"no png file");return -1;}
  else if(sdebug)
      cli << ":SENDING:" << PngFileName << endl;
  uint size = fi.size();
  QFile file(PngFileName);
  file.open(IO_ReadWrite);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawBytes(buff,size); 
  cli <<"!PNG" << endl;
  clo.writeBytes(buff,size);
  delete [] buff;
  file.remove();
  return 0;
  }

int ClientSocket::handlerInput(int action,const QString& dataParam)
  {QStringList fields = QStringList::split(PARAM_SEP,dataParam);
  QString msg;
  int nfield = (int)fields.count();
  bool ok;
  switch(action)
      {case A_INPUT_READ_GRAPH:
          {if(nfield == 0){setError(WRONG_PARAMETERS,"Wrong parameters");return WRONG_PARAMETERS;}
           mw->InputFileName = fields[0];
           int num = 1;
           if(nfield > 1)num = fields[1].toInt(&ok);
           if(!ok)return WRONG_PARAMETERS;
           if(mw->publicLoad(num) < 0){setError(READ_ERROR,"read error");return READ_ERROR;}
           }
           break;
      case  A_INPUT_NEW_GRAPH:
          mw->NewGraph();
          break;
      case A_INPUT_NEW_VERTEX:
          {int n = 1;
          if(nfield > 0)
              {n = fields[0].toInt(&ok);
              if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return WRONG_PARAMETERS;}
              }
          TopologicalGraph G(mw->GC);
          for(int i = 0;i < n;i++)G.NewVertex();
          // Labels
          Prop<long> vlabel(G.Set(tvertex()),PROP_LABEL);
          for(int i = 0;i <= n;i++)vlabel[i] = i;
          // Calcul des coordonnes
          Prop<Tpoint> vcoord(G.PV(),PROP_COORD);
          double angle = 2.*acos(-1.)/G.nv();
          vcoord[0]=Tpoint(0,0);
          for (int i=1; i<=G.nv(); i++)
              vcoord[i] = Tpoint(cos(angle*i),sin(angle*i));
          }
          break;
      case A_INPUT_NEW_EDGE:
          {if(nfield < 2){setError(WRONG_PARAMETERS,"need 2 vertices");return WRONG_PARAMETERS;}
          int v1 = fields[0].toInt(&ok);
          if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return WRONG_PARAMETERS;}
          int v2 = fields[1].toInt(&ok);
          if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return WRONG_PARAMETERS;}
          TopologicalGraph G(mw->GC);
          if(v1 > G.nv() || v2 > G.nv() || v1 == v2)
              {setError(WRONG_PARAMETERS,"Wrong parameters");return WRONG_PARAMETERS;}
          G.NewEdge((tvertex)v1,(tvertex)v2);
          mw->gw->update();
          }
          break;
      default:
          setError( UNKNOWN_COMMAND,"unknown command");return  UNKNOWN_COMMAND;
          break;
      }
  return 0;
  }
int ClientSocket::handlerInfo(int action)
  {TopologicalGraph G(mw->GC);
  mw->graph_properties->updateMenu(false);
  mw->information();
  Graph_Properties *inf = mw->graph_properties;
  cli  << mw->getActionString(action) << ":";
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
          setError( UNKNOWN_COMMAND,"unknown command");
          break;
      }
  return 0;
  }
