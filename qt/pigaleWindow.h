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
#include <qpopupmenu.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qmap.h>
#include <qevent.h> 
#include <qtimer.h>
#include <qthread.h> 

#include <Pigale.h> 

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView
#if _WIN32
#include <qtextbrowser.h>
#endif
#else
#include <qtextedit.h>
#include <qtextbrowser.h>
#include <qsettings.h>
#endif


class QTextEdit;
class pigaleWindow;
class GraphWidget;
class GraphGL;
class GraphSym;
class Graph_Properties;
class Mouse_Actions;
class pigalePaint;
class QTabWidget;
class QSpinBox;
class QCheckBox;
class QToolButton;
class QToolBar;
class QPrinter;
class LineEditNum;
class QProgressBar;
class QLineEdit;
class QPalette;

class PigaleThread : public QThread 
{private:
  int   action; 
  int N,N1,N2,M;
  int delay;
 public:
  PigaleThread( unsigned int stackSize = 0 ) 
      : QThread (stackSize)
      { }
  virtual void run();
  void run(int action,int N = 0,int N1 = 0,int N2 = 0,int M = 0,int delay = 0);
  pigaleWindow *mw;
};

class pigaleWindow: public QMainWindow 
{Q_OBJECT

private slots:
  void load();
  int load(int pos);
  void previous();
  void reload();
  void next();
  int save(bool askTitle = true);
  //void save_ascii();
  void saveAs();
  void deleterecord();
  void switchInputOutput();
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
  void SetPigaleFont();
  void keyPressEvent(QKeyEvent *k);
  void settingsHandler(int action);
  void initServer();
  void spinN1Changed(int val);
  void spinN2Changed(int val);
  void spinMChanged(int val);
private:
  void mapActionsInit();
  int  macroLoad(QString FileName);
  void macroRecord(int action);
  void ParseArguments(); 
  void Message(QString s);
  int postHandler(QCustomEvent *ev);
  void UndoInit();
  void initMenuTest();
public slots:
  void banner();
  void timerWait();
  void UndoSave();
  void information();
  void print();
  void png();
  int  handler(int action);
  void NewGraph();
public:
  pigaleWindow();
  ~pigaleWindow();
  void whenReady();
  void customEvent( QCustomEvent * e );
  void postMessage(const QString &msg);
  void postWait(const QString &msg);
  void postMessageClear();
  void postDrawG();
  void UndoTouch(bool save=false);
  void blockInput(bool t);
  int getKey();
  QString getActionString(int action);
  int getActionInt(QString action_str);
  int publicLoad(int pos);
  int publicSave(QString filename);
  void setUserMenu(int i, const QString &txt);
  void  setShowOrientation(bool val);
  int  getResultHandler(int value = 0);
public:
  QToolButton *left,*right,*redo,*undoL,*undoR,*undoS;
  QTextEdit *messages;
  QProgressBar *progressBar;
  pigalePaint *mypaint;
  QTabWidget *tabWidget;
  GraphWidget *gw;
  GraphGL *graphgl;
  GraphSym *graphsym;
  Graph_Properties *graph_properties;
  Mouse_Actions *mouse_actions;
  GraphContainer GC;
  QString DirFilePng;
  QPalette LightPalette;
  QString InputFileName;
  QString OutputFileName;
  int InputDriver;
  int OutputDriver;
  bool ServerExecuting;
  bool ServerBusy;
  int ServerClientId;
  QTime timer;
  int Gen_N1,Gen_N2,Gen_M;
private:
  PigaleThread pigaleThread;
  QToolBar *tb;
#if QT_VERSION >= 300 || _WINDOWS
  QTextBrowser *browser;
#endif
  QSpinBox *spin_N1,*spin_N2,*spin_M,*spin_N,*spin_MaxNS,*spin_MaxND,*macroSpin;
  LineEditNum *macroLine;
  QLineEdit *seedEdit;
 QPopupMenu *userMenu; 
  QPrinter *printer;
  QString DirFileMacro;
  QString MacroFileName;
  int GraphIndex1,GraphIndex2,*pGraphIndex,UndoIndex,UndoMax;
  // settings
  QString DirFileDoc;
 
  int pigaleWindowInitYsize,pigaleWindowInitXsize;
  int macroRepeat,macroMul;
  int MaxNS,MaxND;
  int PrinterOrientation,PrinterColorMode;
  //
  bool IsUndoEnable;
  bool MacroRecording;
  bool MacroLooping;
  bool MacroExecuting;
  bool MacroWait;
  bool MacroPlay;
  bool Server;
  typedef QMap<int,QString> IntStringMap;
  IntStringMap mapActionsString;
  typedef QMap<QString,int> StringIntMap;
  StringIntMap mapActionsInt;   
  int drawingType;
};


#endif
