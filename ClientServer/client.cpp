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


#include "client.h"
#include <QT/Action_def.h>
#include <QT/Action.h>

/* 
If the input is a file:
- lines starting by '#' are treated as comments
- lines starting by ':' are treated by the client
- line  starting by ':!' signals the end of file
- line  starting by ':D' signals the client to echo the commands
- line  starting by ':d' signals the client not to echo the commands

- otherwise a line contains commands  separated by ':'
- commands may contains arguments separated by ';'
*/

Client::Client(const QString &host, Q_UINT16 port):debug(false),translate(false)
  {infoText = new QTextView( this );
  QHBox *hb1 = new QHBox( this );
  inputText = new QLineEdit( hb1 );
  QPushButton *send = new QPushButton("Send",hb1);
  QPushButton *close = new QPushButton("Close connection",this);
  QPushButton *quit = new QPushButton("Quit",this);
  connect( send, SIGNAL(clicked()), SLOT(sendToServer()) );
  connect( close, SIGNAL(clicked()), SLOT(closeConnection()) );
  connect( quit, SIGNAL(clicked()),  SLOT(stop()) );
  connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
  // create the socket and connect various of its signals
  socket = new QSocket( this );
  connect(socket, SIGNAL(connected()),SLOT(socketConnected()) );
  connect(socket, SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()) );
  connect(socket, SIGNAL(readyRead()),SLOT(socketReadyRead()) );
  connect(socket, SIGNAL(error(int)),SLOT(socketError(int)) );
  // create mapActions
  initMap();
  // connect to the server
  infoText->append("Trying to connect to the server\n" );
  socket->connectToHost(host,port);
  }
void Client::initMap()
  {// not used now: would be if ones prefers alpha commands
  int na = (int)(sizeof(actions)/sizeof(_Action));
  for(int i = 0;i < na;i++)
      mActions[actions[i].name] = actions[i].num;
  }
void Client::stop()
  {terminate();wait();
  }
void Client::closeConnection()
  {socket->close();
  if(socket->state() == QSocket::Closing )
      connect( socket, SIGNAL(delayedCloseFinished()), SLOT(socketClosed()) );
  else
      socketClosed();
  stop();
  }
void Client::sendToServer()
  {if(socket->state() != QSocket::Connected)return;
  QTextStream os(socket);
  os << inputText->text() << "\n";
  inputText->setText( "" );
  }
void Client::sendToServer(QString &str)
  {if(socket->state() != QSocket::Connected)return;
  QTextStream os(socket);
  //split str -> 1 command per line if not a comment
  if(str.at(0) == '#'){os <<str << "\n";return;}
  QStringList fields = QStringList::split(ACTION_SEP,str);
  for(int i = 0; i < (int)fields.count();i++)
      {if(translate)Translate(fields[i]);
      else os <<fields[i] << "\n";
      }
  }
void Client::Translate(QString &str)
  {int pos = str.find(';');
  QString str_action = str.left(pos);
  int action = mActions[str_action];
  if(!action)
      {cout <<"# ERREUR '"<<str <<"' -> "<<"UNKNOWN ACTION" << endl;return;}
  QTextStream os(socket);
  os << action<<str.mid(pos)<<endl;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      if(str.at(0) == ':')
	  infoText->append(str.mid(1));
      else if(str.at(0) != '!')
	  cout << str;
      }
  }
void Client::socketConnected()
  {infoText->append("Connected to server\n");
  connect(this,SIGNAL(threadSendToServer(QString&)),SLOT(sendToServer(QString&)));
  connect(this,SIGNAL(threadWriteToClient(QString&)),SLOT(writeToClient(QString&)));
  start();
  }
void Client::socketConnectionClosed()
  {infoText->append("Connection closed by the server\n");
  stop();
  }
void Client::socketClosed()
  {infoText->append("Connection closed\n");
  }
void Client::socketError(int e)
  {infoText->append(QString("Error number %1 occurred\n").arg(e));
  }
void Client::writeToClient(QString & str)
  {cout << str << endl;
  }
void Client::run()
// read datas from stdin
  {QTextStream stream(stdin,IO_ReadWrite);
  QString str;
  
  while(!stream.atEnd())
      {str = stream.readLine(); 
      QChar ch = str.at(0);
 
      if(ch == ':')
	  {QChar c = str.at(1);
	      switch(c)
		  {case '!':
		       infoText->append("EndOfFile");
		       return;
		  case 'D':
		      debug = true;
		      break;
		  case 'd':
		      debug = false;
		      break;
		  case 'T':
		      translate = true;
		      break;
		  case 't':
		      translate = false;
		      break;
		  default:
		      break;
		  }
	  }
      else if(debug || ch != '#')
	  {emit threadSendToServer(str);
	  continue;
	  }
      }
  }

int main( int argc, char** argv )
  {QApplication app( argc, argv );
  Client client(argc<2 ? "localhost" : argv[1], 4242);
  app.setMainWidget(&client);
  client.show();
  return app.exec();
  }

