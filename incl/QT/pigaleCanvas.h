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

#ifndef  MYCANVAS_H_INCLUDED
#define  MYCANVAS_H_INCLUDED

#include <TAXI/graphs.h> 
#include <TAXI/color.h> 
#include <TAXI/Tpoint.h> 
#include <QT/grid.h> 

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QStyleOptionGraphicsItem>

bool & ShowOrientation();
int  & ShowVertex();

class NodeItem;

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
const int col_z      = 2; // ColorItem //130
const int thick_z    = 2; // ThickItem //130
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
class EdgeItem;


class ArrowItem: public QGraphicsPolygonItem
{public:
  ArrowItem(EdgeItem *edgeitem);
  void ComputeCoord();
  void SetColor(QColor col);
  int type() const {return arrow_rtti;}
 private:
  EdgeItem *edgeItem;
  QVector<QPointF>pts;
};

class EdgeItem: public QGraphicsLineItem
{public:
  EdgeItem(GraphWidgetPrivate* g,tedge &ee,double x_from,double y_from,double x_to,double y_to
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
  GraphWidgetPrivate* gwp;
};

class CursItem: public QGraphicsLineItem
{public:
  CursItem(tvertex &v,QPoint &p,GraphWidgetPrivate* g);
  void setToPoint(int x,int y);
  int type() const {return curs_rtti;}
  tvertex v;
};

class LineItem: public QGraphicsLineItem
{public:
  LineItem(GraphWidgetPrivate* g);
  int type() const {return line_rtti;}
};

class InfoItem: public QGraphicsSimpleTextItem
{public:
  InfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p);
  int type() const {return info_rtti;}
  QGraphicsRectItem* rectitem;
  private:
  GraphWidgetPrivate* gwp;
};

class ColorItem: public QGraphicsRectItem
{public:
  ColorItem(GraphWidgetPrivate* g,QRectF &rect,int pen_color,int brush_color,bool node);
  ~ColorItem() {}
  int type() const {return col_rtti;}
  void SetPenColor(int pen_color);
  int brush_color;
  bool node;
private:
  GraphWidgetPrivate* gwp;
};

class ThickItem: public QGraphicsRectItem
{public:
  ThickItem(GraphWidgetPrivate* g,QRectF &rect,int ewidth,int brush_color);
  ~ThickItem() {}
  int type() const {return thick_rtti;}
  void SetBrushColor(int bcolor);
  int brush_color;
  int width;
private:
  GraphWidgetPrivate* gwp;
};

class NodeItem: public QGraphicsRectItem
{public:
  NodeItem(tvertex &vv,GraphWidgetPrivate* g,QRectF &rect,QColor & col,QString &_t);
  QRectF boundingRect() const;
  void moveBy(double dx, double dy);
  void moveTo(Tpoint &p,double eps = .0);
  void SetColor(QColor c);
  void SetText(QString t);
  int type() const {return node_rtti;}
  void paint(QPainter *painter,const QStyleOptionGraphicsItem * option,QWidget * widget = 0); 
  tvertex v;
private:
  GraphWidgetPrivate* gwp;
  QColor vcolor,tcolor;
  QString t;
};

class GraphEditor : public QGraphicsView
{public:
  GraphEditor(GraphWidgetPrivate *g,QWidget* parent=0);
  ~GraphEditor();
  QSize sizeHint() const;
  double zoom;
  void initialize();
  void clear();
  void load(bool initgrid = true);
  void print(QPrinter* printer);
  void png(int size);
  void Normalise();
  bool InitGrid(Tgrid &g);
  void UndoGrid();
  Tgrid ParamGrid(int nx);
  void DrawGrid(const Tgrid &g);
  void showGrid(bool show);
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
//   void EraseColorVertices();
//   void EraseColorEdges();
//   void EraseThickEdges();
  int nxstep,nystep;
  bool GridDrawn;
  Tgrid current_grid, graph_grid,old_grid,undo_grid;

private:
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent *event);
  void showEvent(QShowEvent*);
  void hideEvent(QHideEvent*);
  void resizeEvent(QResizeEvent*);
  void paintEvent(QPaintEvent *);
  void UpdateSizeGrid();
  void keyPressEvent(QKeyEvent *k);

  QPoint start_position;
  GraphWidgetPrivate* gwp;
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
  bool IsGrid;
  int key_pressed;
};


NodeItem* CreateNodeItem(tvertex &v,GraphWidgetPrivate* g);
void  CreateColorItems(GraphWidgetPrivate* g,int color_node,int color_edge);
void  CreateThickItems(GraphWidgetPrivate* g,int width_edge);
InfoItem* CreateInfoItem(GraphWidgetPrivate* g,QString &t,QPoint &p);
EdgeItem* CreateEdgeItem(tedge &e,GraphWidgetPrivate* g);
void CreatePenBrush();
#endif
