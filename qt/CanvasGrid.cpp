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
#include <QT/MyQcolors.h> 
#include <QT/Misc.h> 
#include <QT/MyCanvas.h>
#include <QT/GraphWidgetPrivate.h>
#include "mouse_actions.h"
#include "gprop.h"
#include <TAXI/Tmessage.h> 
#include <qtabwidget.h>


// in HeapSort.cpp
void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);
static bool Equal(double x, double y);
static bool Equal(double x, double y,double p);
static bool Less(double x, double y);
static int Cmp(int i,int j);

const double Epsilon = 1.E-6;
static double * coord;


bool Equal(double x, double y)
  {if(fabs(x-y) <= Epsilon)return true;
  return false;
  }
bool Equal(double x, double y,double p)
  {if(fabs(x-y) <= p)return true;
  return false;
  }
bool Less(double x, double y)
  {if(!Equal(x,y) && x < y)return true;
  return false;
  }
int Cmp(int i,int j)
  {double a = coord[i];
  double b = coord[j];
  if(Less(a,b))return 1;
  return 0;
  }
double pgcd(double a, double b,double precision)
  {double small = Min(a,b);
  double big = Max(a,b);
  if(Equal(small,.0,precision))return big;
  double r;
  int div;
  for(;;)
      {div = (int) (.5 + big/small);
      r = fabs(big -small*div);
      if(Equal(r,.0,precision))return small;
      big = small;
      small = r;
      }
  }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Methods of GraphWidget
GraphWidget::GraphWidget(QWidget *parent,const char *name,MyWindow *mywindow)
    : QWidget( parent, name )
  {d = new GraphWidgetPrivate;
  d->mywindow = mywindow;
  d->NodeColorItem.resize(1,16);
  d->EdgeColorItem.resize(1,16);
  d->EdgeThickItem.resize(1,3);
  d->GW = this;
  }
GraphWidget::~GraphWidget()
  {delete d->pGG;   delete d;}
void GraphWidget::update()
  {if(!d->is_init)
      {d->editor = new GraphEditor(d,this);
      d->is_init = true;
      }
  else if(d->mywindow->MacroLooping)return;
  else
      delete d->pGG;
  
  d->pGG = new GeometricGraph(d->mywindow->GC);
  d->moving_item = 0;  d->curs_item = 0;  d->info_item = 0; d->moving_subgraph = false;
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  d->SizeGrid = d->FitSizeGrid = d->OldFitSizeGrid = 100;
  d->ForceGrid =false;
  d->OldFitToGrid =false;
  d->RedrawGrid =true;
  d->editor->zoom = 1.;
  d->editor->initialize();
  d->mywindow->tabWidget->showPage(this);
  }
void GraphWidget::refresh()
  {if(!d->is_init)return;
  d->editor->refresh();
  }  
void GraphWidget::print(QPrinter *printer)
  {if(!d->is_init)return;
  d->editor->print(printer);
  }
void GraphWidget::EraseColorVertices()
  {if(d->mywindow->MacroLooping)return;
  d->mywindow->UndoSave();
   d->editor-> EraseColorVertices();
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  }
void GraphWidget::EraseColorEdges()
  {if(d->mywindow->MacroLooping)return;
  d->mywindow->UndoSave();
  d->editor-> EraseColorEdges();
  }
void GraphWidget::EraseThickEdges()
  {if(d->mywindow->MacroLooping)return;
  d->mywindow->UndoSave();
  d->editor-> EraseThickEdges();
  }
void GraphWidget::resizeEvent(QResizeEvent* e)
  {if(d->mywindow->MacroLooping)return;
  if(d->editor)d->editor->initialize();
  QWidget::resizeEvent(e);
  }
void GraphWidget::showgridChanged(bool show)
  {if(d->mywindow->MacroLooping)return;
  d->ShowGrid = show;
  d->editor->showGrid(d->ShowGrid);
  }
void GraphWidget::fitgridChanged(bool fit)
  {if(d->mywindow->MacroLooping)return;
  d->FitToGrid = fit;
  }
void GraphWidget::ForceGrid()
  {if(d->mywindow->MacroLooping)return;
  d->ForceGrid = true;
  d->ForceGridOk = true;
  d->OldFitToGrid = (d->mywindow->mouse_actions->ButtonUndoGrid->isEnabled()) ? true : false;
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(false);
  //d->editor->zoom = 1;
  d->editor->load();
  if(d->ForceGridOk)
      {d->OldFitSizeGrid = d->FitSizeGrid;
      d->FitSizeGrid = d->SizeGrid;
      }
  d->ForceGrid = false;
  if(!d->ForceGridOk)UndoGrid();
  }
void GraphWidget::UndoGrid()
  {if(d->mywindow->MacroLooping)return;
  d->SizeGrid = d->OldFitSizeGrid;
  d->editor->UndoGrid();
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(d->OldFitToGrid);
  }
void GraphWidget::sizegridChanged(int sg)
  {if(d->mywindow->MacroLooping)return;
  if(!d->is_init || d->CanvasHidden)return;
  if(!d->RedrawGrid){d->RedrawGrid = true;return;}
  // should not be called if graph on a rectangular grid unless asked
  d->SizeGrid = sg;
  if(d->FitToGrid && d->SizeGrid != d->FitSizeGrid)//and we are sure that ButtonFitGrid exists
      d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->editor->clearGrid();
  d->editor->DrawGrid(true);// draw a square grid
  }


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Methods of GraphEditor
void GraphWidget::zoom()
  {if(d->mywindow->MacroLooping)return;
  d->editor->zoom *= 1.1;
  d->editor->load(true);
  }
void GraphWidget::ozoom()
  {if(d->mywindow->MacroLooping)return;
  d->editor->zoom = 1;
  d->editor->load(true);
  }
void GraphWidget::uzoom()
  {if(d->mywindow->MacroLooping)return;
  d->editor->zoom /= 1.1;
  d->editor->load(true);
  }
void GraphEditor::showGrid(bool showgrid)
  {if(!canvas())return;
  QCanvasItemList list = gwp->canvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  if(showgrid)
      for (; it != list.end(); ++it)
	  {if((int)(*it)->rtti() == line_rtti)(*it)->show();}
  else
      for (; it != list.end(); ++it)
	 {if((int)(*it)->rtti() == line_rtti)(*it)->hide();}
  canvas()->update();
  }
void GraphEditor::clearGrid()
  {if(!canvas() || !GridDrawn)return;
  QCanvasItemList list = gwp->canvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it)
      {if((int)(*it)->rtti() == line_rtti)
	  if(*it)delete *it;
      }
  GridDrawn = false;
  }
void GraphEditor::UndoGrid()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
//   G.vcoord = scoord;
//   load();
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  double dx,dy;
  for(tvertex v = 1; v <= G.nv();v++)
      {dx = scoord[v].x() - G.vcoord[v].x();
      dy = scoord[v].y() - G.vcoord[v].y();
      nodeitem[v]->moveBy(dx,-dy);
      }
  G.vcoord = scoord; //needed 
  InitGrid();
  canvas()->update(); 
  }
void GraphEditor::UpdateSizeGrid()
  {GeometricGraph & G = *(gwp->pGG);
  int nxstep_old = nxstep;
  int nystep_old = nystep;
  max_used_x = min_used_x = G.vcoord[1].x(); 
  max_used_y = min_used_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {min_used_x = Min(min_used_x,G.vcoord[i].x());
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      min_used_y = Min(min_used_y,G.vcoord[i].y());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }
  nxstep = (int)(.5 + (max_used_x - min_used_x)/xstep);
  nystep = (int)(.5 + (max_used_y - min_used_y)/ystep);
  if(nxstep == nxstep_old && nystep == nystep_old)return; 
  //Update the display
  int nstep = Max(nxstep,nystep);
  gwp->SizeGrid = nstep;
  gwp->RedrawGrid = false; //as the slider will send a message
  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
  }
int GraphEditor::InitGrid()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
  double pos,prevpos;
  int npos;
  int NeedNormalise = 0;
  int i,ns,n = G.nv();
  if(G.nv() == 0)
      {x_min = y_min = BORDER;
      x_max = gwp->canvas->width() -BORDER -space -sizerect;
      y_max = gwp->canvas->height() - BORDER;
      }
  xminstep = (x_max-x_min)/gwp->SizeGrid;  yminstep = (y_max-y_min)/gwp->SizeGrid;
  xepsilon = Epsilon / xminstep;  yepsilon = Epsilon / yminstep;
  if(gwp->ForceGrid)scoord = G.vcoord;
  
  if(G.nv() <= 1)//in particular new graph
      {min_used_x = x_min;	max_used_x = x_max;
      min_used_y = y_min;	max_used_y = y_max;
      xstep = xminstep; nxstep = (int)(.5 + (max_used_x - min_used_x)/xstep);
      ystep = yminstep; nystep = (int)(.5 + (max_used_y - min_used_y)/ystep);
      IsGrid = true;
      //Update the display
      int nstep = Max(nxstep,nystep);nstep = Min(nstep,30);
      gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
      gwp->mywindow->mouse_actions->Slider->setValue(nstep);
      if(GridDrawn)clearGrid();
      DrawGrid(false);
      return 0; 
      }
  int genus0 = -1;
  if(gwp->ForceGrid && G.CheckConnected() && G.CheckSimple())
      genus0 = G.ComputeGeometricCir();
  int *heap = new int[n+1];    coord = new double[n+1];
  // x-coordinates
  for(i = 1;i <= n;i++)coord[i] = G.vcoord[i].x();
  HeapSort (Cmp,1,n,heap);
  min_used_x = G.vcoord[heap[0] + 1].x();
  max_used_x = G.vcoord[heap[n-1] + 1].x();
  xstep = max_used_x - min_used_x;
  prevpos = G.vcoord[heap[0] + 1].x();
  for(i = 1;i < n;i++)
      {pos = G.vcoord[heap[i] + 1].x();
      if(Equal(pos,prevpos,xepsilon))continue;
      else  xstep = Min(xstep,pos-prevpos); 
      prevpos = pos;
      }
  xstep = Max(xstep,xminstep);
  // Check if Grid or snap to it
  IsGrid = true;
  for(i = 1;i <= n;i++)
      {pos = G.vcoord[i].x() - min_used_x;
      npos = pos>0 ? (int) (.5 + pos/xstep) :(int) (-.5 + pos/xstep) ;
      if(!Equal(pos,npos*xstep))
	  {if(gwp->ForceGrid)
	      {double r =  fabs(pos -npos*xstep);
	      double a = pgcd(xstep,r,xminstep);
	      ns = (int)(xstep/a + .5);
	      xstep = xstep/ns;
	      npos = pos>0 ?(int)(.5 + pos/xstep):(int) (-.5 + pos/xstep); 
	      G.vcoord[i].x() = min_used_x + npos*xstep;
	      }
	  else {IsGrid = false;break;}
	  }
      }

  // y-coordinates
  for(i = 1;i <= n;i++)coord[i] = G.vcoord[i].y();
  HeapSort (Cmp,1,n,heap);
  min_used_y = G.vcoord[heap[0] + 1].y();
  max_used_y = G.vcoord[heap[n-1] + 1].y();
  ystep = max_used_y - min_used_y;
  prevpos = G.vcoord[heap[0] + 1].y();
  for(i = 1;i < n;i++)
      {pos = G.vcoord[heap[i] + 1].y();
      if(Equal(pos,prevpos,yepsilon))continue;
      else  ystep = Min(ystep,pos-prevpos); 
      prevpos = pos;
      } 
  ystep = Max(ystep,yminstep);
  // Check if Grid or snap to it
  for(i = 1;i <= n;i++)
      {pos = G.vcoord[i].y() - min_used_y;
      npos = pos>0 ? (int) (.5 + pos/ystep) :(int) (-.5 + pos/ystep) ;
      if(!Equal(pos,npos*ystep))
	  {if(gwp->ForceGrid)
	      {double r =  fabs(pos -npos*ystep);
	      double a = pgcd(ystep,r,yminstep);
	      ns = (int)(ystep/a + .5);
	      ystep = ystep/ns;
	      npos = pos>0 ?(int)(.5 + pos/ystep):(int) (-.5 + pos/ystep); 
	      G.vcoord[i].y() = min_used_y + npos*ystep;
	      }
	  else {IsGrid = false;break;}
	  }
      }
  delete [] heap;    delete [] coord;

  //Check Overlapping vertices
  bool overlap = false;
  bool genuschanged = false;
  if(gwp->ForceGrid && !CheckCoordNotOverlap(G))
      {Tprintf("Forcegrid (%d) -> OVERLAPPING",gwp->SizeGrid);
      overlap = true;
      }
  //ForceGrid == true and connected graph
  if(!overlap && genus0 != -1 && genus0 != G.ComputeGeometricCir()) 
      {Tprintf("Forcegrid (%d) -> modified genus",gwp->SizeGrid);
      genuschanged = true;
      }
  if(overlap || genuschanged)
      {gwp->ForceGridOk = false;IsGrid = false;}


  //Check if the extreme points changed
  double  maxused_x,maxused_y;
  maxused_x = G.vcoord[1].x(); 
  maxused_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {maxused_x = Max(maxused_x,G.vcoord[i].x());
      maxused_y = Max(maxused_y,G.vcoord[i].y());
      }
  //if(!Equal(max_used_x,maxused_x) || !Equal(max_used_y,maxused_y))
  if(max_used_x < maxused_x || max_used_y < maxused_y)
      {NeedNormalise = 1;
      max_used_x = maxused_x; max_used_y = maxused_y;
      }
  nxstep = (int) (.5 + (max_used_x - min_used_x)/xstep);
  nystep = (int) (.5 + (max_used_y - min_used_y)/ystep);
  if(IsGrid)
      {if(!overlap)Tprintf("Graph on Grid=%dx%d",nxstep,nystep);
      gwp->RedrawGrid = false;
      }
  //Update the display
  int nstep = Max(nxstep,nystep);nstep = Min(nstep,100);
  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
  gwp->SizeGrid = nstep;
  if(IsGrid)gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(true);

  if(NeedNormalise)Normalise();
  if(GridDrawn)clearGrid();
  DrawGrid(false);//Do not init the grid
  return NeedNormalise;
  }

void GraphEditor::ToGrid(tvertex &v)
  //Only called when moving or creating vertices
  {if(!IsGrid || !gwp->FitToGrid)return;
    GeometricGraph & G = *(gwp->pGG);
    double pos;
    int npos;
    pos = G.vcoord[v].x() - min_used_x;  
    npos = pos>0 ? (int) (.5 + pos/xstep) :(int) (-.5 + pos/xstep) ;
    G.vcoord[v].x() =  min_used_x + npos*xstep;
    if(pos < 0)min_used_x = G.vcoord[v].x();
    else max_used_x = Max( max_used_x,G.vcoord[v].x());
    pos = G.vcoord[v].y() - min_used_y;        
    npos = pos>0 ? (int) (.5 + pos/ystep) :(int) (-.5 + pos/ystep) ;
    G.vcoord[v].y() =  min_used_y + npos*ystep;
    if(pos < 0)min_used_y = G.vcoord[v].y();
    else max_used_y = Max( max_used_y,G.vcoord[v].y());
    }
void GraphEditor::Normalise()
  {//qDebug("normalise:%d",DoNormalise);
  if(!DoNormalise)return;
  GeometricGraph & G = *(gwp->pGG);
  if(!G.nv())return;
  x_min = BORDER;
  x_max = zoom*gwp->canvas->width() -BORDER -space -sizerect;
  y_min = BORDER;
  y_max = zoom*gwp->canvas->height() - BORDER;

  max_used_x = min_used_x = G.vcoord[1].x(); 
  max_used_y = min_used_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {min_used_x = Min(min_used_x,G.vcoord[i].x());
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      min_used_y = Min(min_used_y,G.vcoord[i].y());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }
  double xmul,xtr,ymul,ytr;
  if(max_used_x > min_used_x + 1E-5)
      {xmul = (x_min - x_max)/(min_used_x - max_used_x);
      xtr  = (min_used_x*x_max - max_used_x*x_min)/(min_used_x - max_used_x);
      }
  else
      {xmul = 1.;
      xtr = (x_max + x_min)/2.-min_used_x;
      }

  if(max_used_y > min_used_y + 1E-5)
      {ymul = (y_min - y_max)/(min_used_y - max_used_y);
      ytr  = (min_used_y*y_max - max_used_y*y_min)/(min_used_y - max_used_y);
      }
  else
      {ymul = 1.;
      ytr = (y_max + y_min)/2.-min_used_y;
      }
  max_used_y = 0;
  max_used_x = 0;
  for (int i = 1;i <= G.nv();i++)
      {G.vcoord[i].x() = xmul*G.vcoord[i].x() + xtr;
      G.vcoord[i].y() = ymul*G.vcoord[i].y() + ytr;
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }
  //qDebug("Norm xmax:%f ymax:%f",max_used_x,max_used_y);
  }

