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

#undef QT3_SUPPORT

#include "ClientSocket.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <QT/pigalePaint.h>
#include <QT/pigaleCanvas.h> 
#include <QT/clientEvent.h> 

#include <QStringList>



PigaleServer::PigaleServer(pigaleWindow *_mw)
    :QTcpServer(_mw),mw(_mw)
  {nconnections = 0;
  mw->ServerClientId = 0;
  listen(QHostAddress::Any,4242); 
  connect(this,SIGNAL(newConnection()),this,SLOT(OneClientOpened()));
  }
void PigaleServer::createNewServer()
  {QTcpSocket *socket = nextPendingConnection();
  ClientSocket *thread = new ClientSocket(mw,socket);
  connect(thread, SIGNAL(finished()),thread,SLOT(deleteLater()));
  connect(socket,SIGNAL(disconnected()),SLOT(OneClientClosed()));
  connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
  mw->threadServer = thread;
  }
void PigaleServer::OneClientClosed()
  {mw->threadServer->quit();
  delete mw->threadServer;
  --nconnections;
  if(hasPendingConnections())createNewServer();
  }
void PigaleServer::OneClientOpened()
  {if(++nconnections == 1)createNewServer();
  }
/**********************************************************/
// thread
ClientSocket::ClientSocket(pigaleWindow *_mw,QTcpSocket *_socket)
    :QThread(_mw),mw(_mw)
    ,abort(false),busy(false),sdebug(0),line(0)
  {socket = _socket;
  clo.setDevice(socket);   clo.setVersion(QDataStream::Qt_4_0);
  mw->postMessageClear();
  mw->ServerExecuting = true;      mw->blockInput(true);
  Tprintf("Server: New connection %d",++mw->ServerClientId);
  writeClientEvent(":Server Ready");
  writeClientEvent("!Server Ready C");
  serverReady();
  }
ClientSocket:: ~ClientSocket()
  {Tprintf("Server: Client disconnects %d",mw->ServerClientId);
  mw->ServerExecuting = false;   mw->blockInput(false);
  }
void ClientSocket::runEvent(QEvent * ev)
  {mutex.lock();
  ev0   =  ev;
  mutex.unlock();
  start();
  }
void ClientSocket::run()
  {mutex.lock();
  QEvent * ev = ev0;
  mutex.unlock();
  if(abort)return;
  if(ev->type() == (int)CLIENT_EVENT) 
      {clientEvent  *event  =  (clientEvent  *)ev;
      int action = event->getAction();
      QString str = event->getParamString();
      handlerInput(action,str);
      }
  else if(ev->type() == (int)SERVER_READY_EVENT) 
      executeAction();
  else if(ev->type() == (int)WRITE_EVENT) 
      {writeEvent  *event  =  (writeEvent  *)ev;
      QString str = event->getString();
      writeClient(str);
      }
  else if(ev->type() == (int)WRITEB_EVENT) 
      {writeBufEvent  *event  =  (writeBufEvent  *)ev;
      char * buf = event->getPtr();
      uint size = event->getSize();
      writeClient(buf,size);
      }
  else if(ev->type() == (int)TEXT_EVENT) 
      {textEvent  *event  =  (textEvent  *)ev;
      QString str = event->getString();
      busy = false;
      xhandler(str);
      }
  mutex.lock();
  busy = false;
  mutex.unlock(); 
  }

bool ClientSocket::event(QEvent * ev)
  {if(ev->type() < QEvent::User)return FALSE;
  ev->accept();
// #ifndef _WINDOWS
//   mutex.lock();
//   busy = true;  
//   mutex.unlock();
//   runEvent(ev);
//   while(busy) msleep(10);
// #else
  customEvent(ev); // non thread version
// #endif
  return TRUE;
  }
void  ClientSocket::customEvent(QEvent * ev)
  {if(ev->type() == (int)CLIENT_EVENT) 
    {clientEvent  *event  =  (clientEvent  *)ev;
    handlerInput(event->getAction(),event->getParamString());
    }
  else if(ev->type() == (int)SERVER_READY_EVENT) 
      executeAction();
  else if(ev->type() == (int)WRITE_EVENT) 
    {writeEvent  *event  =  (writeEvent  *)ev;
    writeClient(event->getString());
    }
  else if(ev->type() == (int)WRITEB_EVENT) 
    {writeBufEvent  *event  =  (writeBufEvent  *)ev;
    writeClient(event->getPtr(),event->getSize());
    }
  else if(ev->type() == (int)TEXT_EVENT) 
    {textEvent  *event  =  (textEvent  *)ev;
    xhandler(event->getString());
    }
  }
void ClientSocket::executeAction()
  {if(abort)return;
  QString str =  readLine();
  if(str.size() == 0){serverReady();return;}// never happens
  ++line;
  if(str.at(0) == '#')
      {writeClientEvent("!"+str);
      serverReady();
      }
  else if(str.at(0) == '!')
      {writeClientEvent(":END OF FILE");
      writeClientEvent("!!");
      }
  else if(str.at(0) == '|')
      {writeClientEvent(":QUIT");
      writeClientEvent("!|");
      }
  else 
      xhandler(str);
  }
void ClientSocket::serverReady()
  {serverReadyEvent *event = new serverReadyEvent(); 
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClientEvent(QString str)
  {writeEvent *event = new writeEvent(str);
  //cout<<"event:"<<(const char*)str<<endl;
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClientEvent(char * buf,uint size)
  {writeBufEvent *event = new writeBufEvent(buf,size);
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClient(QString str)
  {QWriteLocker locker(&lock);
  QString t = str+'\n'; 
  clo.writeRawData(t.toAscii(),t.length()); 
  socket->waitForBytesWritten(-1); 
  //if(sdebug)Tprintf("->%s",(const char *)str);
  //cout<<"write:"<<(const char *)str<<" "<<currentThread()<<endl;
  }
void ClientSocket::writeClient(char * buff,uint size)
  {QWriteLocker locker(&lock);
  clo.writeBytes(buff,size);
  if(socket->state() != QAbstractSocket::ConnectedState)cout<<"Not connected"<<endl;
  socket->waitForBytesWritten(-1);  
  delete [] buff;
  }
uint ClientSocket::readBuffer(char * &buffer)
  {QReadLocker locker(&lock);
  quint32 size = 0; 
  uint nb = 0;
  while((nb = socket->bytesAvailable()) < (int)sizeof(quint32))
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setError(-1,"client not connected");return 0;}
      else socket->waitForReadyRead(10);
      }
  clo >>  size;
  if(size <= 0){setError(-1,"empty file");return 0;}

  buffer  = new char[size+1];
  char *pbuff = buffer;
  while((nb = socket->bytesAvailable()) < size)
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setError(-1,"client not connected");return 0;}
      else socket->waitForReadyRead(10);
      }
  clo.readRawData(pbuff,nb);
  return size;
  }
QString  ClientSocket::readLine()
  {QReadLocker locker(&lock);
  while(socket->state() != QAbstractSocket::ConnectedState || !socket->canReadLine())
      socket->waitForReadyRead(10);
  uint len;  char * buffer = NULL;
  clo.readBytes(buffer,len);
  QString str(buffer); str = str.trimmed();
  delete [] buffer;
  return str;
  }
void ClientSocket::xhandler(const QString& dataAction)
  {int pos = dataAction.indexOf(PARAM_SEP);
  QString beg = dataAction.left(pos);
  QString dataParam = dataAction.mid(pos+1);
  int action = mw->getActionInt(beg);
  if(sdebug)Tprintf("%s ",(const char *)dataAction.toAscii());
  // call the right handler
  // cout <<"line:"<<line<<" "<< (const char *)dataAction<<endl;
  if(action == 0)
      setError(UNKNOWN_COMMAND,"action:0 !!");
  else if(action > A_INFO && action < A_INFO_END)
      handlerInfo(action);
  else if(action > A_INPUT && action < A_INPUT_END)
      {if(action ==  A_INPUT_READ_GRAPH)
          {if(readServerGraph(dataParam) == 0)
              setError(-1,"Could not read graph");
          }
      else
          {clientEvent *event = new clientEvent(action,dataParam);
          QApplication::postEvent(this,event);
          }
      }
  else if(action > A_AUGMENT && action < A_TEST_END)
      {if(mw->graph_properties->actionAllowed(action))
          {mw->handler(action); // when finished pigale will write to the client
          return;
          }
      else 
          writeClientEvent(":ACTION NOT ALLOWED:"+mw->getActionString(action));
      }
  else if (action > A_PROP_DEFAULT && action < A_PROP_DEFAULT_END)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(fields.count() != 3)
          {setError(WRONG_PARAMETERS,"Bad number of parameters");
          writeClientEvent(":ERROR "+getErrorString()+"action: "+mw->getActionString(action));
          setError();
          writeClientEvent(QString("!%1 P").arg(action));
          serverReady();
          return;
          }
      bool ok =true;
      int setnum = fields[0].toInt(&ok); if (ok && (setnum<0 || setnum>2)) ok=false;
      if(!ok)setError(WRONG_PARAMETERS,"Wrong set number");
      PSet *set=(PSet *)0;
      switch(setnum) 
          {case 0:
              set=&mw->GC.Set(tvertex());
              break;
          case 1:
              set=&mw->GC.Set(tedge());
              break;
          case 2:
              set=&mw->GC.Set(tbrin());
              break;
          }
      int pnum=fields[1].toInt(&ok); if (ok && (pnum<0 || pnum > 255)) ok=false;
      if(!ok)setError(WRONG_PARAMETERS,"Wrong property number");
      switch(action)
         {case A_PROP_DEF_SHORT:
             {Prop<short> x(*set,pnum);
             x.definit(fields[2].toShort(&ok));
             }
             break;
         case A_PROP_DEF_INT:
             {Prop<int> x(*set,pnum);
             x.definit(fields[2].toInt(&ok));
             }
             break;
         default:
             setError( UNKNOWN_COMMAND,"unknown command");
             ok=true;
         }
      if(!ok)setError(WRONG_PARAMETERS,"Wrong second parameter");
      }
  else if(action > A_TRANS && action < A_TRANS_END)
      {if(action == A_TRANS_SEND_PNG || action ==  A_TRANS_SEND_PS)
          {int size = 500;
          QStringList fields = dataParam.split(PARAM_SEP);
          if(fields.count())
              {bool ok = true;
              size = fields[0].toInt(&ok);
              if(!ok)size = 500;
              }
          staticData::sizePng = size;
          mw->handler(action);return;
          }
      else if(action == A_TRANS_GET_CGRAPH) 
          // get a graph form client, read a record and display it
          {QStringList fields = dataParam.split(PARAM_SEP);
          int index = 1;
          bool ok =true;
          if(fields.count() > 1)index = fields[1].toInt(&ok);
          if(!ok)setError(WRONG_PARAMETERS,"Wrong parameters");
          else   readClientGraph(index);
          return;
          }
      else  if(action == A_TRANS_SEND_GRAPH_SAVE) 
          // save the graph and send it to the client
          {QStringList fields = dataParam.split(PARAM_SEP);
          if(fields.count() > 1)
              setError(WRONG_PARAMETERS,"Wrong parameters");
          else sendSaveGraph(fields[0]);
          }
      }
  else if(action > A_SET_GEN && action < A_SET_GEN_END)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(fields.count() < 1)
          {setError(WRONG_PARAMETERS,"Missing  parameter");
          writeClientEvent(":ERROR "+ getErrorString()+ "action:"+mw->getActionString(action));
          setError();
          writeClientEvent(QString("!%1 G").arg(action));
          serverReady();
          return;
          }
      bool ok =true;
      int value  = fields[0].toInt(&ok);
      if(!ok)setError(WRONG_PARAMETERS,"Wrong parameters");
      else
          switch(action)
              {case A_SET_GEN_N1:
                  //mw->Gen_N1 = value;
                  staticData::Gen_N1 = value;
                  break;
              case A_SET_GEN_N2:
                  staticData::Gen_N2 = value;
                  break;
              case A_SET_GEN_M:
                  staticData::Gen_M = value;
                  break;
              case A_SET_GEN_SEED:
                  randomSetSeed() = value;
		  break;
              default:
                  setError( UNKNOWN_COMMAND,"unknown command");
                  break;
              }
      }
  else if(action == SERVER_DEBUG)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(fields.count() != 1)
          setError(WRONG_PARAMETERS,"Bad number of parameters");
      else
          {bool ok =true;
          int setnum = fields[0].toInt(&ok); 
          if(!ok)setError(WRONG_PARAMETERS,"Wrong parameter");
          else sdebug = setnum;
          }
      }
  else
      setError( UNKNOWN_COMMAND,"unknown command");
 
  if(getError())
      {if(action)
          writeClientEvent(":ERROR "+ getErrorString()+" action: "+mw->getActionString(action));
      else
          writeClientEvent(":ERROR "+ getErrorString()); 
      writeClientEvent("!!"); // close 
      setError();
      return;
      }
  writeClientEvent(QString("!%1 C").arg(mw->getActionString(action)));
  serverReady();
  }
void ClientSocket::readClientGraph(int indexRemoteGraph)
  {char *buffer = NULL;
  uint size = readBuffer(buffer);
  if(getError()){delete [] buffer;return;}
  QString  GraphFileName;
  GraphFileName.sprintf("/tmp/graph%d.tmp",mw->ServerClientId);
  GraphFileName = universalFileName(GraphFileName);
  QFile file(GraphFileName);  //file.remove();
  file.open(QIODevice::WriteOnly  | QIODevice::Truncate);
  QDataStream stream(&file);
  stream.writeRawData(buffer,size);
  file.close();
  mw->InputFileName = GraphFileName;
  mw->GraphIndex1 = indexRemoteGraph;
  mw->handler(A_TRANS_GET_CGRAPH);
  }
int ClientSocket::readServerGraph(QString &dataParam)
  {QStringList fields = dataParam.split(PARAM_SEP);
  int nfield = (int)fields.count();
  if(nfield == 0){setError(WRONG_PARAMETERS,"Wrong parameters");return -1;}
  bool ok = true;
  int num = 1;
  if(nfield > 1)
      {num = fields[1].toInt(&ok);
      if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return -1;}
      }
  mw->InputFileName = universalFileName(fields[0]);
  mw->GraphIndex1 = num;
  mw->handler(A_INPUT_READ_GRAPH);
  return 0;
  }
void ClientSocket::sendSaveGraph(const QString &FileName)
  {QString graphFileName = QString("/tmp/%1").arg(FileName);
  graphFileName = universalFileName(graphFileName);
  mw->publicSave(graphFileName);
  QFileInfo fi = QFileInfo(graphFileName);
  uint size = fi.size();
  if(size == 0)
      {setError(-1,"no graph file");return;}
  else if(sdebug)
      writeClientEvent(":SENDING:"+FileName);
  QFile file(graphFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!GRAPH;"+FileName);
  writeClientEvent(buff,size);
  file.remove();
  }
void ClientSocket::Png()
  {QString PngFileName =  QString("/tmp/server%1.png").arg(mw->ServerClientId);
  PngFileName = universalFileName(PngFileName);
  qApp->processEvents();
  uint size = QFileInfo(PngFileName).size();
  int retry = 0;
  Tprintf("png size:%d",size);
  // Only for windows !!!
  while((size = QFileInfo(PngFileName).size()) < 1000)
      {msleep(500);
      Tprintf("-> png size:%d retry:%d",size,retry);
      if(++retry > 20)break;
      }
  if(retry)Tprintf("---> png size:%d retry=%d",size,retry);
  if(size == 0)
      {writeClientEvent(QString(":ERROR:%1 not found").arg(PngFileName));
      writeClientEvent("!!");serverReady();return;
      }
  QFile file(PngFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!PNGREADY");
  writeClientEvent(buff,size);
  file.remove();
  serverReady();
  }
void ClientSocket::Ps()
  {QString PsFileName =  QString("/tmp/server%1.ps").arg(mw->ServerClientId);
  PsFileName = universalFileName(PsFileName);
  QFileInfo fi = QFileInfo(PsFileName);
  uint size = fi.size();
  if(size == 0)
      {writeClientEvent(":ERROR: NO PS FILE");
      writeClientEvent("!!");serverReady();return;
      }
  QFile file(PsFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!PSREADY");
  writeClientEvent(buff,size);
  file.remove();
  serverReady();
  }
void ClientSocket::handlerInput(int action,const QString& dataParam)
// actions graphiques -> called by event. les updates devraient etre faits par handler (id NewGraph)
  {QStringList fields = dataParam.split(PARAM_SEP);
  QString msg;
  int nfield = (int)fields.count();
  bool ok = true;
  //cout << "handlerInput:-"<<(const char*)dataParam<<"- "<<nfield<<" field"<<endl;
  switch(action)
      {case  A_INPUT_NEW_GRAPH:
          mw->NewGraph();
          break;
      case A_INPUT_NEW_VERTEX:
          {int n = 1;
          if(nfield > 0)
              {n = fields[0].toInt(&ok);
              if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return;}
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
          {if(nfield < 2){setError(WRONG_PARAMETERS,"need 2 vertices");return;}
          int v1 = fields[0].toInt(&ok);
          if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return;}
          int v2 = fields[1].toInt(&ok);
          if(!ok){setError(WRONG_PARAMETERS,"Wrong parameters");return;}
          TopologicalGraph G(mw->GC);
          if(v1 > G.nv() || v2 > G.nv() || v1 == v2)
              {setError(WRONG_PARAMETERS,"Wrong parameters");return;}
          G.NewEdge((tvertex)v1,(tvertex)v2);
          mw->postDrawG();mw->information();
          }
          break;
      default:
          setError( UNKNOWN_COMMAND,"unknown command");
          break;
      }
  }
void ClientSocket::handlerInfo(int action)
  {TopologicalGraph G(mw->GC);
  mw->graph_properties->updateMenu(false);
  mw->information();
  Graph_Properties *inf = mw->graph_properties;
  switch(action)
      {case A_INFO_N:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(G.nv()));
           break;
      case A_INFO_M:
            writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(G.ne()));
            break;
      case A_INFO_SIMPLE:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Simple()));
          break;
      case A_INFO_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Planar()));
          break;
      case A_INFO_OUTER_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->OuterPlanar()));
          break;
      case A_INFO_SERIE_PAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->SeriePlanar()));
          break;
      case A_INFO_MAX_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Triangulation()));
          break;
      case A_INFO_BIPAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Biparti()));
          break;
      case A_INFO_MAX_BIPAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->MaxBiparti()));
          break;
      case A_INFO_REGULIER:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Regular()));
          break;
      case A_INFO_CON1:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con1()));
          break;
      case A_INFO_CON2:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con2()));
          break;
      case A_INFO_CON3:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con3()));
          break;
      case A_INFO_MIN_D:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->DegreeMin()));
          break;
      case A_INFO_MAX_D:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->DegreeMax()));
          break;
      case A_INFO_COORD:
          {if(!G.Set(tvertex()).exist(PROP_COORD)) 
              {setError(WRONG_PARAMETERS,"NO COORDS");
              lock.unlock();
              return;
              }
          Prop<Tpoint> coord(G.Set(tvertex()),PROP_COORD);
          writeClientEvent(mw->getActionString(action)+":");
          for(tvertex v = 1;v <= G.nv();v++)
              writeClientEvent("?"+QString("%1 %2").arg(coord[v].x()).arg(coord[v].y()));
          }
          break;
      case A_INFO_VLABEL:
          {if(!G.Set(tvertex()).exist(PROP_LABEL))
              {setError(WRONG_PARAMETERS,"NO LABEL");
              return;
              }
          Prop<long> label(G.Set(tvertex()),PROP_LABEL);
          writeClientEvent("?"+mw->getActionString(action)+":");
          for(tvertex v = 1;v <= G.nv();v++)
              writeClientEvent("?"+QString("%1").arg(label[v]));
          }
          break;
      default:
          writeClientEvent("?"+mw->getActionString(action)+":unknown command");
          break;
      }
  }
