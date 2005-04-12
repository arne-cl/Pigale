/****************************************************************************
** $Id: graphml.cpp,v 1.1 2005/04/12 17:12:18 hbonnin Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qfile.h>
#include <qxml.h>
#include <qwindowdefs.h>
#include <qmap.h>
#include <QT/graphml.h>
#include <config.h>

static bool CallParse(tstring fname, GraphmlParser &parser)
  {    
    QFile xmlFile(~fname);
    QXmlInputSource source(&xmlFile);
    QXmlSimpleReader reader;
    MyErrorHandler err;
    reader.setErrorHandler(&err);
    reader.setContentHandler(&parser);
    bool res=reader.parse(source);
    return res;
  }

int Taxi_FileIOGraphml::IsMine(tstring fname)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    bool res=CallParse(fname,xreader);
    delete &xreader;
    return (res ? 1 : 0);
  } 

int Taxi_FileIOGraphml::GetNumRecords(tstring fname)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    if (!CallParse(fname,xreader)) return 0;
    int n=xreader.ng();
    delete &xreader;
    return n;
  }

tstring Taxi_FileIOGraphml::Title(tstring fname,int index)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    if (!CallParse(fname,xreader)) return "???";
    QString title=xreader.Title();
    delete &xreader;
    return (const char *)title;
  }

int Taxi_FileIOGraphml::Read(GraphContainer& G,tstring fname,int& NR,int& index)
  {
    if (index <= 0) index=1;
    GraphmlReaderInfo &inforeader= *new GraphmlReaderInfo(index);
    if (!CallParse(fname,inforeader)) return 1;
    NR=inforeader.ng();
    int nv=inforeader.n();
    int ne=inforeader.m();
    QString title=inforeader.Title();
    QString version=inforeader.Version();
    delete &inforeader;
    if (index > NR)
      { GraphmlReaderInfo &inforeader2= *new GraphmlReaderInfo(index);
	if (!CallParse(fname,inforeader2)) return 1;
	NR=inforeader2.ng();
	nv=inforeader2.n();
	ne=inforeader2.m();
	title=inforeader2.Title();
	version=inforeader.Version();
	delete &inforeader2;
      }
    if (version!="") LogPrintf("Graphml: Pigale version=%s\n",(const char *)version);
    G.clear();
    G.setsize(nv,ne);
    Prop1<tstring> Gtitle(G.Set(),PROP_TITRE);
    Gtitle() = tstring((const char *)title);
    GraphmlReader &graphreader=*new GraphmlReader(G,index);
    if (!CallParse(fname,graphreader)) return 1;
    return 0;
  }

static QString xlabel(GraphAccess &G,tvertex v)
  {QString id;
    if (G.Set().exist(PROP_VSLABEL) && G.Set(tvertex()).exist(PROP_SLABEL))
      { Prop1<svector<tstring *> >vslabel(G.Set(),PROP_VSLABEL);
	Prop<int> slabel(G.Set(tvertex()),PROP_SLABEL,0);
	id=~(*(vslabel()[slabel[v]]));
      }
    else id.sprintf("n%d",v());
    return id;
  }

int  Taxi_FileIOGraphml::Save(GraphAccess& G,tstring fname)
  {
    QFile xmlFile(~fname);
    if (!xmlFile.open(IO_WriteOnly))
      return 1;
    QTextStream stream( &xmlFile );
    Prop1<tstring> title(G.Set(),PROP_TITRE);
    Prop<tvertex> vin(G.Set(tbrin()),PROP_VIN);
    bool oriented1=G.Set(tedge()).exist(PROP_ORIENTED);
    bool oriented = false; 
    if (oriented1)
      {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	eoriented.getinit(oriented);
      }
    QString otherorient="true";
    if (oriented) otherorient="false";

    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    stream << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << endl;  
    stream << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<endl;
    stream << "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns"<<endl;
    stream << "     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << endl;
    stream << "  <key id=\"Pigale/version\" for=\"graph\"";
    stream << " attr.name=\"Pigale version\" attr.type=\"string\">"<<endl;
    stream << "    <default>"<<PACKAGE_VERSION<<"</default>"<<endl;
    stream << "  </key>"<<endl;
    if (G.Set(tvertex()).exist(PROP_COORD))
      { stream << "  <key id=\"Pigale/V/16\" for=\"node\"";
	stream << " attr.name=\"Coordinates\" attr.type=\"string\"/>"<<endl;
      }
    stream << "  <graph id=\""<<(const char *)title()<<"\" edgedefault=\"";
    if (!oriented) stream << "un";
    stream << "directed\">" << endl;
    for (tvertex v=1; v<=G.nv(); v++)
      {if (G.Set(tvertex()).exist(PROP_COORD))
	  {Prop<Tpoint> coord(G.Set(tvertex()),PROP_COORD);
	    stream << "    <node id=\""<<xlabel(G,v)<<"\">"<<endl;
	    stream << "       <data key=\"Pigale/V/16\">";
	    stream << coord[v].x() << "," << coord[v].y() << "</data>" <<endl;
	    stream << "    </node>" << endl;
	  }
	else
	  stream << "    <node id=\""<<xlabel(G,v)<<"\"/>" << endl;
      }
    for (tedge e=1; e<=G.ne(); e++)
      {stream << "     <edge source=\""<<xlabel(G,vin[e])<<"\" target=\""<<xlabel(G,vin[-e])<<"\"";
	if (oriented1)
	  {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	    if (eoriented[e]!=oriented)
	      stream << " directed=\"" << otherorient <<"\"";
	  }
	stream << "/>" <<endl;
      }
    stream << "   </graph>"<<endl;
    stream << "</graphml>"<<endl;
    xmlFile.close();
    return 0;
  }

void Init_IOGraphml() {Taxi_FileIO::reg(new Taxi_FileIOGraphml);}
