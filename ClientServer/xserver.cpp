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


#include "xserver.h"
#include <qtextview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qmenubar.h>

//SimpleServer:public QServerSocket 
SimpleServer::SimpleServer( QObject* parent ):
    QServerSocket(4242,1,parent),id(1),show(false)
    {if(!ok()){qWarning("Failed to bind to port 4242");	exit(1);}
    }
void SimpleServer::newConnection(int socket)
    {ClientSocket *s = new ClientSocket(socket,id++,show,this);
    emit newConnect( s );  //connects signals to ServerInfo
    }
void SimpleServer::display()
  {show = !show;
  }

//ServerInfo: public QVBox
ServerInfo::ServerInfo()
  {SimpleServer *server = new SimpleServer( this );
  QLabel *lb = new QLabel((const char *)"Ready",this);lb->setAlignment(AlignHCenter);
  infoText = new QTextView( this );
  QPushButton *quit = new QPushButton((const char *)"Quit",this);
  QRadioButton *display = new QRadioButton((const char *)"Display client window",this);
  connect(server, SIGNAL(newConnect(ClientSocket*)), SLOT(newConnect(ClientSocket*)) );
  connect(display, SIGNAL(clicked()),server,SLOT(display()));
  connect(quit, SIGNAL(clicked()), qApp,SLOT(quit()));
  }

void ServerInfo::newConnect(ClientSocket *s)
    {infoText->append((const char *)"New connection\n");
    connect(s, SIGNAL(logText(const QString&)),infoText, SLOT(append(const QString&)) );
    connect(s, SIGNAL(connectionClosed()), SLOT(connectionClosed()) );
    }
void ServerInfo::connectionClosed()
    {infoText->append("Client closed connection\n");
    }
int main( int argc, char** argv)
  {QApplication app(argc,argv);
  ServerInfo info;
  app.setMainWidget( &info );
  info.show();
  return app.exec();
  }

