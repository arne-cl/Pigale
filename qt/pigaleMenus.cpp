 
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

#include <config.h>
#include "pigaleWindow.h"
#include <TAXI/Tgf.h>
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include "LineEditNum.h"

#include <QT/pigaleWindow_doc.h> 
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <QT/Action_def.h>
#include <QT/pigalePaint.h> 
#include <QT/pigaleCanvas.h>
#include <QT/clientEvent.h> 

#include <qicon.h>
#include <qtextedit.h>
#include <qmenudata.h> 
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qinputdialog.h> 
#include <qfontdialog.h> 
#include <qfile.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qpalette.h>
#include <qcolordialog.h> 
#include <qprinter.h> 
#include <qprogressbar.h>
#include <qrect.h> 
#include <qlist.h>
#include <qgridlayout.h>
#include "qboxlayout.h"
#include <qvalidator.h>
#include <qdesktopwidget.h>
#include "icones/fileopen.xpm"
#include "icones/filenew.xpm"
#include "icones/fileprint.xpm"
#include "icones/filesave.xpm"
#include "icones/info.xpm"
#include "icones/sleft.xpm"
#include "icones/sright.xpm"
#include "icones/sreload.xpm"
#include "icones/help.xpm"
#include "icones/xman.xpm"
#include "icones/sleftarrow.xpm"
#include "icones/srightarrow.xpm"
#include "icones/sfilesave.xpm"
#include "icones/macroplay.xpm"
#include "icones/film.xpm"

void Init_IOGraphml();
void UndoErase();
void initGraphDebug();

int pigaleWindow::setId(QAction * action,int Id)
  {action->setData(Id);
  menuIntAction[Id] = action;
  return Id;
  }
int pigaleWindow::getId(QAction *action)
  {return (action->data()).toInt();
  }

pigaleWindow::pigaleWindow()
    :QMainWindow(0,"pigaleWindow",Qt::WDestructiveClose)
    ,ServerExecuting(false),ServerClientId(0) // end public
    ,GraphIndex1(1)
    //start private
    ,pigaleThread(this) // ?
    ,pGraphIndex(&GraphIndex1),GraphIndex2(1)
    ,UndoIndex(0),UndoMax(0)
    ,IsUndoEnable(true)
    ,MacroRecording(false),MacroLooping(false)
    ,MacroExecuting(false),MacroPlay(false),Server(false)
    
  {setError();// Initialize Error
#ifdef _WINDOWS
   initGraphDebug();// as the compiler does not initialize static ...
#endif
  // Export some data
  DefinepigaleWindow(this);
  //thread
  pigaleThread.mw = this;
  qApp->setMainWidget(this); // to be able to retrieve the mainWidget
  mapActionsInit();// Create the actions map
  Init_IO();// Initialize input/output drivers
  Init_IOGraphml();
  // Load settings, input and output filenames
  LoadSettings();
  QFileInfo fi0 =  QFileInfo(InputFileName);
  QFileInfo fi = QFileInfo(fi0.dirPath());
  if(!fi.exists() || !fi.isDir())
      {QString DirFile = QFileDialog::
      getExistingDirectory(".",this,"find","Choose the TGF directory",TRUE);
      InputFileName = DirFile + QDir::separator() + fi0.fileName();
      OutputFileName = InputFileName;
      }
  // Load inpu/output drivers
  InputDriver = IO_WhoseIs((const char *)InputFileName);
  if (InputDriver<0) InputDriver=0;
  OutputDriver = IO_WhoseIs((const char *)OutputFileName);
  if (OutputDriver<0) OutputDriver=0;
  // Modify settings according to passed arguments
   ParseArguments();
  // Init random generator
  randomInitSeed();
  // Define some colors
  LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QColorGroup::Base,QColor(QColorDialog::customColor(1)));
  // Create a printer
  printer = new QPrinter; //(QPrinter::HighResolution);
  if(PrinterOrientation == QPrinter::Portrait)
      printer->setOrientation(QPrinter::Portrait); 
  else
      printer->setOrientation(QPrinter::Landscape); 

  if(PrinterColorMode == QPrinter::GrayScale)
      printer->setColorMode(QPrinter::GrayScale);
  else
      printer->setColorMode(QPrinter::Color);

  // Widgets
  QWidget *mainWidget = new QWidget(this,"mainWidget");

  setCentralWidget(mainWidget);
  QDesktopWidget *desktop = QApplication::desktop();
  bool bigScreen = (desktop->height() >= 830); // min 785x830 ou 785x610
  QHBoxLayout *topLayout = new QHBoxLayout(mainWidget,0,0,"topLayout");
  //LEFT LAYOUT: Graph editor,Paint,GL,Browser *************************************************************
  QHBoxLayout * leftLayout = new QHBoxLayout(topLayout,0,"leftLayout");
  tabWidget = new  QTabWidget();
  leftLayout->addWidget(tabWidget,1);
  tabWidget->setMinimumSize(465,425);
  tabWidget->setPalette(LightPalette);
  mypaint =  new pigalePaint(0,"mypaint",this);
  gw = new  GraphWidget(0,"graphwidget",this);
  graphgl  = new GraphGL(0,"graphgl",this);
  graphgl->setPalette(LightPalette);
  graphsym = new GraphSym(0,"graphsym",this);

  tabWidget->addTab(gw,tr("Graph Editor"));
  tabWidget->addTab(mypaint,"");
  tabWidget->addTab(graphgl,""); 
  tabWidget->addTab(graphsym,""); 
 
  browser = new QTextBrowser(0);
  tabWidget->addTab(browser,tr("User Guide")); 
  QPalette bop(QColorDialog::customColor(3));
  browser->setPalette(bop);
  
  /* genealogy:
   mainWindow -> mainWidget -> rtabWidget
   rtabWidget -> ?? -> gInfo
  */
  QTabWidget  *rtabWidget = new  QTabWidget();  
  gSettings = new QWidget();
  gSettings->setPalette(LightPalette); gSettings->setAutoFillBackground(true);  
  QWidget * gInfo = new QWidget();                
  rtabWidget->setMaximumWidth(300);  rtabWidget->setMinimumWidth(300);
  leftLayout->addWidget(rtabWidget,2);
  rtabWidget->addTab(gInfo,tr("Information"));
  rtabWidget->addTab(gSettings,tr("Settings"));

  //rightLayout: Information
  QGridLayout *rightLayout = new  QGridLayout(gInfo,2,3,-1);
  //messages
  messages = new QTextEdit(gInfo);
  QBrush pb(QColorDialog::customColor(1)); //messages->setPaper(pb); 
  messages->setReadOnly(true);
  //graph_properties
  graph_properties = new Graph_Properties(gInfo,menuBar(),"graph_properties");
  //mouse_action
  mouse_actions = new Mouse_Actions(gInfo,"mouseactions",0,gw);
  mouse_actions->setPalette(LightPalette);
  mouse_actions->setAutoFillBackground(true); 
 
  rightLayout->addMultiCellWidget(messages,1,1,1,2);
  rightLayout->addMultiCellWidget(graph_properties,2,2,1,2);
  rightLayout->addMultiCellWidget(mouse_actions,3,3,1,2);


  //rightLayout: Settings ********************************************
  //generators
  spin_N1 = new QSpinBox(1,100000,1,0,"spinN1");  spin_N1->setValue(Gen_N1);     
  spin_N2 = new QSpinBox(1,100000,1,0,"spinN2");  spin_N2->setValue(Gen_N2);     
  spin_M = new QSpinBox(1,300000,1,0,"spinM");    spin_M->setValue(Gen_M);      
  QValidator *validator = new QIntValidator(this);
  seedEdit =  new QLineEdit(0,"seedEdit");
  seedEdit->setValidator(validator);
  connect(seedEdit,SIGNAL(textEdited(const QString & )),SLOT(seedEdited(const QString & )));
  seedEdit->setText(QString("%1").arg(randomSetSeed()));
  connect(spin_N1,SIGNAL(valueChanged(int)),SLOT(spinN1Changed(int)));
  connect(spin_N2,SIGNAL(valueChanged(int)),SLOT(spinN2Changed(int)));
  connect(spin_M,SIGNAL(valueChanged(int)),SLOT(spinMChanged(int)));
  // Macro
  macroSpin = new QSpinBox(0,60,1,0,"macroDelaySpin");  macroSpin->setValue(pauseDelay());
  macroLine = new LineEditNum(0,"macroLineEditNum");
  int macroMul = macroRepeat/100;
  int macroNum = macroMul ? macroRepeat/(macroMul*100) : macroRepeat;
  macroLine->setNum(macroNum); macroLine->setMul(macroMul);
  QSlider *macroSlider = new QSlider(0,100,0,macroRepeat,Qt::Horizontal,0,"macroSlider");macroSlider->setValue(macroNum);
  QSlider *macroSliderM = new QSlider(0,100,0,macroMul,Qt::Horizontal,0,"macroSliderM");macroSliderM->setValue(macroMul);
  connect(macroSlider,SIGNAL(valueChanged(int)),macroLine,SLOT(setNum(int)));
  connect(macroSliderM,SIGNAL(valueChanged(int)),macroLine,SLOT(setMul(int)));
  // Labels of vertices
  QComboBox *comboLabel = new QComboBox(0);
  comboLabel->insertItem(tr("Nothing"));
  comboLabel->insertItem(tr("Index"));
  comboLabel->insertItem(tr("Text"));
  comboLabel->insertItem(tr("Label"));
  int current = ShowVertex();current += 3;
  comboLabel->setCurrentItem(current);showLabel(current);
  connect(comboLabel,SIGNAL(activated(int)),SLOT(showLabel(int)));
  // Partition
  spin_N = new QSpinBox(2,50,1,0,"spinN");  spin_N->setValue(2);     //spin_N->setPrefix(tr("Number of classes: "));
  // Distances
  QComboBox *comboDistance  = new QComboBox(0);
  comboDistance->insertItem(tr("Czekanovski-Dice"));
  comboDistance->insertItem(tr("Bisect"));
  comboDistance->insertItem(tr("Adjacence"));
  comboDistance->insertItem(tr("Adjacence M"));
  comboDistance->insertItem(tr("Laplacian"));
  comboDistance->insertItem(tr("Q-distance"));
  comboDistance->insertItem(tr("Oriented"));
  comboDistance->insertItem(tr("R2"));
  comboDistance->setCurrentItem(useDistance());distOption(useDistance());
  connect(comboDistance,SIGNAL(activated(int)),SLOT(distOption(int)));
  // Limits
  spin_MaxNS = new QSpinBox(1,10000,1,0,"spinMaxNS");  spin_MaxNS->setValue(MaxNS);    
  spin_MaxND = new QSpinBox(1,10000,1,0,"spinMaxND");  spin_MaxND->setValue(MaxND);      
  connect(spin_MaxNS,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNSlowChanged(int)));
  connect(spin_MaxND,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNDisplayChanged(int)));
  // CheckBoxes
  QButtonGroup *Group1 = new QButtonGroup(0);  Group1->setExclusive(false); //0 -> this ==
  connect(Group1,SIGNAL(buttonClicked(int)),this,SLOT(settingsHandler(int)));
  box1 = new  QCheckBox("Debug",0);
  box1->setCheckState(debug() ? Qt::Checked : Qt::Unchecked);                     Group1->addButton(box1,A_SET_DEBUG);
  box2 = new  QCheckBox("Undo",0);
  box2->setCheckState(IsUndoEnable ? Qt::Checked : Qt::Unchecked);                Group1->addButton(box2,A_SET_UNDO);
  box3 = new  QCheckBox("Erase multiple edges ",0);
  box3->setCheckState(randomEraseMultipleEdges() ? Qt::Checked : Qt::Unchecked);  Group1->addButton(box3,A_SET_ERASE_MULT);
  box4 = new  QCheckBox("Use generated cir",0);  
  box4->setCheckState(randomUseGeneratedCir() ? Qt::Checked : Qt::Unchecked);     Group1->addButton(box4,A_SET_GEN_CIR);
  box5 = new  QCheckBox("Random seed",0);    
  box5->setCheckState(randomSeed() ? Qt::Checked : Qt::Unchecked);                Group1->addButton(box5,A_SET_RANDOM_SEED);
  box6 = new  QCheckBox("Show orientation",0);  
  box6->setCheckState(ShowOrientation() ? Qt::Checked : Qt::Unchecked);           Group1->addButton(box6, A_SET_ORIENT);
  box7 = new  QCheckBox("Show arrows",0);    
  box7->setCheckState(ShowArrow() ? Qt::Checked : Qt::Unchecked);                 Group1->addButton(box7, A_SET_ARROW);
  box8 = new  QCheckBox("Show exterior edge",0);  
  box8->setCheckState(ShowExtBrin() ? Qt::Checked : Qt::Unchecked);               Group1->addButton(box8,A_SET_EXTBRIN);
  box9 = new  QCheckBox("Schnyder Rect",0);
  box9->setCheckState(SchnyderRect() ? Qt::Checked : Qt::Unchecked);              Group1->addButton(box9,A_SET_SCH_RECT); 
  box10 = new  QCheckBox("Schnyder color",0);
  box10->setCheckState(SchnyderColor() ? Qt::Checked : Qt::Unchecked);            Group1->addButton(box10,A_SET_SCH_COLOR);
  box11 = new  QCheckBox("Use longest face",0);
  box11->setCheckState(SchnyderLongestFace() ? Qt::Checked : Qt::Unchecked);      Group1->addButton(box11,A_SET_LFACE);

  // Add to the Layout
  //bigScreen = false;
  int mw, mh;
  if(bigScreen)
      {mw = 100; mh = 20;
      gSettings->setMaximumHeight(660);//630
      }
  else
      {mw = 100; mh = 18;
      gSettings->setMaximumHeight(500);
      }
  comboLabel->setMaximumWidth(mw);comboLabel->setMaximumHeight(mh);
  comboDistance->setMaximumWidth(mw);comboDistance->setMaximumHeight(mh);
  seedEdit->setMaximumWidth(mw);seedEdit->setMaximumHeight(mh);
  spin_N->setMaximumWidth(mw);spin_N->setMaximumHeight(mh);
  spin_N1->setMaximumWidth(mw);spin_N1->setMaximumHeight(mh);
  spin_N2->setMaximumWidth(mw);spin_N2->setMaximumHeight(mh);
  spin_M->setMaximumWidth(mw);spin_M->setMaximumHeight(mh);
  spin_MaxNS->setMaximumWidth(mw);spin_MaxNS->setMaximumHeight(mh);
  spin_MaxND->setMaximumWidth(mw);spin_MaxND->setMaximumHeight(mh);
  macroSpin->setMaximumWidth(mw);macroSpin->setMaximumHeight(mh);
  macroLine->setMaximumWidth(mw+20);macroLine->setMaximumHeight(mh);
  macroSlider->setMaximumWidth(mw+20);
  macroSliderM->setMaximumWidth(mw+20);

  QGridLayout *setLayout = new  QGridLayout(gSettings,2,28);
  //QGridLayout *setLayout = new  QGridLayout(gSettings);
  int row = -1;
  // General
  if(bigScreen)setLayout->addWidget(new QLabel(" ",0),++row,1);
  if(bigScreen)setLayout->addWidget(new QLabel(tr("General settings"),0),++row,1,1,2,Qt::AlignHCenter);
  // CheckBoxes
  setLayout->addWidget(box1,++row,1);  setLayout->addWidget(box2,row,2); //debug,undo
  setLayout->addWidget(box6,++row,1);  setLayout->addWidget(box7,row,2); // orientation,arrowq
  setLayout->addWidget(box8,++row,1);  setLayout->addWidget(box11,row,2); // exterioredge, longest face
  setLayout->addWidget(new QLabel(tr("Vertex label"),0),++row,1,Qt::AlignLeft);  setLayout->addWidget(comboLabel,row,2);
  //setLayout->addWidget(new QLabel(tr("Maximum number of vertices:"),0),++row,1,1,2,tQt::AlignHCenter);
  setLayout->addWidget(new QLabel(tr("Max N display"),0),++row,1);
  setLayout->addWidget(new QLabel(tr("Max N slow algorithms"),0),row,2);
  setLayout->addWidget(spin_MaxND,++row,1); setLayout->addWidget(spin_MaxNS,row,2);
  if(bigScreen)setLayout->addWidget(new QLabel(" ",0),++row,1);
  // generators
  if(bigScreen)setLayout->addWidget(new QLabel(tr("Graph Generators"),0),++row,1,1,2,Qt::AlignHCenter);
  setLayout->addWidget(new QLabel("N1",0),++row,1);    setLayout->addWidget(spin_N1,row,2);
  setLayout->addWidget(new QLabel("N2",0),++row,1);    setLayout->addWidget(spin_N2,row,2);
  setLayout->addWidget(new QLabel("M",0),++row,1);     setLayout->addWidget(spin_M,row,2);
  setLayout->addWidget(new QLabel(tr("Seed"),0),++row,1);  setLayout->addWidget(seedEdit,row,2);
  setLayout->addWidget(box3,++row,1);  setLayout->addWidget(box4,row,2); // multiple edges, generated cir
  setLayout->addWidget(box5,++row,1); // random seed
  if(bigScreen)setLayout->addWidget(new QLabel(" ",0),++row,1);
  // macro
  if(bigScreen)setLayout->addWidget(new QLabel(tr("Macro Settings"),0),++row,1,1,2,Qt::AlignHCenter);
  setLayout->addWidget(new QLabel(tr("Delay (seconds)"),0),++row,1);  setLayout->addWidget(macroSpin,row,2);
  setLayout->addWidget(new QLabel(tr("Repeat"),0),++row,1);setLayout->addWidget(macroLine,row,2);
  setLayout->addWidget(new QLabel("x",0),++row,1);  setLayout->addWidget(macroSlider,row,2);
  setLayout->addWidget(new QLabel("x100",0),++row,1); setLayout->addWidget(macroSliderM,row,2);
  if(bigScreen) setLayout->addWidget(new QLabel(" ",0),++row,1);
  // distance
  if(bigScreen)setLayout->addWidget(new QLabel("Factorial analysis parameters",0),++row,1,1,2,Qt::AlignHCenter);
  setLayout->addWidget(new QLabel(tr("Distance"),0),++row,1);  setLayout->addWidget(new QLabel(tr("Number of classes"),0),row,2);
  setLayout->addWidget(comboDistance,++row,1);setLayout->addWidget(spin_N,row,2);
  if(bigScreen)setLayout->addWidget(new QLabel(" ",0),++row,1);
  // Schnyder
  if(bigScreen)setLayout->addWidget(new QLabel("Schyder parameters",0),++row,1,1,2,Qt::AlignHCenter);
  setLayout->addWidget(box9,++row,1); setLayout->addWidget(box10,row,2); //Schnyder rect,color
  if(bigScreen)setLayout->addWidget(new QLabel(" ",0),++row,1);
  //cout<<"row:"<<row<<endl;
  //Pixmaps
  QIcon openIcon = QIcon(fileopen),newIcon = QIcon(filenew),saveIcon = QIcon(filesave);
  QIcon leftIcon = QIcon(sleft),   rightIcon = QIcon(sright);
  QIcon reloadIcon = QIcon(sreload);
  QIcon infoIcon = QIcon(info), helpIcon = QIcon(help),printIcon = QIcon(fileprint);
  QIcon xmanIcon = QIcon(xman), undoLIcon = QIcon(sleftarrow);
  QIcon undoSIcon = QIcon(sfilesave),undoRIcon = QIcon(srightarrow);
  QIcon macroplayIcon = QIcon(macroplay),filmIcon = QIcon(film);
  
  //ToolBar
  // NEW LOAD SAVE
  tb = new QToolBar(this);  addToolBar(tb);  tb->setMovable(true); 
  QAction *newAct = new QAction(newIcon, tr("&New..."), this);
  newAct->setStatusTip(tr("New graph"));
  connect(newAct, SIGNAL(triggered()),this,SLOT(NewGraph()));
  tb->addAction(newAct);
  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
  openAct->setStatusTip(tr("Open a new graph"));openAct->setWhatsThis(fileopen_txt);
  connect(openAct, SIGNAL(triggered()),this,SLOT(load()));
  tb->addAction(openAct);
  QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
  saveAct->setStatusTip(tr("Save the graph"));
  connect(saveAct, SIGNAL(triggered()),this,SLOT(save()));
  tb->addAction(saveAct);
  // PRINT PNG
  QAction *printAct = new QAction(printIcon, tr("&Print..."), this);
  printAct->setStatusTip(tr("Print the graph"));
  connect(printAct, SIGNAL(triggered()),this,SLOT(print()));
  tb->addAction(printAct);
  QAction *pngAct = new QAction(filmIcon, tr("Png..."), this);
  pngAct->setStatusTip(tr("Save image"));
  connect(pngAct, SIGNAL(triggered()),this,SLOT(png()));
  tb->addAction(pngAct);
  // INFO
  QAction *infoAct = new QAction(infoIcon, tr("&Information..."), this);
  infoAct->setStatusTip(tr("Information"));
  connect(infoAct, SIGNAL(triggered()),this,SLOT(information()));
  tb->addAction(infoAct);
  tb->addSeparator();
  // LEFT RELOAD RIGHT
  QAction *leftAct = new QAction(leftIcon, tr("Previous graph..."), this);
  leftAct->setStatusTip(tr("Previous graph (tgf file)"));leftAct->setWhatsThis(left_txt);
  connect(leftAct, SIGNAL(triggered()),this,SLOT(previous()));
  tb->addAction(leftAct);
  QAction *reloadAct = new QAction(reloadIcon, tr("Reload..."), this);
  reloadAct->setStatusTip(tr("Reload graph"));reloadAct->setWhatsThis(redo_txt);
  connect(reloadAct, SIGNAL(triggered()),this,SLOT(reload()));
  tb->addAction(reloadAct);
  QAction *rightAct = new QAction(rightIcon, tr("Next graph..."), this);
  rightAct->setStatusTip(tr("Next graph (tgf file)"));rightAct->setWhatsThis(right_txt);
  connect(rightAct, SIGNAL(triggered()),this,SLOT(next()));
  tb->addAction(rightAct);
  tb->addSeparator();
  // UNDO
  undoLAct = new QAction(undoLIcon, tr("Undo..."), this);
  undoLAct->setStatusTip(tr("Undo last action"));
  connect(undoLAct, SIGNAL(triggered()),this,SLOT(Undo()));
  tb->addAction(undoLAct);
  undoSAct = new QAction(undoSIcon, tr("Undo save..."), this);
  undoSAct->setStatusTip(tr("Undo save"));
  connect(undoSAct, SIGNAL(triggered()),this,SLOT(UndoSave()));
  tb->addAction(undoSAct);
  undoRAct = new QAction(undoRIcon, tr("Redo..."), this);
  undoRAct->setStatusTip(tr("Redo last action"));
  connect(undoRAct, SIGNAL(triggered()),this,SLOT(Redo()));
  tb->addAction(undoRAct);
  undoLAct->setEnabled(false);  undoSAct->setEnabled(IsUndoEnable);   undoRAct->setEnabled(false);
  tb->addSeparator();
  // MACRO
  QAction *macroAct = new QAction(macroplayIcon, tr("Macro..."), this);
  macroAct->setStatusTip(tr("Play the macro"));
  connect(macroAct, SIGNAL(triggered()),this,SLOT(macroPlay()));
  tb->addAction(macroAct);
  QAction *helpAct =  QWhatsThis::createAction(); 
  //connect(helpAct, SIGNAL(triggered()),this,SLOT(whatsThis()));
  helpAct->setShortcut(Qt::SHIFT+Qt::Key_F1);
  tb->addAction(helpAct);


  // Status bar
  progressBar = new QProgressBar(statusBar());
  connect(menuBar(),SIGNAL(triggered(QAction *)),this,SLOT(handler(QAction *)));
  //QIcon xmanIcone = QIcon("xman.xpm");
  QAction *action;

  QMenu *file = menuBar()->addMenu( tr("&File"));  //-> inutil
  file->addAction(newIcon,tr("&New Graph"),this, SLOT(NewGraph()));
  action = file->addAction(openIcon,tr("&Open"),this, SLOT(load()));
  action->setWhatsThis(fileopen_txt);
  file->addAction(tr("Save &as ..."), this, SLOT(saveAs()));
  file->insertItem(saveIcon,tr("&Save"), this, SLOT(save()));
  file->insertSeparator();
  file->addAction(tr("&Delete current record"),this,SLOT(deleterecord()));
  file->addAction(tr("S&witch Input/Output files"),this,SLOT(switchInputOutput()));
  file->insertSeparator();
  file->addAction(printIcon,tr("&Print"),this, SLOT(print()));
  file->insertSeparator();
  file->addAction(tr("&Init server"),this, SLOT(initServer()));
  file->insertSeparator();
  file->addAction(tr("&Quit"),qApp,SLOT(closeAllWindows()));

  QMenu *augment = menuBar()->addMenu( tr("&Augment")); 
  action = augment->addAction(xmanIcon,tr("Make &Connected (edge)")); 
  setId(action,A_AUGMENT_CONNECT);
  action->setWhatsThis(tr("Make a graph connected adding edges"));
  action = augment->addAction(xmanIcon,tr("Make &Connected (vertex)")); 
  setId(action,A_AUGMENT_CONNECT_V);
  action->setWhatsThis(tr("Make a graph connected adding a vertex"));
  action = augment->addAction(xmanIcon,tr("Make &2-Connected a planar graph (edge)")); 
  setId(action,A_AUGMENT_BICONNECT);
  action->setWhatsThis(tr("Make a PLANAR graph 2-connected adding edges"));
  action = augment->addAction(xmanIcon,tr("Make 2-Connected a planar graph &Opt (edge)"));
  setId(action,A_AUGMENT_BICONNECT_6);
  action->setWhatsThis(tr("Make a PLANAR graph 2-connected\n adding edges ( minimal degree increase)"));
  action = augment->addAction(xmanIcon,tr("Make 2-Connected  (edge)")); 
  setId(action,A_AUGMENT_BICONNECT_NP);
  action->setWhatsThis(tr("Make 2-connected a graph (planar or not)  adding edges"));
  action = augment->addAction(xmanIcon,tr("Make 2-Connected   (vertex)")); 
  setId(action,A_AUGMENT_BICONNECT_NP_V);
  action->setWhatsThis(tr("Make 2-connected a graph (planar or not) adding vertices"));
  action = augment->addAction(xmanIcon,tr("&Vertex Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_V);
  action->setWhatsThis(tr("Triangulate a PLANAR graph  adding vertices"));
  action = augment->addAction(xmanIcon,tr("&ZigZag Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_ZZ);
  action->setWhatsThis(tr("Triangulate a PLANAR graph by adding edges"));
  action = augment->addAction(xmanIcon,tr("T&ricon. Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_3C);
  action->setWhatsThis(tr("Optimally triangulate a PLANAR graph by adding edges"));
  action = augment->addAction(xmanIcon,tr("Vertex &Quadrangulate")); 
  setId(action,A_AUGMENT_QUADRANGULATE_V);
  action->setWhatsThis(tr("Quadrangulate a PLANAR  bipartite grap"));
  augment->addSeparator();
  action = augment->addAction(xmanIcon,tr("&Bisect all edges")); 
  setId(action,A_AUGMENT_BISSECT_ALL_E);
 
  QMenu *remove = menuBar()->addMenu( tr("&Remove")); 
  action = remove->addAction(tr("&Isolated vertices")); 
  setId(action,A_REMOVE_ISOLATED_V);
  action = remove->addAction(tr("&Multiple edges")); 
  setId(action,A_REMOVE_MULTIPLE_E);
  action = remove->addAction(tr("Ist&hmus")); 
  setId(action,A_REMOVE_BRIDGES);
  remove->addSeparator();
  action = remove->addAction(tr("Colored &vertices")); 
  setId(action,A_REMOVE_COLOR_V);
  action = remove->addAction(tr("Colored &edges")); 
  setId(action,A_REMOVE_COLOR_E);

  QMenu *embed = menuBar()->addMenu( tr("E&mbed")); 
  action = embed->addAction(xmanIcon,tr("&FPP Fary")); 
  action->setWhatsThis(tr(fpp_txt));
  setId(action,A_EMBED_FPP);
  action = embed->addAction(xmanIcon,tr("&Schnyder")); 
  action->setWhatsThis(tr(schnyder_txt));
  setId(action,A_EMBED_SCHNYDER_E);
  action = embed->addAction(xmanIcon,tr("Schnyder &V")); 
  action->setWhatsThis(tr(schnyder_txt));
  setId(action,A_EMBED_SCHNYDER_V);
  action = embed->addAction(xmanIcon,tr("Co&nvex Drawing")); 
  action->setWhatsThis(tr(cd_txt));
  setId(action,A_EMBED_CD);
  action = embed->addAction(xmanIcon,tr("&Convex Compact Dra&wing")); 
  action->setWhatsThis(tr(ccd_txt));
  setId(action,A_EMBED_CCD);
  embed->addSeparator();
  action = embed->addAction(tr("&Tutte")); 
  setId(action, A_EMBED_TUTTE);
  embed->addSeparator();
  action = embed->addAction(xmanIcon,tr("Tutte &Circle")); 
  action->setWhatsThis(tr(tutte_circle_txt));
  setId(action,A_EMBED_TUTTE_CIRCLE);
 embed->addSeparator();
#if VERSION_ALPHA
  action = embed->addAction(tr("Double Occurrence (&DFS)")); 
  setId(action, A_EMBED_POLREC_DFS);
#endif
  action = embed->addAction(tr("Double Occurrence (&LR DFS)")); 
  setId(action, A_EMBED_POLREC_DFSLR);  
  action = embed->addAction(tr("Double Occurrence (&BFS)")); 
  setId(action, A_EMBED_POLREC_BFS);
#if VERSION_ALPHA
  action = embed->addAction(tr("Double Occurrence &Cir (&BFS)")); 
  setId(action, A_EMBED_POLAR);
#endif
  embed->addSeparator();
  action = embed->addAction(tr("&Visibility"));   setId(action,A_EMBED_VISION);
  action = embed->addAction(tr("FPP Visi&bility"));   setId(action,A_EMBED_FPP_RECTI);
  action = embed->addAction(tr("&General Visibility"));   setId(action,A_EMBED_GVISION);
  embed->addSeparator();
  action = embed->addAction(tr("&T Contact"));   setId(action,A_EMBED_T_CONTACT);
  action = embed->addAction(tr("&R Triangle Contact"));   setId(action,A_EMBED_TRIANGLE);
  action = embed->addAction(tr("&Contact Biparti"));   setId(action,A_EMBED_CONTACT_BIP);
  embed->addSeparator();
  action = embed->addAction(tr("&Polyline"));   setId(action,A_EMBED_POLYLINE);
  action = embed->addAction(tr("&Curves"));   setId(action,A_EMBED_CURVES);
  embed->addSeparator();
  action = embed->addAction(xmanIcon,tr("Spring (Map &Preserving)")); 
  action->setWhatsThis(tr(springPM_txt));  
  setId(action,A_EMBED_SPRING_PM);
#if VERSION_ALPHA
  action = embed->addAction(xmanIcon,tr("Spring Planar")); 
  action->setWhatsThis(tr(jacquard_txt));  
  setId(action,A_EMBED_JACQUARD);
#endif
  action = embed->addAction(xmanIcon,tr("Sprin&g")); 
  action->setWhatsThis(tr(spring_txt));  
  setId(action,A_EMBED_SPRING);
  embed->addSeparator(); 
  action = embed->addAction(xmanIcon,tr("&Embedding in Rn")); 
  action->setWhatsThis(tr(embed3d_txt));  
  setId(action,A_EMBED_3d);
  action = embed->addAction(xmanIcon,tr("Schnyder in R3")); 
  action->setWhatsThis(tr(embed3dSchnyder_txt));  
  setId(action,A_EMBED_3dSCHNYDER);
 
  QMenu *dual = menuBar()->addMenu(tr("&Dual/Angle")); 
  action = dual->addAction(tr("&Dual"));   setId(action,A_GRAPH_DUAL);
  action = dual->addAction(xmanIcon,tr("&Geometric Dual")); 
  action->setWhatsThis(tr(dual_g_txt));  
  setId(action,A_GRAPH_DUAL_G);
  dual->addSeparator();
  action = dual->addAction(tr("&Angle"));   setId(action,A_GRAPH_ANGLE);
  action = dual->addAction(xmanIcon,tr("G&eometric Angle")); 
  action->setWhatsThis(tr(angle_g_txt));  
  setId(action,A_GRAPH_ANGLE_G);

  QMenu *algo = menuBar()->addMenu(tr("&Algo")); 
  action = algo->addAction(tr("Find &Kuratowski"));   setId(action,A_ALGO_KURATOWSKI);
  action = algo->addAction(tr("Find &Cotree Critical"));   setId(action, A_ALGO_COTREE_CRITICAL);
  action = algo->addAction(tr("Color red  non critical edges"));   setId(action,A_ALGO_COLOR_NON_CRITIC);
  action = algo->addAction(tr("Max.Planar (simple graph) Fast"));   setId(action,A_ALGO_NPSET);
  action = algo->addAction(tr("&Max.Planar (simple graph) Slow"));   setId(action,A_ALGO_MAX_PLANAR);
  algo->addSeparator();
  action = algo->addAction(tr("Use &Geometric Map"));   setId(action,A_ALGO_GEOMETRIC_CIR);
  action = algo->addAction(tr("Use &LRALGO Map"));   setId(action,A_ALGO_LRALGO_CIR);
  algo->addSeparator();
  action = algo->addAction(tr("&Color everything"));   setId(action,A_ALGO_RESET_COLORS);
  action = algo->addAction(tr("Color &bipartite"));   setId(action,A_ALGO_COLOR_BIPARTI);
  action = algo->addAction(tr("Color current e&xterior face"));   setId(action,A_ALGO_COLOR_EXT);
  action = algo->addAction(tr("Color c&onnected components"));   setId(action,A_ALGO_COLOR_CONNECTED);
  algo->addSeparator();
  action = algo->addAction(tr("&Symmetry"));   setId(action,A_ALGO_SYM);
  algo->addSeparator();
  action = algo->addAction(tr("&Partition"));   setId(action, A_ALGO_NETCUT);

  QMenu *orient = menuBar()->addMenu(tr("&Orient")); 
  action = orient->addAction(tr("&Orient all edges"));   setId(action,A_ORIENT_E);
  action = orient->addAction(tr("&Unorient all edges"));   setId(action, A_ORIENT_NOE);
  action = orient->addAction(tr("&Color Poles"));   setId(action,A_ORIENT_SHOW);
  action = orient->addAction(tr("&ReOrient color edges"));   setId(action,A_ORIENT_SHOW);
  action = orient->addAction(tr("&Inf Orientation"));   setId(action,A_ORIENT_INF);
  action = orient->addAction(tr("&Inf Orientation"));   setId(action,A_ORIENT_TRICON);
  action = orient->addAction(tr("&Inf Orientation"));   setId(action,A_ORIENT_BIPAR);
  action = orient->addAction(tr("&Inf Orientation"));   setId(action,A_ORIENT_SCHNYDER);
  action = orient->addAction(tr("B&ipolarOrient Planar"));   setId(action,A_ORIENT_BIPOLAR);
  action = orient->addAction(tr("BipolarOrient"));   setId(action,A_ORIENT_BIPOLAR_NP);
  action = orient->addAction(tr("BFS Orientation"));   setId(action,A_ORIENT_BFS);

  QMenu *generate = menuBar()->addMenu(tr("&Generate")); 
  QMenu *outer    = generate->addMenu(tr("&Outer Planar"));
  action = outer->addAction(tr("&Outer Planar (N1)"));   setId(action, A_GENERATE_P_OUTER_N);
  action = outer->addAction(tr("O&uter Planar (N1,M))"));   setId(action,A_GENERATE_P_OUTER_NM);
  QMenu *plan    = generate->addMenu(tr("&Planar"));
  action = plan->addAction(tr("connected (M)"));   setId(action,A_GENERATE_P);
  action = plan->addAction(tr("2-connected (M)"));   setId(action,A_GENERATE_P_2C);
  action = plan->addAction(tr("3-connected (M))"));   setId(action,A_GENERATE_P_3C);
  QMenu *cubic    = generate->addMenu(tr("Planar &cubic"));
  action = cubic->addAction(tr("2-connected (M)"));   setId(action,A_GENERATE_P_3R_2C);
  action = cubic->addAction(tr("3-connected (M)"));   setId(action,A_GENERATE_P_3R_3C);
  action = cubic->addAction(tr("dual:4-connected (M)"));   setId(action,A_GENERATE_P_3R_D4C);
  QMenu *four = generate->addMenu(tr("Planar &4-regular"));
  action = four->addAction(tr("4-regular 2-connected (M)"));   setId(action,A_GENERATE_P_4R_2C);
  action = four->addAction(tr("4-regular 3-connected (M)"));   setId(action,A_GENERATE_P_4R_3C);
  action = four->addAction(tr("4-regular bipartite (M)"));   setId(action,A_GENERATE_P_4R_BIP);
  QMenu *bip = generate->addMenu(tr("Planar &bipartite"));
  action = bip->addAction(tr("Bipartite (M)"));   setId(action,A_GENERATE_P_BIP);
  action = bip->addAction(tr("Bipartite cubic 2-connected (M)"));   setId(action,A_GENERATE_P_BIP_2C);
  action = bip->addAction(tr("Bipartite cubic 3-connected (M)"));   setId(action,A_GENERATE_P_BIP_3C);
  generate->addSeparator();
  action = generate->addAction(tr("&Grid (N1,N2)"));   setId(action,A_GENERATE_GRID);
  generate->addSeparator();
  action = generate->addAction(tr("&Complete (N1)"));   setId(action, A_GENERATE_COMPLETE);
  action = generate->addAction(tr("&Bipartite complete (N1,N2)"));   setId(action,A_GENERATE_COMPLETE_BIP);
  generate->addSeparator();
  action = generate->addAction(tr("&Random (N1,M)"));   setId(action,A_GENERATE_RANDOM);

  QMenu *macro = menuBar()->addMenu(tr("&Macro")); 
  connect(macro,SIGNAL(triggered(QAction *)),SLOT(macroHandler(QAction *)));
  action = macro->addAction(tr("Start recording"));action->setData(1);
  action = macro->addAction(tr("Stop  recording"));action->setData(2);
  action = macro->addAction(tr("Continue recording"));action->setData(3);
  macro->addSeparator();
  action = macro->addAction(tr("Display Macro"));action->setData(6);
  action = macro->addAction(tr("Save Macro"));action->setData(7);
  action = macro->addAction(tr("Read Macro"));action->setData(8);
  macro->addSeparator();
  action = macro->addAction(tr("Insert a Pause"));action->setData(5);
  action = macro->addAction(tr("Repeat macro"));action->setData(4);

  userMenu = menuBar()->addMenu(tr("&User menu")); 
  action = userMenu->addAction(tr("Test &1"));   setId(action,A_TEST_1);
  action = userMenu->addAction(tr("Test &2"));   setId(action,A_TEST_2);
  action = userMenu->addAction(tr("Test &3"));   setId(action,A_TEST_3);

  QMenu *set = menuBar()->addMenu(tr("&Settings")); 
  QMenu *profile = set->addMenu(tr("&Pigale colors")); 
  profile->addAction(tr("&Edit Pigale Colors"),this,SLOT(EditPigaleColors()));
  profile->addAction(tr("&Gray profile"),this,SLOT(SetPigaleColorsProfile1()));
  profile->addAction(tr("&Yellow profile"),this,SLOT(SetPigaleColorsProfile2()));
  set->addAction(tr("Set a font"),this,SLOT(SetPigaleFont()));
  set->addAction(tr("&Documentation path"),this,SLOT(SetDocumentPath()));
  set->addAction(tr("&Save Settings"),this,SLOT(SaveSettings()));

  QMenu *help = menuBar()->addMenu(tr("&Information")); 
  help->addAction(infoIcon,tr("&Graph properties"),this,SLOT(computeInformation()),Qt::SHIFT+Qt::Key_F2);
  help->addSeparator();
  help->addAction(helpAct);
  help->addSeparator();
  help->addAction(tr("About &Qt..."), this, SLOT(aboutQt()));
  help->addAction(tr("&About..."), this, SLOT(about()),Qt::Key_F1);
  //End of the menuBar():window
 

  //Resize
#if TDEBUG
  setCaption(tr("Qt4 Pigale Editor ")+PACKAGE_VERSION+" "+tr("Debug Mode"));
#else
  setCaption(tr("Qt4 Pigale Editor ")+PACKAGE_VERSION);
#endif

  resize(pigaleWindowInitXsize,pigaleWindowInitYsize);
  QRect rect_status(0,0,pigaleWindowInitXsize/2,30);
  progressBar->setGeometry(rect_status); 
  
  QPalette Palette1; 
  Palette1.setColor(QPalette::Window,QColor(QColorDialog::customColor(1)));
  Palette1.setColor(QPalette::WindowText,Qt::darkGreen);
  statusBar()->setPalette(Palette1,TRUE);
  statusBar()->setAutoFillBackground(true); 
  progressBar->hide();

  mainWidget->setFocus();
  //Check for documentation repertory
  QFileInfo fdoc = QFileInfo(DirFileDoc);
  if(!fdoc.exists() || !fdoc.isDir())
      {int rep = QMessageBox::warning(this,"Pigale Editor"
                                      ,tr("I cannot find the repertory <b>Doc<br>"
                                          "Load manually ?")
                                      ,QMessageBox::Ok
                                      ,QMessageBox::Cancel);
      if(rep == 1)
          {DirFileDoc = QFileDialog::
          getExistingDirectory(".",this,"find", tr("Choose the documentation directory <em>Doc</em>"),TRUE);
          if(!DirFileDoc.isEmpty())
              {QFileInfo fi =  
              QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
              if(fi.exists())
                  {browser->setSearchPaths(QStringList(DirFileDoc));
                  browser->setSource(QUrl("manual.html"));
                  SaveSettings();
                  }
              else
                  QMessageBox::information(this,"Pigale Editor"
                                           ,tr("I cannot find the inline manual<em>manual.html</em>")
                                           ,QMessageBox::Ok);
              }
          }
      }
  else
      {browser->setSearchPaths(QStringList(DirFileDoc));
      QUrl url = QUrl("manual.html");
      browser->setSource(url);
      }
  qApp->processEvents();
  initMenuTest();
  if(CheckLogFile() == -1)Twait("Impossible to write in log.txt");
  UndoInit();// Create a tgf file with no records
  LogPrintf("Init seed:%ld\n",randomSetSeed());
  gw->update();
  if(!Server)load(0);
  }
pigaleWindow::~pigaleWindow()
  {delete printer;
  pigaleThread.terminate();pigaleThread.wait();
  UndoErase();
  LogPrintf("END\n");
  }
void pigaleWindow::AllowAllMenus()
  {QMapIterator<int,QAction *> i(menuIntAction);
    while(i.hasNext()) 
        {i.next();
        i.value()->setVisible(true);
        }
  }
void  pigaleWindow::setUserMenu(int i, const QString &txt)
 {QList<QAction *> list = userMenu->actions();
 list.at(i-1)->setText(txt);
 }
void pigaleWindow::seedEdited(const QString & t)
  {randomSetSeed() = atol((const char *)t); 
  }
void pigaleWindow::spinN1Changed(int val)
  {Gen_N1 = val;}
void pigaleWindow::spinN2Changed(int val)
  {Gen_N2 = val;}
void pigaleWindow::spinMChanged(int val)
  {Gen_M = val;}
void  pigaleWindow::distOption(int use)
  {useDistance() = use;
  }
void  pigaleWindow::setShowOrientation(bool val)
  {ShowOrientation() = val;
  box6->setCheckState(ShowOrientation() ? Qt::Checked : Qt::Unchecked);
  }
void pigaleWindow::SetPigaleFont()
 {bool ok;
  QFont font = QFontDialog::getFont( &ok, this->font(), this );
  if(!ok)return;
  QApplication::setFont(font,true);
 }
void pigaleWindow::showLabel(int show)
  {int _show = ShowVertex();
  ShowVertex() = show -3;
  if(ShowVertex() != _show && GC.nv())
      {switch(tabWidget->currentPageIndex())
          {case 0:
              gw->update();
              break;
          case 1:
              mypaint->update();
              break;
          }
      }
  }
void pigaleWindow::about()
  {QMessageBox::about(this,tr("Pigale Editor"), 
                      "<b>"+tr("Pigale Editor")+"</b> (version:  "+PACKAGE_VERSION+")"
                      "<br><b>Copyright (C) 2001</b>"
                      +"<br>Hubert de Fraysseix"
	    +"<br>Patrice Ossona de Mendez "
	    +"<br> See <em>license.html</em>");
  }
void pigaleWindow::aboutQt()
  {QMessageBox::aboutQt(this,"Qt Toolkit");
  }
void pigaleWindow::print()
  {switch(tabWidget->currentPageIndex())
      {case 0:
          gw->print(printer);
          break;
      case 1:
          mypaint->print(printer);
          break;
      case 3:
          graphsym->print(printer);
          break;
      default:
          break;
      }
  }
void pigaleWindow::png()
  {switch(tabWidget->currentPageIndex())
      {case 0:
          gw->png();
          break;
      case 1:
          mypaint->png();
          break;
      case 2:
          graphgl->png();
          break;
      case 3:
          graphsym->png();
          break;
      default:
          break;
      }
  }
