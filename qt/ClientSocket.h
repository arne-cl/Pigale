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
//#include <stdlib.h>

#include <Pigale.h>
#include "MyWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>
 
class PigaleServer;

class ClientSocket : public QSocket
// created each time a client connects
{Q_OBJECT
public:
 ClientSocket(int sock,int nid,bool show,QObject *parent=0,const char *name=0);
 ClientSocket(int sock,MyWindow *p,PigaleServer *server,QObject *parent=0,const char *name=0);
  ~ClientSocket() { }
  

signals:
  void logText(const QString&);
  void signalId(int);

private slots:
  void readClient();
  void ClientClosed();

private: 
  void sendServerId();
  void writeServer(QString&  msg);
  int xhandler(const QString& data);
  int Png();
  int ReadRemoteGraph(QString &dataParam);
  int handlerInfo(int action);
  int handlerInput(int action,const QString& data);
  int line;
  int prId;
  int sdebug;
  QTextStream cli;
  QDataStream clo;
  MyWindow *mw; 
};

class PigaleServer : public QServerSocket
// create a ClientSocket each time a client connects
{Q_OBJECT
public:
  PigaleServer(MyWindow *mw,QObject* parent=0);
    ~PigaleServer() { }
  void newConnection(int socket);
  MyWindow *mw;
  ClientSocket *clientsocket;
  int nconnections;

public slots:
  void OneClientClosed();
};

#endif
