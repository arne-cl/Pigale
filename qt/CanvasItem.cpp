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
#include "mouse_actions.h"
#include <QT/MyQcolors.h> 
#include <QT/Misc.h> 
#include <QT/MyCanvas.h>
#include <QT/GraphWidgetPrivate.h>
#include <QT/grid.h>
#include <qtabwidget.h>

static QBrush *tb = 0;
static QPen *tp = 0;

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
void GraphWidget::update(bool compute)
// called when loading a graph compute = true
// if we only have erased eges and/or vertice compute = false
  {if(!d->is_init)
      {d->editor = new GraphEditor(d,this);
      d->is_init = true;
      }
  else if(d->mywindow->MacroLooping)return;
  else
      delete d->pGG;
  
  d->pGG = new GeometricGraph(d->mywindow->GC);
  d->moving_item = 0;  d->curs_item = 0;  d->info_item = 0; d->moving_subgraph = false;
  if(compute)
      {d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      d->SizeGrid = d->FitSizeGrid = d->OldFitSizeGrid = 100;
      d->mywindow->mouse_actions->LCDNumberX->display(100);
      d->mywindow->mouse_actions->LCDNumberY->display(100);
      d->editor->nxstep = d->editor->nystep  = d->SizeGrid;
      d->editor->GridDrawn = false;
      d->ForceGrid = d->OldFitToGrid =false;
      d->RedrawGrid =true;
      d->editor->zoom = 1.;
      d->editor->initialize();// will call d->editor->load(true);
      }
  else
      d->editor->load(false);
      
  d->mywindow->tabWidget->showPage(this);
  }

void GraphWidget::print(QPrinter *printer)
  {if(!d->is_init)return;
  d->editor->print(printer);
  }
void GraphWidget::png()
  {if(!d->is_init)return;
  d->editor->png();
  }
void GraphWidget::Spring()
  {if(!d->is_init)return;
  d->editor->Spring();
  }
void GraphWidget::SpringJacquard()
  {if(!d->is_init)return;
  d->editor->SpringJacquard();
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
  // RedrawGrid == false iff we sent the message
  d->editor->nxstep = d->editor->nystep =d->SizeGrid = sg;
  d->editor->current_grid = d->editor->ParamGrid(sg);
  if(d->FitToGrid && d->SizeGrid != d->FitSizeGrid)//and we are sure that ButtonFitGrid exists
      d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->editor->DrawGrid(d->editor->current_grid);// compute the grid
  d->editor->canvas()->update();
  }
void GraphWidget::zoom()
  {if(d->mywindow->MacroLooping)return;
  //d->editor->zoom *= 1.1;  d->editor->load(true);
  d->editor->Zoom(1);
  }
void GraphWidget::ozoom()
  {if(d->mywindow->MacroLooping)return;
  //d->editor->zoom = 1;  d->editor->load(true);
  d->editor->Zoom(0);
  }
void GraphWidget::uzoom()
  {if(d->mywindow->MacroLooping)return;
  //d->editor->zoom /= 1.1;  d->editor->load(true);
  d->editor->Zoom(-1);
  }


//*****************************************************
// all drawing functions
//*****************************************************
void CreatePenBrush()
  {if(!tb) tb = new QBrush( Qt::red );
  if(!tp) tp = new QPen( Qt::black );
  }
ColorItem:: ColorItem(GraphWidgetPrivate* g,QRect &rect,int pcolor,int bcolor,bool is_node)
    :QCanvasRectangle(rect,g->canvas)
    ,brush_color(bcolor),node(is_node)
  {gwp = g;
  tp->setColor(color[pcolor]);  tp->setWidth(3); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZ(col_z);
  show();
  }
void ColorItem::SetPenColor(int pcolor)
  {tp->setColor(color[pcolor]);tp->setWidth(3);setPen(*tp);
  }
int ColorItem::rtti() const
  {return col_rtti;
  }
void CreateColorItems(GraphWidgetPrivate* gwp,int color_node,int color_edge)
  {ColorItem *coloritem;

  int x = gwp->canvas->width() - sizerect -space;
  int y = space;
  // ColorItems for vertices
  for(int i = 1;i <= 16;i++)
      {QRect rect(x,y,sizerect,sizerect);
      if(i == color_node)
	  coloritem = new ColorItem(gwp,rect,i,i,true);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,true);
      gwp->NodeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  // ColorItems for edges
  //y = gwp->canvas->height() - 16*(sizerect + space);
  y = gwp->canvas->height() - 20*(sizerect + space);
  for(int i = 1;i <= 16;i++)
      {QRect rect(x,y,sizerect,sizerect);
      if(i == color_edge)
	  coloritem = new ColorItem(gwp,rect,i,i,false);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,false);
      gwp->EdgeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  }
ThickItem:: ThickItem(GraphWidgetPrivate* g,QRect &rect,int ewidth,int bcolor)
    :QCanvasRectangle(rect,g->canvas)
    ,brush_color(bcolor),width(ewidth)
  {gwp = g;
  tp->setColor(color[Black]);  tp->setWidth(ewidth); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZ(thick_z);
  show();
  }
void ThickItem::SetBrushColor(int bcolor)
  {tb->setColor(color[bcolor]);  setBrush(*tb); 
  }
int ThickItem::rtti() const
  {return thick_rtti;
  }
void CreateThickItems(GraphWidgetPrivate* gwp,int width_edge)
  {ThickItem *thickitem;

  int x = gwp->canvas->width() - sizerect -space;
  int y = gwp->canvas->height() - 3*(sizerect + space);
  for(int i = 1;i <= 3;i++)
      {QRect rect(x,y,sizerect,sizerecth);
      if(i == width_edge)
	  thickitem = new ThickItem(gwp,rect,i,Yellow);
      else
	  thickitem = new ThickItem(gwp,rect,i,White);
      gwp->EdgeThickItem[i] = thickitem;
      y = y + sizerect + space;
      }
  }


//********************************************************
//CursItem is used to add an edge
CursItem::CursItem(tvertex &vv,QPoint &p,GraphWidgetPrivate* g)
    : QCanvasLine(g->canvas)
  {gwp = g;
  v = vv;
  tp->setColor(Qt::green);tp->setWidth(2);setPen(*tp);
  setPoints(p.x(),p.y(),p.x(),p.y());
  setZ(curs_z);
  show();
  }
int CursItem::rtti() const
  {return curs_rtti;
  }
void CursItem::setToPoint(int x,int y)
  {setPoints(startPoint().x(),startPoint().y(),x,y);
  }
//*********************************************************
InfoItem::InfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p)
: QCanvasText(t,g->canvas)
  {gwp = g;
  setFont(QFont("lucida",gwp->fontsize));
  setTextFlags(Qt::AlignCenter);
  setColor(Qt::blue);
  //bug si texte trop long
  //qDebug("-> %s  %d:%d",(const char *) t,p.x(),p.y());
  move(p.x(),p.y());
  show();
  setZ(info_z); 
  }
int InfoItem::rtti() const
  {return node_rtti;
  }
InfoItem* CreateInfoItem(GraphWidgetPrivate* gwp,QString &t,QPoint &p)
  {QFont font = QFont("lucida",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 8;  int dy =size.height() +6;
  p.ry() -= dy;
  p.rx() = bound(p.x(),dx/2,gwp->canvas->width()-dx/2);
  p.ry() = bound(p.y(),dy/2,gwp->canvas->height()-dy/2);
  InfoItem *infoitem  = new InfoItem(gwp,t,p);
  QRect rect = QRect(p.x()-dx/2,p.y()-dy/2,dx,dy);
  QCanvasRectangle *rectitem = new QCanvasRectangle(rect,gwp->canvas);
  tp->setWidth(2); tp->setColor(Qt::red); tb->setColor(Qt::white);
  rectitem->setBrush(*tb);rectitem->setPen(*tp);
  rectitem->setZ(inforect_z);
  rectitem->show();
  infoitem->rectitem = rectitem;
  return infoitem;
  }
//**********************************************************************************
EdgeItem* CreateEdgeItem(tedge &e,GraphWidgetPrivate* g)
  {GeometricGraph & G = *(g->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
  tvertex v0 = G.vin[e];  tvertex v1 = G.vin[-e];
  int h = g->canvas->height();
  int x0 = (int)G.vcoord[v0].x();  int y0 =  (int)G.vcoord[v0].y();
  int x1 = (int)G.vcoord[v1].x();  int y1 =  (int)G.vcoord[v1].y();
  int x  = (int)(x0 * xorient + x1*(1.-xorient));
  int y  = (int)(y0 * xorient + y1*(1.-xorient));
  QColor col = color[G.ecolor[e]];
  tp->setColor(col);tp->setWidth(G.ewidth[e]);
  EdgeItem *edge0 = new EdgeItem(e,x0,h-y0,x,h-y,true,g);
  if(ShowOrientation() && eoriented[e])tp->setColor(Desaturate(col));
  EdgeItem *edge1 = new EdgeItem(e,x,h-y,x1,h-y1,false,g);
  edge0->opp = edge1;  edge1->opp = edge0;
  return edge0;
  }
EdgeItem::EdgeItem(tedge &ee,int x_from,int y_from,int x_to,int y_to,bool l
		   ,GraphWidgetPrivate* g)
    : QCanvasLine(g->canvas)
  {gwp = g;
  lower = l;
  e = ee;
  setPen(*tp);
  setPoints(x_from,y_from,x_to,y_to);
  setZ(edge_z);
  show();
  }
EdgeItem::EdgeItem(tedge &ee,GraphWidgetPrivate* g)
    : QCanvasLine(g->canvas)
  {gwp = g;
  e = ee;
  GeometricGraph & G = *(gwp->pGG);
  tp->setColor(color[G.ecolor[e]]);tp->setWidth(G.ewidth[e]);
  setPen(*tp);
  int h = gwp->canvas->height();
  setPoints((int)G.vcoord[G.vin[e]].x(),h-(int)G.vcoord[G.vin[e]].y(),
	    (int)G.vcoord[G.vin[-e]].x(),h-(int)G.vcoord[G.vin[-e]].y());
  setZ(edge_z);
  show();
  }

int EdgeItem::rtti() const
  {return edge_rtti;
  }
void EdgeItem::SetColor(QColor c)
  {GeometricGraph & G = *(gwp->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
  tp->setColor(c);tp->setWidth(G.ewidth[e]);
  setPen(*tp);
  if(lower)
      {if(eoriented[this->e] && ShowOrientation())
	  opp->SetColor(Desaturate(c));
      else
	  opp->SetColor(c);
      }
  }
void EdgeItem::setFromPoint(int x,int y)
  {setPoints(x,y,endPoint().x(),endPoint().y());
  }
void EdgeItem::setToPoint( int x, int y )
  {setPoints(startPoint().x(),startPoint().y(),x,y);
  }
//**************************************************************************
int NodeItem::rtti() const
  {return node_rtti;
  }
NodeItem* CreateNodeItem(tvertex &v,GraphWidgetPrivate* gwp)
  {GeometricGraph & G = *(gwp->pGG);
  int x = (int) G.vcoord[v].x();
  int y =  gwp->canvas->height() - (int) G.vcoord[v].y();
  QString t;
  int prop = ShowVertex();
  if(prop == -1 || !G.Set(tvertex()).exist(prop))
      t.sprintf("%2.2d",v());
  else 
      {Prop<long> label(G.Set(tvertex()),prop);
      t.sprintf("%2.2ld",label[v()]);
      }
  QFont font = QFont("lucida",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 6;  int dy =size.height() + 2;
  QRect rect = QRect(x-dx/2,y-dy/2,dx,dy);
  QColor col = color[G.vcolor[v]];
  tp->setWidth(1);tp->setColor(Qt::black); 
  tb->setColor(col);
  NodeItem* nodeitem = new NodeItem(v,gwp,rect);
  nodeitem->show();
  NodeTextItem* nodetextitem = new NodeTextItem(gwp,t);
  nodetextitem->SetColor(OppCol(col));
  nodetextitem->move(x,y);
  nodetextitem->show();
  nodeitem->nodetextitem = nodetextitem;
  nodetextitem->nodeitem = nodeitem;
  return nodeitem;
  }
NodeItem::NodeItem(tvertex &vv,GraphWidgetPrivate* g,QRect& rect)
    : QCanvasRectangle(rect,g->canvas)
  {gwp = g;
  v = vv;
  width = rect.width();
  height = rect.height();
  setBrush(*tb);  setPen(*tp);
  setZ(node_z);
  }
void NodeItem::SetColor(QColor c)
  {tb->setColor(c);
  setBrush(*tb);
  this->nodetextitem->SetColor(OppCol(c));
  }
void NodeItem::moveTo(Tpoint &p)
  {QPoint qp = QRect(rect()).center();
  moveBy(p.x() - qp.x(),gwp->canvas->height() - p.y() - qp.y());
  }
void NodeItem::moveBy(double dx, double dy)
//Move the left upper part of the item and
// set the coordinates to the center
  {GeometricGraph & G = *(gwp->pGG);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
  int h = gwp->canvas->height();
  double nx = x() + dx; //new x position
  double ny = y() + dy;
  QCanvasRectangle::moveBy(nx-x(),ny-y());
  nx += width/2;  ny += height/2;
  G.vcoord[v].x() = nx;  G.vcoord[v].y() = h - ny;
  nodetextitem->move(nx,ny);// move the text
  // Deplacer les aretes
  EdgeItem *ei,*opp;
  int x0,y0;
  double x,y;
  for(tbrin b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
      {ei = (EdgeItem *)(edgeitem[b.GetEdge()]); //lower part
      opp = ei->opp; //upper part
      if(b() > 0)  
	  {x0 = opp->endPoint().x(); y0 = opp->endPoint().y();
	  x  = (int)(nx * xorient + x0*(1.-xorient));
	  y  = (int)(ny * xorient + y0*(1.-xorient));
	  ei->setPoints((int)nx,(int)ny,(int)x,(int)y);
	  opp->setFromPoint((int)x,(int)y);
	  }
      else  
	  {x0 = ei->startPoint().x(); y0 = ei->startPoint().y();
	  x  = (int)(x0 * xorient + nx*(1.-xorient));
	  y  = (int)(y0 * xorient + ny*(1.-xorient));
	  opp->setPoints((int)x,(int)y,(int)nx,(int)ny);
	  ei->setToPoint((int)x,(int)y);   
	  }
      }
  canvas()->update();
  }
//***************************************************************
int NodeTextItem::rtti() const
  {return ntxt_rtti;
  }
NodeTextItem::NodeTextItem(GraphWidgetPrivate* g,QString &t)
    : QCanvasText(t,g->canvas)
  {gwp = g;
  setFont(QFont("lucida",gwp->fontsize)); 
  setTextFlags(Qt::AlignCenter);
  setZ(ntxt_z);  
  }
void NodeTextItem::SetColor(QColor c)
  {setColor(c);
  }
//***************************************************************
void GraphEditor::DrawGrid(const Tgrid &grid)
// input: min_used_x, max_used_x, nxstep (id for y)
// compute xstep and the grid
  {if(GridDrawn)clearGrid();
  QCanvasLine *line;
  tp->setColor(color[Grey2]);tp->setWidth(1);
  double x0 = grid.orig.x() - (int)(grid.orig.x()/grid.delta.x())*grid.delta.x();
  double dy = gwp->canvas->height()- grid.orig.y();
  double y0 = dy - (int)(dy/grid.delta.y())*grid.delta.y();
  double x1 = gwp->canvas->width() -space -sizerect;
  x1 = (int)((x1-x0)/grid.delta.x())*grid.delta.x() + x0 +1.5;
  double y1 = gwp->canvas->height();
  y1 =  (int)((y1-y0)/grid.delta.y())*grid.delta.y() + y0 +1.5;
  double x,y;
  double i = .0;
  // we move the grid by one pisel
  //x0 -= .5; y0 -= .5;
  x0 += .5; y0 += .5;
  for(;;) //horizontales
      {y = i++ * grid.delta.y() + y0;
      if(y > y1 )break;
      line = new QCanvasLine(gwp->canvas);
      line->setPoints((int)x0,(int)y,(int)x1,(int)y);
      line->setPen(*tp); line->setZ(grid_z); 
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
  i = .0;
  for(;;) //verticales
      {x =i++*grid.delta.x() + x0;
      if(x > x1)break;
      line = new QCanvasLine(gwp->canvas);
      line->setPoints((int)x,(int)y0,(int)x,(int)y1);
      line->setPen(*tp); line->setZ(grid_z);
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
    GridDrawn = true;
  }
