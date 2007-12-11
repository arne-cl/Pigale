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

#ifndef _GRAPH_WIDGET_H_INCLUDED_
#define _GRAPH_WIDGET_H_INCLUDED_

/*! \file
This header contains the definition of all the classes used by the Pigale Editor
*/
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QStyleOptionGraphicsItem>
#include <TAXI/graphs.h> 
#include <TAXI/color.h> 
#include <TAXI/Tpoint.h> 
#include <QT/grid.h> 

bool & ShowOrientation();
int  & ShowVertex();

const int min_rtti = QGraphicsItem::UserType;
const int line_rtti    = min_rtti+20;
const int arrow_rtti   = min_rtti+11;
const int ntxt_rtti    = min_rtti+4;
const int node_rtti    = min_rtti+5;
const int col_rtti     = min_rtti+6;
const int edge_rtti    = min_rtti+7;
const int curs_rtti    = min_rtti+8;
const int info_rtti    = min_rtti+9;
const int thick_rtti   = min_rtti+9;

const int grid_z     =   1; // Grid
const int ntxt_z     = 128; // NodeTextItem
const int node_z     = 127; // NodeItem
const int col_z      = 2;   // ColorItem //130
const int thick_z    = 2;   // ThickItem //130
const int edge_z     = 126; // EdgeItem
const int arrow_z    = 125; // ArrowItem
const int curs_z     = 129; // CursItem
const int inforect_z = 131; // rect containing the info
const int info_z     = 132; // InfoItem

const double xorient = .4; 
const int sizerect   = 12;  //size and space are used to draw the color palets
const int sizerecth  = 8;   //size and space are used to draw the thick palets
const int space      = 1;   //spece + sizerect <= 13 for screens 800x600
const int BORDER     = 30;  // free space around the graph drawing 

class pigaleWindow; 
class QPrinter;
class NodeItem;
class EdgeItem;
class GraphEditor;
class GraphWidget;

class ArrowItem: public QGraphicsPolygonItem
//! used to display an arrow whose size and loaction depends on the edge length
{public:
  ArrowItem(EdgeItem *edgeitem);
  void ComputeCoord(); //!<  compute the shape and position of the arrow
  void SetColor(QColor col);
  int type() const {return arrow_rtti;}
 private:
  EdgeItem *edgeItem;
  QVector<QPointF>pts;
};

/*! An edge is represented by two EdgeItem (corresponding to its lower and upper part) and an ArrowItem
*/
class EdgeItem: public QGraphicsLineItem
{public:
  EdgeItem(GraphWidget* g,tedge &ee,double x_from,double y_from,double x_to,double y_to
           ,bool _lower,NodeItem *node);
  void paint(QPainter *painter,const QStyleOptionGraphicsItem * option,QWidget * widget = 0); 
  void SetColor(QColor c,bool both=true);
  void SetColors(QColor c1, QColor c2);
  void setFromPoint(double x,double y) ;
  void setToPoint(double x,double y);
  int type() const {return edge_rtti;}
  QRectF boundingRect() const;
  tedge e;
  EdgeItem *opp;
  ArrowItem *arrow;
  NodeItem *node;
  bool lower;
private:
  GraphWidget* gwp;
};

//!  used when creating an edge
class CursItem: public QGraphicsLineItem
{public:
  CursItem(tvertex &v,QPoint &p,GraphWidget* g);
  void setToPoint(int x,int y);
  int type() const {return curs_rtti;}
  tvertex v;
};

//! LineItem is used for the grid
class LineItem: public QGraphicsLineItem
{public:
  LineItem(GraphWidget* g);
  int type() const {return line_rtti;}
};

//! used to display some info
class InfoItem: public QGraphicsSimpleTextItem
{public:
  InfoItem(GraphWidget* g,QString &t,QPoint &p);
  int type() const {return info_rtti;}
  QGraphicsRectItem* rectitem;
};

//! are the coloured rectangles used to define the colors of vertices and edges
class ColorItem: public QGraphicsRectItem
{public:
  ColorItem(GraphWidget* g,QRectF &rect,int pen_color,int brush_color,bool node);
  ~ColorItem() {}
  int type() const {return col_rtti;}
  void SetPenColor(int pen_color);
  int brush_color;
  bool node;
private:
  GraphWidget* gwp;
};

//! used to modify  the width of an edge
class ThickItem: public QGraphicsRectItem
{public:
  ThickItem(GraphWidget* g,QRectF &rect,int ewidth,int brush_color);
  ~ThickItem() {}
  int type() const {return thick_rtti;}
  void SetBrushColor(int bcolor);
  int brush_color;
  int width;
private:
  GraphWidget* gwp;
};

//! used to represent a vertex with/witout a label
class NodeItem: public QGraphicsRectItem
{public:
  NodeItem(tvertex &vv,GraphWidget* g,QRectF &rect,QColor & col,QString &_t);
  QRectF boundingRect() const;
  void moveBy(double dx, double dy);
  void moveTo(Tpoint &p,double eps = .0);
  void SetColor(QColor c);
  void SetText(QString t);
  int type() const {return node_rtti;}
  void paint(QPainter *painter,const QStyleOptionGraphicsItem * option,QWidget * widget = 0); 
  tvertex v;
private:
  GraphWidget* gwp;
  QColor vcolor,tcolor;
  QString t;
};

/*!
widget containing QGraphicsScene pointer which will be populated by QGraphicsItem and
a GraphEditor pointer (derived from QGraphicsView) which displays the  QGraphicsScene
*/
class GraphWidget : public QWidget
{

  Q_OBJECT
public:
  GraphWidget( QWidget *parent=0,pigaleWindow* mw=0);
  ~GraphWidget();
private:
  void resizeEvent(QResizeEvent*); //!< resize the editor
public:
  QGraphicsScene* canvas;
  NodeItem* moving_item;
  CursItem* curs_item;
  InfoItem* info_item;
  GeometricGraph* pGG;
  GraphEditor* editor;
  bool moving_subgraph;
  svector<ColorItem *>NodeColorItem;
  svector<ColorItem *>EdgeColorItem;
  svector<ThickItem *>EdgeThickItem;
  int fontsize;
};

//!  The main class of the Pigale Editor.
/*!
  It derives from QGraphicsView and displays the QGraphicsScene constructed by its parent
*/
class GraphEditor : public QGraphicsView
{
  Q_OBJECT
public:
  GraphEditor(GraphWidget*parent,pigaleWindow *mywindow);
  ~GraphEditor(){};
  QSize sizeHint() const;
  void setsize();
  void clear();
  void load(bool initgrid = true);
  void print(QPrinter* printer);
  void png(int size);
  void Normalise();
  bool InitGrid(Tgrid &g);
  Tgrid ParamGrid(int nx);
  void DrawGrid(const Tgrid &g);
  void PrintSizeGrid();
  void clearGrid();
  void ToGrid(tvertex &v);
  void ToGrid(QPoint &p);
  void Zoom(int dir);
  void Spring();
  void SpringPreservingMap(bool draw=true);
  int SpringJacquard();
  int FindItem(QPoint &p,NodeItem* &node,EdgeItem* &edge);
  int FindItem(QPoint &p,EdgeItem* &edge);
  int FindItem(QPoint &p,ColorItem* &coloritem);
  int FindItem(QPoint &p,ThickItem* &thickitem);
  void resizeEvent(QResizeEvent*);
  void update(int compute);

public slots:
  void showGrid(bool show);
  void fitgridChanged(bool i);
  void ForceToGrid();
  void UndoGrid();
  void sizegridChanged(int i);

private:
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent *event);

  void UpdateSizeGrid();
  void keyPressEvent(QKeyEvent *k);

  GraphWidget* gwp;
  pigaleWindow *mywindow;
  QPoint start_position;
  bool DoNormalise;
  bool is_init;
  short color_node;
  short color_edge;
  int width_edge;
  double xepsilon,yepsilon;
  double min_used_x,max_used_x;
  double min_used_y,max_used_y;
  double xminstep,yminstep;
  double x_min,x_max,y_min,y_max;
  double xstep,ystep;
  double undoxstep,undoystep;
  int key_pressed;
  bool IsGrid;
  double zoom;
  int nxstep,nystep;
  bool GridDrawn;
  Tgrid current_grid, graph_grid,old_grid,undo_grid;
  bool ShowGrid;
  bool FitToGrid;
  bool ForceGrid;
  bool ForceGridOk;
  bool RedrawGrid;
  int FitSizeGrid ;
  int OldFitSizeGrid ;
  int SizeGrid ;
};


NodeItem* CreateNodeItem(tvertex &v,GraphWidget* g);
void  CreateColorItems(GraphWidget* g,int color_node,int color_edge);
void  CreateThickItems(GraphWidget* g,int width_edge);
InfoItem* CreateInfoItem(GraphWidget* g,QString &t,QPoint &p);
EdgeItem* CreateEdgeItem(tedge &e,GraphWidget* g);
void CreatePenBrush();
#endif
