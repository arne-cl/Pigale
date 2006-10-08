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

#ifndef CLIENTSOCKET_H 
#define CLIENTSOCKET_H


#include <config.h>

#include "pigaleWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>
#include <QT/clientEvent.h> 

#include <QDataStream>
 
class PigaleServer : public QTcpServer
{Q_OBJECT
public:
  PigaleServer(pigaleWindow *mw);
  void createNewServer();
  int nconnections;
  pigaleWindow *mw;
 
public slots:
  void OneClientClosed();
  void OneClientOpened();
};

class ClientSocket : public QThread
{Q_OBJECT
public:
 ClientSocket(pigaleWindow *mw,QTcpSocket *socket);
  ~ClientSocket();
  void executeAction();
  bool event(QEvent * e);
  void customEvent(QEvent * e);
  void run();
  void runEvent(QEvent * ev);
  void writeClientEvent(QString str);
  void writeClientEvent(char * buf,uint size);
  void Png();
  void Ps();
  void serverReady();

private: 
  void writeClient(QString  str);
  void writeClient(char * buf,uint size);
  QString  readLine();
  void xhandler(const QString& data);
  void sendServerId();
  void sendSaveGraph(const QString &FileName);
  int readServerGraph(QString &dataParam);
  void readClientGraph(int indexRemoteGraph);
  void handlerInfo(int action);
  void handlerInput(int action,const QString& data);
  uint readBuffer(char  *  &buff);
  pigaleWindow *mw; 
  QMutex mutex;
  QEvent *ev0;
  QTcpSocket *socket;
  int socketDescriptor;
  PigaleServer *server;
  QDataStream clo;
  QReadWriteLock lock;
  bool abort,busy;
  int sdebug,line;
};


#endif
