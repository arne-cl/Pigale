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

#include "server.h"
#include <QT/Action_def.h>

// as now we do not link with /pigale/lib/libqt
void DrawGraph(Graph &G) {}
void DrawGraph(void) {}
int Twait(const char *) {return 0;}
void Tprintf(const char *,...) {}

//ClientSocket: public QSocket
ClientSocket::ClientSocket(int sock,const int id,QObject *parent,const char *name) :
    QSocket(parent,name),line(0),prId(id),sdebug(0),cli(this)
  {connect(this, SIGNAL(readyRead()),SLOT(readClient()));
  connect(this, SIGNAL(connectionClosed()),SLOT(deleteLater()));
  setSocket(sock);
  }
void ClientSocket::writeServer(QString& msg)
  {emit logText(QString("  %1->%2: %3\n").arg(prId).arg(line).arg(msg));
  }
void ClientSocket::readClient()
  {while (canReadLine())
      {QString str = cli.readLine();
      if(++line == 10000)line = 0;
      //emit logText( QString("%1->%2: %3\n").arg(prId).arg(line).arg(str));
      if(str.at(0) == '#')
	  cli << str << endl;
      else 
	  //parseSplitAction(str);  
	  handler(str); // now the server split actions  
      }
  }
int ClientSocket::parseSplitAction(const QString& data)
  {QStringList fields = QStringList::split(ACTION_SEP,data);
  for(int i = 0;i < (int)fields.count();i++)
      handler(fields[i]);
  cli <<"!" << endl; // tells the client that processing is over
  return 0;
  }
int ClientSocket::handler(const QString& dataAction)
  {int pos = dataAction.find(PARAM_SEP);
  QString beg = dataAction.left(pos);
  QString dataParam = dataAction.mid(pos+1);
  if(sdebug)cli <<"# '"<<dataAction<<"'"<<endl;
  bool ok;
  int action = beg.toInt(&ok);
  int err = 0;
  if(!ok)
      {err = ACTION_NOT_INT;
      cli <<"# ERREUR '"<<dataAction <<"' -> "<<err << endl;
      return err;
      }
  
  // call the right handler
  if(action > A_INFO && action < A_INFO_END)
      err =  handlerInfo(action);
  else if(action > A_INPUT && action < A_INPUT_END)
      err =  handlerInput(action,dataParam);
  else if(action > A_EMBED && action < A_EMBED_END)
      err =  handlerEmbed(action);
  else if(action == SERVER_DEBUG)
      {sdebug = 1;debug() = 1;}
  else
      err = UNKNOWN_COMMAND;
  if(err)cli <<"# ERREUR '"<<dataAction <<"' -> "<<err << endl;
  return err;
  }
int ClientSocket::handlerInput(int action,const QString& dataParam)
  {QStringList fields = QStringList::split(PARAM_SEP,dataParam);
  QString msg;
  int nfield = (int)fields.count();
  bool ok;
  switch(action)
      {case A_INPUT_READ_GRAPH:
	   {if(nfield == 0)return WRONG_PARAMETERS;
	   QString InputFileName = fields[0];
	   msg = QString("R: %1").arg(InputFileName);
	   writeServer(msg);
	   int NumRecords = GetNumRecords((const char *)InputFileName);
	   // GraphIndex: the graph to read in file containing NumRecords graphs
	   int GraphIndex = 1,num = 1; 
	   if(nfield > 1)num = fields[1].toInt(&ok);
	   if(ok && num <= NumRecords)GraphIndex = num;
	   if(ReadGraph(GC,(const char *)InputFileName,NumRecords,GraphIndex))
	       return READ_ERROR;
	   TopologicalGraph G(GC);
	   cli << ": Read "<<InputFileName <<":"<<GraphIndex<<"/"<<NumRecords <<endl;
	   cli << ": N:"<<G.nv()<<":M:"<<G.ne() << endl;
	   }
	   break;
      case  A_INPUT_NEW_GRAPH:
	  {Graph G(GC);
	  G.StrictReset();
	  }
	  break;
      case A_INPUT_NEW_VERTEX:
	  {int n = 1;
	  if(nfield > 0)
	      {n = fields[0].toInt(&ok);
	      if(!ok)return WRONG_PARAMETERS; 
	      }
	  TopologicalGraph G(GC);
	  for(int i = 0;i < n;i++)G.NewVertex();
	  }
	  break;
      case A_INPUT_NEW_EDGE:
	  {if(nfield < 2)return WRONG_PARAMETERS; 
	  int v1 = fields[0].toInt(&ok);
	  if(!ok)return WRONG_PARAMETERS; 
	  int v2 = fields[1].toInt(&ok);
	  if(!ok)return WRONG_PARAMETERS;
	  TopologicalGraph G(GC);
	  if(v1 > G.nv() || v2 > G.nv() || v1 == v2)return WRONG_PARAMETERS;
	  G.NewEdge((tvertex)v1,(tvertex)v2);
	  }
	  break;
      default:
	  return  UNKNOWN_COMMAND;
	  break;
      }
  
  return 0;
  }
int ClientSocket::handlerInfo(int action)
  {TopologicalGraph G(GC);
  ComputeGraphProperties();
  switch(action)
      {case A_INFO_N:
	   cli << G.nv() << endl;
	   break;
      case A_INFO_M:
	    cli << G.ne() << endl;
	    break;
      case A_INFO_SIMPLE:
	  cli << (int)Simple << endl;
	  break;
      case A_INFO_PLANAR:
	  cli << (int)Planar << endl;
	  break;
      case A_INFO_OUTER_PLANAR:
	  cli << (int)Outer << endl;
	  break;
      case A_INFO_SERIE_PAR:
	  cli << (int)Serie << endl;
	  break;
      case A_INFO_MAX_PLANAR:
	  cli << MaxPlanar << endl;
	  break;
      case A_INFO_BIPAR:
	  cli << (int)Bipar << endl;
	  break;
      case A_INFO_MAX_BIPAR:
	  cli << (int)MaxBi << endl;
	  break;
      case A_INFO_REGULIER:
	  cli << (int)Regul << endl;
	  break;
      case A_INFO_CON1:
	  cli << (int)Con1 << endl;
	  break;
      case A_INFO_CON2:
	  cli << (int)Con2 << endl;
	  break;
      case A_INFO_CON3:
	  cli << (int)Con3 << endl;
	  break;
      case A_INFO_MIN_D:
	  cli << dmin << endl;
	  break;
      case A_INFO_MAX_D:
	  cli << dmax << endl;
	  break;
      case A_INFO_COORD:
	  {if(!G.Set(tvertex()).exist(PROP_COORD)) return PROP_NOT_DEFINED;
	  Prop<Tpoint> coord(G.Set(tvertex()),PROP_COORD);
	  for(tvertex v = 1;v <= G.nv();v++)
	      cli << coord[v].x() <<" "<< coord[v].y() << endl;
	  }
	  break;
      case A_INFO_VLABEL:
	  {if(!G.Set(tvertex()).exist(PROP_LABEL)) return PROP_NOT_DEFINED;
	  Prop<long> label(G.Set(tvertex()),PROP_LABEL);
	  for(tvertex v = 1;v <= G.nv();v++)
	      cli << label[v] << endl;
	  }
	  break;
      default:
	  return  UNKNOWN_COMMAND;
	  break;
      }
  return 0;
  }
int ClientSocket::handlerEmbed(int action)
  {TopologicalGraph G(GC);
  ComputeGraphProperties();
  int err = 0;
  switch(action)
      {case A_EMBED_SCHNYDER_E:
	   if(!(Simple && Planar))
	       err =  ACTION_NOT_APPLICABLE;
	   else
	       err = G.Schnyder(0);	
	   break;
      case A_EMBED_SCHNYDER_V:
	   if(!(Simple && Planar))
	       err =  ACTION_NOT_APPLICABLE;
	   else
	       err = G.Schnyder(1);	
	   break;
      case A_EMBED_FPP:
	   if(!(Simple && Planar))
	       err =  ACTION_NOT_APPLICABLE;
	   else
	       err = EmbedFPP(G);	
	   break;
      case A_EMBED_FPP_RECTI:
	   if(!(Simple && Planar))
	       err =  ACTION_NOT_APPLICABLE;
	   else
	       err = EmbedFPPRecti(G);	
	   break;
      case A_EMBED_TUTTE_CIRCLE:
	   if(!(Simple && Planar))
	       err =  ACTION_NOT_APPLICABLE;
	   else
	       EmbedTutteCircle(G);	
	   break;
      default:
	  err = UNKNOWN_COMMAND;
	  return -1;
	  break;
      }
  if(err < 0)
      {cli <<"# "<< action << ":"<< err << endl; return err;}
  return 0;
  }
void ClientSocket::ComputeGraphProperties()
  {TopologicalGraph G(GC); 
  G.RemoveLoops();
  Simple = G.CheckSimple();
  Planar = G.CheckPlanar();
  bool SMALL = (G.nv() < 3) ? true : false;
  bool M = (!SMALL  && (G.ne() == 3*G.nv() - 6)) ? true : false;
  MaxPlanar = Planar && Simple && (G.ne() == 3*G.nv() - 6);
  Bipar = G.CheckBipartite();
  MaxBi = Planar && Bipar && Simple && (G.ne() == (2*G.nv() - 4) && (G.nv() >= 4));
  G.MinMaxDegree(dmin,dmax);
  Regul = (dmin == dmax) ? true : false;
  
  Con1 = Con2 = Con3 = false;
  Outer = Serie = false;
  if(Simple && G.nv() == 2 && G.ne() == 1) //One edge graph
      Con1 = true;
  if(MaxPlanar && G.nv() == 3)
      Con1 = Con2 = true;
  else if(MaxPlanar && G.nv() > 3) // MaxPlanar
      Con3 = true;
  else if(Planar && Simple && !M && dmin > 2  && G.CheckTriconnected())
      Con3 = true;
  else if(!MaxPlanar)
      {if(G.CheckBiconnected())
	  Con1 = Con2 = true;
      else if(G.CheckConnected())
	  Con1 = true;
      }
  if(Planar && Con2 && !Con3)
      {Serie = G.CheckSerieParallel();
      if(Serie && dmin == 2 && TestOuterPlanar(G))Outer = true;
      }
  else if(G.nv() == 2 && G.ne() == 1)
      Serie = Outer = true;
  if(Con3)Con2 = true;
  if(Con2)Con1 = true;
  }

//SimpleServer:public QServerSocket 
SimpleServer::SimpleServer( QObject* parent ):
    QServerSocket(4242,1,parent),id(1)
    {if(!ok()){qWarning("Failed to bind to port 4242");	exit(1);}
    }
void SimpleServer::newConnection(int socket)
    {ClientSocket *s = new ClientSocket(socket,id++,this);
    emit newConnect( s );  //connects signals to ServerInfo
    }
//ServerInfo: public QVBox
ServerInfo::ServerInfo()
  {SimpleServer *server = new SimpleServer( this );
  QLabel *lb = new QLabel((const char *)"Ready",this);lb->setAlignment(AlignHCenter);
  infoText = new QTextView( this );
  QPushButton *quit = new QPushButton((const char *)"Quit",this);
  connect(server, SIGNAL(newConnect(ClientSocket*)), SLOT(newConnect(ClientSocket*)) );
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

