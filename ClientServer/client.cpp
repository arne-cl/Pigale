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

using namespace std;

Client::Client(const QString &host, Q_UINT16 port)
    :debug(false),numPng(0)
  {infoText = new QTextView( this );
  QHBox *hb1 = new QHBox( this );
  inputText = new QLineEdit( hb1 );
  QPushButton *send = new QPushButton("Send",hb1);
  QPushButton *close = new QPushButton("Close connection",this);
  QPushButton *quit = new QPushButton("Quit",this);
  connect(send,SIGNAL(clicked()),SLOT(sendToServer()));
  connect(close,SIGNAL(clicked()),SLOT(closeConnection()));
  connect(quit,SIGNAL(clicked()),SLOT(stop()));
  connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));
  // create the socket and connect various of its signals
  socket = new QSocket( this );
  cls.setDevice(socket);
  clo.setDevice(socket);
  connect(socket,SIGNAL(connected()),SLOT(socketConnected()));
  connect(socket,SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()));
  connect(socket,SIGNAL(readyRead()),SLOT(socketReadyRead()));
  connect(socket,SIGNAL(error(int)),SLOT(socketError(int)));
  // connect to the server
  infoText->append("Trying to connect to the server\n" );
  socket->connectToHost(host,port);
  }
void Client::stop()
  {terminate();wait();
  }
void Client::closeConnection()
  {socket->close();
  if(socket->state() == QSocket::Closing )
      connect(socket,SIGNAL(delayedCloseFinished()),SLOT(socketClosed()));
  else
      socketClosed();
  stop();
  }
void Client::sendToServer()
  {QString str = inputText->text();
  sendToServer(str);
  inputText->setText("");
  }
void Client::sendToServer(QString &str)
  {if(socket->state() != QSocket::Connected)return;
  //split str -> 1 command per line if not a comment
  if(str.at(0) == '#' || str.at(0) == '!')
      {cls << str << endl;return;}
  QStringList fields = QStringList::split(ACTION_SEP,str);
  for(int i = 0; i < (int)fields.count();i++)
      cls << fields[i].stripWhiteSpace() << endl;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.stripWhiteSpace();
      if(str.at(0) == ':')
	  infoText->append(str.mid(1));
      else if(str == "!PNG")// receiving a png image
	  {QString PngFile = QString("image%1.png").arg(++numPng);
	  QString m = QString("getting:%1").arg(PngFile);
	  infoText->append(m);
	  QFile file(PngFile);
	  file.open(IO_ReadWrite);
	  QDataStream stream(&file);
	  char *buff;
	  uint size;
	  clo.readBytes(buff,size);
	  stream.writeRawBytes(buff,size);
	  file.close();
	  delete [] buff;
	  infoText->append("got image");
	  }
      //else if(str.at(0) != '!')
      else 
	  cout << str << endl;
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
  if(socket->state() != QSocket::Connected)return;
  while(!stream.atEnd())
      {str = stream.readLine(); 
      QChar ch = str.at(0);
      if(ch == ':')
	  {QChar c = str.at(1);
	      switch(c)
		  {case '!':
		       str = "!";
		       emit threadSendToServer(str);
		       return;
		  case 'D':
		      debug = true;
		      break;
		  case 'd':
		      debug = false;
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

