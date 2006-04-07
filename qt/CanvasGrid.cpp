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

#include "pigaleWindow.h"
#include "GraphWidget.h"
#include <QT/pigaleQcolors.h> 
#include <QT/Misc.h> 
#include <QT/pigaleCanvas.h>
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
//Methods of GraphEditor

void GraphEditor::showGrid(bool showgrid)
  {if(!canvas())return;
  Q3CanvasItemList list = gwp->canvas->allItems();
  Q3CanvasItemList::Iterator it = list.begin();
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
  Q3CanvasItemList list = gwp->canvas->allItems();
  Q3CanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it)
      {if((int)(*it)->rtti() == line_rtti)
	  if(*it)delete *it;
      }
  GridDrawn = false;
  }
void GraphEditor::UndoGrid()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
  // move back the vertices
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
  double dx,dy;
  for(tvertex v = 1; v <= G.nv();v++)
      {dx = scoord[v].x() - G.vcoord[v].x();
      dy = scoord[v].y() - G.vcoord[v].y();
      nodeitem[v]->moveBy(dx,-dy);
      }
  G.vcoord = scoord;  
  max_used_x = min_used_x = G.vcoord[1].x(); 
  max_used_y = min_used_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {min_used_x = Min(min_used_x,G.vcoord[i].x());
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      min_used_y = Min(min_used_y,G.vcoord[i].y());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }
  old_grid = current_grid = undo_grid;
  nxstep = (int)((max_used_x-min_used_x)/current_grid.delta.x()+.5);
  nystep = (int)((max_used_y-min_used_y)/current_grid.delta.y()+.5);
  //Update the display
  gwp->mywindow->mouse_actions->LCDNumberX->display(nxstep);
  gwp->mywindow->mouse_actions->LCDNumberY->display(nystep);
  int nstep = Max(nxstep,nystep);
  gwp->SizeGrid = nstep;
  gwp->RedrawGrid = false; //as the slider will send a message
  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
  DrawGrid(current_grid);
  canvas()->update(); 
  }
void GraphEditor::UpdateSizeGrid()
// called when end moving a vertex forced on a grid
  {if(!IsGrid)return;
  return;
  int nxstep_old = nxstep;
  int nystep_old = nystep;
  GeometricGraph & G = *(gwp->pGG);
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
  int nstep = Max(nxstep,nystep); gwp->SizeGrid = nstep;
  gwp->RedrawGrid = false; //as the slider will send a  message
  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
  }
void GraphEditor::PrintSizeGrid()
  {if(!IsGrid)return;
  GeometricGraph & G = *(gwp->pGG);
  if(!G.nv())return;
  double minused_x,maxused_x, minused_y,maxused_y;
  maxused_x = minused_x = G.vcoord[1].x(); 
  maxused_y = minused_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {minused_x = Min(minused_x,G.vcoord[i].x());
      maxused_x = Max(maxused_x,G.vcoord[i].x());
      minused_y = Min(minused_y,G.vcoord[i].y());
      maxused_y = Max(maxused_y,G.vcoord[i].y());
      }
  int nx = (int)(.5 + (maxused_x - minused_x)/xstep);
  int ny = (int)(.5 + (maxused_y - minused_y)/ystep);
  gwp->mywindow->mouse_actions->LCDNumberX->display(nx);
  gwp->mywindow->mouse_actions->LCDNumberY->display(ny);
  }
bool GraphEditor::InitGrid(Tgrid &g)
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_CANVAS_COORD);
  svector<tbrin> cir0,cir1;
  double pos,prevpos;
  int npos;
  int NeedNormalise = 0;
  int i,ns,n = G.nv();
  int m = G.ne();

  if(n == 0)
      {x_min = y_min = BORDER;
      x_max = gwp->canvas->width() -BORDER -space -sizerect;
      y_max = gwp->canvas->height() - BORDER;
      }
  xminstep = (x_max-x_min)/gwp->SizeGrid;  yminstep = (y_max-y_min)/gwp->SizeGrid;
  xepsilon = Epsilon / xminstep;  yepsilon = Epsilon / yminstep;
  
  if(n == 0)// new graph
      {min_used_x = x_min;	max_used_x = x_max;
      min_used_y = y_min;	max_used_y = y_max;
      xstep = xminstep; nxstep = (int)(.5 + (max_used_x - min_used_x)/xstep);
      ystep = yminstep; nystep = (int)(.5 + (max_used_y - min_used_y)/ystep);
      IsGrid = true;
      //Update the display
      int nstep = Max(nxstep,nystep);nstep = Min(nstep,30);
      gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
      gwp->mywindow->mouse_actions->Slider->setValue(nstep);
      g = Tgrid(xstep,ystep,min_used_x,min_used_y);
      old_grid = current_grid = g;
      return true; 
      }

  // Save coords and compute the geometric cir
  if(gwp->ForceGrid)
      {scoord = G.vcoord;
      cir0.resize(-m,+m);      
      ComputeGeometricCir(G,cir0);
      }

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
  bool cir_changed,overlap;
  overlap = !CheckCoordNotOverlap(G); // in case there were overlapping
  if(overlap)IsGrid = false;
  if(overlap)Tprintf("Forcegrid (%d) -> OVERLAPPING",gwp->SizeGrid);
  if(gwp->ForceGrid) 
      {cir1.resize(-m,+m);      
      ComputeGeometricCir(G,cir1);
      cir_changed = (cir0 == cir1) ? false : true;
      if(cir_changed)Tprintf("Forcegrid (%d) -> MODIFIED CIR",gwp->SizeGrid);
      if(overlap || cir_changed) 
	  {gwp->ForceGridOk = false;      //IsGrid = false;
	  G.vcoord = scoord;
	  // reset nxstep and nystep
	  undo_grid = current_grid = old_grid;
	  nxstep = (int)((max_used_x-min_used_x)/current_grid.delta.x()+.5);
	  nystep = (int)((max_used_y-min_used_y)/current_grid.delta.y()+.5);
	  int nstep = Max(nxstep,nystep);nstep = Min(nstep,100);
 	  gwp->SizeGrid = nstep;
	  gwp->RedrawGrid = false;
	  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
	  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  return false;
	  }
      }

  //Check if the extreme points changed
  double  maxused_x,maxused_y;
  maxused_x = G.vcoord[1].x(); 
  maxused_y = G.vcoord[1].y();
  for (i = 2;i <= G.nv();i++)
      {maxused_x = Max(maxused_x,G.vcoord[i].x());
      maxused_y = Max(maxused_y,G.vcoord[i].y());
      }
  if(max_used_x < maxused_x || max_used_y < maxused_y)
      {NeedNormalise = 1;
      max_used_x = maxused_x; max_used_y = maxused_y;
      }
  nxstep = (int) (.5 + (max_used_x - min_used_x)/xstep);
  nystep = (int) (.5 + (max_used_y - min_used_y)/ystep); 

  undo_grid = old_grid;

  //Update the menu
  if(IsGrid)
      {gwp->mywindow->mouse_actions->LCDNumberX->display(nxstep);
      gwp->mywindow->mouse_actions->LCDNumberY->display(nystep);
      gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(true);
      }
  // if IsGrid == false, may be we should not have modified xstep and ystep ?
  gwp->RedrawGrid = false;
  int nstep = Max(nxstep,nystep);nstep = Min(nstep,100);
  gwp->mywindow->mouse_actions->LCDNumber->display(nstep);
  gwp->mywindow->mouse_actions->Slider->setValue(nstep);
  gwp->SizeGrid = nstep;
  if(NeedNormalise)
      {Normalise();
      if(IsGrid){xstep = (max_used_x - min_used_x)/nxstep; ystep = (max_used_y - min_used_y)/nystep;}
      }
  g = Tgrid(xstep,ystep,min_used_x,min_used_y);
  old_grid = g;
  return true;
  }

Tgrid GraphEditor::ParamGrid(int nstep)
  {nxstep=nystep=nstep;
  if(max_used_x ==  min_used_x) nxstep = 30;
  else if ((xstep =( max_used_x - min_used_x)/nxstep) < gwp->canvas->width()/100)
      {nxstep=(int)((max_used_x - min_used_x)*100/gwp->canvas->width()+.5);
      xstep=(max_used_x - min_used_x)/nxstep;
      }
  if(max_used_y ==  min_used_y)nystep = 30; 
  else if ((ystep = (max_used_y - min_used_y)/nystep) < gwp->canvas->height()/100)
      {nystep=(int)((max_used_y - min_used_y)*100/gwp->canvas->height()+.5);
      ystep=(max_used_y - min_used_y)/nystep;
      }
  return Tgrid(xstep,ystep,min_used_x,min_used_y);
  }

void GraphEditor::ToGrid(QPoint &p)
  // called to compute cooresponding coord of the mouse on the grid
  {if(!gwp->FitToGrid)return;
    double pos;
    int npos;
    pos = p.x() - min_used_x;  
    npos = pos>0 ? (int) (.5 + pos/xstep) :(int) (-.5 + pos/xstep) ;
    p.setX((int)  (min_used_x + npos*xstep));
    pos = p.y() - min_used_y;        
    npos = pos>0 ? (int) (.5 + pos/ystep) :(int) (-.5 + pos/ystep) ;
    p.setY((int) (min_used_y + npos*ystep));
  }
void GraphEditor::ToGrid(tvertex &v)
  //Only called when moving or creating vertices
  {if(!gwp->FitToGrid){IsGrid = false;return;}
  GeometricGraph & G = *(gwp->pGG);
  double pos;
  int npos;
  pos = G.vcoord[v].x() - min_used_x;  
  npos = pos>0 ? (int) (.5 + pos/xstep) :(int) (-.5 + pos/xstep) ;
  G.vcoord[v].x() =  min_used_x + npos*xstep;
  pos = G.vcoord[v].y() - min_used_y;        
  npos = pos>0 ? (int) (.5 + pos/ystep) :(int) (-.5 + pos/ystep) ;
  G.vcoord[v].y() =  min_used_y + npos*ystep;
  }
void GraphEditor::Zoom(int dir)
  {double zoom_old = zoom;
  if(dir == -1)zoom /= 1.1;
  else if(dir == 0)zoom = 1.;
  else zoom *= 1.1;
  double zz = zoom/zoom_old;

  DoNormalise = true;  Normalise();
  xstep *= zz;  ystep *= zz;
  if(IsGrid) // Recompute coords 
      {GeometricGraph & G = *(gwp->pGG);
      double pos;
      int npos;
      for(tvertex v = 1;v <= G.nv();v++)
          {pos = G.vcoord[v].x() - min_used_x;  
          npos = pos>0 ? (int) (.5 + pos/xstep) :(int) (-.5 + pos/xstep) ;
          G.vcoord[v].x() =  min_used_x + npos*xstep;
          pos = G.vcoord[v].y() - min_used_y;        
          npos = pos>0 ? (int) (.5 + pos/ystep) :(int) (-.5 + pos/ystep) ;
          G.vcoord[v].y() =  min_used_y + npos*ystep;
          }
      }
  current_grid = Tgrid(xstep,ystep,min_used_x,min_used_y);
  load(false); // do not recompute the grid
  }
void GraphEditor::Normalise()
  {//qDebug("normalise:%d",DoNormalise);
  if(!DoNormalise)return;
  GeometricGraph & G = *(gwp->pGG);
  x_min = BORDER;
  x_max = zoom*gwp->canvas->width() -BORDER -space -sizerect;
  y_min = BORDER;
  y_max = zoom*gwp->canvas->height() - BORDER;
  if(!G.nv())return;

  max_used_x = min_used_x = G.vcoord[1].x(); 
  max_used_y = min_used_y = G.vcoord[1].y();
  int i;
  for (i = 2;i <= G.nv();i++)
      {min_used_x = Min(min_used_x,G.vcoord[i].x());
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      min_used_y = Min(min_used_y,G.vcoord[i].y());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }

  double xmul,xtr,ymul,ytr;
  if (max_used_x > min_used_x+ 1E-5)
      xmul = (x_min - x_max)/(min_used_x - max_used_x);
  else
      xmul = 1.;
  
  if(max_used_y > min_used_y + 1E-5)
      ymul = (y_min - y_max)/(min_used_y - max_used_y);
  else
      ymul = 1.;
  if (xmul/ymul > 10) xmul=ymul;
  else if (ymul/xmul > 10) ymul=xmul;

  xtr = ((x_max+x_min)-xmul*(min_used_x+max_used_x))/2;
  ytr = ((y_max+y_min)-ymul*(min_used_y+max_used_y))/2;

  max_used_x = min_used_x=G.vcoord[1].x()= xmul*G.vcoord[1].x() + xtr;
  max_used_y = min_used_y=G.vcoord[1].y()= ymul*G.vcoord[1].y() + ytr;
  for (i = 2;i <= G.nv();i++)
      {G.vcoord[i].x() = xmul*G.vcoord[i].x() + xtr;
      G.vcoord[i].y() = ymul*G.vcoord[i].y() + ytr;
      min_used_x = Min(min_used_x,G.vcoord[i].x());
      max_used_x = Max(max_used_x,G.vcoord[i].x());
      min_used_y = Min(min_used_y,G.vcoord[i].y());
      max_used_y = Max(max_used_y,G.vcoord[i].y());
      }
  //qDebug("Norm xmax:%f ymax:%f",max_used_x,max_used_y);
  }

