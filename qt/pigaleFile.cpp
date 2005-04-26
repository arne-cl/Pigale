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

#include <config.h>
#include "pigaleWindow.h"
#include "GraphWidget.h"
#include <TAXI/Tgf.h>
#include <QT/Misc.h> 
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qfiledialog.h>
#include <qinputdialog.h> 
#include <qfile.h>
#include <qfileinfo.h>

void Init_IO();

void pigaleWindow::load()
  {QFileInfo fi =  QFileInfo(InputFileName);
  QString extension = fi.extension(false);
  QStringList formats;
  QString filter, selfilter;
  selfilter = "All (*)";
  for (int  i = 0; i< IO_n();i++)
      {filter = IO_Name(i);
      filter += "(*";
      if (IO_Ext(i) != "!") {filter += "."; filter += IO_Ext(i);}
      filter += ")";
      formats += filter;
      if(filter.contains(extension))selfilter=filter;
      }
    formats += "All (*)";
    QString FileName = QFileDialog::getOpenFileName(fi.filePath(),formats.join(";;"),this,
                                                    "load dialog",tr("Choose a file to open"),&selfilter);
    //setError();
    if(!FileName.isEmpty())
      {InputFileName = FileName;
      int i = 0;
      for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++i ) 
          {if (selfilter==*it) break;
          }
      if (i == IO_n()) // All
          {i = IO_WhoseIs((const char *)FileName);
          if (i<0) 
              {QString m;
              m.sprintf("Could not read:%s",(const char *)InputFileName);
              statusBar()->message(m,2000);
              LogPrintf("%s: unrecognized format\n",(const char *)InputFileName);
              return;
              }	  
          }
      InputDriver = i;
      *pGraphIndex = 1;
      int NumRecords = IO_GetNumRecords(i,(const char *)InputFileName);
      if(NumRecords > 1)
          { bool ok = FALSE;
          QStringList titles;
          QString item;
          for (int j=1; j<=NumRecords; j++)
              {item.sprintf("%d: %s",j,(const char *)IO_Title(i,(const char *)InputFileName,j));
              titles+=item;
              }
          QString res = QInputDialog::getItem("Graph Selection", "Select a record:", titles, 0, FALSE, &ok, this );
          if ( ok ) 
              {*pGraphIndex=res.left(res.find(':')).toInt();
              load(0);
              }
          }
      else load(0);
      }
  }
int pigaleWindow::publicLoad(int pos)
  {QFileInfo fi(InputFileName);
  QString m;
   if(!fi.exists() || fi.size() == 0)
       {m = QString("file -%1- does not exist").arg(InputFileName);
       if(!ServerBusy)statusBar()->message(m,2000);
       LogPrintf("%s\n",(const char *)m);
       return -1;
      } 
   int i = IO_WhoseIs((const char *)InputFileName);
   if(i < 0) 
       {m = QString("%1: unrecognized format").arg(InputFileName);
     if(!ServerBusy)statusBar()->message(m,2000);
     LogPrintf("%s\n",(const char *)m);
     return -1;
     }
   InputDriver = i;
   UndoClear();UndoSave();
  int NumRecords =IO_GetNumRecords(i,(const char *)InputFileName);
  *pGraphIndex = pos;
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(i,GC,(const char *)InputFileName,NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      if(!ServerBusy)statusBar()->message(m,2000);
      LogPrintf("%s\n",(const char *)m);
      return -2;
      }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName,*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  if(!ServerBusy)banner();
  information(); gw->update();
  return *pGraphIndex;
  }
int pigaleWindow::load(int pos)
  {setError();
  QString m;
  QFileInfo fi(InputFileName);
  if(!fi.exists() || fi.size() == 0)
      {m = QString("file -%1- does not exist").arg(InputFileName);
      if(!ServerBusy)statusBar()->message(m,2000);
      LogPrintf("%s\n",(const char *)m);
      return -1;
    }      
  if (!IO_IsMine(InputDriver,(const char *)InputFileName))
    {m.sprintf("file -%s- is not a valid %s",(const char *)InputFileName,IO_Name(InputDriver));
    if(!ServerBusy)statusBar()->message(m,2000);
    LogPrintf("%s\n",(const char *)m);
      return -1;
    }
  UndoClear();UndoSave();
  int NumRecords =IO_GetNumRecords(InputDriver,(const char *)InputFileName);
  if(pos == 1)++(*pGraphIndex);
  else if(pos == -1)--(*pGraphIndex);
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(InputDriver,GC,(const char *)InputFileName,NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      if(!ServerBusy)statusBar()->message(m,2000);
      return -2;
    }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName,*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  banner();
  information(); gw->update();
  return *pGraphIndex;
  }
int pigaleWindow::save(bool askTitle)
  {TopologicalGraph G(GC);
  if(askTitle)
      {Prop1<tstring> title(G.Set(),PROP_TITRE);
      QString titre(~title());
      bool ok = TRUE;
      titre = QInputDialog::getText("Pigale","Enter the graph name",
                                    QLineEdit::Normal,titre, &ok, this );
      if(ok && !titre.isEmpty()) title() = (const char *)titre;
      else if(!ok) return -1;
      }
  if(IO_Save(OutputDriver,G,(const char *)OutputFileName) == 1)
      {setError(-1,QString("Cannot open file:%1").arg(OutputFileName));
      return -1;
      }
  GraphIndex2 = IO_GetNumRecords(OutputDriver,(const char *)OutputFileName);
  banner();
  return 0;
  }
int pigaleWindow::publicSave(QString FileName)
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  title() = "G";
  QFileInfo fi =  QFileInfo(FileName);
  QString fileExt =  fi.extension(false);
  if(fileExt.length() < 3){setError(-1,"UNKNOWN EXTENSION");return -1;}
  int driver = -1;
  QString extName;
  for (int i=0; i<IO_n();i++)
      {extName = IO_Ext(i);
      if(fileExt.contains(extName))driver = i;
      }
  if(driver == -1)
      {setError(-1,"UNKNOWN DRIVER");return -1;}
  OutputFileName = FileName;
  OutputDriver = driver;
  save(false);
  return 0;
  }
void pigaleWindow::saveAs()
  {QFileInfo fi =  QFileInfo(OutputFileName);
    QStringList formats;
    QString filter, selfilter;
    for (int i=0; i<IO_n();i++)
      { filter = tr(IO_Name(i));
	filter += "(*";
	if (IO_Ext(i)!="!") {filter += "."; filter += IO_Ext(i);}
	filter += ")";
	formats += filter;
	if (i==0) selfilter=filter;
      }
    QString FileName = QFileDialog::getSaveFileName(fi.filePath(),formats.join(";;"),this,
                                                    "save dialog",tr("Choose a filename to save under"),
                                                    &selfilter);
    if(FileName.isEmpty())return;
    QString ext="";
    int id=0;
    for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++id ) {
      if (selfilter==*it) break;
    }
    if ((QFileInfo(FileName).extension(false) != IO_Ext(id)) && (IO_Ext(id)!=""))
      {FileName += ".";
	FileName += IO_Ext(id);
      }
    OutputFileName = FileName;
    OutputDriver = id;
    save();
  }
void pigaleWindow::deleterecord()
  {if (IO_Capabilities(InputDriver)&TAXI_FILE_RECORD_DEL)
      IO_DeleteRecord(InputDriver,(const char *)InputFileName,*pGraphIndex);
    else
      {QString m=(const char *)IO_Name(InputDriver);
	m += " does not allow record deletion";
	Twait((const char *)m);
      }
    banner();
  }
void pigaleWindow::switchInputOutput()
  {Tswap(InputFileName,OutputFileName);
  Tswap(InputDriver,OutputDriver);
  Tswap(GraphIndex1,GraphIndex2);
  pGraphIndex   =  &GraphIndex1;
  load(0);
}
void pigaleWindow::NewGraph()
  {setError();
  statusBar()->message("New graph");
  UndoClear();UndoSave();
  Graph G(GC);
  G.StrictReset();
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  if(debug())DebugPrintf("**** New graph");
  information();gw->update();
  }
void pigaleWindow::previous()
  {load(-1);}
void pigaleWindow::reload()
  {load(0);}
void pigaleWindow::next()
  {load(1);}
