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

#ifndef TFILE_H
#define TFILE_H

#include  <TAXI/graphs.h>
#define TAXI_FILE_READ          0x0001
#define TAXI_FILE_WRITE         0x0002
#define TAXI_FILE_RECORD_NUM    0x0004
#define TAXI_FILE_RECORD_DEL    0x0008

#define TAXI_FILE_MINI          (TAXI_FILE_READ|TAXI_FILE_WRITE)
#define TAXI_FILE_RECORD        (TAXI_FILE_RECORD_NUM | TAXI_FILE_RECORD_DEL)
#define TAXI_FILE_FULL          (TAXI_FILE_MINI | TAXI_FILE_RECORD)

class Taxi_FileIO {
 public:
  virtual int IsMine(tstring fname)=0;
  virtual int Save(GraphAccess& G,tstring fname)=0;
  virtual int Read(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex)=0;
  virtual int GetNumRecords(tstring fname) {return 1;}
  virtual int DeleteRecord(tstring fname,int index) {return -1;}
  virtual int Capabilities(void) { return 0;}
  virtual const char *Name(void) {return "No Name";}
  virtual const char *Ext(void) {return "";}
};



int IsFileTgf(char const *name);
int IsFileAscii(char const *name);
int SaveGraphTgf(GraphAccess& G,tstring filename,int tag=2);
int SaveGraphAscii(GraphAccess& G,tstring filename);
int ReadGraphAscii(GraphContainer& G,tstring filename);
int DeleteTgfRecord(tstring filename,int index);
int GetTgfNumRecords(tstring fname);
int ReadGeometricGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
int ReadGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
int ReadTgfGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);

class Taxi_FileIO_Tgf : public Taxi_FileIO
{
 public:
  int IsMine(tstring fname) {return IsFileTgf(~fname);}
  int Save(GraphAccess& G,tstring fname) {return SaveGraphTgf(G,fname);}
  int Read(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex) 
    {return ReadTgfGraph(G,fname,NumRecords,GraphIndex);}
  int GetNumRecords(tstring fname) {return GetTgfNumRecords(fname);}
  int DeleteRecord(tstring fname,int index) {return DeleteTgfRecord(fname,index);}
  int Capabilities(void) { return TAXI_FILE_FULL;}
  const char *Name(void) {return "Tgf file";}
  const char *Ext(void) {return "tgf";}
};
static  Taxi_FileIO_Tgf FileIO_Tgf;

class Taxi_FileIO_ASCII : public Taxi_FileIO
{
 public:
  int IsMine(tstring fname) {return IsFileAscii(~fname);}
  int Save(GraphAccess& G,tstring fname) {return SaveGraphAscii(G,fname);}
  int Read(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex)
  {NumRecords=1; GraphIndex=1; return ReadGraphAscii(G,fname);}
  //int GetNumRecords(tstring fname) {return 1;}
  //int DeleteRecord(tstring fname,int index) {return -1;}
  int Capabilities(void) { return TAXI_FILE_MINI;}
  const char *Name(void) {return "Ascii file";}
  const char *Ext(void) {return "txt";}
};

static Taxi_FileIO_ASCII FileIO_ASCII;

class Taxi_FileIOHandler {
  svector <Taxi_FileIO *> drivers;
 public:

  void add(Taxi_FileIO &driver, int where=-1) 
  { int n=(where==-1)?drivers.n():where; drivers.resize(0,drivers.n()); 
      for (int i=drivers.n()-1; i>n; i--) drivers[i]=drivers[i-1];
      drivers[n]=&driver;
    }
  Taxi_FileIOHandler() {add(FileIO_Tgf); add(FileIO_ASCII);}
  ~Taxi_FileIOHandler() {}
  int WhoseIs(tstring fname) 
    { for (int i=0; i<drivers.n();i++)
	if (drivers[i]->IsMine(fname)) return i;
      return -1;
    }
  int IsMine(int d, tstring fname) {return drivers[d]->IsMine(fname);}
  int Save(int d, GraphAccess& G,tstring fname) {return drivers[d]->Save(G,fname);}
  int Read(int d, GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex) 
    {return drivers[d]->Read(G,fname,NumRecords,GraphIndex);}
  int Read(int d, GraphContainer& G,tstring fname) { int x,y; return drivers[d]->Read(G,fname,x,y);}
  int GetNumRecords(int d, tstring fname) {return drivers[d]->GetNumRecords(fname);}
  int DeleteRecord(int d, tstring fname,int index=1) {return drivers[d]->DeleteRecord(fname,index);}
  int Capabilities(int d) {return drivers[d]->Capabilities();}
  const char *Name(int d) {return drivers[d]->Name();}
  const char *Ext(int d) {return drivers[d]->Ext();}
  int n() {return drivers.n();}
};
static Taxi_FileIOHandler FileIOHandler;

#endif

