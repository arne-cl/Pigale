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

#ifndef _GRAPH_SYM_H_INCLUDED_
#define _GRAPH_SYM_H_INCLUDED_
#include <qwidget.h>
//Added by qt3to4:
//#include <QResizeEvent>
#include <qevent.h>

class GraphSymPrivate;
class pigaleWindow; 
class QPrinter;

class GraphSym : public QWidget
{
  Q_OBJECT
public:
  GraphSym( QWidget *parent=0, const char *name=0,pigaleWindow* mw=0);
  ~GraphSym();
  void print(QPrinter *printer);
  void png();

public slots:
  int update();
  void Next();
  void Factorial();
  void Optimal();
  void SymLabel();
private:
  void resizeEvent(QResizeEvent* e);
  GraphSymPrivate* d;
};

#endif
