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

#ifndef XSERVER_H 
#define XSERVER_H

#include <qapplication.h>
#include <qvbox.h>
#include "ClientSocket.h"

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
