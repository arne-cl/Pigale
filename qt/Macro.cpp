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

#include "MyWindow.h"
#include "GraphWidget.h"
#include <TAXI/graphs.h> 
#include <TAXI/Tgf.h> 
#include <TAXI/Tmessage.h> 
#include <QT/Misc.h> 
#include <qapplication.h> 
#include <qspinbox.h> 
#include <qstatusbar.h> 

static TSArray<int> MacroActions(4),MacroEwidth(4);
static TSArray<short> MacroVcolor(4),MacroEcolor(4);
static int MacroNumActions = 0;

void MacroRecord(int action)
  {if(action > 10000)return;
  MacroActions(++MacroNumActions) = action;
  GeometricGraph G(GetMainGraph());
  short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
  short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
  int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
  }
void MyWindow::macroHandler(int event)
  {int repeat = spinMacro->value();
  QString m;
  switch(event)
      {case 1://start recording
	   if(debug())LogPrintf("\nRecord macro\n");
	   MacroRecording = true;
	   MacroNumActions = 0;
	   break;
      case 2://stop recording
	  MacroRecording = false;
	  break;
      case 3://continue recording
	  MacroRecording = true;
	  break; 
      case 4:// play repeat times
	  for(int i = 1;i <= repeat;i++)
	      {m.sprintf("Executing: %d/%d",i,repeat);
	      //statusBar()->message(m);
	      statusBar()->clear();
	      macroPlay();
	      }
	  break;
      default:
	  break;
      }
  }
void macroDefColors(int record)
  {GeometricGraph G(GetMainGraph());
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
void MyWindow::macroPlay()
  {if(debug())LogPrintf("\nPlay macro:%d records",MacroNumActions);
  MacroRecording = false;
  
  if(MacroNumActions == 0){load(1);return;}
  MacroExecuting = true;
  int record = 1;
  if(MacroActions[record] < 500 || MacroActions[record] > 600)
      load(1); 
  while(record <= MacroNumActions)
      {macroDefColors(record);
      if(MacroExecuting && record == MacroNumActions)
	  {MacroExecuting = false;
	  // Call editor if last action is an embedding
	  if(MacroActions[record] > 205 && MacroActions[record] < 300 )
	      gw->update();
	  }
      handler(MacroActions[record++]);
      }
  information();
  }
