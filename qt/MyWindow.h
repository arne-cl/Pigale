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

#ifndef MYWINDOW_H 
#define MYWINDOW_H

#include <qglobal.h>
#include <qmainwindow.h>
#include <qdatetime.h>
#include <qstring.h>
#include <Pigale.h> 

#if QT_VERSION < 300
#define QTextEdit QTextView
#endif

class QTextEdit;
class MyWindow;
class GraphWidget;
class GraphGL;
class GraphSym;
class Graph_Properties;
class Mouse_Actions;
class MyPaint;
class QTabWidget;
class QSpinBox;
class QCheckBox;
class QToolButton;
class QToolBar;
class QPrinter;

class MyWindow: public QMainWindow 
{Q_OBJECT

public:
  MyWindow();
  ~MyWindow();

public slots:
  void load(int pos);
  void banner();

private slots:
  void newgraph();
  void load();
  void previous();
  void reload();
  void next();
  void save();
  void save_ascii();
  void outputfile();
  void deleterecord();
  void switchInputOutput();
  void handler(int action);
  void userHandler(int action);
  void macroHandler(int action);
  void macroPlay();
  void about();
  void aboutQt();
  void showLabel(int action);
  void distOption(int use);
  void Undo();
  void Redo();
  void UndoClear();
  void UndoEnable(bool enable);
  void LoadSettings();
  void SaveSettings();
  void SetPigaleColors();
  void keyPressEvent(QKeyEvent *k);
public slots:
  void UndoSave();
  void information();
  void print();
  void png();
public:
  void Message(QString s);
  void MessageClear();
  void UndoTouch(bool save=false);

public:
  QToolButton *left,*right,*redo,*undoL,*undoR,*undoS;
  QTextEdit *e;
  MyPaint *mypaint;
  QTabWidget *tabWidget;
  GraphWidget *gw;
  GraphGL *graphgl;
  GraphSym *graphsym;
  Graph_Properties *graph_properties;
  Mouse_Actions *mouse_actions;
  GraphContainer GC;
  bool MacroLooping;
 
private:
  QToolBar *tb;
  QSpinBox *spin_N1,*spin_N2,*spin_M,*spin_N,*spin_MaxNS,*spin_MaxND,*spinMacro;
  QTime timer;
  QPrinter *printer;
  QString InputFileName;
  QString OutputFileName;
  QString DirFile;
  int GraphIndex1,GraphIndex2,*pGraphIndex,UndoIndex,UndoMax;
  bool IsUndoEnable;
  bool MacroRecording;
  bool MacroExecuting;
};


#endif
