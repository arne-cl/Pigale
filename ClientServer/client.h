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
#include <qthread.h>
#include <qmutex.h>
#include <qmap.h>


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
  QSocket *socket;
  QTextStream cls;
  QTextView *infoText;
  QLineEdit *inputText;
  typedef QMap<QString, int> ActionMap;
  ActionMap mActions;
  bool debug;

  void initMap();
  void Client::Translate(QString str);
};

#endif 
