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
    :ActionsToDo(0),dbg(false),numPng(0)
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
  i=(ActionsToDo += delta);
  mutex.unlock();
  if(delta < 0)
      {if(debug()) writeToClient("!"+*stack.top());
      stack.pop(); 
      }
  else if(debug() && !stack.isEmpty() && delta > 0 )
      writeToClient(QString("%1").arg(*stack.top()));
  return i;
  }
void Client::socketConnected()
  {infoText->append("Connected to server");
  ActionsToDo = 1; // wait server answers
  QString *txt  =  new QString("WAIT_SERVER");
  stack.push(txt);
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
  {if(e == QSocket::ErrConnectionRefused)
      infoText->append(QString("Connection refused\n"));
  else if(e  == QSocket::ErrHostNotFound) 
      infoText->append(QString("Host not found\n"));           
  else
      infoText->append(QString("A read from the socket failed \n"));
  }

void Client::writeToClient(QString str)
  {textEvent *e = new textEvent(str);
  QApplication::postEvent(this,e);
  }

void Client:: customEvent(QCustomEvent * e ) 
  {if( e->type() != TEXT_EVENT ) return;
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
  if(str.at(0) == '#' || str.at(0) == '!') //split str -> 1 command per line if not a comment
      {cls << str << endl;
      QString *txt  =  new QString(str);
      stack.push(txt);
      ChangeActionsToDo(1);
      return;
      }
  QStringList fields = QStringList::split(ACTION_SEP,str);
  for(int i = 0; i < (int)fields.count();i++)
      {fields[i].stripWhiteSpace();
      if(fields[i].contains("RC_GRAPH",true))
          sendToServerGraph(fields[i]);
      else
          {cls << fields[i] << endl;
          QString *txt  =  new QString(fields[i]);
          stack.push(txt);
          ChangeActionsToDo(1);
          //if(debug())writeToClient(QString(" action:%1 -%2-").arg(ActionsToDo).arg( fields[i]));
          }
      }
  }

int Client::sendToServerGraph(QString &data)
  {QStringList fields = QStringList::split(PARAM_SEP,data);
  if(fields.count() < 2){writeToClient("MISSING ARGUMENT");return -1;}
  QString GraphFileName = fields[1].stripWhiteSpace();
  QFileInfo fi = QFileInfo(GraphFileName);
  if(GraphFileName.isEmpty() || !fi.isFile() || !fi.size())
      {writeToClient(QString("NO FILE:%1").arg(GraphFileName));
      return -1;
      }
  QString *txt  =  new QString("RC_GRAPH");
  stack.push(txt);
  ChangeActionsToDo(1);
  uint size = fi.size();
  if(debug()) writeToClient(QString("SENDING:%1 %2 bytes").arg(GraphFileName).arg(size));
  cls << data << endl;
  QFile file(GraphFileName);
  file.open(IO_ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawBytes(buff,size); 
  clo.writeBytes(buff,size);
  delete [] buff;
  return 0;
  }
uint Client::readBuffer(char  *  &buffer)
  {uint size;
  while(socket->bytesAvailable() < 4)socket->waitForMore(10); 
  clo >> size;
  buffer = new char[size+1];
  char *pbuff = buffer;
  int retry = 0;
  uint nread = 0;
  uint size0 = 0;
  uint nb;
  while(nread  < size)
      {socket->waitForMore(10);  // in millisec
      nb = socket->bytesAvailable();
      if(nb == 0)
          {if(++retry > 1000){ writeToClient("TIMEOUT");ChangeActionsToDo(-1);return 0;}
          continue;
          }
      retry = 0;
      if(nb > size-nread)nb = size-nread;
      nread += nb;
      clo.readRawBytes(pbuff,nb);
      pbuff += nb;
      if(nread >= size0 && debug())
          {int percent = (int)(nread*100./size + .5);
          size0 = nread + size/10; // we write when at least 10% more  is read
          writeToClient(QString("%1 % (%2 / %3)").arg(percent).arg(nread).arg(size));
          }
      }
  return size;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.stripWhiteSpace();
      if(str.at(0) == ':')
          writeToClient(str.mid(1));
      else if(str == "!PNG")// receiving a png image
          {QString *txt  =  new QString("GET_PNG");
          stack.push(txt);
          ChangeActionsToDo(1);
          char * buffer = NULL;
          uint size = readBuffer(buffer);
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PngFile = QString("image%1.png").arg(++numPng);
          QFile file(PngFile);          file.open(IO_ReadWrite);
          QDataStream stream(&file);
          stream.writeRawBytes(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeToClient(QString("GOT:%1").arg(PngFile));
          ChangeActionsToDo(-1);
          }
      else if(str == "!!")// server ha finished everything
          {ChangeActionsToDo(-1);
          closeConnection();
          writeToClient("Connection Closed");
          }
      else if(str.at(0) == '!')//server has finished one action
          ChangeActionsToDo(-1);
      else 
          writeToClient(str);
      }
  }

void threadRead::run() 
// read datas from stdin
  {QTextStream stream(stdin,IO_ReadWrite);
  QString str;
  while(!stream.atEnd())
      { int retry = 0;
      while(pclient->ChangeActionsToDo(0) > 0)
          {msleep(10);// milliseconds
          if(++retry %100 == 0 && pclient->debug())
              {QString action = *pclient->stack.top(); 
              pclient-> writeToClient(QString("Waiting %1s (%2)").arg(retry/100).arg(action));
              }
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

