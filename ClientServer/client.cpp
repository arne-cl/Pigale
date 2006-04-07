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
#include <qtextstream.h>
#include <qevent.h>

using namespace std;

QString  universalFileName(QString const & fileName)
  {QString filename = fileName;
  filename.replace('/', QDir::separator());
  return filename;
  }

//Client::Client(const QString &host, Q_UINT16 port)
Client::Client(const QString &host, quint16 port)
  :ActionsToDo(0),dbg(false),numFiles(0)
  {infoText = new QTextEdit( this );
  inputText = new QLineEdit( this );
  QPushButton *send = new QPushButton("Send",this);
  QPushButton *close = new QPushButton("Close connection",this);
  QPushButton *quit = new QPushButton("Quit",this);
  QVBoxLayout *layoutV = new QVBoxLayout;
  setLayout(layoutV);
  layoutV->addWidget(infoText);
  QHBoxLayout *layoutH = new QHBoxLayout();
  layoutH->addWidget(inputText);
  layoutH->addWidget(send);
  layoutV->addLayout(layoutH);
  layoutV->addWidget(close);
  layoutV->addWidget(quit);
  connect(send,SIGNAL(clicked()),SLOT(sendToServer()));
  connect(close,SIGNAL(clicked()),SLOT(closeConnection()));
  connect(quit,SIGNAL(clicked()),SLOT(stop()));
  connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));
  // create the socket and connect various of its signals
  socket = new QTcpSocket(this);
  socket->connectToHost(host,port);
  clo.setDevice(socket); clo.setVersion(QDataStream::Qt_4_0);
  connect(socket,SIGNAL(connected()),SLOT(socketConnected()));
  connect(socket,SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()));
  connect(socket,SIGNAL(readyRead()),SLOT(socketReadyRead()));
  connect(socket,SIGNAL(error(int)),SLOT(socketError(int)));
  // connect to the server
  infoText->append("Trying to connect to the server" );
  inputText->setFocus();
  }
void Client::stop()
  {ThreadRead.terminate();ThreadRead.wait();
  }
void Client::exit()
  {qApp->quit();
  } 
void Client::socketConnected()
  {infoText->append("Connected to server");
  stack.push("WAIT_SERVER");
  ChangeActionsToDo(1);
  ThreadRead.pclient = this;
  ThreadRead.start();
  }
void Client::socketConnectionClosed()
  {infoText->append("Connection closed by the server");
  stop();
  }
void Client::socketClosed()
  {//infoText->append("Connection closed");
  }
void Client::closeConnection()
  {socket->close();
   if(socket->state() == QAbstractSocket::ClosingState )
       connect(socket,SIGNAL(delayedCloseFinished()),SLOT(socketClosed()));
   else
       socketClosed();
  stop();
  }
void Client::socketError(int e)
  {if(e == QAbstractSocket::ConnectionRefusedError)
      infoText->append(QString("Connection refused"));
  else if(e  == QAbstractSocket::HostNotFoundError) 
      infoText->append(QString("Host not found"));           
  else
      infoText->append(QString("A read from the socket failed"));
  }
bool Client::event(QEvent * ev)
  {if(ev->type() >=  QEvent::User)
      {customEvent(ev);
      return TRUE;
      }
  return FALSE;
  }
void Client:: customEvent(QEvent * e ) 
  {if( e->type() != (int)TEXT_EVENT ) return;
  textEvent *event  =  (textEvent  *)e;
  infoText->append(event->getString());
  infoText->ensureCursorVisible();
  }
void Client::writeClient(QString str)
  {textEvent *e = new textEvent(str);
  QApplication::postEvent(this,e);
  }
void Client::writeServer(QString str)
  {QWriteLocker locker(&lock);
  QString t = str+'\n';
  clo.writeBytes(t.toAscii(),t.length()); 
  socket->waitForBytesWritten(-1); 
  }
void Client::writeServer(char * buff,quint32 size)
  {QWriteLocker locker(&lock);
  clo.writeBytes(buff,size);
  socket->waitForBytesWritten(-1); 
//   qint64 towrite;
//   while((towrite = socket->bytesToWrite()) != 0)
//       {cout<<"wb:"<<size<<"/"<<towrite<<endl;sleep(0);}
  delete [] buff;
  }
/*******************************************************/
int Client::ChangeActionsToDo(int delta)
  {int i;
  mutex.lock();
  i=(ActionsToDo += delta);
  mutex.unlock();


  if(delta == 0)return i;

  if(delta < 0)
      {if(stack.isEmpty())
          {writeClient(QString("ERROR:-STACK EMPTY %1 ").arg(i));
          mutex.lock();
          ActionsToDo = 0;
          mutex.unlock();
          return 0;
          }
      stack.pop(); 
      }
  else if(debug())
      {if(stack.isEmpty()){writeClient(QString("ERROR:+STACK EMPTY %1 ").arg(i));return i;}
      writeClient(QString("%1 (%2)").arg(stack.top()).arg(i));
      }
  return i;
  } 
void Client:: debug(bool b) 
  {mutex.lock(); dbg=b; mutex.unlock();
  QString str = QString("S_DEBUG;%1").arg(b);
  sendToServer(str);
  }
void Client::sendToServer()
  {if(socket->state() != QAbstractSocket::ConnectedState)return;
  QString str = inputText->text();
  sendToServer(str);
  inputText->setText(""); 
  }
void Client::sendToServer(QString &str)
  {if(str.at(0) == '#' || str.at(0) == '!') //split str -> 1 command per line if not a comment
      {stack.push(str);
      ChangeActionsToDo(1);
      writeServer(str);
      return;
      }
  QStringList fields = QStringList::split(ACTION_SEP,str);//not qt4
  for(int i = 0; i < (int)fields.count();i++)
      {fields[i].simplified();
      if(fields[i].contains("RC_GRAPH"))
          sendToServerGraph(fields[i]); 
      //if the file exists sendToServerGraph will add the command on the stack
      else
          {stack.push(fields[i]);
          ChangeActionsToDo(1);
           writeServer(fields[i]);
           }
      }
  }
int Client::sendToServerGraph(QString &data)
  {if(socket->state() != QAbstractSocket::ConnectedState)return -1;
  QStringList fields = QStringList::split(PARAM_SEP,data);
  if(fields.count() < 2){writeClient("MISSING ARGUMENT");return -1;}
  QString FileName = fields[1].simplified();
  QString GraphFileName = universalFileName(FileName);
  QFileInfo fi = QFileInfo(GraphFileName);
  if(GraphFileName.isEmpty() || !fi.isFile() || !fi.size())
      {writeClient(QString("NO FILE:%1").arg(GraphFileName));
      return -1;
      }
  stack.push("RC_GRAPH");
  ChangeActionsToDo(1);
  quint32 size = fi.size();
  if(debug()) writeClient(QString("Client sending:%1 %2 bytes").arg(GraphFileName).arg(size));
  writeServer(data);
  QFile file(GraphFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeServer(buff,size);
  return 0;
  }
uint Client::readBuffer(char*  &buffer)
  {uint size;
  while(socket->bytesAvailable() < (int)sizeof(uint))socket->waitForReadyRead(10);
  clo >> size;
  //cout<<"getting:"<<size<<endl;
  buffer = new char[size+1];
  char *pbuff = buffer;
  int retry = 0;
  uint nread = 0; 
  uint size0 = 0;
  uint nb;
  while(nread  < size)
      {socket->waitForReadyRead(10);
      nb = socket->bytesAvailable();
      if(nb == 0)
          {if(++retry > 1000){writeClient("TIMEOUT");ChangeActionsToDo(-1);return 0;}
          continue;
          }
      retry = 0;
      if(nb > size-nread)nb = size-nread;
      nread += nb;
      clo.readRawData(pbuff,nb);
      pbuff += nb;
      if(nread >= size0 && debug())
          {int percent = (int)(nread*100./size + .5);
          size0 = nread + size/10; // we write when at least 10% more  is read
          writeClient(QString("%1 % (%2 / %3)").arg(percent).arg(nread).arg(size));
          }
      }
  return size;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.simplified();
      if(str.at(0) == ':')
          writeClient(str.mid(1));
      else if(str.contains("!PNGREADY"))// receiving a png image
          {char * buffer = NULL;
          lock.lockForRead();
          uint size = readBuffer(buffer);
          lock.unlock();
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PngFile = QString("image%1.png").arg(++numFiles);
          QFile file(PngFile);          file.open(QIODevice::ReadWrite);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          writeClient("!PNG");
          ChangeActionsToDo(-1);
          }
      else if(str.contains("!PSREADY"))// receiving a ps image
          {char * buffer = NULL;
          uint size = readBuffer(buffer);
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PsFile = QString("image%1.ps").arg(++numFiles);
          QFile file(PsFile);          file.open(QIODevice::ReadWrite);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          writeClient("!PS");
          ChangeActionsToDo(-1);
          }
      else if(str.contains("!GRAPH"))// receiving a graph
          {stack.push("GET_GRAPH");
          ChangeActionsToDo(1);
          char * buffer = NULL;
          uint size = readBuffer(buffer);
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          int pos = str.find(PARAM_SEP);//not QT4
          QString graphFile = str.mid(pos+1);
          QFile file(graphFile);  
          QFileInfo fi = QFileInfo(graphFile);
          if(fi.exists())file.remove();
          file.open(QIODevice::ReadWrite);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeClient(QString("GOT:%1").arg(graphFile));
          ChangeActionsToDo(-1);
          }
      else if(str == "!!")// server has finished everything
          {ChangeActionsToDo(-1);
          //writeClient("Connection close");
          closeConnection();
          }
      else if(str == "!|")// server ha finished everything and client wants to quit
          {closeConnection();
          exit();
          }
      else if(str.at(0) == '!')//server has finished one action
          {if(!stack.isEmpty())
              writeClient("!"+stack.top()+QString(" (%1)").arg(ChangeActionsToDo(0)-1)+"   "+str);
          else
              writeClient("<- "+str+QString(" EMPTY:%1").arg(ChangeActionsToDo(0)-1));
          ChangeActionsToDo(-1);
          }
      else // comments sent by the server
          writeClient(str);
      }
  }

void threadRead::run() 
// read datas from stdin
  {QTextStream stream(stdin,QIODevice::ReadWrite);
  QString str;
  while(!stream.atEnd())
      {int retry = 0;
      while(pclient->ChangeActionsToDo(0) > 0)
          {msleep(10);// milliseconds
          if(++retry %200 == 0 && pclient->debug())
              {//QString action = pclient->stack.top(); 
              pclient->writeClient(QString("Waiting %1s (%2:%3)").arg(retry/100)
                  .arg(pclient->stack.top()).arg(pclient->ChangeActionsToDo(0)));
              }
          }
      str = stream.readLine(); 
      QChar ch = str.at(0);
      if(ch == ':')
          {QChar c = str.at(1);
          if(c == QChar('!'))
              {str = "!";
              pclient->sendToServer(str);
              return;
              }
          else if(c == 'D')
              pclient->debug(true);
          else if(c == 'd')
              pclient->debug(false);
          else if(c == 'X')	
              {str = "|";
              pclient->sendToServer(str);
              }
          }
      else if(pclient->debug() || ch != '#')
          pclient->sendToServer(str);
      }
}

int main( int argc, char** argv )
  {QApplication app( argc, argv );
  Client client(argc<2 ? "localhost" : argv[1], 4242);
  client.show();
  return app.exec();
  }

