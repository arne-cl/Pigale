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

#include <stdlib.h>
#include <stdio.h>

#include "MyWindow.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <TAXI/Tfile.h> 
#include <TAXI/Tgf.h> 
#include <TAXI/Tmessage.h> 
#include <TAXI/Tdebug.h>
#include <QT/MyWindow_doc.h> 
#include <QT/Misc.h> 
#include <QT/Handler.h>
#include <QT/MyPaint.h>
#include <QT/MyCanvas.h>

#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qinputdialog.h> 
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

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView
#else
#include <qtextedit.h>
#include <qtextbrowser.h>
#include <qsettings.h>
#endif


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

void Test(GraphContainer &GC,int action);
void macroRecord(int action);
void UndoErase();
void SaveSettings();
static char undofile[L_tmpnam];


MyWindow::MyWindow()
    : QMainWindow(0,"_Pigale",WDestructiveClose )
    ,MacroLooping(false)
    ,GraphIndex1(1),GraphIndex2(1),pGraphIndex(&GraphIndex1)
    ,IsUndoEnable(true)
    ,MacroRecording(false),MacroExecuting(false)
  {int id;

  // Export some data
  DefineGraphContainer(&GC);
  DefineMyWindow(this);
  //Create an  undofile name
  tmpnam(undofile);
  // Atexit: Erase undo.tgf
  atexit(UndoErase);
#if QT_VERSION >= 300
  LoadSettings();
  QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
#endif
  // Define some colors
  QPalette LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QColorGroup::Base,QColor(QColorDialog::customColor(1)));
  // Create a printer
  printer = new QPrinter;
#if QT_VERSION >= 300
  if(setting.readNumEntry("/pigale/printer orientation",QPrinter::Landscape) != QPrinter::Landscape)
      printer->setOrientation(QPrinter::Portrait); 
  else
      printer->setOrientation(QPrinter::Landscape); 

  if(setting.readNumEntry("/pigale/printer colormode",QPrinter::Color) != QPrinter::Color)
      printer->setColorMode(QPrinter::GrayScale);
  else
      printer->setColorMode(QPrinter::Color);
#else
  printer->setOrientation(QPrinter::Landscape); 
  printer->setColorMode(QPrinter::Color);
#endif

  // Window size
#if QT_VERSION >= 300
  QRect rect = QApplication::desktop()->screenGeometry();
  int MyWindowInitYsize = setting.readNumEntry("/pigale/geometry height",rect.height());
  int MyWindowInitXsize = setting.readNumEntry("/pigale/geometry width",817);
#else
  QWidget *d = QApplication::desktop();
  int MyWindowInitYsize = d->height();
  int MyWindowInitXsize = 817; 
#endif
  int MyEditorMinXsize  = 220, MyEditorMinYsize  = 180;
  int MyEditorMaxXsize  = 300, MyEditorMaxYsize  = 500;


  // Widgets
  QWidget *mainWidget = new QWidget(this,"mainWidget");
  setCentralWidget(mainWidget);
  QHBoxLayout *topLayout = new QHBoxLayout(mainWidget,0,0,"topLayout");

  //leftLayout: Graph editor,Paint,GL,Browser
  QHBoxLayout * leftLayout = new QHBoxLayout(topLayout,0,"leftLayout");
  tabWidget = new  QTabWidget(mainWidget,"tab");
  leftLayout->addWidget(tabWidget,1);
  tabWidget->setMinimumSize(MyWindowInitXsize-MyEditorMinXsize-10,100);
  tabWidget->setPalette(LightPalette);
  mypaint =  new MyPaint(tabWidget,"paint",this);
  gw = new  GraphWidget(tabWidget,"graphwidget",this);
  graphgl  = new GraphGL(tabWidget,"graphgl",this);
  graphsym = new GraphSym(tabWidget,"graphsym",this);
#if QT_VERSION >= 300
  QTextBrowser *browser = new QTextBrowser(tabWidget,"doc");
//   browser->mimeSourceFactory()->setFilePath("documentation");
//   browser->setSource("manual.html");
#endif
  tabWidget->addTab(gw,"Graph &Editor");
  tabWidget->addTab(mypaint,"");
  tabWidget->addTab(graphgl,""); 
  tabWidget->addTab(graphsym,""); 
#if QT_VERSION >= 300
  tabWidget->addTab(browser,"User Guide"); 
  QPalette bop(QColorDialog::customColor(3));
  browser->setPalette(bop);
#endif
 
   // rightLayout
  QGridLayout *rightLayout = new  QGridLayout(topLayout,2,4,-1,"rightLayout");
  e = new QTextEdit(mainWidget,"informations");
  e->setMinimumSize(MyEditorMinXsize,MyEditorMinYsize);
  e->setMaximumSize(MyEditorMaxXsize,MyEditorMaxYsize);
  QBrush pb(QColorDialog::customColor(1)); e->setPaper(pb); 
#if QT_VERSION >= 300
  e->setReadOnly(true);
#endif
  // editor menu
  mouse_actions = new Mouse_Actions(mainWidget,"mouseactions",0,gw,graphgl);
  mouse_actions->setPalette(LightPalette);
  graph_properties = new Graph_Properties(mainWidget,menuBar(),"graph_properties");
  graph_properties->setMinimumSize(MyEditorMinXsize,170);
  graph_properties->setMaximumSize(MyEditorMinXsize,190);
  QLabel *lab0 = new QLabel(mainWidget);
  lab0->setText("<br><center><b>Messages</b></center>");
  lab0->setTextFormat(Qt::RichText);
  lab0->setMinimumSize(MyEditorMinXsize,20);
  lab0->setMaximumSize(MyEditorMinXsize,25);
  rightLayout->addMultiCellWidget(lab0,1,1,1,2);
  rightLayout->addMultiCellWidget(e,2,2,1,2);
  rightLayout->addMultiCellWidget(graph_properties,3,3,1,2);
  rightLayout->addMultiCellWidget(mouse_actions,4,4,1,2);

  //Pixmaps
  QPixmap openIcon = QPixmap(fileopen),newIcon = QPixmap(filenew),saveIcon = QPixmap(filesave);
  QPixmap leftIcon = QPixmap(sleft),   rightIcon = QPixmap(sright);
  QPixmap reloadIcon = QPixmap(sreload);
  QPixmap infoIcon = QPixmap(info), helpIcon = QPixmap(help),printIcon = QPixmap(fileprint);
  QPixmap xmanIcon = QPixmap(xman), undoLIcon = QPixmap(sleftarrow);
  QPixmap undoSIcon = QPixmap(sfilesave),undoRIcon = QPixmap(srightarrow);
  QPixmap macroplayIcon = QPixmap(macroplay);
 
  //ToolBar
  tb = new QToolBar(this,"toolbar" );
  QToolButton *fileopen,*filenew,*filesave,*fileprint,*info,*macroplay;
  filenew = new QToolButton(newIcon,"New Graph",QString::null,this,SLOT(newgraph()),tb,"New");
  fileopen = new QToolButton(openIcon,"Open File (tgf/txt)",QString::null,this,SLOT(load()),tb,"Open");
  QWhatsThis::add(fileopen,fileopen_txt);
  filesave = new QToolButton(saveIcon,"Save File",QString::null,this,SLOT(save()),tb,"Save graph" );
  fileprint = new QToolButton(printIcon,"Print window",QString::null,this,SLOT(print()),tb,"Print" );
  info = new QToolButton(infoIcon,"Information",QString::null,this,SLOT(information()),tb,"info" );
  tb->addSeparator();
  left = new QToolButton(leftIcon,"Previous Graph",QString::null,this,SLOT(previous()),tb,"Previous");
  QWhatsThis::add(left,left_txt);
  redo = new QToolButton(reloadIcon,"Reload Graph",QString::null,this,SLOT(reload()),tb,"Reload");
  QWhatsThis::add(redo,redo_txt);
  right = new QToolButton(rightIcon,"Next Graph",QString::null,this,SLOT(next()),tb,"Next");
  QWhatsThis::add(right,right_txt);
  tb->addSeparator();
  undoL = new QToolButton(undoLIcon,"Undo",QString::null,this,SLOT(Undo()),tb,"Undo L");
  undoS = new QToolButton(undoSIcon,"Save",QString::null,this,SLOT(UndoSave()),tb,"Save");
  undoR = new QToolButton(undoRIcon,"Redo",QString::null,this,SLOT(Redo()),tb,"Redo");
  tb->addSeparator();
  macroplay = new QToolButton(macroplayIcon,"Play macro",QString::null,
			      this,SLOT(macroPlay()),tb,"macroplay");
  tb->addSeparator();
  //toggle button:    setToggleButton(true); left->isOn()
  (void)QWhatsThis::whatsThisButton(tb);
  tb->addSeparator();

  //PopMenus
  QPopupMenu * file = new QPopupMenu( this );
  menuBar()->insertItem( "&File", file );
  file->insertItem(newIcon,"&New Graph",this, SLOT(newgraph()));
  id = file->insertItem(openIcon,"&Open",this, SLOT(load()));
  file->setWhatsThis(id,fileopen_txt);
  file->insertItem("Save &as ...", this, SLOT(outputfile()));
  file->insertItem(saveIcon,"&Save", this, SLOT(save()));
  file->insertItem(saveIcon,"Save Asc&ii", this, SLOT(save_ascii()));
  file->insertSeparator();
  file->insertItem("&Delete current record",this,SLOT(deleterecord()));
  file->insertItem("S&witch Input/Output files",this,SLOT(switchInputOutput()));
  file->insertSeparator();
  file->insertItem(printIcon,"&Print",this, SLOT(print()));
  file->insertSeparator();
  file->insertItem("&Quit",qApp,SLOT(closeAllWindows()));

  QPopupMenu * augment = new QPopupMenu(this);
  menuBar()->insertItem("&Augment",augment);
  connect(augment,SIGNAL(activated(int)),SLOT(handler(int)));
  augment->insertItem(xmanIcon,"Make &Connected",        101);
  augment->setWhatsThis(101,"Make a graph 2-connected");
  augment->insertItem(xmanIcon,"Make &2-Connected",      102);
  augment->setWhatsThis(102,"Make a PLANAR graph 2-connected");
  augment->insertItem(xmanIcon,"Make 2-Connected &Opt",  103);
  augment->setWhatsThis(103,"Make a PLANAR graph 2-connected\n with a minimal degree increase");
  augment->insertItem(xmanIcon,"Make 2-Connected &NP",   104);
  augment->setWhatsThis(104,"Make a graph 2-connected");
  augment->insertItem(xmanIcon,"&Vertex Triangulate",    105);
  augment->setWhatsThis(105,"Triangulate a PLANAR graph by adding vertices");
  augment->insertItem(xmanIcon,"&ZigZag Triangulate",    106);
  augment->setWhatsThis(106,"Triangulate a PLANAR graph by adding edges");
  augment->insertItem(xmanIcon,"T&ricon. Triangulate",   107);
  augment->setWhatsThis(107,"Optimally triangulate a PLANAR graph by adding edges");
  augment->insertItem(xmanIcon,"Vertex &Quadrangulate",  108);
  augment->setWhatsThis(108,"Quadrangulate a planar bipartite graph");
  augment->insertSeparator();
  augment->insertItem("&Bisect all edges",     109);

  QPopupMenu *remove = new QPopupMenu( this );
  menuBar()->insertItem("&Remove",remove);
  connect(remove,SIGNAL(activated(int)),SLOT(handler(int)));
  remove->insertItem("&Isolated vertices",   401);
  remove->insertItem("&Multiple edges",      403);
  remove->insertItem("Ist&hmus",             404);
  remove->insertSeparator();
  remove->insertItem("Colored &vertices",    405);
  remove->insertItem("Colored &edges",       406);
  remove->insertItem("&Thick edges",         407);
//   remove->insertItem("Colored &vertices", gw,SLOT(EraseColorVertices()));
//   remove->insertItem("Colored &edges",    gw,SLOT(EraseColorEdges()));
//   remove->insertItem("&Thick edges",      gw,SLOT(EraseThickEdges()));

  QPopupMenu *embed = new QPopupMenu(this);
  menuBar()->insertItem("E&mbed",embed);
  connect(embed,SIGNAL(activated(int)),SLOT(handler(int)));
  embed->insertItem("&FPP Fary",             203);
  embed->insertItem("&Schnyder",             201);
  embed->insertItem("Schnyder &V ",          202);
  embed->insertSeparator();
  embed->insertItem("&Tutte"      ,          205);
  embed->insertItem("Tutte &Circle",         204);
  embed->insertSeparator();
  embed->insertItem("&Visibility",           250);
  embed->insertItem("FPP Visi&bility",       253);
#ifdef ALPHA 
  embed->insertItem("&General Visibility",   254);
  embed->insertItem("&T Contact",            255);
#endif
  embed->insertItem("&Contact Biparti",      251);
  embed->insertSeparator();
  embed->insertItem("&Polar",                252);
  embed->insertSeparator(); 
  embed->insertItem(xmanIcon,"&Embedding in Rn",298);
  embed->setWhatsThis(298,embed3d_txt);

  QPopupMenu *dual = new QPopupMenu( this );
  menuBar()->insertItem("&Dual/Angle",dual);
  connect(dual,SIGNAL(activated(int)),SLOT(handler(int)));
  dual->insertItem("&Dual",                 301);
  dual->insertItem("&Geometric Dual",       302);
  dual->insertSeparator();
  dual->insertItem("&Angle",                303);
  dual->insertItem("G&eometric Angle",      304);

  QPopupMenu *algo = new QPopupMenu( this );
  menuBar()->insertItem("&Algo",algo);
  connect(algo,SIGNAL(activated(int)),SLOT(handler(int)));
  algo->insertItem("Find &Kuratowski",              601);
  algo->insertItem("Find &Cotree Critical",         602);
  algo->insertItem("Color red  non critical edges ",612);
  algo->insertItem("Max.Planar (simple graph) Fast",603);
  algo->insertItem("&Max.Planar (simple graph) Slow ",604);
  algo->insertSeparator();
  algo->insertItem("Use &Geometric Map",            607);
  algo->insertItem("Use &LRALGO Map",               608);
  algo->insertSeparator();
  algo->insertItem("&Color everything",             605);
  algo->insertItem("Color &bipartite",              610);
  algo->insertItem("Color e&xterior face",          611);
  algo->insertSeparator();
  algo->insertItem("&Symmetry",                     609);
  algo->insertSeparator();
  spin_N = new QSpinBox(2,50,1,algo,"spinN");
  spin_N->setValue(2);     spin_N->setPrefix("Number of classes: ");
  algo->insertItem(spin_N);
  algo->insertItem("&Partition",                    606);

  QPopupMenu *orient = new QPopupMenu( this );
  menuBar()->insertItem("&Orient",orient);
  connect(orient,SIGNAL(activated(int)),SLOT(handler(int)));
  orient->insertItem("&Show orientation",   10020);
  orient->setItemChecked(10020,ShowOrientation());
  orient->insertItem("&Orient all edges",     701);
  orient->insertItem("&Inf Orientation",      702);
  orient->insertItem("Planar &3-Con.",        703);
  orient->insertItem("Planar &Biparti",       704);
  orient->insertItem("Planar &Schnyder",      705);
  orient->insertItem("B&ipolarOrient Planar", 706);

  QPopupMenu *generate      = new QPopupMenu( this );
  QPopupMenu *popupCubic    = new QPopupMenu(this);
  QPopupMenu *popupQuadric  = new QPopupMenu(this);
  QPopupMenu *popupBipar    = new QPopupMenu(this);
  QPopupMenu *popupPlan     = new QPopupMenu(this);
  menuBar()->insertItem("&Generate",generate);
  connect(generate,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupCubic,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupQuadric,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupBipar,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupPlan,SIGNAL(activated(int)),SLOT(handler(int)));
  generate->insertItem("&Planar",popupPlan);
  popupPlan->insertItem("connnected (M)",           505);
  popupPlan->insertItem("2-connnected (M)",         506);
  popupPlan->insertItem("3-connnected (M)",         507);
  generate->insertItem("&Planar cubic",popupCubic);
  popupCubic->insertItem("dual:2 connnected (M)",  510);
  popupCubic->insertItem("dual:3 connnected (M)",  511);
  popupCubic->insertItem("dual:4 connnected (M)",  512);
  generate->insertItem("&Planar quadric",popupQuadric);
  popupQuadric->insertItem("dual:1 connnected (M)",513);
  popupQuadric->insertItem("dual:2 connnected (M)",514);
  popupQuadric->insertItem("dual:3 connnected (M)",515);
  //popupQuadric->insertItem("Bi-Quadric (N1)"       ,516);
  generate->insertItem("&Planar bipartite",popupBipar);
  popupBipar->insertItem("Bipartite (M)"           ,517);
  popupBipar->insertItem("Bipartite cubic dual:2 connected (M)",518);
  popupBipar->insertItem("Bipartite cubic dual:3 connected (M)",519);
  generate->insertItem("&Grid (N1,N2)",             501);
  generate->insertSeparator();
  generate->insertItem("&Complete (N1)",            502);
  generate->insertItem("&BipartiteComplete (N1,N2)",503);
  generate->insertItem("&Random (N1,M)",            504);
  generate->insertSeparator();
  // Erase multiple edges
  generate->insertItem("Erase multiple edges",10006);
  generate->setItemChecked(10006,EraseMultipleEdges());
#if QT_VERSION >= 300
  int Gen_N1 = setting.readNumEntry("/pigale/generate/gen N1",10);
  int Gen_N2 = setting.readNumEntry("/pigale/generate/gen N2",10);
  int Gen_M  = setting.readNumEntry("/pigale/generate/gen M",30);
#else
  int Gen_N1 = 10;
  int Gen_N2 = 10;
  int Gen_M  = 30;
#endif
  spin_N1 = new QSpinBox(1,100000,1,generate,"spinN1");
  spin_N1->setValue(Gen_N1);     spin_N1->setPrefix("N1: ");
  spin_N2 = new QSpinBox(1,100000,1,generate,"spinN2");
  spin_N2->setValue(Gen_N2);     spin_N2->setPrefix("N2: ");
  spin_M = new QSpinBox(1,300000,1,generate,"spinM");
  spin_M->setValue(Gen_M);      spin_M->setPrefix("M: ");
  generate->insertItem(spin_N1);
  generate->insertItem(spin_N2);
  generate->insertItem(spin_M);

  QPopupMenu *macroMenu = new QPopupMenu( this );
  menuBar()->insertItem("&Macro",macroMenu);
  connect(macroMenu,SIGNAL(activated(int)),SLOT(macroHandler(int)));
  spinMacro = new QSpinBox(1,10000,1,macroMenu,"spinMacro");
#if QT_VERSION >= 300
  int Repeat = setting.readNumEntry("/pigale/generate/gen Repeat",100);
#else
  int Repeat = 100;
#endif
  spinMacro->setValue(Repeat);spinMacro->setPrefix("Repeat: ");
  macroMenu->insertItem("Start recording",1);
  macroMenu->insertItem("Stop  recording",2);
  macroMenu->insertItem("Continue recording",3);
  macroMenu->insertSeparator();
  macroMenu->insertItem(spinMacro);
  macroMenu->insertItem("Repeat macro",4);

  QPopupMenu *userMenu = new QPopupMenu( this );
  menuBar()->insertItem("&UserMenu",userMenu);
  connect(userMenu,SIGNAL(activated(int)),SLOT(userHandler(int)));
  userMenu->insertItem("Test &1",1);
  userMenu->insertItem("Test &2",2);
  userMenu->insertItem("Test &3",3);

  QPopupMenu *settings      = new QPopupMenu(this);
  QPopupMenu *popupLabel    = new QPopupMenu(this);
  QPopupMenu *popupEmbed    = new QPopupMenu(this);
  QPopupMenu *popupDistance = new QPopupMenu(this);
  QPopupMenu *popupLimits   = new QPopupMenu(this);
  QPopupMenu *popupSave     = new QPopupMenu(this);
  QComboBox *comboLabel     = new QComboBox(popupLabel,"LabelCombo");
  QComboBox *comboDistance  = new QComboBox(popupDistance,"DistCombo");
  spin_MaxNS                = new QSpinBox(500,10000,1,popupLimits,"spinMaxNS");
  spin_MaxND                = new QSpinBox(100,10000,1,popupLimits,"spinMaxND");
  QPushButton *button_save  = new QPushButton(popupSave,"SaveSettings");
  menuBar()->insertItem("&Settings",settings);
  connect(settings,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(spin_MaxNS,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNSlowChanged(int)));
  connect(spin_MaxND,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNDisplayChanged(int)));
  connect(comboLabel,SIGNAL(activated(int)),SLOT(showLabel(int)));
  connect(comboDistance,SIGNAL(activated(int)),SLOT(distOption(int)));
  connect(popupEmbed,SIGNAL(activated(int)),SLOT(handler(int)));
  settings->insertTearOffHandle();
  //debug()
  settings->insertItem("&Debug",10001);
  settings->setItemChecked(10001,debug());
  //undoEnable
  settings->insertItem("&Undo Enable",10005);
  settings->setItemChecked(10005,IsUndoEnable);
  //Pigale colors
  settings->insertItem("&Pigale Colors",10010);
  //Pigale limits
  settings->insertItem("Limit number of vertices",popupLimits);
  popupLimits->insertItem("for slow algorithms");
#if QT_VERSION >= 300
  int MaxNS = setting.readNumEntry("/pigale/limits slow_algo",500);
  int MaxND = setting.readNumEntry("/pigale/limits display",500);
#else
  int MaxNS = 500;
  int MaxND = 500;
#endif
  spin_MaxNS->setValue(MaxNS);    
  spin_MaxNS->setPrefix("Slow algorithms: ");
  popupLimits->insertItem(spin_MaxNS);
  popupLimits->insertItem("for displaying graph");
  spin_MaxND->setValue(MaxND);      
  spin_MaxND->setPrefix("Display: ");
  popupLimits->insertItem(spin_MaxND);
   //Vertex Label Options
  settings->insertSeparator();
  settings->insertItem("&Vertex show",popupLabel);
  popupLabel->insertItem(comboLabel);
  comboLabel->insertItem("Index");
  comboLabel->insertItem("Label");
  int current = (ShowVertex() == -1) ? 0 : 1;
  comboLabel->setCurrentItem(current);showLabel(current);
  //Embed Settings
  settings->insertSeparator();
  settings->insertItem("&Embed Options",popupEmbed);
  popupEmbed->insertItem("Schnyder: &Rect",        10002);
  popupEmbed->insertItem("Schnyder: &Longest Face",10003);
  popupEmbed->insertItem("Schnyder: &Color Edges", 10004);
  popupEmbed->setItemChecked(10002,SchnyderRect());
  popupEmbed->setItemChecked(10003,SchnyderLongestFace());
  popupEmbed->setItemChecked(10004,SchnyderColor());
  //Distance Settings 
  settings->insertItem("&Distance Options",popupDistance);
  popupDistance->insertItem(comboDistance);
  comboDistance->insertItem("Czekanovski-Dice");
  comboDistance->insertItem("Bisect");
  comboDistance->insertItem("Incidence");
  comboDistance->insertItem("Incidence M");
  comboDistance->insertItem("Oriented");
  comboDistance->setCurrentItem(useDistance());distOption(useDistance());
  //Save Settings
  settings->insertSeparator();
  settings->insertItem("&Save Settings",popupSave);
  button_save->setText("Save");
  popupSave->insertItem(button_save);
  connect(button_save,SIGNAL(clicked()),SLOT(SaveSettings()));

  //End of the menuBar():window
  //help
  QPopupMenu * help = new QPopupMenu(this);
  menuBar()->insertItem("&Help",help);
  help->insertItem(infoIcon,"&Graph properties",this,SLOT(information())
		   ,SHIFT+Key_F2);
  help->insertSeparator();
  help->insertItem("&About", this, SLOT(about()),Key_F1);
  help->insertItem("About &Qt", this, SLOT(aboutQt()));
  help->insertSeparator();
  help->insertItem(helpIcon,"What is ?",this,SLOT(whatsThis()),SHIFT+Key_F1);
  //Resize
  setCaption("Pigale");
  statusBar()->setBackgroundColor(QColor(QColorDialog::customColor(1)));

  resize(MyWindowInitXsize,MyWindowInitYsize);
  mainWidget->setFocus();
  DebugPrintf("Debug Messages\nUndoFile:%s",undofile);
  if(Error() == -1){Twait("Impossible to write in log.txt");Error() = 0;}
  
  
  QFileInfo fi  = QString(getenv("TGF"));
  if(fi.exists() && fi.isDir() )DirFile = fi.filePath();
  else {Twait("The variable TGF should point on a tgf directory");exit(1);}
#if QT_VERSION >= 300
  InputFileName = setting.readEntry("/pigale/TgfFile input",DirFile + QDir::separator() + "a.tgf");
  OutputFileName = setting.readEntry("/pigale/TgfFile output",InputFileName);
  QFileInfo fis  = InputFileName;
  if(fis.exists() && fis.isDir() )DirFile = fis.filePath();
#else
  InputFileName = DirFile + QDir::separator() + "a.tgf";
  OutputFileName = InputFileName;
#endif
#if QT_VERSION >= 300
  //Check for documentation repertory
  QString DocPath = setting.readEntry("/pigale/Documentation dir","documentation");
  QFileInfo fdoc = QFileInfo(DocPath);
  if(!fdoc.exists() || !fdoc.isDir())
      {int rep = QMessageBox::warning(this,"Pigale Editor"
				      ,"I cannot find the repertory <b>documentation.<br>"
				      "Load manually ?"
				      ,QMessageBox::Ok
				      ,QMessageBox::Cancel);
      if(rep == 1)
	  {DocPath = QFileDialog::getExistingDirectory(".",this,"find",
						       "Choose the documentation directory",TRUE);
	  if(!DocPath.isEmpty())
	      {browser->mimeSourceFactory()->setFilePath(DocPath);
	      browser->setSource("manual.html");
	      setting.writeEntry("/pigale/Documentation dir",DocPath);
	      }
	  }
      }
  else
      {browser->mimeSourceFactory()->setFilePath(DocPath);
      browser->setSource("manual.html");
      }
  setting.writeEntry("/pigale/Documentation dir",DocPath);
#endif  
  // if no load check pgraphindex
  load(0);
  }

MyWindow::~MyWindow()
  {delete printer;
  }
void MyWindow::load()
  {QString FileName = QFileDialog::
  getOpenFileName(DirFile,"Tgf Files(*.tgf);;Text Files (*.txt)",this);

  if(FileName.isEmpty())
      newgraph();
  else
      {InputFileName = FileName;
      *pGraphIndex = 1;
      int NumRecords =GetNumRecords((const char *)InputFileName);
      if(NumRecords > 1)
	  {bool ok = FALSE;
	  QString m;
	  m.sprintf("Select a graph (1/%d)",NumRecords);
	  *pGraphIndex = QInputDialog::
	  getInteger("Pigale",m,1,1,NumRecords,1,&ok,this);
	  if(ok)load(0);
	  }
      else load(0);
      }
  }
void MyWindow::load(int pos)
  {QString m;
  if(IsFileTgf((const char *)InputFileName) == -1)//file does not exist
      {m.sprintf("file %s does not exist",(const char *)InputFileName);
      statusBar()->message(m,2000);
      gw->update();
      return;
      }      
  UndoClear();UndoSave();
  int NumRecords =GetNumRecords((const char *)InputFileName);
  if(pos == 1)++(*pGraphIndex);
  else if(pos == -1)--(*pGraphIndex);
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  if(ReadGraph(GC,(const char *)InputFileName,NumRecords,*pGraphIndex) != 0)
      {m.sprintf("Could not read:%s",(const char *)InputFileName);
      statusBar()->message(m,2000);
      return;
      }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName,*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  G.RemoveLoops();
  banner();
  if(MacroExecuting)return;
  information(); gw->update();
  }
void MyWindow::save()
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  QString titre(~title());
  bool ok = TRUE;
  titre = QInputDialog::getText("Pigale","Enter the graph name",
                    QLineEdit::Normal,titre, &ok, this );
  if(ok && !titre.isEmpty()) title() = (const char *)titre;
  else return;
  if(SaveGraphTgf(G,(const char *)OutputFileName,1) == 1)
      {QString t;
      t.sprintf("Cannot open file:%s",(const char *)OutputFileName);
      Twait((const char *)t);
      return;
      }
  GraphIndex2 = GetNumRecords((const char *)OutputFileName);
  banner();
  }
void MyWindow::save_ascii()
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  QString titre(~title());
  bool ok = TRUE;
  titre = QInputDialog::getText("Pigale","Enter the graph name",
                    QLineEdit::Normal,titre, &ok, this );
  if(ok && !titre.isEmpty()) title() = (const char *)titre;
  QString OutputAsciiFile = QFileDialog::
  getSaveFileName(DirFile,"Txt Files(*.txt)",this);
  if(OutputAsciiFile.isEmpty())return;
  QFileInfo fi = QFileInfo(OutputAsciiFile);
  if(fi.exists())
      {int rep = QMessageBox::warning(this,"Pigale Editor"
			    ,"This file already exixts.<br>"
			    "Overwrite ?"
			    ,QMessageBox::Ok 
			    ,QMessageBox::Cancel);
      if(rep == 2)return;
      } 
  SaveGraphAscii(G,(const char *)OutputAsciiFile);
  }
void MyWindow::outputfile()
  {QString FileName = QFileDialog::getSaveFileName(DirFile,"Tgf Files(*.tgf)",this);
  if(FileName.isEmpty())return;
  OutputFileName = FileName;
  save();
  }
void MyWindow::deleterecord()
  {if(IsFileTgf((const char *)InputFileName))
      DeleteTgfRecord((const char *)InputFileName,*pGraphIndex);
  else
      Twait("Not a tgf file");
  banner();
  }
void MyWindow::switchInputOutput()
  {Tswap(InputFileName,OutputFileName);
  Tswap(GraphIndex1,GraphIndex2);
  pGraphIndex   =  &GraphIndex1;
  load(0);
  }
void MyWindow::newgraph()
  {statusBar()->message("New graph");
  UndoClear();UndoSave();
  Graph G(GetMainGraph());
  G.StrictReset();
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  if(debug())DebugPrintf("**** New graph");
  information();gw->update();
  }
void MyWindow::previous()
  {load(-1);}
void MyWindow::reload()
  {load(0);}
void MyWindow::next()
  {load(1);}
void MyWindow::information()
  {MessageClear();
  graph_properties ->update();
  }
void MyWindow::MessageClear()
  {e->setText("");}
void MyWindow::Message(QString s)
  {e->append(s);
#if QT_VERSION == 300
  e->scrollToBottom ();
#endif
  } 
void MyWindow::handler(int action)
  {int ret = 0;
  int drawing;
  if(debug())DebugPrintf("handler:%d",action);
  if(MacroRecording)macroRecord(action);
  if(action < 200)
      {UndoSave();timer.restart();
      ret = AugmentHandler(action);
      }
  else if(action < 300)
      {timer.restart();
      ret = EmbedHandler(action,drawing);
      }
  else if(action < 400)
      {UndoClear();UndoSave();timer.restart();
      ret = DualHandler(action);
      }
  else if(action < 500)
      {UndoSave();timer.restart();
      ret = RemoveHandler(action);
      }
  else if(action < 600)
      {UndoClear();UndoSave();timer.restart();
      ret = GenerateHandler(action,spin_N1->value(),spin_N2->value(),spin_M->value());
      }
  else if(action < 700)
      {timer.restart();
      ret = AlgoHandler(action,spin_N->value());
      }
  else if(action < 800)
      {timer.restart();
      ret = OrientHandler(action);
      menuBar()->setItemChecked(10020,ShowOrientation());
      }
  else if(action > 10000)
      {if(action == 10010){SetPigaleColors();return;}
      menuBar()->setItemChecked(action,!menuBar()->isItemChecked(action));
      debug()               =  menuBar()->isItemChecked(10001);
      SchnyderRect()        =  menuBar()->isItemChecked(10002);
      SchnyderLongestFace() =  menuBar()->isItemChecked(10003);
      SchnyderColor()       =  menuBar()->isItemChecked(10004);
      ShowOrientation()     =  menuBar()->isItemChecked(10020);
      EraseMultipleEdges()  =  menuBar()->isItemChecked(10006);

      if(action == 10005)UndoEnable(menuBar()->isItemChecked(action));
      if(action == 10020)gw->update();
      return;
      }
  else
      return;
  if(MacroExecuting )return;
  //-1:Error 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 
  // 3:(Drawing) 4:(3d) 5:symetrie
  if(ret < 0)return;
  double Time = timer.elapsed()/1000.;
  if(ret == 1)
      gw->update();
  else if(ret == 2)
      {information();
      gw->update();
      }
   else if(ret == 3)
       mypaint->update(drawing); 
  else if(ret == 4) //3d
      graphgl->update(); 
  else if(ret == 5) //symetrie
      {graphgl->update(); 
      graphsym->update();
      }
  double TimeG = timer.elapsed()/1000.;
  if(!MacroExecuting || debug())Tprintf("Used time:%3.3f (G+I:%3.3f)",Time,TimeG);
  }
void MyWindow::banner()
  {QString m;  
  QFileInfo fi((const char *)InputFileName);
  QFileInfo fo((const char *)OutputFileName);
  int NumRecords =GetNumRecords((const char *)InputFileName);
  int NumRecordsOut =GetNumRecords((const char *)OutputFileName);
  m.sprintf("Input file: %s %d/%d  Output file: %s %d Undo:%d/%d",(const char *)fi.fileName()
	    ,*pGraphIndex,NumRecords,(const char *)fo.fileName(),NumRecordsOut
      ,UndoIndex,UndoMax);
  statusBar()->message(m);
  }
void MyWindow::about()
  {QMessageBox::about(this,"Pigale Editor",
		      "<b>Copyright (C) 2001</b>"
		      "<br>Hubert de Fraysseix"
		      "<br>Patrice Ossona de Mendez "
		      "<br> See <em>pigale/license.html</em>");
  }
void MyWindow::aboutQt()
  {QMessageBox::aboutQt(this,"Qt Toolkit");
  }
void MyWindow::userHandler(int event)
  {
//   GeometricGraph G(GC);
//   load(1);
//   if(G.ne() != G.ecolor.vector().n()-1)qDebug("ERROR ecolor");
//   else qDebug("OK ecolor");
  Test(GC,event);
  //GeometricGraph G(GC);
//   if(G.ne() != G.ecolor.vector().n()-1)qDebug("ERROR ecolor");
//   else qDebug("OK ecolor");
  
  //Call the editor
  gw->update();
  }
void MyWindow::showLabel(int show)
  {//Now We can only show the index of a vertex or a long
  int _show = ShowVertex();
  switch(show)
      {case 0:
	   ShowVertex() = -1;
	   break;
      case 1:// PROP_LABEL = 0
	  ShowVertex() = PROP_LABEL; 
	  break;
      default:
	  return;
      }
  if(ShowVertex() != _show && GC.nv())gw->update();
  }
void  MyWindow::distOption(int use)
  {useDistance() = use;
  }
void MyWindow::print()
  {switch(tabWidget->currentPageIndex())
      {case 0:
	   gw->print(this->printer);
	   break;
      case 1:
	  mypaint->print(this->printer);
	  break;
      case 3:
	  graphsym->print(this->printer);
	  break;
      default:
	  break;
      }
  }

/*
The copies are cleared when a new graph is loaded or generated, but the last graph is saved
A copy is created when edges/vertices are added/deleted outside the editor

UndoMax  : # of save graphs
UndoSave : index of graph to restore [1,UndoMax]
Should always be possible to restore 1
*/
void MyWindow::Undo()
  {if(UndoIndex > 1)--UndoIndex;
  if(ReadGraph(GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  this->undoR->setEnabled(UndoMax != 0 && UndoIndex < UndoMax);
  }
void MyWindow::UndoSave()
  {if(!IsUndoEnable)return;
  TopologicalGraph G(GC);
  if(G.nv() == 0)return;
  if(SaveGraphTgf(G,undofile,1) == 1)
      {handler(10005);return;}
  UndoMax = GetNumRecords(undofile);
  UndoIndex = UndoMax;
  this->undoL->setEnabled(true);
  banner();
  }
void MyWindow::Redo()
  {if(UndoIndex < UndoMax)++UndoIndex;
  if(ReadGraph(GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  this->undoR->setEnabled(UndoIndex < UndoMax);
  }
void MyWindow::UndoClear()
  {if(!IsUndoEnable)return;
  UndoIndex = UndoMax = 0;
  this->undoL->setEnabled(false);
  this->undoR->setEnabled(false);
  QFileInfo fi(undofile);
  if(!fi.exists())return;
  QFile undo_tgf(undofile);
  if(!undo_tgf.remove())
      {Twait("Cannot remove undofile");
      menuBar()->setItemChecked(10005,false);
      }
  }
void MyWindow::UndoEnable(bool enable)
  {this->undoS->setEnabled(enable);
  if(!enable){UndoClear();banner();}
  IsUndoEnable = enable;
  }
// UndoErase is called when the program exit
void UndoErase()
  {QFileInfo fi(undofile);
  if(!fi.exists())return;
  QFile undo_tgf(undofile);
  undo_tgf.remove();
  }


