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

#include <QFileDialog>
#include <QInputDialog>
#include <QStatusBar>
#include <QMessageBox>

void Init_IO();

void pigaleWindow::load()
  {QFileInfo fi =  QFileInfo(InputFileName);
  QString extension = fi.suffix();
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
    QString FileName = QFileDialog::getOpenFileName(this
                                                    ,tr("Choose a file to open")
                                                    ,fi.filePath()
                                                    ,formats.join(";;")
                                                    ,&selfilter);

    if(!FileName.isEmpty())
      {InputFileName = FileName;
      int i = 0;
      for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++i ) 
          {if (selfilter==*it) break;
          }
      if (i == IO_n()) // All
          {i = IO_WhoseIs((const char *)FileName.toAscii());
          if (i<0) 
              {QString m;
              m.sprintf("Could not read:%s",(const char *)InputFileName.toAscii());
              statusBar()->showMessage(m,2000);
              LogPrintf("%s: unrecognized format\n",(const char *)InputFileName.toAscii());
              return;
              }	  
          }
      InputDriver = i;
      *pGraphIndex = 1;
      int NumRecords = IO_GetNumRecords(i,(const char *)InputFileName.toAscii());
      if(NumRecords > 1)
          {bool ok = FALSE;
          QStringList titles;
          QString item;
          for (int j=1; j<=NumRecords; j++)
              {item.sprintf("%d: %s",j,(const char *)IO_Title(i,(const char *)InputFileName.toAscii(),j));
              titles+=item;
              }
          QString res = QInputDialog::getItem(this,
                                              "Graph Selection",
                                              "Select a record:",
                                              titles,
                                              0,
                                              false,
                                              &ok);
          if(ok) 
              {*pGraphIndex=res.left(res.indexOf(':')).toInt();
              load(0);
              }
          }
      else load(0);
      }
  }
int pigaleWindow::publicLoad(int pos)
// Only called by the server
  {QFileInfo fi(InputFileName);
  QString m;
   if(!fi.exists() || fi.size() == 0)
       {m = QString("file -%1- does not exist").arg(InputFileName);
       LogPrintf("%s\n",(const char *)m.toAscii());
       setError(-1,"Non existing file");
       return -1;
      } 
   int i = IO_WhoseIs((const char *)InputFileName.toAscii());
   if(i < 0) 
       {m = QString("%1: unrecognized format").arg(InputFileName);
     LogPrintf("%s\n",(const char *)m.toAscii());
     setError(-1,"unrecognized format");
     return -1;
     }
   InputDriver = i;
   UndoClear();UndoSave();
  int NumRecords =IO_GetNumRecords(i,(const char *)InputFileName.toAscii());
  *pGraphIndex = pos;
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(i,GC,(const char *)InputFileName.toAscii(),NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      LogPrintf("%s\n",(const char *)m.toAscii());
      return -2;
      }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName.toAscii(),*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  return *pGraphIndex;
  }
int pigaleWindow::load(int pos)
  {setError();
  QString m;
  QFileInfo fi(InputFileName);
  if(!fi.exists() || fi.size() == 0)
      {m = QString("file -%1- does not exist").arg(InputFileName);
      if(!ServerExecuting)statusBar()->showMessage(m,2000);
      LogPrintf("%s\n",(const char *)m.toAscii());
      return -1;
    }      
  if (!IO_IsMine(InputDriver,(const char *)InputFileName.toAscii()))
    {m.sprintf("file -%s- is not a valid %s",(const char *)InputFileName.toAscii(),IO_Name(InputDriver));
    if(!ServerExecuting)statusBar()->showMessage(m,2000);
    LogPrintf("%s\n",(const char *)m.toAscii());
      return -1;
    }
  UndoClear();UndoSave();
  int NumRecords =IO_GetNumRecords(InputDriver,(const char *)InputFileName.toAscii());
  if(pos == 1)++(*pGraphIndex);
  else if(pos == -1)--(*pGraphIndex);
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(InputDriver,GC,(const char *)InputFileName.toAscii(),NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      if(!ServerExecuting)statusBar()->showMessage(m,2000);
      return -2;
    }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName.toAscii(),*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  banner();
  information(); 
  gw->update();
  return *pGraphIndex;
  }
int pigaleWindow::save(bool manual)
  {TopologicalGraph G(GC);
  if(manual) // check overwrite
      {QFileInfo fi =  QFileInfo(OutputFileName);
      if(!(IO_Capabilities(OutputDriver)&TAXI_FILE_RECORD_ADD) && fi.exists() )
          {if(QMessageBox::warning(this,"Pigale Editor"
                                   ,"This file already exixts.<br>"
                                   "Overwrite ?"
                                   ,QMessageBox::Ok 
                                   ,QMessageBox::Cancel) == QMessageBox::Cancel)return 0;
          }
      }
  if(manual)// ask for a title
      {Prop1<tstring> title(G.Set(),PROP_TITRE);
      QString titre(~title());
      bool ok = TRUE;
      titre = QInputDialog::getText(this,
                                    "Pigale","Enter the graph name",
                                    QLineEdit::Normal,titre, &ok);
      if(ok && !titre.isEmpty()) title() = (const char *)titre.toAscii();
      else if(!ok) return -1;
      }
 
  if(IO_Save(OutputDriver,G,(const char *)OutputFileName.toAscii()) == 1)
      {setError(-1,QString("Cannot open file:%1").arg(OutputFileName).toAscii());
      return -1;
      }
  GraphIndex2 = IO_GetNumRecords(OutputDriver,(const char *)OutputFileName.toAscii());
  banner();
  return 0;
  }
int pigaleWindow::publicSave(QString FileName)
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  title() = "G";
  QFileInfo fi =  QFileInfo(FileName);
  QString fileExt =  fi.suffix();
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
    QString FileName = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a filename to save under"),
                                                    fi.filePath(),
                                                    formats.join(";;"),
                                                    &selfilter);
    if(FileName.isEmpty())return;
    QString ext="";
    int id=0;
   
    for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++id ) 
        if (selfilter==*it) break;
        
    if ((QFileInfo(FileName).suffix() != IO_Ext(id)) && (IO_Ext(id)!=""))
      {FileName += ".";
      FileName += IO_Ext(id);
      }
    OutputFileName = FileName;
    OutputDriver = id;
    save();
  }
void pigaleWindow::deleterecord()
  {if (IO_Capabilities(InputDriver)&TAXI_FILE_RECORD_DEL)
      IO_DeleteRecord(InputDriver,(const char *)InputFileName.toAscii(),*pGraphIndex);
    else
      {QString m=(const char *)IO_Name(InputDriver);
	m += " does not allow record deletion";
	Twait((const char *)m.toAscii());
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
  statusBar()->showMessage("New graph");
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
