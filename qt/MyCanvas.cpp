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
#include "gprop.h"
#include <TAXI/Tbase.h> 
#include <TAXI/Tmessage.h> 
#define COLORNAMES
#include <QT/MyQcolors.h> 
#include <QT/Misc.h> 

#include <QT/MyCanvas.h>
#include <qcursor.h>
#include <qprinter.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>

// in HeapSort.cpp
void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);

//in mouse_actions.cpp
int GetMouseAction_1();


static bool Equal(double x, double y);
static bool Equal(double x, double y,double p);
static bool Less(double x, double y);
static int Cmp(int i,int j);

static QBrush *tb = 0;
static QPen *tp = 0;

const int BORDER     = 30;  // free space around the graph drawing 
const int sizerect   = 12;  //size and space are used to draw the color palets
const int sizerecth  = 8;   //size and space are used to draw the thick palets
const int space      = 3;
const double xorient = .4; 
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

//***************************************************************************
class GraphWidgetPrivate
{public:
  GraphWidgetPrivate()
      {is_init = 0;
      pGG = 0;
      canvas = 0;
      editor = 0;
      SizeGrid = 100;
      CanvasHidden = false;
      }
  ~GraphWidgetPrivate()
      {delete canvas;
      }
  bool is_init;
  bool ShowGrid;
  bool FitToGrid;
  bool ForceGrid;
  bool ForceGridOk;
  bool OldFitToGrid;
  bool CanvasHidden;
  bool RedrawGrid;
  int FitSizeGrid ;
  int OldFitSizeGrid ;
  int SizeGrid ;
  QCanvas* canvas;
  NodeItem* moving_item;
  CursItem* curs_item;
  InfoItem* info_item;
  GeometricGraph* pGG;
  GraphEditor* editor;
  MyWindow *mywindow;
  bool moving_subgraph;
  GraphWidget *GW;
  svector<ColorItem *>NodeColorItem;
  svector<ColorItem *>EdgeColorItem;
  svector<ThickItem *>EdgeThickItem;
  int fontsize;
};
//************************************************************
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
  else
      delete d->pGG;
  d->mywindow->tabWidget->showPage(this);

  d->pGG = new GeometricGraph(d->mywindow->GC);
  d->moving_item = 0;  d->curs_item = 0;  d->info_item = 0; d->moving_subgraph = false;
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  d->SizeGrid = d->FitSizeGrid = d->OldFitSizeGrid = 100;

  d->ForceGrid =false;
  d->OldFitToGrid =false;
  d->RedrawGrid =true;
  d->editor->initialize();
  }
void GraphWidget::refresh()
  {if(!d->is_init)return;
  d->editor->refresh();
  }  
void GraphWidget::print()
  {d->editor->print();}
void GraphWidget::zoom()
  {d->editor->zoom *= 1.1;
  d->editor->load(true);
  }
void GraphWidget::uzoom()
  {d->editor->zoom /= 1.1;
  d->editor->load(true);
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
  //d->OldFitSizeGrid = d->FitSizeGrid;
  d->OldFitToGrid = (d->mywindow->mouse_actions->ButtonUndoGrid->isEnabled()) ? true : false;
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(false);
  //d->FitSizeGrid = d->SizeGrid;
  d->editor->load();
  if(d->ForceGridOk)
      {d->OldFitSizeGrid = d->FitSizeGrid;
      d->FitSizeGrid = d->SizeGrid;
      }
  d->ForceGrid = false;
  if(!d->ForceGridOk)UndoGrid();
  }
void GraphWidget::UndoGrid()
  {d->SizeGrid = d->OldFitSizeGrid;
  d->editor->UndoGrid();
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  d->mywindow->mouse_actions->ButtonFitGrid->setChecked(d->OldFitToGrid);
  }
void GraphWidget::sizegridChanged(int sg)
  {if(d->CanvasHidden)return;
  if(!d->RedrawGrid){d->RedrawGrid = true;return;}
  // should not be called if graph on a rectangular grid unless asked
  d->SizeGrid = sg;
  if(d->FitToGrid && d->SizeGrid != d->FitSizeGrid)//and we are sure that ButtonFitGrid exists
      d->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
  if(d->is_init)
     {d->editor->clearGrid();
     d->editor->DrawGrid(true);// draw a square grid
     }
  }
void GraphWidget::EraseColorVertices()
  {d->editor-> EraseColorVertices();
  d->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
  }
void GraphWidget::EraseColorEdges()
  {d->editor-> EraseColorEdges();
  }
void GraphWidget::EraseThickEdges()
  {d->editor-> EraseThickEdges();
  }
void GraphWidget::resizeEvent(QResizeEvent* e)
  {if(d->editor)d->editor->initialize();
  QWidget::resizeEvent(e);
  }

//**************************************************
static QColor Desaturate(QColor & col)
  {int hue,sat,val;
  col.hsv(&hue,&sat,&val);
  int val0 = 192;
  val = val0 +(int)(val*((double)(255.-val0)/255.));
  col.setHsv(hue,sat/4,val); 
  //col.setHsv((hue+180)%360,sat/4,val); 
  return col;
  }
//********************************************************
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
      {if(eoriented[this->e])
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
void NodeItem::moveBy(double dx, double dy)
//Move the left upper part of the item and
// set the coordinates to the center
  {GeometricGraph & G = *(gwp->pGG);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_EDGE);
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

//**********************************************************************************
GraphEditor::GraphEditor(GraphWidgetPrivate *g,QWidget* parent,const char* name, WFlags f)
    :QCanvasView(g->canvas,parent,name,f)
  {gwp = g;
  DoNormalise = true;
  is_init = false;
  if(!tb) tb = new QBrush( Qt::red );
  if(!tp) tp = new QPen( Qt::black );
  color_node = Yellow;
  color_edge = Black;
  width_edge = 1;
  printer = 0;
  zoom = 1.;
  }
GraphEditor::~GraphEditor()
  {delete printer;
  }
int GraphEditor::FindItem(QPoint &p,NodeItem* &node,EdgeItem* &edge)
  {int rtt;
  QCanvasItemList l=canvas()->collisions(p);
  for(QCanvasItemList::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->rtti();
      if(rtt == ntxt_rtti)      
	  {node = ((NodeTextItem *)(*it))->nodeitem;
	  rtt = node_rtti;
	  return rtt;
	  }
      else if(rtt == node_rtti) 
	  {node = (NodeItem *)(*it);
	  return rtt;
	  }
      else if(rtt == edge_rtti) 
	  {edge = (EdgeItem *)(*it);
	  return rtt;
	  }
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,EdgeItem* &edge)
  {int rtt;
  QRect rect(p.x()-3,p.y()-3,6,6);
  QCanvasItemList l=canvas()->collisions(rect);
  for(QCanvasItemList::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->rtti();
      if(rtt == edge_rtti) 
	  {edge = (EdgeItem *)(*it);
	  return rtt;
	  }
      //else  return 0;
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,ColorItem* &coloritem)
  {int rtt;
  QCanvasItemList l=canvas()->collisions(p);
  for(QCanvasItemList::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->rtti();
      if(rtt == col_rtti) 
	  {coloritem = (ColorItem *)(*it);
	  return rtt;
	  }
      else  return 0;
      }
  return 0;
  }
int GraphEditor::FindItem(QPoint &p,ThickItem* &thickitem)
  {int rtt;
  QCanvasItemList l=canvas()->collisions(p);
  for(QCanvasItemList::Iterator it=l.begin();it!=l.end(); ++it)
      {rtt = (int)(*it)->rtti();
      if(rtt == thick_rtti) 
	  {thickitem = (ThickItem *)(*it);
	  return rtt;
	  }
      else  return 0;
      }
  return 0;
  }
void GraphEditor::contentsMousePressEvent(QMouseEvent* e)
  {GeometricGraph & G = *(gwp->pGG);
  QPoint p(e->pos());
  ColorItem *coloritem;
  if(FindItem(p,coloritem) != 0)
      {if(e->button() == QMouseEvent::LeftButton)
	  {if(coloritem->node)
	      {gwp->NodeColorItem[color_node]->SetPenColor(White);
	      coloritem->SetPenColor(coloritem->brush_color);
	      color_node = coloritem->brush_color;
	      G.vcolor.definit(color_node);
	      }
	  else
	      {gwp->EdgeColorItem[color_edge]->SetPenColor(White);
	      coloritem->SetPenColor(coloritem->brush_color);
	      color_edge = coloritem->brush_color;
	      G.ecolor.definit(color_edge);
	      }
	  canvas()->update();
	  return;
	  }
      else if(e->button() == QMouseEvent::RightButton)
	  {QString t;
	  if(coloritem->node)
	      t.sprintf("Vertex color:%s",ColorName[coloritem->brush_color]);
	  else
	      t.sprintf("Edge color:%s",ColorName[coloritem->brush_color]);
	  gwp->info_item = CreateInfoItem(gwp,t,p);
#if QT_VERSION >= 300
	  this->setCursor(QCursor(Qt::WhatsThisCursor));
#endif
	  canvas()->update();
	  return;
	  }
      }
  ThickItem *thickitem;
  if(FindItem(p,thickitem) != 0)
      {if(e->button() == QMouseEvent::LeftButton)
	  {gwp->EdgeThickItem[width_edge]->SetBrushColor(White);
	  thickitem->SetBrushColor(Yellow);
	  width_edge = thickitem->width;
	  G.ewidth.definit(width_edge);
	  canvas()->update();
	  return;
	  }
      else if(e->button() == QMouseEvent::RightButton)
	  {QString t;
	  t.sprintf("Edge width:%d",thickitem->width);
	  gwp->info_item = CreateInfoItem(gwp,t,p);
#if QT_VERSION >= 300
	  this->setCursor(QCursor(Qt::WhatsThisCursor));
#endif
	  canvas()->update();
	  return;
	  }
      }
  if(e->button() == QMouseEvent::RightButton )
      {NodeItem* node;
      EdgeItem *edge;
      QString t;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0)rtt = FindItem(p,edge);//augment the collision zone
      if(rtt == node_rtti)
	  t.sprintf("vertex: %d Label: %ld",(node->v)()
		    ,G.vlabel[node->v]);
      else if(rtt == edge_rtti) 
	  t.sprintf("edge: %d Label: %ld",(edge->e)(),G.elabel[edge->e]);
      else
	   {t.sprintf("(%d,%d)",e->pos().x(),e->pos().y());
#if QT_VERSION >= 300
	   this->setCursor(QCursor(Qt::BlankCursor));
#endif
	   }
      gwp->info_item = CreateInfoItem(gwp,t,p);
      canvas()->update();
      return;
      }
  //GetMouseAction_1 returns the active radio button (0,5)
  //e->stateAfter() -> current state
  int MouseAction = GetMouseAction_1();
  bool Shift    = (e->state() ==  QMouseEvent::ShiftButton);
  bool Control  = (e->state() ==  QMouseEvent::ControlButton);
  bool SControl = (e->state() ==  (QMouseEvent::ShiftButton | QMouseEvent::ControlButton));
  if(e->button() == QMouseEvent::MidButton)//move
      MouseAction = (Shift) ? -3 : 3;
  else if(Shift && MouseAction == 1)//add
      MouseAction = 2;
  else if(Control && MouseAction == 1)//duplicate
      MouseAction = 10;
  else if(SControl && MouseAction == 1)//duplicate +
      MouseAction = 11;
  else if(Shift && MouseAction == 2)//delete
      MouseAction = 1;
  else if(Shift && MouseAction == 3)//move
      MouseAction = -3;
  else if(Shift && MouseAction == 4)//bissect
      MouseAction = 5;
  else if(Shift && MouseAction == 5)//contract
      MouseAction = 4;

  if(MouseAction == 0) // color
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
	  {rtt = FindItem(p,edge);
	  if(rtt == 0)return;
	  } 
      if(rtt == node_rtti)
	  {G.vcolor[node->v] = color_node;
	  node->SetColor(color[color_node]);
	  }
      else if(rtt == edge_rtti)
	  {G.ecolor[edge->e] = color_edge;
	  G.ewidth[edge->e]  = width_edge;
	  if(edge->lower) edge->SetColor(color[color_edge]);
	  else edge->opp->SetColor(color[color_edge]);
	  }
      canvas()->update();
      return;
      }
  else if(MouseAction == 1) // Create an edge
      {Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v;
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)// add a vertex
	  {int h = gwp->canvas->height();
	  Tpoint pp((double)p.x(),(double)(h - p.y()));
	  v = G.NewVertex(pp);
	  ToGrid(v);
	  nodeitem[v] = node  = CreateNodeItem(v,gwp);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  }
      else
	  v = node->v;
      start_position = e->pos();
      node->SetColor(Qt::red);
      gwp->curs_item = new CursItem(v,start_position,gwp);
      canvas()->update();
      }
  else if(MouseAction == 2) // Delete
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
	  {rtt = FindItem(p,edge);
	  if(rtt == 0)return;
	  } 
      if(rtt == node_rtti)
	  {G.DeleteVertex(node->v);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  }
      else
	  G.DeleteEdge(edge->e);
      load(false);
      gwp->mywindow->information();// Informations
      return;
      }
  else if(MouseAction == 3) // Move a vertex
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)return;
      gwp->moving_item = node;
      start_position = e->pos();
      return;
      }
  else if(MouseAction == -3) // Move a subgraph
      {gwp->moving_subgraph = true;
      //if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
      //  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      start_position = e->pos();
      return;
      }
  else if(MouseAction == 4) // Bissect an edge
      {if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
	  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      G.BissectEdge(edge->e);
      DoNormalise = false;
      load();
      DoNormalise = true;
      gwp->mywindow->information();// Informations
      return;
      }
  else if(MouseAction == 5) // Contract an edge
      {EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      G.ContractEdge(edge->e);
      load(false);
      gwp->mywindow->information();// Informations
      return;
      }
  else if(MouseAction == 10)//Duplicate the sugraph of the current color
      {if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
	  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      short vcol;  G.vcolor.getinit(vcol);
      G.vcolor.definit((vcol+1)%16);
      GeometricGraph & G = *(gwp->pGG);
      Tpoint translate(this->width()/2.,0);
      int n = G.nv();
      svector<tvertex> newvertex(1,n);
      for(tvertex v = 1; v <= n;v++)//translate all the graph
	  G.vcoord[v] /= 2.;
      for(tvertex v = 1; v <= n;v++)
	  {if(G.vcolor[v] != vcol)continue;
	  newvertex[v] = G.NewVertex(G.vcoord[v] + translate);
	  }
      int m = G.ne();
      tvertex v1,v2;
      for(tedge e = 1; e <= m;e++)
	  {v1 = G.vin[e];v2 = G.vin[-e];
	  if(G.vcolor[v1] == vcol && G.vcolor[v2] == vcol)
	      G.NewEdge(newvertex[v1],newvertex[v2]);
	  }
      load();
      gwp->mywindow->information();// Informations
      }
  else if(MouseAction == 11)
      //Duplicate the sugraph of the current color
      // and add edges between a new vertex and its father
      {if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
	  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      short vcol;  G.vcolor.getinit(vcol);
      G.vcolor.definit((vcol+1)%16);
      GeometricGraph & G = *(gwp->pGG);
      Tpoint translate(this->width()/2.,0);
      int n = G.nv();
      svector<tvertex> newvertex(1,n);
      for(tvertex v = 1; v <= n;v++)//translate all the graph
	  G.vcoord[v] /= 2.;
      for(tvertex v = 1; v <= n;v++)
	  {if(G.vcolor[v] != vcol)continue;
	  newvertex[v] = G.NewVertex(G.vcoord[v] + translate);
	  }
      int m = G.ne();
      tvertex v1,v2;
      for(tedge e = 1; e <= m;e++)
	  {v1 = G.vin[e];v2 = G.vin[-e];
	  if(G.vcolor[v1] == vcol && G.vcolor[v2] == vcol)
	      G.NewEdge(newvertex[v1],newvertex[v2]);
	  }
      for(tvertex v = 1; v <= n;v++)
	  {if(G.vcolor[v] != vcol)continue;
	  G.NewEdge(v,newvertex[v]);
	  }
      load();
      gwp->mywindow->information();// Informations
      }
  }
void GraphEditor::contentsMouseMoveEvent(QMouseEvent* e)
  {if(gwp->moving_item)
      {gwp->moving_item->moveBy(e->pos().x() - start_position.x(),
		       e->pos().y() - start_position.y());
      start_position = e->pos();
      canvas()->update();
      }
  else if(gwp->moving_subgraph)
      {GeometricGraph & G = *(gwp->pGG);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE);
      short vcol;  G.vcolor.getinit(vcol);
      NodeItem * node;
      for(tvertex v = 1; v <= G.nv();v++)
	  {node =(NodeItem *)nodeitem[v];
	  if(G.vcolor[node->v] != vcol)continue;
	  node->moveBy(e->pos().x() - start_position.x(),
			      e->pos().y() - start_position.y());
	  }
      start_position = e->pos();
      canvas()->update();  
      }
  else if(gwp->curs_item)
      {gwp->curs_item->setToPoint(e->pos().x(),e->pos().y());
      canvas()->update();	  
      }
  }
void GraphEditor::contentsMouseReleaseEvent(QMouseEvent* event)
  {if(gwp->info_item) //end info
      {delete gwp->info_item->rectitem;
      delete gwp->info_item;
      gwp->info_item = 0;
#if QT_VERSION >= 300
      this->setCursor(QCursor(Qt::ArrowCursor));
#endif
      canvas()->update();
      return;
      }
  if(gwp->moving_item) //end moving a vertex
      {if(!IsGrid)
	  {gwp->moving_item = 0;return;}
      GeometricGraph & G = *(gwp->pGG);
      NodeItem *node = gwp->moving_item;
      tvertex v = node->v;
      double prev_x = G.vcoord[v].x();
      double prev_y = G.vcoord[v].y();
      ToGrid(v);
      double dx =  G.vcoord[v].x() - prev_x;
      double dy =  G.vcoord[v].y() - prev_y;
      node->moveBy(dx,-dy);
      gwp->moving_item = 0;
      canvas()->update();
      return;
      }
  if(gwp->moving_subgraph == true)
      {gwp->moving_subgraph = false;
      if(!IsGrid)return;
      GeometricGraph & G = *(gwp->pGG);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE);
      tvertex v = 1;
      double prev_x = G.vcoord[v].x();
      double prev_y = G.vcoord[v].y();
      ToGrid(v);
      double dx =  G.vcoord[v].x() - prev_x;
      double dy =  G.vcoord[v].y() - prev_y;
      for(v = 1;v <= G.nv();v++)
	  nodeitem[v]->moveBy(dx,-dy);
      canvas()->update();
      return;
      }
  if(gwp->curs_item)// end creating an edge
      {GeometricGraph & G = *(gwp->pGG);
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_EDGE);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v1,v2;
      // Reset the color of starting vertex
      v1 = gwp->curs_item->v;
      node = (NodeItem *)(nodeitem[v1]); node->SetColor(color[G.vcolor[v1]]);
      QPoint p(event->pos());
      int rtt = FindItem(p,node,edge);
      v2 = (rtt != node_rtti) ? 0 : node->v;
      if(rtt != node_rtti) //create a vertex
	  {int h = gwp->canvas->height();
	  Tpoint pp((double)p.x(),(double)(h - p.y()));
	  v2 = G.NewVertex(pp);ToGrid(v2);
	  nodeitem[v2] = CreateNodeItem(v2,gwp);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  }
      else
	  v2 = node->v;
      if(v1 != v2) // Create an edge 
	  {tedge e = G.NewEdge(v1,v2);
	  edgeitem[e] = CreateEdgeItem(e,gwp);
	  }
      delete gwp->curs_item;
      gwp->curs_item = 0;
      canvas()->update();
      gwp->mywindow->information();// Informations
      return;
      }
  }
void GraphEditor::EraseColorVertices()
  {GeometricGraph & G = *(gwp->pGG);
  for(tvertex v= G.nv() ;v > 0;v--)
    if(G.vcolor[v] == color_node)G.DeleteVertex(v);
  DoNormalise = false;
  load();
  DoNormalise = true;
  gwp->mywindow->information();// Informations
  }
void GraphEditor::EraseColorEdges()
  {GeometricGraph & G = *(gwp->pGG);
  for(tedge e= G.ne();e > 0;e--)
    if(G.ecolor[e] == color_edge)G.DeleteEdge(e);
  DoNormalise = false;
  load();
  DoNormalise = true;
  gwp->mywindow->information();// Informations
  }
void GraphEditor::EraseThickEdges()
  {GeometricGraph & G = *(gwp->pGG);
  for(tedge e= G.ne();e > 0;e--)
    if(G.ewidth[e] == width_edge)G.DeleteEdge(e);
  DoNormalise = false;
  load();
  DoNormalise = true;
  gwp->mywindow->information();// Informations
  }
void GraphEditor::clear()
  {//erase all the items of the canvas, but not the canvas itself
  if(!canvas())return;
  QCanvasItemList list = gwp->canvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it)
      if(*it)delete *it;
  GridDrawn = false;
  }
void GraphEditor::showEvent(QShowEvent*)
  {if(!gwp->canvas)
      {resize(sizeHint());
      gwp->canvas = new QCanvas(contentsRect().width(),contentsRect().height());
      }
  initialize();
  if(gwp->CanvasHidden)// to assure mouse_action is initialise
      {gwp->mywindow->mouse_actions->LCDNumber->display(gwp->SizeGrid);
      gwp->mywindow->mouse_actions->Slider->setValue(gwp->SizeGrid);
      //Enable load buttons
      gwp->mywindow->left->setEnabled(true);
      gwp->mywindow->right->setEnabled(true);
      gwp->mywindow->redo->setEnabled(true);
      }
  gwp->CanvasHidden = false;
  }
void GraphEditor::hideEvent(QHideEvent*)
  {
  if(!gwp->CanvasHidden)//Disable load buttons
      {gwp->mywindow->left->setDisabled(true);
      gwp->mywindow->right->setDisabled(true);
      gwp->mywindow->redo->setDisabled(true);
      }
  gwp->CanvasHidden = true;
  if(gwp->canvas)return;
  resize(sizeHint());
  gwp->canvas = new QCanvas(contentsRect().width(),contentsRect().height());
  initialize();
  }
QSize GraphEditor::sizeHint() const
  {return QSize(gwp->GW->width(),gwp->GW->height());
  }
void GraphEditor::resizeEvent(QResizeEvent* e)
  {if(gwp->canvas)
      gwp->canvas->resize(contentsRect().width(),contentsRect().height());
  QCanvasView::resizeEvent(e);
  }
void GraphEditor::paintEvent(QPaintEvent *e)
  {QCanvasView::paintEvent(e);
  // Before the first paintEvent,the sizes are wrong
  if(!is_init){is_init = true;initialize();}
  } 
void GraphEditor::initialize()
  {resize(sizeHint()); 
  if(gwp->canvas == 0)
      gwp->canvas = new QCanvas(contentsRect().width(),contentsRect().height());
  if(!canvas())setCanvas(gwp->canvas);
  if(!is_init){canvas()->update();return;}
  //Compute the font size
  int fs = (int)((double)QMIN(gwp->canvas->width(),gwp->canvas->height())/50.); 
  if((fs%2) == 1)++fs; fs = Min(fs,10);
  gwp->fontsize = fs; 
  zoom = 1.;
 
  load();
  }
void GraphEditor::refresh()
  {if(gwp->CanvasHidden)return;
  //should update coordinates if necessary
  Normalise();
  InitGrid();
  canvas()->update(); 
  }
void GraphEditor::load(bool initgrid) 
  {if(gwp->CanvasHidden)return;
  clear();// delete all items
  canvas()->update();
  
  GeometricGraph & G = *(gwp->pGG);
  int nmaxdisplay = gwp->mywindow->graph_properties->MaxNDisplay;
  if(G.nv() > nmaxdisplay)
      {Tprintf("Too big graph nv= %d (>%d)",G.nv(),nmaxdisplay);return;}
  if(initgrid)
      {Normalise();
      InitGrid();
      }
  else
      {DrawGrid(false);
      if(gwp->ShowGrid)showGrid(true);
      }

  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE,(NodeItem *)NULL);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_EDGE,(EdgeItem *)NULL);
  //qDebug("nodeitemsize:%d",(nodeitem.vector()).SizeElmt());

   for(tvertex v = 1;v <= G.nv();v++)
       nodeitem[v] =  CreateNodeItem(v,gwp);
  for(tedge e = 1;e <= G.ne();e++)
      edgeitem[e] = CreateEdgeItem(e,gwp); 

  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
  CreateColorItems(gwp,color_node,color_edge);
  G.vcolor.definit(color_node);
  G.ecolor.definit(color_edge);
  CreateThickItems(gwp,width_edge);
  G.ewidth.definit(width_edge);
  canvas()->update();
  }
void GraphEditor::print()
  {if(!printer)printer = new QPrinter;
  if(printer->setup(this))
      {QPainter pp(printer);
      gwp->canvas->drawArea(QRect(0,0,gwp->canvas->width()-space-sizerect-20
				  ,gwp->canvas->height()),&pp,FALSE);
      }
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
void GraphEditor::UndoGrid()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_COORD_S);
//   G.vcoord = scoord;
//   load();
  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_NODE);
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
int GraphEditor::InitGrid()
  {GeometricGraph & G = *(gwp->pGG);
  Prop<Tpoint> scoord(G.Set(tvertex()),PROP_COORD_S);
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
  if(gwp->ForceGrid && G.CheckConnected())
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
  if(overlap || genuschanged)gwp->ForceGridOk = false;


  //Check if the extreme points changed
  double  maxused_x,maxused_y;
  maxused_x = G.vcoord[1].x(); 
  maxused_y = G.vcoord[1].y();
  for (int i = 2;i <= G.nv();i++)
      {maxused_x = Max(maxused_x,G.vcoord[i].x());
      maxused_y = Max(maxused_y,G.vcoord[i].y());
      }
  if(!Equal(max_used_x,maxused_x) || !Equal(max_used_y,maxused_y))
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

  if(NeedNormalise)Normalise();
  if(GridDrawn)clearGrid();
  DrawGrid(false);//Do not init the grid
  return NeedNormalise;
  }
void GraphEditor::DrawGrid(bool init)
  {//moving the slider
  if(init)nxstep = nystep = gwp->SizeGrid;  
  QCanvasLine *line;
  tp->setColor(color[Grey2]);tp->setWidth(1);
  if(max_used_x ==  min_used_x)nxstep = 30;  
  else xstep = (max_used_x - min_used_x)/nxstep;
  if(max_used_y ==  min_used_y)nystep = 30; 
  else ystep = (max_used_y - min_used_y)/nystep;
  double x0 = min_used_x - (int)(min_used_x/xstep)*xstep;
  double dy = gwp->canvas->height()- min_used_y;
  double y0 = dy - (int)(dy/ystep)*ystep;
  double x1 = gwp->canvas->width() -space -sizerect;
  x1 = (int)((x1-x0)/xstep)*xstep + x0 +1.5;
  double y1 = gwp->canvas->height();
  y1 =  (int)((y1-y0)/ystep)*ystep + y0 +1.5;
  double x,y;
  double i = .0;
  // we move the grid by one pisel
  //x0 -= .5; y0 -= .5;
  x0 += .5; y0 += .5;
  for(;;) //horizontales
      {y = i++ * ystep + y0;
      if(y > y1 )break;
      line = new QCanvasLine(gwp->canvas);
      line->setPoints((int)x0,(int)y,(int)x1,(int)y);
      line->setPen(*tp); line->setZ(grid_z); 
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
  i = .0;
  for(;;) //verticales
      {x =i++*xstep + x0;
      if(x > x1)break;
      line = new QCanvasLine(gwp->canvas);
      line->setPoints((int)x,(int)y0,(int)x,(int)y1);
      line->setPen(*tp); line->setZ(grid_z);
      if(gwp->ShowGrid) line->show();
      else line->hide();
      }
    GridDrawn = true;
    if(init)canvas()->update();
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
