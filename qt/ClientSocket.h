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
#include <qmutex.h>
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
#include <QT/clientEvent.h> 


class PigaleServer;

class ClientSocket : public QSocket, QThread
// created each time a client connects
{Q_OBJECT
public:
  ClientSocket(int sock,pigaleWindow *p,PigaleServer *server,QObject *parent=0,const char *name=0);
  ~ClientSocket() { }
  void customEvent(QCustomEvent * e );

signals:
  void logText(const QString&);
  void signalId(int);

private slots:
  void readClient();
  void ClientClosed();
  void writeServer(const QString&  msg);

private: 
  void xhandler(const QString& data);
  void run();
  void sendServerId();
  void Png();
  void  sendSaveGraph(const QString &FileName);
  void readServerGraph(QString &dataParam);
  void readClientGraph(int indexRemoteGraph);
  void handlerInfo(int action);
  void handlerInput(int action,const QString& data);
  uint readBuffer(char  *  &buff);
  int prId;
  int sdebug;
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
