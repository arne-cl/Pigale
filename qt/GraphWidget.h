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
#include <qwidget.h>

class GraphWidgetPrivate;
class MyWindow; 
class QPrinter;

class GraphWidget : public QWidget
{
  Q_OBJECT
public:
  GraphWidget( QWidget *parent=0, const char *name=0,MyWindow* mw=0);
  ~GraphWidget();
  int init();
  void print(QPrinter *printer);
public slots:
  void update();
  void refresh();
  void zoom();
  void ozoom();
  void uzoom();
  void EraseColorVertices();
  void EraseColorEdges();
  void EraseThickEdges();
  void showgridChanged(bool i);
  void fitgridChanged(bool i);
  void sizegridChanged(int i);
  void ForceGrid();
  void UndoGrid();
private:
  void resizeEvent(QResizeEvent*);
  GraphWidgetPrivate* d;
};

#endif
