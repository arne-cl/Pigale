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

//class Client : public QVBox
Client::Client(const QString &host, Q_UINT16 port)
    :dbg(false),numPng(0)
  {
  infoText = new QTextView( this );
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
  inputText->setFocus();
  }
int Client::ChangeActionsToDo(int delta)
  {int i;
  mutex.lock();
  i=(ActionsToDo+=delta);
  mutex.unlock();
  return i;
  }
void Client::socketConnected()
  {infoText->append("Connected to server");
  ActionsToDo = 1; // wait server ready
  ThreadRead.pclient = this;
  ThreadRead.start();
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
  {textEvent *e = new textEvent(str);
  QApplication::postEvent(this,e);
  //qApp->processEvents (); 
  }
void Client:: customEvent(QCustomEvent * e ) 
  {if( e->type() != TEXTEVENT ) return;
  textEvent *event  =  (textEvent  *)e;
  infoText->append(event->getString());
  }
void Client::stop()
  {ThreadRead.terminate();ThreadRead.wait();
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
  }
void Client::sendToServer(QString &str)
  {if(socket->state() != QSocket::Connected)
      {writeToClient(QString("state:%1").arg(socket->state()));return;}
  //split str -> 1 command per line if not a comment
  if(str.at(0) == '#' || str.at(0) == '!')
      {cls << str << endl;
      ChangeActionsToDo(1);
      return;
      }
  QStringList fields = QStringList::split(ACTION_SEP,str);
  for(int i = 0; i < (int)fields.count();i++)
      {fields[i].stripWhiteSpace();
      if(fields[i].contains("RC_GRAPH",true))
          sendToServerGraph(fields[i]);
      else
          cls << fields[i] << endl;
      ChangeActionsToDo(1);
      }
  }
int Client::sendToServerGraph(QString &data)
  {QStringList fields = QStringList::split(PARAM_SEP,data);
  if(fields.count() < 1)return -1;
  QString GraphFileName = fields[1].stripWhiteSpace();
  QFileInfo fi = QFileInfo(GraphFileName);
  if(GraphFileName.isEmpty())
      {writeToClient(QString("NO FILE:%1").arg(GraphFileName));
      return -1;
      }
  uint size = fi.size();
  if(size && debug())
      writeToClient(QString("SENDING:%1").arg(GraphFileName));
  else if(!size)
      {writeToClient("empty file");
      return -1;
      }
  cls << data << endl;
  QFile file(GraphFileName);
  file.open(IO_ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawBytes(buff,size); 
  clo.writeBytes(buff,size);
  if(debug())writeToClient(QString("SENT:%1 bytes").arg(size));
  delete [] buff;
  return 0;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.stripWhiteSpace();
      if(str.at(0) == ':')
          writeToClient(str.mid(1));
      else if(str == "!PNG")// receiving a png image
          {ChangeActionsToDo(1);
          QString PngFile = QString("image%1.png").arg(++numPng);
          QFile file(PngFile);
          file.open(IO_ReadWrite);
          QDataStream stream(&file);
          uint size;
          while(socket->bytesAvailable() < 4)
              {socket->waitForMore(100);writeToClient("+");} 
          clo >> size;
          char *buff = new char[size+1];
          Q_ULONG  nb;
          int i = 1;
          Q_ULONG nread = 0;
          char *pbuff = buff;
          while(nread  < size)
              {nb = socket->bytesAvailable();
              nread += nb;
              clo.readRawBytes(pbuff,nb);
              pbuff += nb;
              socket->waitForMore(100);  // in millisec
              if(++i > 50)return ;
              }
          stream.writeRawBytes(buff,size);
          file.close();
          delete [] buff;
          writeToClient(QString("GOT:%1").arg(PngFile));
          ChangeActionsToDo(-1);
          }
      else if(str.at(0) == '!')//server has finished
          ChangeActionsToDo(-1);
      else 
          writeToClient(str);
      }
  }

void threadRead::run() 
// read datas from stdin
  {QTextStream stream(stdin,IO_ReadWrite);
  QString str;
  //if(pclient->socket->state() != QSocket::Connected)return;
  while(!stream.atEnd())
      { int i = 0;
      while(pclient->ChangeActionsToDo(0))
          {msleep(10);// milliseconds
          if(++i %100 == 0)
              pclient-> writeToClient(QString("Waiting %1s (%2)").arg(i/100).arg(pclient->ChangeActionsToDo(0)));
          }
      str = stream.readLine(); 
      QChar ch = str.at(0);
      if(ch == ':')
          {QChar c = str.at(1);
          switch(c)
              {case '!':
                  str = "!";
                  pclient->sendToServer(str);
                  return;
              case 'D':
                  pclient->debug(true);
                  break;
              case 'd':
                  pclient->debug(false);
                  break;
              default:
                  break;
              }
          }
      else if(pclient->debug() || ch != '#')
          pclient->sendToServer(str);
      }
  }

int main( int argc, char** argv )
  {QApplication app( argc, argv );
  Client client(argc<2 ? "localhost" : argv[1], 4242);
  app.setMainWidget(&client);
  client.show();
  return app.exec();
  }

