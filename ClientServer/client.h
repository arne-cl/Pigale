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

#include <QT/Action_def.h>
#include <qsocket.h>
#include <qapplication.h>
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
- commands may contains arguments separated by ';'

When reading from the server
- lines starting with ! are commands
- lines starting by : are diplayed in the text window
otherwise they are output to the terminal 
*/

class Client : public QVBox, QThread
{Q_OBJECT
public:
  Client( const QString &host, Q_UINT16 port);
  void run(); 
  ~Client(){}

signals:
  void threadSendToServer(QString &str);
  void threadWriteToClient(QString &str);

private slots:
  void closeConnection();
  void sendToServer();
  void sendToServer(QString& str);
  void socketReadyRead();
  void socketConnected();
  void socketConnectionClosed();
  void socketClosed();
  void socketError( int e );
  void writeToClient(QString& str);
  void stop();

private:
  int sendToServerGraph(QString &str);
  QSocket *socket;
  QTextStream cls;
  QDataStream clo;
  QTextView *infoText;
  QLineEdit *inputText;
  bool ActionTreated;
  bool debug;
  int numPng;
};

#endif 
