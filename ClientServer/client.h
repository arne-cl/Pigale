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

#ifndef CLIENT_H 
#define CLIENT_H
#include <config.h>
#include <qsocket.h>
#include <qapplication.h>
#include <qevent.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qthread.h>
#include <qmutex.h>
#include <QT/clientEvent.h> 
#include <QT/Action_def.h>
#include <unistd.h>

#define GCC_VERSION (__GNUC__ * 10000 \
                              + __GNUC_MINOR__ * 100 \
                              + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >=  30000
#include <iostream>
#else
#include <iostream.h>
#endif

/* 
In the input:
- lines starting by '#' are treated as comments
- lines starting by ':' are treated by the client
- line  starting by ':!' signals the end of file
- line  starting by ':D' signals the client to echo the comments
- line  starting by ':d' signals the client not to echo the comments

- otherwise a line contains commands  separated by ':'
- commands may conTains arguments separated by ';'

When reading from the server
- lines starting with ! are commands
- lines starting by : are diplayed in the text window
otherwise they are output to the terminal 
*/

class Client;

class threadRead : public QThread 
{
public:
  virtual void run();
  
  Client* pclient; 
};

class Client : public QVBox 
{Q_OBJECT
public:
  Client( const QString &host, Q_UINT16 port);
  ~Client(){}
  void sendToServer(QString& str);
  void writeToClient(QString str);
  void customEvent(QCustomEvent * e );

private slots:
  void closeConnection();
  void sendToServer();
  void socketReadyRead();
  void socketConnected();
  void socketConnectionClosed();
  void socketClosed();
  void socketError(int e);
  void stop();

public:
  QSocket *socket;
  int ActionsToDo;
  bool dbg;
  int ChangeActionsToDo(int delta);

private:
  int sendToServerGraph(QString &str);
  QMutex mutex;
  QTextStream cls;
  QDataStream clo;
  QTextView *infoText;
  QLineEdit *inputText;
  threadRead ThreadRead;
  int numPng;

public:
  bool debug() {bool b; mutex.lock(); b=dbg; mutex.unlock(); return b;}
  void debug(bool b) {mutex.lock(); dbg=b; mutex.unlock();}
  
};


#endif 
