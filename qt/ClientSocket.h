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
#include <qsocket.h>
#include <qserversocket.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qtextview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qthread.h>

#include <Pigale.h>
#include "pigaleWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>
 

class PigaleServer;
 

class ClientSocket : public QSocket, QThread
// created each time a client connects
{Q_OBJECT
public:
 ClientSocket(int sock,int nid,bool show,QObject *parent=0,const char *name=0);
 ClientSocket(int sock,pigaleWindow *p,PigaleServer *server,QObject *parent=0,const char *name=0);
  ~ClientSocket() { }
  

signals:
  void logText(const QString&);
  void signalId(int);
  void threadDataAction(const QString&);

private slots:
  void readClient();
  void ClientClosed();
  int xhandler(const QString& data);
  void writeServer(const QString&  msg);

private: 
  void run();
  void sendServerId();
  int Png();
  int ReadRemoteGraph(QString &dataParam);
  int GetRemoteGraph();
  int handlerInfo(int action);
  int handlerInput(int action,const QString& data);
  int line;
  int prId;
  int sdebug;
  bool getRemoteGraph;
  int indexRemoteGraph;
  QTextStream cli;
  QDataStream clo;
  pigaleWindow *mw; 
};

class PigaleServer : public QServerSocket
// create a ClientSocket each time a client connects
{Q_OBJECT
public:
  PigaleServer(pigaleWindow *mw,QObject* parent=0);
    ~PigaleServer() { }
  void newConnection(int socket);
  pigaleWindow *mw;
  ClientSocket *clientsocket;
  int nconnections;

public slots:
  void OneClientClosed();
};

#endif
