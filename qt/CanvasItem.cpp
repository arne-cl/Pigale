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
#include "mouse_actions.h"
#include <TAXI/Tprop.h>
#include <QT/pigaleQcolors.h> 
#include <QT/Misc.h> 
#include <QT/pigaleCanvas.h>
#include <QT/GraphWidgetPrivate.h>
#include <QT/grid.h>

static QBrush *tb = 0;
static QPen *tp = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Methods of GraphWidget
GraphWidget::GraphWidget(QWidget *parent,pigaleWindow *mywindow)
    : QWidget(parent)
  {d = new GraphWidgetPrivate;
  d->mywindow = mywindow;
  d->NodeColorItem.resize(1,16);
  d->EdgeColorItem.resize(1,16);
  d->EdgeThickItem.resize(1,3);
  d->GW = this;
  }
GraphWidget::~GraphWidget()
  {delete d->pGG;   delete d;
  }
void GraphWidget::update(int compute)
// called when loading a graph compute = 1 or -1
// if we only have erased edges and/or vertice compute = 0
  {if(!d->is_init)
      {d->editor = new GraphEditor(d,this);
      d->is_init = true;
      }
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
  if(compute != -1)d->mywindow->tabWidget->setCurrentIndex(d->mywindow->tabWidget->indexOf(this));
  }

void GraphWidget::print(QPrinter *printer)
  {if(!d->is_init)return;
  d->editor->print(printer);
  }
void GraphWidget::png(int size)
  {if(!d->is_init)return;
  d->editor->png(size);
  }
void GraphWidget::Spring()
  {if(!d->is_init)return;
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->editor->Spring();
  }
void GraphWidget::SpringPreservingMap()
  {if(!d->is_init)return;
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->editor->SpringPreservingMap();
  }
void GraphWidget::SpringJacquard()
  {if(!d->is_init)return;
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
#ifndef _WINDOWS
  d->editor->SpringJacquard();
#endif
  }
void GraphWidget::resizeEvent(QResizeEvent* e)
  {if(d->editor)d->editor->initialize();
  QWidget::resizeEvent(e);
  }
void GraphWidget::showgridChanged(bool show)
  {d->ShowGrid = show;
  d->editor->showGrid(d->ShowGrid);
  }
void GraphWidget::fitgridChanged(bool fit)
  {d->FitToGrid = fit;
  }
void GraphWidget::ForceGrid()
  {d->ForceGrid = true;
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
  {d->SizeGrid = d->OldFitSizeGrid;
  d->editor->UndoGrid();
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(d->OldFitToGrid);
  }
void GraphWidget::sizegridChanged(int sg)
  {if(!d->is_init || d->CanvasHidden)return;
  if(!d->RedrawGrid){d->RedrawGrid = true;return;}
  // RedrawGrid == false iff we sent the message
  d->editor->nxstep = d->editor->nystep =d->SizeGrid = sg;
  d->editor->current_grid = d->editor->ParamGrid(sg);
  if(d->FitToGrid && d->SizeGrid != d->FitSizeGrid)//and we are sure that ButtonFitGrid exists
      d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->editor->DrawGrid(d->editor->current_grid);// compute the grid
  d->editor->scene()->update();
  }

//*****************************************************
// all drawing functions
//*****************************************************
void CreatePenBrush()
  {if(!tb) tb = new QBrush( Qt::red );
  if(!tp) tp = new QPen( Qt::black );
  }
ColorItem:: ColorItem(GraphWidgetPrivate* g,QRectF &rect,int pcolor,int bcolor,bool is_node)
  :QGraphicsRectItem(rect,0,g->canvas)
    ,brush_color(bcolor),node(is_node)
  {gwp = g;
  tp->setColor(color[pcolor]);  tp->setWidth(3); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZValue(col_z);
  }
void ColorItem::SetPenColor(int pcolor)
  {tp->setColor(color[pcolor]);tp->setWidth(3);setPen(*tp);
  }
void CreateColorItems(GraphWidgetPrivate* gwp,int color_node,int color_edge)
  {ColorItem *coloritem;
  int x = (int)gwp->canvas->width() - sizerect -space;
  int y = space;
  int i;
  // ColorItems for vertices
  for(i = 1;i <= 16;i++)
      {QRectF rect(x,y,sizerect,sizerect);
      if(i == color_node)
	  coloritem = new ColorItem(gwp,rect,i,i,true);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,true);
      gwp->NodeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  // ColorItems for edges
  //y = gwp->canvas->height() - 16*(sizerect + space);
  y = (int)gwp->canvas->height() - 20*(sizerect + space);
  for(i = 1;i <= 16;i++)
      {QRectF rect(x,y,sizerect,sizerect);
      if(i == color_edge)
	  coloritem = new ColorItem(gwp,rect,i,i,false);
      else
	  coloritem = new ColorItem(gwp,rect,White,i,false);
      gwp->EdgeColorItem[i] = coloritem;
      y = y + sizerect + space;
      }
  }
ThickItem:: ThickItem(GraphWidgetPrivate* g,QRectF &rect,int ewidth,int bcolor)
  :QGraphicsRectItem(rect,0,g->canvas)
  ,brush_color(bcolor),width(ewidth)
  {gwp = g;
  tp->setColor(color[Black]);  tp->setWidth(ewidth); setPen(*tp);
  tb->setColor(color[bcolor]);  setBrush(*tb); 
  setZValue(thick_z);
  }
void ThickItem::SetBrushColor(int bcolor)
  {tb->setColor(color[bcolor]);  setBrush(*tb); 
  }
void CreateThickItems(GraphWidgetPrivate* gwp,int width_edge)
  {ThickItem *thickitem;
  int x = (int)gwp->canvas->width() - sizerect -space;
  int y = (int)gwp->canvas->height() - 3*(sizerect + space);
  for(int i = 1;i <= 3;i++)
      {QRectF rect(x,y,sizerect,sizerecth);
      if(i == width_edge)
	  thickitem = new ThickItem(gwp,rect,i,Yellow);
      else
	  thickitem = new ThickItem(gwp,rect,i,White);
      gwp->EdgeThickItem[i] = thickitem;
      y = y + sizerect + space;
    }
  }

//********************************************************
//LineItem is used for the grid
LineItem::LineItem(GraphWidgetPrivate* g)
  :QGraphicsLineItem(0,g->canvas)
{}
//CursItem is used to add an edge
CursItem::CursItem(tvertex &_v,QPoint &p,GraphWidgetPrivate* g)
  :QGraphicsLineItem(0,g->canvas)
  {v = _v;
  tp->setColor(Qt::green);tp->setWidth(2);setPen(*tp);
  setLine(p.x(),p.y(),p.x(),p.y());
  setZValue(curs_z);
  }
void CursItem::setToPoint(int x,int y)
  {setLine(line().p1().x(),line().p1().y(),x,y);
  }

//**********************************************************************************
ArrowItem::ArrowItem( EdgeItem *edgeitem,GraphWidgetPrivate* g)
  :QGraphicsPolygonItem(0,g->canvas)
  {gwp = g;
  pts.resize(4);
  //refresh.resize(2);
  edgeItem = edgeitem;
  ComputeCoord();
  SetColor();
  setPen(*tp);
  setZValue(arrow_z);
  setFlags(0);
  }
void ArrowItem::ComputeCoord()
  {//prepareGeometryChange();
  QPointF u =  edgeItem->line().p2() - edgeItem->line().p1();
  double ml = sqrt(double(u.x()*u.x() + u.y()*u.y()))+1.5;
  double diviseur = 12;
  // for short edges or long edges
  if(ml > 10 && ml < 50){diviseur = (diviseur*ml)/50;}
  else if(ml > 100) {diviseur = (diviseur*ml)/100;}
  diviseur = Max(diviseur,1);
  QPointF v = QPointF(-u.y()/diviseur,u.x()/diviseur);
  QPointF p0 = pts[0] = edgeItem->line().p2();
  QPointF p1 = pts[1] =  p0 - v -(u*2)/diviseur;
  pts[2] = p0  -u/diviseur;
  QPointF p3 = pts[3] = p0 + v - (u*2)/diviseur;
  setPolygon(pts);
  // boundingrect
//   double xmin = Min(p0.x(),p1.x(),p3.x());  double xmax = Max(p0.x(),p1.x(),p3.x());
//   double ymin = Min(p0.y(),p1.y(),p3.y());  double ymax = Max(p0.y(),p1.y(),p3.y());
//   refresh[0] = QPointF(xmin-4,ymax+4);   
//   refresh[1] = QPointF(xmax+4,ymin-4);
  }
/*
QRectF ArrowItem::boundingRect() const
  {return QRectF(refresh[0],refresh[1]);
  }
*/
void ArrowItem::SetColor()
  {GeometricGraph & G = *(gwp->pGG);
  int col = bound(G.ecolor[ edgeItem->e],1,16);
  tp->setColor(color[col]); // needed not to get the desaturated color
  tb->setColor(color[col]); // needed not to get the desaturated color
  setPen(*tp);setBrush(*tb);
  }
void ArrowItem::SetColor(QColor col)
  {tp->setColor(col); // needed not to get the desaturated color
  tb->setColor(col); // needed not to get the desaturated color
  setPen(*tp);setBrush(*tb);
  }

//**********************************************************************************
EdgeItem* CreateEdgeItem(tedge &e,GraphWidgetPrivate* g)
  {GeometricGraph & G = *(g->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  tvertex v0 = G.vin[e];  tvertex v1 = G.vin[-e];
  double h = g->canvas->height();
  double x0 = G.vcoord[v0].x();  double y0 = G.vcoord[v0].y();
  double x1 = G.vcoord[v1].x();  double y1 = G.vcoord[v1].y();
  double x  = x0 * xorient + x1*(1.-xorient);
  double y  = y0 * xorient + y1*(1.-xorient);
  QColor col = color[bound(G.ecolor[e],1,16)];
  QColor col2 = col;
  if (G.Set(tedge()).exist(PROP_COLOR2))
      {Prop<short> ecolor2(G.Set(tedge()),PROP_COLOR2);
      ecolor2.definit(1);
      col2 = color[bound(ecolor2[e],1,16)];
      }
  // first EdgeItem representing the lower part
  tp->setColor(col);tp->setWidth(G.ewidth[e]);
  EdgeItem *edge0 = new EdgeItem(e,x0,h-y0,x,h-y,true,g);
  // second EdgeItem  representing the upper part
  if(staticData::ShowOrientation() && eoriented[e])
      {edge0->arrow->show();
      tp->setColor(Desaturate(col));
      }
  else
      {edge0->arrow->hide();
      tp->setColor(col2);
      }
  EdgeItem *edge1 = new EdgeItem(e,x,h-y,x1,h-y1,false,g);
  edge0->opp = edge1;  edge1->opp = edge0;
  return edge0;
  }
EdgeItem::EdgeItem(tedge &ee,double x_from,double y_from,double x_to,double y_to,bool _lower
		   ,GraphWidgetPrivate* g) // used when CreateEdgeItem is called (load)
  :QGraphicsLineItem(0,g->canvas)
  {gwp = g;
  lower = _lower;
  e = ee;
  setLine(x_from,y_from,x_to,y_to);
  if(lower)
      arrow = new  ArrowItem(this,g);
  else 
      arrow = NULL;
  setPen(*tp);
  setZValue(edge_z);
  setFlags(0);
  }
void EdgeItem::SetColor(QColor c,bool both)
  {GeometricGraph & G = *(gwp->pGG);
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  tp->setColor(c);tp->setWidth(G.ewidth[e]);
  setPen(*tp);
  if(lower && both)
      {if(eoriented[this->e] && staticData::ShowOrientation())
	  opp->SetColor(Desaturate(c));
      else
	  opp->SetColor(c);
      arrow->SetColor(c);
      }
  }
void EdgeItem::SetColors(QColor c1, QColor c2)
  {GeometricGraph & G = *(gwp->pGG);
    Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
    tp->setColor(c1);tp->setWidth(G.ewidth[e]);
    setPen(*tp);
    if(lower)
    opp->SetColor(c2);
 else
 opp->SetColor(c1);
  }
void EdgeItem::setFromPoint(double x,double y)
  {setLine(x,y,line().p2().x(),line().p2().y());
  }
void EdgeItem::setToPoint(double x,double y)
  {setLine(line().p1().x(),line().p1().y(),x,y);
  }

//*********************************************************
InfoItem::InfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p)
  :QGraphicsSimpleTextItem(t,0,g->canvas)
  {gwp = g;
  setFont(QFont("sans",gwp->fontsize));
  tb->setColor(Qt::blue);  setBrush(*tb); 
  setPos(p.x(),p.y());
  setZValue(info_z); 
  }
InfoItem* CreateInfoItem(GraphWidgetPrivate* gwp,QString &t,QPoint &p)
  {QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 8;  int dy =size.height() +4;
  p.ry() -= dy;
  p.rx() = bound(p.x(),dx/2,gwp->canvas->width()-dx/2);
  p.ry() = bound(p.y(),dy/2,gwp->canvas->height()-dy/2);
  QRectF rect = QRectF(p.x()-dx/2,p.y()-dy/2,dx,dy);
  QGraphicsRectItem *rectitem = new QGraphicsRectItem(rect,0,gwp->canvas);
  QPoint q  = QPoint(p.x()-dx/2+2,p.y()-size.height()/2);
  InfoItem *infoitem  = new InfoItem(gwp,t,q);
  tp->setWidth(2); tp->setColor(Qt::red); tb->setColor(Qt::white);
  rectitem->setBrush(*tb);rectitem->setPen(*tp);
  rectitem->setZValue(inforect_z);
  infoitem->rectitem = rectitem;
  return infoitem;
  }

//**************************************************************************
NodeItem::NodeItem(tvertex &_v,GraphWidgetPrivate* g,QRectF &rect,QColor &col,QString &_t)
  :QGraphicsRectItem(rect,0,g->canvas)
  {gwp = g;
  v = _v;
  t = _t;
  vcolor = col;
  tcolor = OppCol(col);
  setZValue(node_z);
  setFlags(0);
  }
void NodeItem::paint(QPainter *painter,const QStyleOptionGraphicsItem * option,QWidget *)
  {painter->setBrush(vcolor);
  painter->drawRect(option->exposedRect);
  painter->setPen(tcolor);
  painter->drawText(option->exposedRect,Qt::AlignCenter,t);
  }
NodeItem* CreateNodeItem(tvertex &v,GraphWidgetPrivate* gwp)
  {GeometricGraph & G = *(gwp->pGG);
  double x =  G.vcoord[v].x();
  double y =  gwp->canvas->height() - G.vcoord[v].y();
  QString t = getVertexLabel(G.Container(),v);
  QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx = size.width() + 4;  int dy = size.height();
  if(t.length() == 0){dx = 8; dy = 8;}
  QRectF rect = QRectF(x-dx/2,y-dy/2,dx,dy);
  QColor col = color[G.vcolor[v]];
  return  new NodeItem(v,gwp,rect,col,t);
  }
void NodeItem::SetText(QString _t)
  {t = _t;
  QFont font = QFont("sans",gwp->fontsize);
  QSize size = QFontMetrics(font).size(Qt::AlignCenter,t);
  int dx =size.width() + 4;  int dy =size.height();
  if(t.length() == 0){dx = 8; dy = 8;}
  setRect(QRectF(rect().center().x()-dx/2,rect().center().y()-dy/2,dx,dy));
  }
void NodeItem::SetColor(QColor c)
  {vcolor = c;
  update();
  }
void NodeItem::moveTo(Tpoint &p,double eps)
// does not modify vertex coordinates: used in spring embedders
  {QPointF qp = QRectF(rect()).center() + pos(); //position approximative
  double dx = p.x() - qp.x();
  double dy = gwp->canvas->height() - p.y() - qp.y();
  if(Abs(dx) < eps && Abs(dy) < eps)return;
  gwp->editor->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
  GeometricGraph & G = *(gwp->pGG);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
  double nx = p.x(); //new x position
  double ny = gwp->canvas->height() - p.y();
  QGraphicsRectItem::moveBy(dx,dy);
  // Deplacer les aretes
  EdgeItem *ei,*up;
  double x0,y0;
  double x,y;
  for(tbrin b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
      {ei = (EdgeItem *)(edgeitem[b.GetEdge()]); //lower part
      up = ei->opp; //upper part
      if(b() > 0)  
	  {x0 = up->line().p2().x(); y0 = up->line().p2().y();
	  x  = nx * xorient + x0*(1.-xorient);
	  y  = ny * xorient + y0*(1.-xorient);
	  ei->setLine(nx,ny,x,y); 
 	  up->setFromPoint(x,y); //up->setLine(x,y,x0,y0);
	  }
      else  
	  {x0 = ei->line().p1().x(); y0 = ei->line().p1().y();
	  x  = x0 * xorient + nx*(1.-xorient);
	  y  = y0 * xorient + ny*(1.-xorient);
	  up->setLine(x,y,nx,ny);
 	  ei->setToPoint(x,y); 
	  }
      ei->arrow->ComputeCoord();
      }
  gwp->editor->QGraphicsView::setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  }
 void NodeItem::moveBy(double dx, double dy)
// modify vertex coordinates
  {gwp->editor->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
  QGraphicsRectItem::moveBy(dx,dy);
  GeometricGraph & G = *(gwp->pGG);
  G.vcoord[v].x() += dx;   G.vcoord[v].y() -= dy; 
  // Deplacer les aretes
  double nx = G.vcoord[v].x();  
  double ny = gwp->canvas->height() - G.vcoord[v].y();
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
  EdgeItem *ei,*opp;
  double x0,y0;
  double x,y;
  for(tbrin b=G.FirstBrin(v);b!=0;b=G.NextBrin(v,b))
      {ei = (EdgeItem *)(edgeitem[b.GetEdge()]); //lower part
      opp = ei->opp; //upper part
      if(b() > 0)  
	  {x0 = opp->line().p2().x(); y0 = opp->line().p2().y();
	  x  = nx * xorient + x0*(1.-xorient);
	  y  = ny * xorient + y0*(1.-xorient);
	  ei->setLine(nx,ny,x,y);   
	  opp->setFromPoint(x,y);
	  }
      else  
	  {x0 = ei->line().p1().x(); y0 = ei->line().p1().y();
	  x  = x0 * xorient + nx*(1.-xorient);
	  y  = y0 * xorient + ny*(1.-xorient);
	  opp->setLine(x,y,nx,ny); 
	  ei->setToPoint(x,y);    
	  }
      ei->arrow->ComputeCoord();
      }
  gwp->editor->QGraphicsView::setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  }

//***************************************************************
void GraphEditor::DrawGrid(const Tgrid &grid)
// input: min_used_x, max_used_x, nxstep (id for y)
// compute xstep and the grid
  {if(GridDrawn)clearGrid();
  if( grid.delta.x() < 7 ||  grid.delta.y() < 7){clearGrid();return;}
  QGraphicsLineItem *line;
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
  // we move the grid by one pixel
  x0 += .5; y0 += .5;
  for(;;) //horizontales
      {y = i++ * grid.delta.y() + y0 +.5;
      if(y > y1 )break;
      line = new LineItem(gwp);
      line->setLine((int)x0,(int)y,(int)x1,(int)y);
      line->setPen(*tp); line->setZValue(grid_z); 
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
  i = .0;
  for(;;) //verticales
      {x =i++*grid.delta.x() + x0 + .5;
      if(x > x1)break;
      line = new LineItem(gwp);
      line->setLine((int)x,(int)y0,(int)x,(int)y1);
      line->setPen(*tp); line->setZValue(grid_z);
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
    GridDrawn = true;
  }
