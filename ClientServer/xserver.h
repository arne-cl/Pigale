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

#ifndef SERVER_H 
#define SERVER_H

#include <qsocket.h>
#include <qserversocket.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qtextview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <stdlib.h>

#include <Pigale.h>
#include "MyWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>


class ClientSocket : public QSocket
// created each time a client connects
{Q_OBJECT
public:
 ClientSocket(int sock,int nid,bool show,QObject *parent=0,const char *name=0);
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
  int parseSplitAction(const QString& data);
  int xhandler(const QString& data);
  int handlerInfo(int action);
  int handlerInput(int action,const QString& data);
  int line;
  int prId;
  int sdebug;
  QTextStream cli;
  MyWindow *mw; 
};

class SimpleServer : public QServerSocket
// create a ClientSocket each time a client connects
{Q_OBJECT
public:
  SimpleServer(QObject* parent=0);
    ~SimpleServer() { }
  int id;
  bool show;
  void newConnection(int socket);
public slots:
  void display();
signals:
  void newConnect(ClientSocket*);
};

class ServerInfo : public QVBox
// create the SimpleServer
{Q_OBJECT
public:
  ServerInfo();
  ~ServerInfo() {}

private slots:
  void newConnect( ClientSocket *s );
  void connectionClosed();

private:
  QTextView *infoText;
};

#endif
