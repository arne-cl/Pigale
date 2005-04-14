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
#include <QT/Action_def.h>
#include <qfileinfo.h>
#include <qtoolbutton.h>
/*
The copies are cleared when a new graph is loaded or generated, but the last graph is saved
A copy is created when edges/vertices are added/deleted outside the editor

UndoMax  : # of save graphs
UndoSave : index of graph to restore [1,UndoMax]
Should always be possible to restore 1
*/

//static char undofile[L_tmpnam] = "/tmp/undo_XXXXXX" ;
#ifndef _WINDOWS
static char undofile[L_tmpnam] = "/tmp/_undo.tgf" ;
#else
static char undofile[L_tmpnam] = "_undo.tgf" ;
#endif

void pigaleWindow::UndoInit()
  {Tgf undo_tgf;
  undo_tgf.open(undofile,Tgf::create);
  DebugPrintf("Debug Messages\nUndoFile:%s",undofile);
  }
void pigaleWindow::Undo()
  {if (UndoIndex > UndoMax) UndoSave();
  if(UndoIndex > 1)--UndoIndex;
  if(IO_Read(0,GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  this->undoR->setEnabled(UndoMax != 0 && UndoIndex < UndoMax);
  }
void pigaleWindow::UndoTouch(bool save)
  {if (UndoIndex<=UndoMax) {UndoMax=UndoIndex; UndoIndex++;banner();}
  else if (save) { UndoSave(); UndoIndex++;banner();}
  }
void pigaleWindow::UndoSave()
  {if(!IsUndoEnable)return;
  TopologicalGraph G(GC);
  if(G.nv() == 0)return;
  int nb = IO_GetNumRecords(0,undofile); if (nb<0) nb=0;
  int last = UndoMax>UndoIndex ? UndoIndex : UndoMax;
  if(last < nb)
      {Tgf file;
      if(file.open(undofile, Tgf::old) == 0)return;
      while (last < nb)
          {file.DeleteRecord(nb);
          nb--;
          }
      }
  if(IO_Save(0,G,undofile) == 1)
      {handler(A_SET_UNDO);return;}
  UndoIndex=UndoMax=++nb;
  undoL->setEnabled(true);
  banner();
  }
void pigaleWindow::Redo()
  {if(UndoIndex < UndoMax)++UndoIndex;
    if(IO_Read(0,GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  undoR->setEnabled(UndoIndex < UndoMax);
  }
void pigaleWindow::UndoClear()
  {if(!IsUndoEnable)return;
  UndoIndex = 0;
  UndoMax = 0;
  undoL->setEnabled(false);
  undoR->setEnabled(false);
  // empty the tgf file
  Tgf undo_tgf;
  undo_tgf.open(undofile,Tgf::create);
  }
void pigaleWindow::UndoEnable(bool enable)
  {this->undoS->setEnabled(enable);
  if(!enable)
      UndoClear();
  else
      {IsUndoEnable = true;UndoSave();}
  banner();
  IsUndoEnable = enable;
  }
// UndoErase is called when the program exit
void UndoErase()
  {QFileInfo fi(undofile);
  if(!fi.exists())return;
  QFile undo_tgf(undofile);
  undo_tgf.remove();
  }

