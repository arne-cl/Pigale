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
#include <qmutex.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qthread.h>
#include <qreadwritelock.h>
#include <qevent.h>
#include <qtextstream.h>
#include <QtNetwork>
#include <Pigale.h>
#include "pigaleWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>
#include <QT/clientEvent.h> 

class PigaleServer : public QTcpServer
{Q_OBJECT
public:
  PigaleServer(pigaleWindow *mw);
  ~PigaleServer() { }
  int nconnections;
  pigaleWindow *mw;
 
protected:
  void incomingConnection(int socketDescriptor);

public slots:
  void OneClientClosed();
};

class ClientSocket : public QThread
{Q_OBJECT
public:
 ClientSocket(pigaleWindow *p,PigaleServer *server,int socketDescriptor);
  ~ClientSocket();
  void customEvent(QEvent * e);
  bool event(QEvent * e);
  void run();
  void writeClientEvent(QString str);
  void writeClientEvent(char * buf,uint size);
  void writeClient(QString  str);
  void writeClient(char * buf,uint size);
  void lockMutex();
  void unlockMutex();
  QString  readLine();
  void Png();
  void Ps();

signals:
  void logText(const QString&);
  void signalId(int);
  void error(int socketError, const QString &message);

private slots:
  void readClient();
  void ClientClosed();
  void writeServer(const QString&  msg);
  void errorReport(int socketError, const QString & message);

private: 
  void xhandler(const QString& data);
  void sendServerId();

  void sendSaveGraph(const QString &FileName);
  int readServerGraph(QString &dataParam);
  void readClientGraph(int indexRemoteGraph);
  void handlerInfo(int action);
  void handlerInput(int action,const QString& data);
  uint readBuffer(char  *  &buff);

  QTcpSocket *socket;
  QDataStream clo;
  QReadWriteLock lock;
  QMutex mutex;
  QWaitCondition condition;
  bool restart;
  bool abort;
  bool Reading;
  pigaleWindow *mw; 
  int prId;
  int sdebug;
  int line;
};


#endif
