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

//class Client : public QVBox, QThread
Client::Client(const QString &host, Q_UINT16 port)
    :ActionTreated(false),debug(false),numPng(0)
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
  connect(this,SIGNAL(WriteToClient(QString)),SLOT(writeToClient(QString)));
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
  //emit WriteToClient("Trying to connect to the server");
  socket->connectToHost(host,port);
  }
void Client::socketConnected()
  {connect(this,SIGNAL(threadSendToServer(QString&)),SLOT(sendToServer(QString&)));
  start();
  infoText->append("Connected to server");
  //emit WriteToClient("Connected to server");
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
void Client::writeToClient(QString str)
  {//infoText->append(str);
  cout << str << endl;
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
  {if(socket->state() != QSocket::Connected)return;
  QString str = inputText->text();
  sendToServer(str);
  inputText->setText("");
  emit WriteToClient("");
  }
void Client::sendToServer(QString &str)
  {if(socket->state() != QSocket::Connected)
      {qDebug("state:%d",socket->state());return;}
  //else qDebug("client:%s",(const char *)str);
  //split str -> 1 command per line if not a comment
  if(str.at(0) == '#' || str.at(0) == '!')
      {cls << str << endl;
      ActionTreated = false;
      return;
      }
  QStringList fields = QStringList::split(ACTION_SEP,str);
  for(int i = 0; i < (int)fields.count();i++)
      {fields[i].stripWhiteSpace();
      if(fields[i].contains("RC_GRAPH",true))
	  sendToServerGraph(fields[i]);
      else
	  cls << fields[i] << endl;
      ActionTreated = false;
      }
  }
int Client::sendToServerGraph(QString &data)
  {QStringList fields = QStringList::split(PARAM_SEP,data);
  if(fields.count() < 1)return -1;
  QString GraphFileName = fields[1].stripWhiteSpace();
  QFileInfo fi = QFileInfo(GraphFileName);
  if(GraphFileName.isEmpty())
      {emit WriteToClient(QString("NO FILE:%1").arg(GraphFileName));
      return -1;
      }
  uint size = fi.size();
  if(size)
      emit WriteToClient(QString("SENDING:%1").arg(GraphFileName));
  else 
      {emit WriteToClient("empty file");return -1;}
  
  cls << data << endl;
  QFile file(GraphFileName);
  file.open(IO_ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawBytes(buff,size); 
//   clo << size;   clo.writeRawBytes(buff,size);
  clo.writeBytes(buff,size);
  emit WriteToClient(QString("SENT:%1 bytes").arg(size));
  delete [] buff;
  return 0;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.stripWhiteSpace();
      if(str.at(0) == ':')
	  //infoText->append(str.mid(1));
	  emit WriteToClient(str.mid(1));
      else if(str == "!PNG")// receiving a png image
	  {ActionTreated = false;
	  QString PngFile = QString("image%1.png").arg(++numPng);
	  emit WriteToClient(QString("GETTING:%1").arg(PngFile));
	  QFile file(PngFile);
	  file.open(IO_ReadWrite);
	  QDataStream stream(&file);
	  char *buff;
	  uint size;
// 	  while(socket->bytesAvailable() < 4){socket->waitForMore(100);qDebug(".");} 
// 	  clo >> size;
// 	  char *buff = new char[size+1];
// 	  Q_ULONG  nb;
// 	  while((nb = socket->bytesAvailable()) < size)
// 	      {socket->waitForMore(100);qDebug(". %ld",nb);}
// 	  clo.readRawBytes(buff,size);
	  clo.readBytes(buff,size);
	  stream.writeRawBytes(buff,size);
	  file.close();
	  delete [] buff;
	  //emit WriteToClient("END PNG");
	  ActionTreated = true;
	  }
      else if(str.at(0) == '!')//server has finished
	  ActionTreated = true;
      else 
	  cout << str << endl;
      }
  }

void Client::run() 
// read datas from stdin
  {QTextStream stream(stdin,IO_ReadWrite);
  usleep(500000);// sleep.5s
  QString str;
  if(socket->state() != QSocket::Connected)return;
  int i = 0;
  while(!stream.atEnd())
      {i = 0;
      while(!ActionTreated)
	  {usleep(1000);// microseconds
	  if(++i%1000 == 0)cout << "." << endl;
	  }
      str = stream.readLine(); 
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

