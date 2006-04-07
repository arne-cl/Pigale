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

#include <qnamespace.h>
#include <qglobal.h>
#include <qmutex.h>
#include <qthread.h>
#include <QtNetwork>
#include <qwaitcondition.h>
#include <qactiongroup.h>      
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qmap.h>
#include <qevent.h> 
#include <qtimer.h>
#include <qevent.h>
#include <qcoreevent.h>
#include <qtextedit.h>
#include <qtextbrowser.h>
#include <qsettings.h>
#include <qtextstream.h>
#include <Pigale.h> 



class QTextEdit;
class pigaleWindow;
class GraphWidget;
class GraphGL;
class GraphSym;
class Graph_Properties;
class Mouse_Actions;
class pigalePaint;
class ClientSocket;
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
{Q_OBJECT

private:
  QMutex mutex;
  QWaitCondition condition;
  bool restart;
  bool abort;
  int action; 
  int N,N1,N2,M;
  int delay;
public:
  PigaleThread(QObject *parent = 0) ;
  ~PigaleThread();
  void run(int action,int N = 0,int N1 = 0,int N2 = 0,int M = 0,int delay = 0);
  pigaleWindow *mw;
signals:
  void handlerSignal(int ret,int drawingType,int saveType);
protected:
  void run();
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
  void saveAs();
  void deleterecord();
  void switchInputOutput();
  void macroHandler(QAction *);
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
  void SetPigaleFont();
  void keyPressEvent(QKeyEvent *k);
  void settingsHandler(int action);
  void initServer();
  void seedEdited(const QString & t);
  void spinN1Changed(int val);
  void spinN2Changed(int val);
  void spinMChanged(int val);
  void EditPigaleColors();
  void SetPigaleColorsProfile1(); 
  void SetPigaleColorsProfile2(); 
private:
  int setId(QAction *action,int Id);
  void AllowAllMenus();
  void mapActionsInit();
  int  macroLoad(QString FileName);
  void macroRecord(int action);
  void ParseArguments(); 
  void Message(QString s);
  void UndoInit();
  void initMenuTest();
  int postHandler(QEvent *ev);
  void UpdatePigaleColors();
public slots:
  int postHandler(int ret,int drawingType,int saveType);
  void handler(QAction *action);
  void SaveSettings();
  void SetDocumentPath();
  void banner();
  void timerWait();
  void UndoSave();
  void computeInformation();
  void information();
  void print();
  void png();
  int  handler(int action);
  void NewGraph();
public:
  pigaleWindow();
  ~pigaleWindow();
  void whenReady();
  bool event(QEvent * e);
  void customEvent( QEvent * e );
  void postMessage(const QString &msg);
  void postWait(const QString &msg);
  void postMessageClear();
  void postDrawG();
  void wait(int millisec);
  void UndoTouch(bool save=false);
  void blockInput(bool t);
  int getKey();
  QString getActionString(int action);
  int getActionInt(QString action_str);
  int publicLoad(int pos);
  int publicSave(QString filename);
  void setUserMenu(int i, const QString &txt);
  void setShowOrientation(bool val);
  int  getResultHandler(int value = 0);
  int getId(QAction *action);
public:
  QActionGroup *menuActions;
  QMap<int,QAction*> menuIntAction;
  //QAction *leftAct,*rightAct,*redoAct,*undoLAct,*undoRAct,*undoSAct;
  QAction *undoLAct,*undoRAct,*undoSAct;
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
  ClientSocket *threadServer;
  int MaxNS,MaxND;
  int GraphIndex1;
private:
  PigaleThread pigaleThread;
  QToolBar *tb;
  QTextBrowser *browser;
  QSpinBox *spin_N1,*spin_N2,*spin_M,*spin_N,*spin_MaxNS,*spin_MaxND,*macroSpin;
  QCheckBox *box1,*box2,*box3,*box4,*box5,*box6,*box7,*box8,*box9,*box10,*box11;
  LineEditNum *macroLine;
  QLineEdit *seedEdit;
  QWidget *gSettings;
  QMenu *userMenu; 
  QPrinter *printer;
  QString DirFileMacro;
  QString MacroFileName;
  int *pGraphIndex,GraphIndex2,UndoIndex,UndoMax;
  QString DirFileDoc;
  int pigaleWindowInitYsize,pigaleWindowInitXsize;
  int macroRepeat;
  int PrinterOrientation,PrinterColorMode;
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
