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
#define COLORNAMES
#include <QT/MyQcolors.h> 
#include <QT/Misc.h> 
#include <QT/MyCanvas.h>
#include <QT/GraphWidgetPrivate.h>
#include <qcursor.h>
#include <qprinter.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfiledialog.h>

GraphEditor::GraphEditor(GraphWidgetPrivate *g,QWidget* parent,const char* name, WFlags f)
    :QCanvasView(g->canvas,parent,name,f)
  {gwp = g;
  DoNormalise = true;
  is_init = false;
  CreatePenBrush();
  color_node = Yellow;
  color_edge = Black;
  width_edge = 1;
  zoom = 1.;
  setFocusPolicy(QWidget::ClickFocus);
  }
GraphEditor::~GraphEditor()
  { }
void GraphEditor::keyPressEvent(QKeyEvent *k)
  {if(k->key() == Qt::Key_Escape)
      Tprintf("ESC");
  qDebug("Ackey pressed:%d",k->key());
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

  if(e->button() == QMouseEvent::MidButton) //move
      MouseAction = (Shift) ? -3 : 3;
  else if(Shift && MouseAction == 1)        //add
      MouseAction = -1;                     //-> delete
  else if(Control && MouseAction == 1)
      MouseAction = 10;                     //duplicate
  else if(SControl && MouseAction == 1)
      MouseAction = 11;                     //duplicate +
  else if(Shift && MouseAction == 2)        //Orient or reorient if oriented
      MouseAction = -2;                     //disorient
  else if(Shift && MouseAction == 3)        //move
      MouseAction = -3;                     //move colored edges 
  else if(Shift && MouseAction == 4)        //bissect
      MouseAction = -4;                     //contract
  if(MouseAction == 0) // color
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
	  {rtt = FindItem(p,edge);
	  if(rtt == 0)return;
	  } 
      if(rtt == node_rtti)
	  {gwp->mywindow->UndoTouch(false);
	  G.vcolor[node->v] = color_node;
	  node->SetColor(color[color_node]);
	  }
      else if(rtt == edge_rtti)
	  {gwp->mywindow->UndoTouch(false);
	  G.ecolor[edge->e] = color_edge;
	  G.ewidth[edge->e]  = width_edge;
	  if(edge->lower) edge->SetColor(color[color_edge]);
	  else edge->opp->SetColor(color[color_edge]);
	  }
      canvas()->update();
      return; 
      }
  else if(MouseAction == 1) // Start create an edge
      {gwp->mywindow->UndoTouch(false);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v;
      int h = gwp->canvas->height();
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)// add a vertex
	  {Tpoint pp((double)p.x(),(double)(h - p.y()));
	  v = G.NewVertex(pp);
	  ToGrid(v);
	  nodeitem[v] = node  = CreateNodeItem(v,gwp);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  }
      else
	  v = node->v;
      //start_position = e->pos(); // should be the vertex position if had moved by ToGrid
      start_position = QPoint((int)G.vcoord[v].x(),h-(int)G.vcoord[v].y());
      node->SetColor(Qt::red);
      gwp->curs_item = new CursItem(v,start_position,gwp);
      canvas()->update();
      }
  else if(MouseAction == -1) // Delete
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt == 0) //augment the collision zone
	  {rtt = FindItem(p,edge);
	  if(rtt == 0)return;
	  } 
      if(rtt == node_rtti)
	  {gwp->mywindow->UndoTouch(true);
	  G.DeleteVertex(node->v);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  }
      else
	  {gwp->mywindow->UndoTouch(true);
	  G.DeleteEdge(edge->e);
	  }
      load(false);
      gwp->mywindow->information();// Informations
      PrintSizeGrid();
      return;
      }
  else if(MouseAction == 3) // Start moving a vertex
      {NodeItem* node;
      EdgeItem *edge;
      int rtt = FindItem(p,node,edge);
      if(rtt != node_rtti)return;
      gwp->mywindow->UndoTouch(false);
      gwp->moving_item = node;
      start_position = e->pos();
      return;
      }
  else if(MouseAction == -3) // Start moving a subgraph
      {gwp->mywindow->UndoTouch(false);
      gwp->moving_subgraph = true;
      //if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
      //  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      start_position = e->pos();
      return;
      }
  else if(MouseAction == 4) // Bissect an edge
      {gwp->mywindow->UndoTouch(true);
      if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
	  gwp->mywindow->mouse_actions->ButtonFitGrid->setChecked(false);
      gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
      EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      G.BissectEdge(edge->e);
      DoNormalise = false;
      load(true);
      DoNormalise = true;
      gwp->mywindow->information();// Informations
      return;
      }
  else if(MouseAction == -4) // Contract an edge
      {EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      gwp->mywindow->UndoTouch(true);
      G.ContractEdge(edge->e);
      load(false);
      gwp->mywindow->information();// Informations
      return;
      }
  else if(MouseAction == 5) // Define Exterior face
	  {GeometricGraph & G = *(gwp->pGG);
	  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
	  Tpoint pp((double)p.x(),(double)(gwp->canvas->height()-p.y()));
	  if(G.FindExteriorFace(pp) == 0)return;
	  //ColorExteriorface
	  tedge e;
	  ForAllEdges(e,G)edgeitem[e]->SetColor(color[color_edge]);
	  tbrin b0 = G.extbrin();
	  tbrin b = b0;
	  do
	      {e = b.GetEdge();
	      edgeitem[e]->SetColor(color[Red]);
	      } while((b = G.cir[-b]) != b0);
	  canvas()->update();
	  return; 	  
	  }
  else if(MouseAction == 2  || MouseAction == -2) // Orient/Reverse or deorient
      {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
      EdgeItem *edge;
      int rtt = FindItem(p,edge);
      if(rtt != edge_rtti)return;
      gwp->mywindow->UndoTouch(true);
      if(MouseAction == 2)
	  {if(eoriented[edge->e])
	      {G.ReverseEdge(edge->e); eoriented[edge->e] = true;}
	  else
	      eoriented[edge->e] = true;
	  }
      else
	  eoriented[edge->e] = false;
      load(false);
      return;
      }
  else if(MouseAction == 10)//Duplicate the sugraph of the current color
      {gwp->mywindow->UndoTouch(true);
      if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
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
      load(true);
      gwp->mywindow->information();// Informations
      }
  else if(MouseAction == 11)
      //Duplicate the sugraph of the current color
      // and add edges between a new vertex and its father
      {gwp->mywindow->UndoTouch(true);
      if(gwp->FitToGrid)//and we are sure that ButtonFitGrid exists
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
      load(true);
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
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
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
  else if(gwp->curs_item) // moving the elastic cursor
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
      {if(!gwp->FitToGrid)
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
      UpdateSizeGrid();
      PrintSizeGrid();
      return;
      }
  if(gwp->moving_subgraph == true)
      {gwp->moving_subgraph = false;
      //if(!IsGrid)return;
      if(!gwp->FitToGrid)return;
      GeometricGraph & G = *(gwp->pGG);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      short vcol;  G.vcolor.getinit(vcol);
      NodeItem * node;
      // Find a vertex of the subgraph
      tvertex mv = 0;
      for(tvertex v = 1; v <= G.nv();v++)
	  {node =(NodeItem *)nodeitem[v];
	  if(G.vcolor[node->v] != vcol)continue;
	  else {mv = v;break;}
	  }
      if(!mv)return;
      double prev_x = G.vcoord[mv].x();
      double prev_y = G.vcoord[mv].y();
      ToGrid(mv);
      double dx =  G.vcoord[mv].x() - prev_x;
      double dy =  G.vcoord[mv].y() - prev_y;
      for(tvertex v = 1;v <= G.nv();v++)
	  {node =(NodeItem *)nodeitem[v];
	  if(G.vcolor[node->v] != vcol)continue;
	  node->moveBy(dx,-dy);
	  }
      canvas()->update();
      UpdateSizeGrid();
      PrintSizeGrid();
      return;
      }
  if(gwp->curs_item)// end creating an edge
      {GeometricGraph & G = *(gwp->pGG);
      Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM);
      Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM);
      NodeItem* node;
      EdgeItem *edge;
      tvertex v1,v2;
      // Reset the color of starting vertex
      v1 = gwp->curs_item->v;
      node = (NodeItem *)(nodeitem[v1]); node->SetColor(color[G.vcolor[v1]]);
      QPoint p(event->pos()); 
      ToGrid(p);
      int rtt = FindItem(p,node,edge);
      v2 = (rtt != node_rtti) ? 0 : node->v;
      if(rtt != node_rtti) //create a vertex
	  {int h = gwp->canvas->height();
	  Tpoint pp((double)p.x(),(double)(h - p.y()));
	  v2 = G.NewVertex(pp);ToGrid(v2);
	  nodeitem[v2] = CreateNodeItem(v2,gwp);
	  gwp->mywindow->mouse_actions->ButtonUndoGrid->setDisabled(true);
	  if(IsGrid)UpdateSizeGrid();
	  }
      else
	  v2 = node->v;
      if(v1 != v2) // Create an edge 
	  {tedge e = G.NewEdge(v1,v2);
	  edgeitem[e] = CreateEdgeItem(e,gwp);
	  }
      delete gwp->curs_item;      gwp->curs_item = 0;
      canvas()->update();
      gwp->mywindow->information();// Informations
      if(v1 == v2) // We have created a vertex
	  PrintSizeGrid();
      return;
      }
  }
// void GraphEditor::EraseColorVertices()
//   {GeometricGraph & G = *(gwp->pGG);
//   for(tvertex v= G.nv() ;v > 0;v--)
//     if(G.vcolor[v] == color_node)G.DeleteVertex(v);
//   //DoNormalise = false;
//   load(false);
//   //DoNormalise = true;
//   gwp->mywindow->information();// Informations
//   }
// void GraphEditor::EraseColorEdges()
//   {GeometricGraph & G = *(gwp->pGG);
//   for(tedge e= G.ne();e > 0;e--)
//     if(G.ecolor[e] == color_edge)G.DeleteEdge(e);
//   //DoNormalise = false;
//   load(false);
//   //DoNormalise = true;
//   gwp->mywindow->information();// Informations
//   }
// void GraphEditor::EraseThickEdges()
//   {GeometricGraph & G = *(gwp->pGG);
//   for(tedge e= G.ne();e > 0;e--)
//     if(G.ewidth[e] == width_edge)G.DeleteEdge(e);
//   //DoNormalise = false;
//   load(false);
//   //DoNormalise = true;
//   gwp->mywindow->information();// Informations
//   }
void GraphEditor::showEvent(QShowEvent* e)
  {if(gwp->mywindow->MacroLooping)return;
  if(gwp->CanvasHidden)// to assure mouse_action is initialise
      {gwp->mywindow->mouse_actions->LCDNumber->display(gwp->SizeGrid);
      gwp->mywindow->mouse_actions->Slider->setValue(gwp->SizeGrid);
      //Enable load buttons
//       gwp->mywindow->left->setEnabled(true);
//       gwp->mywindow->right->setEnabled(true);
//       gwp->mywindow->redo->setEnabled(true);
      }
  // need initialize in case size changed
  initialize();
  gwp->CanvasHidden = false;
  QCanvasView::showEvent(e);
  }
void GraphEditor::hideEvent(QHideEvent* e)
  {
//   if(!gwp->CanvasHidden)//Disable load buttons
//       {gwp->mywindow->left->setDisabled(true);
//       gwp->mywindow->right->setDisabled(true);
//       gwp->mywindow->redo->setDisabled(true);
//       }
  gwp->CanvasHidden = true;
  QCanvasView::hideEvent(e);
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
  {if(gwp->mywindow->MacroLooping)return;
  QCanvasView::paintEvent(e);
  // Before the first paintEvent,the sizes are wrong
  if(!is_init)
      {is_init = true;//initialize();
      resize(sizeHint());
      if(gwp->canvas == 0)
	  gwp->canvas = new QCanvas(contentsRect().width(),contentsRect().height());
      if(!canvas())setCanvas(gwp->canvas);
      canvas()->update();
      initialize();
      }
  } 
void GraphEditor::initialize()
  {if(!is_init)return; 
  // very important to set here in case size was modified while hidden
  resize(sizeHint());
  //Compute the font size
  int fs = (int)((double)QMIN(gwp->canvas->width(),gwp->canvas->height())/50.); 
  if((fs%2) == 1)++fs; fs = Min(fs,10);
  gwp->fontsize = fs; 
  load(true);
  }

void GraphEditor::load(bool initgrid) 
// by default initgrid = true
// when editng (erasing edges, vertices) initgrig = false
  {clear();// delete all items
  canvas()->update();
  //if(debug())DebugPrintf("Graph_Editor:load:%d",(int)initgrid);
  int nmaxdisplay = gwp->mywindow->graph_properties->MaxNDisplay;
  if(gwp->pGG->nv() > nmaxdisplay)
      {Tprintf("Too big graph nv= %d (>%d)",gwp->pGG->nv(),nmaxdisplay);return;}
  GeometricGraph & G = *(gwp->pGG);
  if(initgrid)
      {Normalise();
      if(InitGrid(current_grid))
	  DrawGrid(current_grid);
      else
	  DrawGrid(current_grid);
      }
  else
      {DrawGrid(current_grid);
      if(gwp->ShowGrid)showGrid(true);
      }

  Prop<NodeItem *> nodeitem(G.Set(tvertex()),PROP_CANVAS_ITEM,(NodeItem *)NULL);
  Prop<EdgeItem *> edgeitem(G.Set(tedge()),PROP_CANVAS_ITEM,(EdgeItem *)NULL);
  nodeitem.SetName("nodeitem");edgeitem.SetName("edgeitem");
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
void GraphEditor::print(QPrinter *printer)
  {if(printer->setup(this))
      {QPainter pp(printer);
      gwp->canvas->drawArea(QRect(0,0,gwp->canvas->width()-space-sizerect-20
				  ,gwp->canvas->height()),&pp,FALSE);
      }
  }

void GraphEditor::png()
  {if(index < 0)return;
  QString FileName = QFileDialog::getSaveFileName(gwp->mywindow->DirFilePng,"Images(*.png)",this);
  if(FileName.isEmpty())return; 
  if(QFileInfo(FileName).extension(false) != (const char *)"png")
      FileName += (const char *)".png";
  gwp->mywindow->DirFilePng = QFileInfo(FileName).dirPath(true);
  QPixmap pixmap = QPixmap::grabWidget(this,2,2,gwp->canvas->width()-space-sizerect-1
				       ,gwp->canvas->height()); 
  pixmap.save(FileName,"PNG");
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
void GraphEditor::clear()
  {//erase all the items of the canvas, but not the canvas itself
  if(!canvas())return;
  QCanvasItemList list = gwp->canvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it)
      if(*it)delete *it;
  GridDrawn = false;
  }
