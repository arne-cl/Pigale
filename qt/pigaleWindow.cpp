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
#include <QT/Action.h>
#include <QT/pigalePaint.h> 
#include <QT/pigaleCanvas.h>
#include <QT/clientEvent.h>

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

int Test(GraphContainer &GC,int action,int & drawing);
void UndoErase();
void SaveSettings();
//static char undofile[L_tmpnam] = "/tmp/undo_XXXXXX" ;

#ifndef _WINDOWS
static char undofile[L_tmpnam] = "/tmp/_undo.tgf" ;
#else
static char undofile[L_tmpnam] = "_undo.tgf" ;
void initGraphDebug();
#endif

int InitPigaleServer(pigaleWindow *w);

pigaleWindow::pigaleWindow()
    :QMainWindow(0,"_Pigale",WDestructiveClose )
    ,ServerExecuting(false),ServerClientId(0)
    ,GraphIndex1(1),GraphIndex2(1),pGraphIndex(&GraphIndex1)
    ,UndoIndex(0),UndoMax(0)
    ,IsUndoEnable(true)
    ,MacroRecording(false),MacroLooping(false)
    ,MacroExecuting(false),MacroPlay(false),Server(false)
  {int id;
  // Initialze Error
  setError();
#ifdef _WINDOWS
   // as the compiler does not initialize static ...
   initGraphDebug();
#endif
   
  // Export some data
  DefineGraphContainer(&GC);
  DefinepigaleWindow(this);
  pigaleThread.mw = this;
  // Create the actions map
  mapActionsInit();
  // Create an  undofile name
  //mkstemp(undofile);

  // Create a tgf file with no records
  {Tgf undo_tgf;
  undo_tgf.open(undofile,Tgf::create);
  }
  // Atexit: Erase undo_tgf_XXXXXX
  atexit(UndoErase);
  // Load settings, input and output filenames
#if QT_VERSION >= 300
  QSettings setting;
#endif
  {LoadSettings();
  QFileInfo fi0 =  QFileInfo(InputFileName);
  QFileInfo fi = QFileInfo(fi0.dirPath());
  if(!fi.exists() || !fi.isDir())
      {QString DirFile = QFileDialog::
      getExistingDirectory(".",this,"find","Choose the TGF directory",TRUE);
      InputFileName = DirFile + QDir::separator() + fi0.fileName();
      OutputFileName = InputFileName;
      }
  }
  InputDriver = FileIOHandler.WhoseIs((const char *)InputFileName);
  if (InputDriver<0) InputDriver=0;
  OutputDriver = FileIOHandler.WhoseIs((const char *)OutputFileName);
  if (OutputDriver<0) OutputDriver=0;

  // Modify settings according to passed arguments
   ParseArguments();

  // Init random generator
  randomInitSeed();

  // Define some colors
  LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QColorGroup::Base,QColor(QColorDialog::customColor(1)));

  // Create a printer
  printer = new QPrinter;
#if QT_VERSION >= 300 || _WINDOWS
  if(PrinterOrientation == QPrinter::Portrait)
      printer->setOrientation(QPrinter::Portrait); 
  else
      printer->setOrientation(QPrinter::Landscape); 

  if(PrinterColorMode == QPrinter::GrayScale)
      printer->setColorMode(QPrinter::GrayScale);
  else
      printer->setColorMode(QPrinter::Color);
#else
  printer->setOrientation(QPrinter::Landscape); 
  printer->setColorMode(QPrinter::Color);
#endif

  // sizes of the editor window
  // int pigaleEditorMinXsize  = 250, pigaleEditorMaxXsize  = 250;
  int pigaleEditorMinXsize  = 280, pigaleEditorMaxXsize  = 280;
  int pigaleEditorMinYsize  = 150;
  // When testing screens 800x600
  //pigaleWindowInitXsize = 800;pigaleWindowInitYsize = 600;

  // Widgets
  QWidget *mainWidget = new QWidget(this,"mainWidget");
  setCentralWidget(mainWidget);
  QHBoxLayout *topLayout = new QHBoxLayout(mainWidget,0,0,"topLayout");

  //leftLayout: Graph editor,Paint,GL,Browser
  QHBoxLayout * leftLayout = new QHBoxLayout(topLayout,0,"leftLayout");
  tabWidget = new  QTabWidget(mainWidget,"tabwidget");
  leftLayout->addWidget(tabWidget,1);
  tabWidget->setMinimumSize(465,425);
  tabWidget->setPalette(LightPalette);
  mypaint =  new pigalePaint(tabWidget,"mypaint",this);
  gw = new  GraphWidget(tabWidget,"graphwidget",this);
  graphgl  = new GraphGL(tabWidget,"graphgl",this);
  graphgl->setPalette(LightPalette);
  graphsym = new GraphSym(tabWidget,"graphsym",this);

  tabWidget->addTab(gw,tr("Graph Editor"));
  tabWidget->addTab(mypaint,"");
  tabWidget->addTab(graphgl,""); 
  tabWidget->addTab(graphsym,""); 

#if QT_VERSION >= 300 || _WINDOWS
  browser = new QTextBrowser(tabWidget,"doc");
  tabWidget->addTab(browser,tr("User Guide")); 
  QPalette bop(QColorDialog::customColor(3));
  browser->setPalette(bop);
#endif
 
   // rightLayout
  QGridLayout *rightLayout = new  QGridLayout(topLayout,2,4,-1,"rightLayout");
  messages = new QTextEdit(mainWidget,"informations");
  messages->setMinimumSize(pigaleEditorMinXsize,pigaleEditorMinYsize);
  messages->setMaximumWidth(pigaleEditorMaxXsize);
  QBrush pb(QColorDialog::customColor(1)); messages->setPaper(pb); 
#if QT_VERSION >= 300
  messages->setReadOnly(true);
#endif
  mouse_actions = new Mouse_Actions(mainWidget,"mouseactions",0,gw);
  mouse_actions->setPalette(LightPalette);
  graph_properties = new Graph_Properties(mainWidget,menuBar(),"graph_properties");
  graph_properties->setMinimumSize(pigaleEditorMinXsize,180);
  graph_properties->setMaximumSize(pigaleEditorMinXsize,250);
  QLabel *lab0 = new QLabel(mainWidget);
#ifdef _WINDOWS
  lab0->setText("<center>Messages</center>");
#else
  lab0->setText("<center><b>Messages</b></center><br>");
#endif
  lab0->setTextFormat(Qt::RichText);	
  lab0->setMinimumSize(pigaleEditorMinXsize,20);  
  lab0->setMaximumSize(pigaleEditorMinXsize,25);
  rightLayout->addMultiCellWidget(lab0,1,1,1,2);
  rightLayout->addMultiCellWidget(messages,2,2,1,2);
  rightLayout->addMultiCellWidget(graph_properties,3,3,1,2);
  rightLayout->addMultiCellWidget(mouse_actions,4,4,1,2);

  //Pixmaps
  QPixmap openIcon = QPixmap(fileopen),newIcon = QPixmap(filenew),saveIcon = QPixmap(filesave);
  QPixmap leftIcon = QPixmap(sleft),   rightIcon = QPixmap(sright);
  QPixmap reloadIcon = QPixmap(sreload);
  QPixmap infoIcon = QPixmap(info), helpIcon = QPixmap(help),printIcon = QPixmap(fileprint);
  QPixmap xmanIcon = QPixmap(xman), undoLIcon = QPixmap(sleftarrow);
  QPixmap undoSIcon = QPixmap(sfilesave),undoRIcon = QPixmap(srightarrow);
  QPixmap macroplayIcon = QPixmap(macroplay),filmIcon = QPixmap(film);
 
  //ToolBar
  tb = new QToolBar(this,"toolbar" );
  QToolButton *fileopen,*filenew,*filesave,*fileprint,*savepng,*info,*macroplay;
  filenew = new QToolButton(newIcon,"New Graph",QString::null,this,SLOT(NewGraph()),tb,"New");
  fileopen = new QToolButton(openIcon,"Open File",QString::null,this,SLOT(load()),tb,"Open");
  QWhatsThis::add(fileopen,fileopen_txt);
  filesave = new QToolButton(saveIcon,"Save File",QString::null,this,SLOT(save()),tb,"Save graph" );
  fileprint = new QToolButton(printIcon,"Print window",QString::null,this,SLOT(print()),tb,"Print" );
  savepng = new QToolButton(filmIcon,"Capture window",QString::null,this,SLOT(png()),tb,"Save png" );
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

  // Status bar
  progressBar = new QProgressBar(statusBar(),"progressBar");
  
  //PopMenus
  QPopupMenu * file = new QPopupMenu( this );
  menuBar()->insertItem( tr("&File"), file );
  file->insertItem(newIcon,tr("&New Graph"),this, SLOT(NewGraph()));
  id = file->insertItem(openIcon,tr("&Open"),this, SLOT(load()));
  file->setWhatsThis(id,fileopen_txt);
  file->insertItem(tr("Save &as ..."), this, SLOT(saveAs()));
  file->insertItem(saveIcon,tr("&Save"), this, SLOT(save()));
  //file->insertItem(saveIcon,tr("Save Asc&ii"), this, SLOT(save_ascii()));
  file->insertSeparator();
  file->insertItem(tr("&Delete current record"),this,SLOT(deleterecord()));
  file->insertItem(tr("S&witch Input/Output files"),this,SLOT(switchInputOutput()));
  file->insertSeparator();
  file->insertItem(printIcon,tr("&Print"),this, SLOT(print()));
  file->insertSeparator();
  file->insertItem(tr("&Init server"),this, SLOT(initServer()));
  file->insertSeparator();
  file->insertItem(tr("&Quit"),qApp,SLOT(closeAllWindows()));

  QPopupMenu * augment = new QPopupMenu(this);
  menuBar()->insertItem(tr("&Augment"),augment);
  connect(augment,SIGNAL(activated(int)),SLOT(handler(int)));
  augment->insertItem(xmanIcon,tr("Make &Connected (edge)"),        A_AUGMENT_CONNECT);
  augment->setWhatsThis(A_AUGMENT_CONNECT,tr("Make a graph connected adding edges"));
  augment->insertItem(xmanIcon,tr("Make Connected (vertex) "),        A_AUGMENT_CONNECT_V);
  augment->setWhatsThis(A_AUGMENT_CONNECT_V,tr("Make a graph connected adding a vertex"));
  augment->insertItem(xmanIcon,tr("Make &2-Connected a planar graph (edge)"),      A_AUGMENT_BICONNECT);
  augment->setWhatsThis(A_AUGMENT_BICONNECT,tr("Make a PLANAR graph 2-connected adding edges"));
  augment->insertItem(xmanIcon,tr("Make 2-Connected a planar graph &Opt (edge)"),  A_AUGMENT_BICONNECT_6);
  augment->setWhatsThis(A_AUGMENT_BICONNECT_6
			,tr("Make a PLANAR graph 2-connected\n adding edges with a minimal degree increase"));
  augment->insertItem(xmanIcon,tr("Make 2-Connected  (edge)"),   A_AUGMENT_BICONNECT_NP);
  augment->setWhatsThis(A_AUGMENT_BICONNECT_NP,tr("Make 2-connected a graph (planar or not)  adding edges"));
  augment->insertItem(xmanIcon,tr("Make 2-Connected   (vertex)"),   A_AUGMENT_BICONNECT_NP_V);
  augment->setWhatsThis(A_AUGMENT_BICONNECT_NP_V,tr("Make 2-connected a graph (planar or not) adding vertices"));
  augment->insertItem(xmanIcon,tr("&Vertex Triangulate"),    A_AUGMENT_TRIANGULATE_V);
  augment->setWhatsThis(A_AUGMENT_TRIANGULATE_V,tr("Triangulate a PLANAR graph  adding vertices"));
  augment->insertItem(xmanIcon,tr("&ZigZag Triangulate"),    A_AUGMENT_TRIANGULATE_ZZ);
  augment->setWhatsThis(A_AUGMENT_TRIANGULATE_ZZ,tr("Triangulate a PLANAR graph by adding edges"));
  augment->insertItem(xmanIcon,tr("T&ricon. Triangulate"),   A_AUGMENT_TRIANGULATE_3C);
  augment->setWhatsThis(A_AUGMENT_TRIANGULATE_3C,tr("Optimally triangulate a PLANAR graph by adding edges"));
  augment->insertItem(xmanIcon,tr("Vertex &Quadrangulate"),  A_AUGMENT_QUADRANGULATE_V);
  augment->setWhatsThis(A_AUGMENT_QUADRANGULATE_V,tr("Quadrangulate a PLANAR  bipartite graph"));
  augment->insertSeparator();
  augment->insertItem(tr("&Bisect all edges"),     A_AUGMENT_BISSECT_ALL_E);

  QPopupMenu *remove = new QPopupMenu( this );
  menuBar()->insertItem(tr("&Remove"),remove);
  connect(remove,SIGNAL(activated(int)),SLOT(handler(int)));
  remove->insertItem(tr("&Isolated vertices"),   A_REMOVE_ISOLATED_V);
  remove->insertItem(tr("&Multiple edges"),      A_REMOVE_MULTIPLE_E); 
  remove->insertItem(tr("Ist&hmus"),             A_REMOVE_BRIDGES);
  remove->insertSeparator();
  remove->insertItem(tr("Colored &vertices"),    A_REMOVE_COLOR_V);
  remove->insertItem(tr("Colored &edges"),       A_REMOVE_COLOR_E);
  remove->insertItem(tr("&Thick edges"),         A_REMOVE_THICK_E);

  QPopupMenu *embed = new QPopupMenu(this);
  menuBar()->insertItem(tr("E&mbed"),embed);
  connect(embed,SIGNAL(activated(int)),SLOT(handler(int)));
  embed->insertItem(xmanIcon,"&FPP Fary",    A_EMBED_FPP);
  embed->setWhatsThis(A_EMBED_FPP,fpp_txt);
  embed->insertItem(xmanIcon,"&Schnyder",    A_EMBED_SCHNYDER_E);
  embed->setWhatsThis(A_EMBED_SCHNYDER_E,schnyder_txt);
  embed->insertItem(xmanIcon,"Schnyder &V ", A_EMBED_SCHNYDER_V);
  embed->setWhatsThis(A_EMBED_SCHNYDER_V,schnyder_txt);
  embed->insertItem(xmanIcon,"Co&nvex Drawing", A_EMBED_CD);
  embed->setWhatsThis(A_EMBED_CD,cd_txt);
  embed->insertItem(xmanIcon,"&Convex Compact Dra&wing",    A_EMBED_CCD);
  embed->setWhatsThis(A_EMBED_CCD,ccd_txt);
  embed->insertSeparator();
  embed->insertItem("&Tutte"      ,          A_EMBED_TUTTE);
  embed->insertItem(xmanIcon,"Tutte &Circle",A_EMBED_TUTTE_CIRCLE);
  embed->setWhatsThis(A_EMBED_TUTTE_CIRCLE,tutte_circle_txt);
  embed->insertSeparator();
  embed->insertItem(tr("&Visibility"),           A_EMBED_VISION );
  embed->insertItem(tr("FPP Visi&bility"),       A_EMBED_FPP_RECTI);
  embed->insertItem(tr("&General Visibility"),   A_EMBED_GVISION);
  embed->insertItem(tr("&T Contact"),            A_EMBED_T_CONTACT);
  embed->insertItem(tr("&Contact Biparti"),      A_EMBED_CONTACT_BIP);
  embed->insertItem(tr("&Polyline"),             A_EMBED_POLYLINE);  
  embed->insertItem(tr("&Curves")  ,             A_EMBED_CURVES);  
 embed->insertSeparator();
#if VERSION_ALPHA
  embed->insertItem(tr("&Polar"),                A_EMBED_POLAR);
  embed->insertItem(xmanIcon,"Sprin&g",      A_EMBED_SPRING);
  embed->setWhatsThis(A_EMBED_SPRING,spring_txt);
#endif
  embed->insertItem(xmanIcon,tr("Spring (Map &Preserving)"),A_EMBED_SPRING_PM);
  embed->setWhatsThis(A_EMBED_SPRING_PM,springPM_txt);
  embed->insertItem(xmanIcon,tr("Spring Planar"),           A_EMBED_JACQUARD);
  embed->setWhatsThis(A_EMBED_JACQUARD,jacquard_txt);
  embed->insertSeparator(); 
  embed->insertItem(xmanIcon,tr("&Embedding in Rn"),        A_EMBED_3d);
  embed->setWhatsThis(A_EMBED_3d,embed3d_txt);
  embed->insertItem(xmanIcon,tr("Schnyder in R3"),          A_EMBED_3dSCHNYDER);
  embed->setWhatsThis(A_EMBED_3dSCHNYDER,embed3dSchnyder_txt);

  QPopupMenu *dual = new QPopupMenu( this );
  menuBar()->insertItem(tr("&Dual/Angle"),dual);
  connect(dual,SIGNAL(activated(int)),SLOT(handler(int)));
  dual->insertItem(tr("&Dual"),                 A_GRAPH_DUAL);
  dual->insertItem(xmanIcon,tr("&Geometric Dual"),A_GRAPH_DUAL_G);
  dual->setWhatsThis(A_GRAPH_DUAL_G,dual_g_txt);
  dual->insertSeparator();
  dual->insertItem(tr("&Angle"),                A_GRAPH_ANGLE);
  dual->insertItem(xmanIcon,"G&eometric Angle",A_GRAPH_ANGLE_G);
  dual->setWhatsThis(A_GRAPH_ANGLE_G,angle_g_txt);
  QPopupMenu *algo = new QPopupMenu( this );
  menuBar()->insertItem(tr("&Algo"),algo);
  connect(algo,SIGNAL(activated(int)),SLOT(handler(int)));
  algo->insertItem(tr("Find &Kuratowski"),                A_ALGO_KURATOWSKI);
  algo->insertItem(tr("Find &Cotree Critical"),           A_ALGO_COTREE_CRITICAL);
  algo->insertItem(tr("Color red  non critical edges "),  A_ALGO_COLOR_NON_CRITIC);
  algo->insertItem(tr("Max.Planar (simple graph) Fast"),  A_ALGO_NPSET);
  algo->insertItem(tr("&Max.Planar (simple graph) Slow "),A_ALGO_MAX_PLANAR);
  algo->insertSeparator();
  algo->insertItem(tr("Use &Geometric Map"),              A_ALGO_GEOMETRIC_CIR);
  algo->insertItem(tr("Use &LRALGO Map"),                 A_ALGO_LRALGO_CIR);
  algo->insertSeparator();
  algo->insertItem(tr("&Color everything"),               A_ALGO_RESET_COLORS);
  algo->insertItem(tr("Color &bipartite"),                A_ALGO_COLOR_BIPARTI);
  algo->insertItem(tr("Color current e&xterior face"),    A_ALGO_COLOR_EXT);
  algo->insertItem(tr("Color c&onnected components"),     A_ALGO_COLOR_CONNECTED);
  algo->insertSeparator();
  algo->insertItem(tr("&Symmetry"),                       A_ALGO_SYM);
  algo->insertSeparator();
  spin_N = new QSpinBox(2,50,1,algo,"spinN");
  spin_N->setValue(2);     spin_N->setPrefix(tr("Number of classes: "));
  algo->insertItem(spin_N);
  algo->insertItem(tr("&Partition"),                       A_ALGO_NETCUT );

  QPopupMenu *orient = new QPopupMenu( this );
  menuBar()->insertItem(tr("&Orient"),orient);
  connect(orient,SIGNAL(activated(int)),SLOT(handler(int)));
  orient->insertItem(tr("&Orient all edges"),     A_ORIENT_E);
  orient->insertItem(tr("&Color Poles"),          A_ORIENT_SHOW);
  orient->insertItem(tr("&ReOrient color edges"), A_REORIENT_COLOR);
  orient->insertItem(tr("&Inf Orientation"),      A_ORIENT_INF);
  orient->insertItem(tr("Planar &3-Con."),        A_ORIENT_TRICON);
  orient->insertItem(tr("Planar &Biparti"),       A_ORIENT_BIPAR);
  orient->insertItem(tr("Planar &Schnyder"),      A_ORIENT_SCHNYDER);
  orient->insertItem(tr("B&ipolarOrient Planar"), A_ORIENT_BIPOLAR);
  orient->insertItem(tr("BipolarOrient"),         A_ORIENT_BIPOLAR_NP);

  QPopupMenu *generate      = new QPopupMenu( this );
  QPopupMenu *popupCubic    = new QPopupMenu(this);
  QPopupMenu *popupQuadric  = new QPopupMenu(this);
  QPopupMenu *popupBipar    = new QPopupMenu(this);
  QPopupMenu *popupPlan     = new QPopupMenu(this);
  QPopupMenu *popupOuter    = new QPopupMenu(this);
  QPopupMenu *popupSeed     = new QPopupMenu(this);
  menuBar()->insertItem(tr("&Generate"),generate);
  connect(generate,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupCubic,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupQuadric,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupBipar,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupPlan,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupOuter,SIGNAL(activated(int)),SLOT(handler(int)));
  connect(popupSeed,SIGNAL(activated(int)),SLOT(handler(int)));
  generate->insertItem(tr("&Outer Planar"),popupOuter);
  popupOuter->insertItem(tr("&Outer Planar (N1)"),              A_GENERATE_P_OUTER_N);
  popupOuter->insertItem(tr("O&uter Planar (N1,M))"),           A_GENERATE_P_OUTER_NM);
  generate->insertItem(tr("&Planar"),popupPlan);
  popupPlan->insertItem(tr("connected (M)"),                  A_GENERATE_P);
  popupPlan->insertItem(tr("2-connected (M)"),                A_GENERATE_P_2C);
  popupPlan->insertItem(tr("3-connected (M))"),               A_GENERATE_P_3C);
  generate->insertItem(tr("Planar &cubic"),popupCubic);
  popupCubic->insertItem(tr("2-connected (M)"),               A_GENERATE_P_3R_2C);
  popupCubic->insertItem(tr("3-connected (M)"),               A_GENERATE_P_3R_3C);
  popupCubic->insertItem(tr("dual:4-connected (M)"),          A_GENERATE_P_3R_D4C);
  generate->insertItem(tr("Planar &4-regular"),popupQuadric);
  //popupQuadric->insertItem("connected (M)",             A_GENERATE_P_4R_C);
  popupQuadric->insertItem(tr("4-regular 2-connected (M)"),   A_GENERATE_P_4R_2C);
  popupQuadric->insertItem(tr("4-regular 3-connected (M)"),   A_GENERATE_P_4R_3C);
  popupQuadric->insertItem(tr("4-regular bipartite (M)"),     A_GENERATE_P_4R_BIP);
  generate->insertItem(tr("Planar &bipartite"),popupBipar);
  popupBipar->insertItem(tr("Bipartite (M)"),                  A_GENERATE_P_BIP);
  popupBipar->insertItem(tr("Bipartite cubic 2-connected (M)"),A_GENERATE_P_BIP_2C );
  popupBipar->insertItem(tr("Bipartite cubic 3-connected (M))"),A_GENERATE_P_BIP_3C);
  generate->insertItem(tr("&Grid (N1,N2)"),                    A_GENERATE_GRID);
  generate->insertSeparator();
  generate->insertItem(tr("&Complete (N1)"),                   A_GENERATE_COMPLETE);
  generate->insertItem(tr("&Bipartite complete (N1,N2)"),      A_GENERATE_COMPLETE_BIP);
  generate->insertSeparator();
  generate->insertItem(tr("&Random (N1,M)"),                   A_GENERATE_RANDOM );
  generate->insertSeparator(); 
  // Erase multiple edges
  generate->insertItem(tr("Erase multiple edges"),A_SET_ERASE_MULT);
  generate->setItemChecked(A_SET_ERASE_MULT,randomEraseMultipleEdges());
  spin_N1 = new QSpinBox(0,100000,1,generate,"spinN1");
  spin_N1->setValue(Gen_N1);     spin_N1->setPrefix("N1: ");
  spin_N1->setFocusPolicy(QWidget::StrongFocus); 
  spin_N2 = new QSpinBox(0,100000,1,generate,"spinN2");
  spin_N2->setValue(Gen_N2);     spin_N2->setPrefix("N2: ");
  spin_M = new QSpinBox(0,300000,1,generate,"spinM");
  spin_M->setValue(Gen_M);      spin_M->setPrefix("M: ");
  generate->insertItem(spin_N1);
  generate->insertItem(spin_N2);
  generate->insertItem(spin_M);
  seedEdit =  new QLineEdit(popupSeed,"seedEdit");
  seedEdit->setText(QString("%1").arg(randomSetSeed()));
  generate->insertItem(tr("Seed"),popupSeed);
  popupSeed->insertItem(tr("Change Seed"),A_SET_RANDOM_SEED_CHANGE);
  popupSeed->insertItem(seedEdit);

  QPopupMenu *macroMenu = new QPopupMenu( this );
  menuBar()->insertItem("&Macro",macroMenu);
  connect(macroMenu,SIGNAL(activated(int)),SLOT(macroHandler(int)));
  macroSpin = new QSpinBox(0,60,1,macroMenu,"macroSpin");
  macroSpin->setValue(pauseDelay());macroSpin->setPrefix(tr("Seconds: "));
  macroMenu->insertItem(tr("Start recording"),1);
  macroMenu->insertItem(tr("Stop  recording"),2);
  macroMenu->insertItem(tr("Continue recording"),3);
  macroMenu->insertSeparator();
  macroMenu->insertItem(tr("Display Macro"),6);
  macroMenu->insertItem(tr("Save Macro"),7);
  macroMenu->insertItem(tr("Read Macro"),8);
  macroMenu->insertSeparator();
  macroMenu->insertItem(macroSpin);
  macroMenu->insertItem(tr("Insert a Pause"),5);
  macroMenu->insertSeparator();
  macroLine = new LineEditNum(macroMenu,"macroLineEditNum");
  macroLine->setPrefix(tr("Repeat:")); macroLine->setNum(macroRepeat); macroLine->setMul(macroMul);
  macroMenu->insertItem(macroLine);
  QSlider *macroSlider = new QSlider(0,100,0,macroRepeat,QSlider::Horizontal,macroMenu,"macroSlider");
  QSlider *macroSliderM = new QSlider(0,100,0,macroMul,QSlider::Horizontal,macroMenu,"macroSliderM");
  macroMenu->insertItem(macroSlider);
  macroMenu->insertItem(macroSliderM);
  connect(macroSlider,SIGNAL(valueChanged(int)),macroLine,SLOT(setNum(int)));
  connect(macroSliderM,SIGNAL(valueChanged(int)),macroLine,SLOT(setMul(int)));
  macroMenu->insertItem(tr("Repeat macro"),4);
 
   userMenu = new QPopupMenu( this );
  menuBar()->insertItem(tr("&UserMenu"),userMenu);
  connect(userMenu,SIGNAL(activated(int)),SLOT(handler(int)));
  userMenu->insertItem("Test &1",A_TEST_1);
  userMenu->insertItem("Test &2",A_TEST_2);
  userMenu->insertItem("Test &3",A_TEST_3);

  QPopupMenu *settings      = new QPopupMenu(this);
  QPopupMenu *popupLabel    = new QPopupMenu(this);
  QPopupMenu *popupEmbed    = new QPopupMenu(this);
  QPopupMenu *popupDistance = new QPopupMenu(this);
  QPopupMenu *popupLimits   = new QPopupMenu(this);
  QComboBox *comboLabel     = new QComboBox(popupLabel,"LabelCombo");
  QComboBox *comboDistance  = new QComboBox(popupDistance,"DistCombo");
  spin_MaxNS                = new QSpinBox(0,10000,1,popupLimits,"spinMaxNS");
  spin_MaxND                = new QSpinBox(0,10000,1,popupLimits,"spinMaxND");
  menuBar()->insertItem(tr("&Settings"),settings);
  connect(settings,SIGNAL(activated(int)),SLOT(settingsHandler(int)));
  connect(spin_MaxNS,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNSlowChanged(int)));
  connect(spin_MaxND,SIGNAL(valueChanged(int)),graph_properties,SLOT(MaxNDisplayChanged(int)));
  connect(comboLabel,SIGNAL(activated(int)),SLOT(showLabel(int)));
  connect(comboLabel,SIGNAL(activated(int)),popupLabel,SLOT(hide()));
  connect(comboDistance,SIGNAL(activated(int)),SLOT(distOption(int)));
  connect(comboDistance,SIGNAL(activated(int)),popupDistance,SLOT(hide()));
  connect(popupEmbed,SIGNAL(activated(int)),SLOT(handler(int)));
  settings->insertTearOffHandle();
  settings->insertItem(tr("&Debug"),A_SET_DEBUG);
  settings->setItemChecked(A_SET_DEBUG,debug());
  //undoEnable
  settings->insertItem(tr("&Undo Enable"),A_SET_UNDO);
  settings->setItemChecked(A_SET_UNDO,IsUndoEnable);
  undoL->setEnabled(false);
  undoS->setEnabled(IsUndoEnable);
  undoR->setEnabled(false);
  // randomSeed 
  settings->insertItem(tr("&Random Seed"),A_SET_RANDOM_SEED);
  settings->setItemChecked(A_SET_RANDOM_SEED,randomSeed());
  //Pigale colors
  settings->insertItem(tr("&Pigale Colors"),A_SET_COLOR);
  //Fonte
  settings->insertItem(tr("Set a font"),this, SLOT(SetPigaleFont()));
#if QT_VERSION >= 300 || _WINDOWS
  // Pigale Doc Path
  settings->insertItem(tr("&Documentation path"),A_SET_DOC);
#endif
  //Pigale limits
  settings->insertItem(tr("Limit number of vertices"),popupLimits);
  popupLimits->insertItem(tr("for slow algorithms"));
  spin_MaxNS->setValue(MaxNS);    
  spin_MaxNS->setPrefix(tr("Slow algorithms: "));
  popupLimits->insertItem(spin_MaxNS);
  popupLimits->insertItem(tr("for displaying graph"));
  spin_MaxND->setValue(MaxND);      
  spin_MaxND->setPrefix(tr("Display: "));
  popupLimits->insertItem(spin_MaxND);
  //Edge arrow
  settings->insertItem(tr("Show arrows"),A_SET_ARROW);
  settings->setItemChecked(A_SET_ARROW,ShowArrow());
  //Show Orientation
  settings->insertItem(tr("&Show orientation"),   A_SET_ORIENT);
  settings->setItemChecked(A_SET_ORIENT,ShowOrientation());
   //Vertex Label Options
  settings->insertSeparator();
  settings->insertItem(tr("&Vertex show"),popupLabel);
  popupLabel->insertItem(comboLabel);
  comboLabel->insertItem(tr("Index"));
  comboLabel->insertItem(tr("Label"));
  comboLabel->insertItem(tr("Nothing"));
  int current = ShowVertex();++current;
  comboLabel->setCurrentItem(current);showLabel(current);
  //Embed Settings
  settings->insertSeparator();
  settings->insertItem(tr("&Embed Options"),popupEmbed);
  popupEmbed->insertItem(tr("Schnyder: &Rect"),        A_SET_SCH_RECT);
  popupEmbed->insertItem(tr("Schnyder: &Longest Face"),A_SET_LFACE);
  popupEmbed->insertItem(tr("Schnyder: &Color Edges"), A_SET_SCH_COLOR);
  popupEmbed->setItemChecked(A_SET_SCH_RECT,SchnyderRect());
  popupEmbed->setItemChecked(A_SET_LFACE,SchnyderLongestFace());
  popupEmbed->setItemChecked(A_SET_SCH_COLOR,SchnyderColor());
  //Distance Settings 
  settings->insertItem(tr("&Distance Options"),popupDistance);
  popupDistance->insertItem(comboDistance);
  comboDistance->insertItem(tr("Czekanovski-Dice"));
  comboDistance->insertItem(tr("Bisect"));
  comboDistance->insertItem(tr("Adjacence"));
  comboDistance->insertItem(tr("Adjacence M"));
  comboDistance->insertItem(tr("Laplacian"));
  comboDistance->insertItem(tr("Q-distance"));
  comboDistance->insertItem(tr("Oriented"));
  comboDistance->insertItem(tr("R2"));
  comboDistance->setCurrentItem(useDistance());distOption(useDistance());
  //Save Settings
  settings->insertSeparator();
  settings->insertItem(tr("&Save Settings"),A_SET_SAVE);

  //End of the menuBar():window
  //help
  QPopupMenu * help = new QPopupMenu(this);
  menuBar()->insertItem(tr("&Help"),help);
  help->insertItem(infoIcon,tr("&Graph properties"),this,SLOT(information())
		   ,SHIFT+Key_F2);
  help->insertSeparator();
  help->insertItem(helpIcon,tr("What is ?"),this,SLOT(whatsThis()),SHIFT+Key_F1);
  help->insertSeparator();
  help->insertItem(tr("About &Qt..."), this, SLOT(aboutQt()));
  help->insertItem(tr("&About..."), this, SLOT(about()),Key_F1);
  
  
  //Resize
#if TDEBUG
  setCaption(tr("Pigale Editor")+PACKAGE_VERSION+" "+tr("Debug Mode"));
#else
  setCaption(tr("Pigale Editor")+PACKAGE_VERSION);
#endif
  statusBar()->setBackgroundColor(QColor(QColorDialog::customColor(1)));
  resize(pigaleWindowInitXsize,pigaleWindowInitYsize);
  QRect rect_status(0,0,pigaleWindowInitXsize/2,30);
  progressBar->setGeometry(rect_status); 
  progressBar->hide();

  mainWidget->setFocus(); 
  if(CheckLogFile() == -1)Twait("Impossible to write in log.txt");
  DebugPrintf("Debug Messages\nUndoFile:%s",undofile);
  DebugPrintf("Init seed:%ld",randomSetSeed());
  
#if QT_VERSION >= 300 || _WINDOWS
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
                  {browser->mimeSourceFactory()->setFilePath(DirFileDoc);
                  browser->setSource("manual.html");
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
      {browser->mimeSourceFactory()->setFilePath(DirFileDoc);
      browser->setSource("manual.html");
      }
#endif  
  initMenuTest();
  gw->update();
  if(!Server)load(0);
  }

pigaleWindow::~pigaleWindow()
  {delete printer;
  pigaleThread.terminate();pigaleThread.wait();
  }
void pigaleWindow::whenReady()
  {if(MacroPlay && macroLoad(MacroFileName) != -1)  macroPlay();
  ServerClientId = 0;
  if(Server)InitPigaleServer(this); 
  }
void pigaleWindow::initServer()
  {ServerClientId = 0;
  InitPigaleServer(this); 
  }
void pigaleWindow::customEvent( QCustomEvent * ev)
  {switch( ev->type())
      {case CLIENT_EVENT:
          {clientEvent *event  =  (clientEvent  *)ev;
          int action  = event->getAction();
          QString msg = event->getParamString();
          if(action > A_AUGMENT && action < A_TEST_END)
              handler(action);
          ServerBusy = false;
          }
          break;
      case  TEXT_EVENT:
          {textEvent *event  =  (textEvent  *)ev;
          Message(event->getString());
          }
          break;
      case CLEARTEXT_EVENT:
          messages->setText("");
          break;
      case BANNER_EVENT:
          {bannerEvent *event  =  (bannerEvent  *)ev;
          statusBar()->message(event->getString());
          }
          break;
      case WAIT_EVENT:
          {waitEvent *event  =  (waitEvent  *)ev;
          QString msg = event->getString();
          int rep = QMessageBox::information (this,"Wait",msg,"","CANCEL","EXIT",0,0);
          if(rep == 2)close();
          }
      case DRAWG_EVENT:
          gw->update();
          break;
      case HANDLER_EVENT:
           postHandler(ev);    
          break;
      case PROGRESS_EVENT:
          {progressEvent *event  =  (progressEvent  *)ev;
          int action  = event->getAction();
          if(action == -1)
              progressBar->hide();
          else if(action == 1)
              {progressBar->setTotalSteps(event->getStep());
              progressBar->setProgress(0);
              progressBar->show();
              }
          else
              progressBar->setProgress(event->getStep());
          }
          break;
      default:
          qDebug("UNKNOWN EVENT");
          break;
      }
  }
void pigaleWindow::postMessage(const QString &msg)
  {textEvent *e = new textEvent(msg);
  QApplication::postEvent(this,e);
  //qApp->processEvents();  would block the application
  }
void pigaleWindow::postMessageClear()
  {QCustomEvent *e = new QCustomEvent(CLEARTEXT_EVENT);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postWait(const QString &msg)
  {waitEvent *e = new waitEvent(msg);
  QApplication::postEvent(this,e);
  }
 void pigaleWindow::postDrawG()
  {QCustomEvent *e = new QCustomEvent(DRAWG_EVENT);
  QApplication::postEvent(this,e);
  }

void pigaleWindow::Message(QString s)
  {messages->append(s);
#if QT_VERSION >= 300
  messages->scrollToBottom ();
#endif
  } 

void pigaleWindow::mapActionsInit()
  {int na = (int)(sizeof(actions)/sizeof(_Action));
  for(int i = 0;i < na;i++)
      {mapActionsString[actions[i].num] = actions[i].name;
      mapActionsInt[actions[i].name] = actions[i].num;
      }
  }
QString pigaleWindow::getActionString(int action)
  {return mapActionsString[action];
  }
int pigaleWindow::getActionInt(QString action_str)
  {return mapActionsInt[action_str];
  }
void pigaleWindow::load()
  {QFileInfo fi =  QFileInfo(InputFileName);
    QStringList formats;
    QString filter, selfilter;
    for (int i=0; i<FileIOHandler.n();i++)
      { filter = tr(FileIOHandler.Name(i));
	filter += "(*";
	if (FileIOHandler.Ext(i)!="!") {filter += "."; filter += FileIOHandler.Ext(i);}
	filter += ")";
	formats += filter;
	if (i==0) selfilter=filter;
      }
    formats += "All (*)";
    QString FileName = QFileDialog::getOpenFileName(fi.filePath(),formats.join(";;"),this,
						    "load dialog",tr("Choose a file to open"),
						    &selfilter);
    setError();
    if(!FileName.isEmpty())
      {
	InputFileName = FileName;
	int i=0;
	for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++i ) {
	  if (selfilter==*it) break;
	}
	if (i==FileIOHandler.n()) // All
	  { i = FileIOHandler.WhoseIs((const char *)FileName);
	    if (i<0) 
	      {QString m;
		m.sprintf("Could not read:%s",(const char *)InputFileName);
		statusBar()->message(m,2000);
		LogPrintf("%s: unrecognized format\n",(const char *)InputFileName);
		return;
	      }	  
	  }
	InputDriver=i;
	*pGraphIndex = 1;
	int NumRecords = FileIOHandler.GetNumRecords(i,(const char *)InputFileName);
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
int pigaleWindow::publicLoad(int pos)
  {setError();
  QString m;
   QFileInfo fi(InputFileName);
   if(!fi.exists() || fi.size() == 0)
     {//m.sprintf("file -%s- does not exist",(const char *)InputFileName);
      //statusBar()->message(m,2000);
      LogPrintf("%s\n",(const char *)m);
      return -1;
      } 
   int i;
   i = FileIOHandler.WhoseIs((const char *)InputFileName);
   if (i<0) 
     {QString m;
       LogPrintf("%s: unrecognized format\n",(const char *)InputFileName);
       return -1;
     }
   InputDriver=i;
   UndoClear();UndoSave();
  int NumRecords =FileIOHandler.GetNumRecords(i,(const char *)InputFileName);
  *pGraphIndex = pos;
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(FileIOHandler.Read(i,GC,(const char *)InputFileName,NumRecords,*pGraphIndex) != 0)
      {m.sprintf("Could not read:%s",(const char *)InputFileName);
      //statusBar()->message(m,2000);
      return -2;
      }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName,*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  //banner();
  information(); gw->update();
  return *pGraphIndex;
  }
int pigaleWindow::load(int pos)
  {setError();
  QString m;
  QFileInfo fi(InputFileName);
  if(!fi.exists() || fi.size() == 0)
    {m.sprintf("file -%s- does not exist",(const char *)InputFileName);
      statusBar()->message(m,2000);
      LogPrintf("%s\n",(const char *)m);
      return -1;
    }      
  if (!FileIOHandler.IsMine(InputDriver,(const char *)InputFileName))
    {
      m.sprintf("file -%s- is not a valid %s",(const char *)InputFileName,FileIOHandler.Name(InputDriver));
      statusBar()->message(m,2000);
      LogPrintf("%s\n",(const char *)m);
      return -1;
    }
  UndoClear();UndoSave();
  int NumRecords =FileIOHandler.GetNumRecords(InputDriver,(const char *)InputFileName);
  if(pos == 1)++(*pGraphIndex);
  else if(pos == -1)--(*pGraphIndex);
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(FileIOHandler.Read(InputDriver,GC,(const char *)InputFileName,NumRecords,*pGraphIndex) != 0)
    {m.sprintf("Could not read:%s",(const char *)InputFileName);
      statusBar()->message(m,2000);
      return -2;
    }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName,*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  banner();
  information(); gw->update();
  return *pGraphIndex;
  }
void pigaleWindow::save()
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  QString titre(~title());
  bool ok = TRUE;
  titre = QInputDialog::getText("Pigale","Enter the graph name",
                    QLineEdit::Normal,titre, &ok, this );
  if(ok && !titre.isEmpty()) title() = (const char *)titre;
  else if(!ok) return;
  //G.FixOrientation();
  if(FileIOHandler.Save(OutputDriver,G,(const char *)OutputFileName) == 1)
      {QString t;
      t.sprintf("Cannot open file:%s",(const char *)OutputFileName);
      Twait((const char *)t);
      return;
      }
  GraphIndex2 = FileIOHandler.GetNumRecords(OutputDriver,(const char *)OutputFileName);
  banner();
  }
// void pigaleWindow::save_ascii()
//   {TopologicalGraph G(GC);
//   Prop1<tstring> title(G.Set(),PROP_TITRE);
//   QString titre(~title());
//   bool ok = TRUE;
//   titre = QInputDialog::getText("Pigale","Enter the graph name",
//                     QLineEdit::Normal,titre, &ok, this );
//   if(ok && !titre.isEmpty()) title() = (const char *)titre;
//   else if(!ok)return;
//   QFileInfo fi0 =  QFileInfo(OutputFileName);
//   QString FileName = QFileDialog::
//   getSaveFileName(fi0.dirPath(),"Txt Files(*.txt)",this);
//   if(FileName.isEmpty())return; 
//   if(QFileInfo(FileName).extension(false) != (const char *)"txt")
//       FileName += (const char *)".txt";
//   QString OutputAsciiFile = FileName;
//   QFileInfo fi = QFileInfo(OutputAsciiFile);
//   if(fi.exists())
//       {int rep = QMessageBox::warning(this,"Pigale Editor"
// 			    ,"This file already exixts.<br>"
// 			    "Overwrite ?"
// 			    ,QMessageBox::Ok 
// 			    ,QMessageBox::Cancel);
//       if(rep == 2)return;
//       } 
//   SaveGraphAscii(G,(const char *)OutputAsciiFile);
//   }
void pigaleWindow::saveAs()
  {QFileInfo fi =  QFileInfo(OutputFileName);
    QStringList formats;
    QString filter, selfilter;
    for (int i=0; i<FileIOHandler.n();i++)
      { filter = tr(FileIOHandler.Name(i));
	filter += "(*";
	if (FileIOHandler.Ext(i)!="!") {filter += "."; filter += FileIOHandler.Ext(i);}
	filter += ")";
	formats += filter;
	if (i==0) selfilter=filter;
      }
    QString FileName = QFileDialog::getSaveFileName(fi.filePath(),formats.join(";;"),this,
						    "save dialog",tr("Choose a filename to save under"),
						    &selfilter);
    if(FileName.isEmpty())return;
    QString ext="";
    int i=0;
    for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++i ) {
      if (selfilter==*it) break;
    }
    if ((QFileInfo(FileName).extension(false) != FileIOHandler.Ext(i)) && (FileIOHandler.Ext(i)!=""))
      {FileName += ".";
	FileName += FileIOHandler.Ext(i);
      }
    OutputFileName = FileName;
    OutputDriver = i;
    save();
  }
void pigaleWindow::deleterecord()
  {
    if (FileIOHandler.Capabilities(InputDriver)&TAXI_FILE_RECORD_DEL)
      FileIOHandler.DeleteRecord(InputDriver,(const char *)InputFileName,*pGraphIndex);
      //if(IsFileTgf((const char *)InputFileName))
      //DeleteTgfRecord((const char *)InputFileName,*pGraphIndex);
    else
      //Twait("Not a tgf file");
      {QString m=(const char *)FileIOHandler.Name(InputDriver);
	m += " does not allow record deletion";
	Twait((const char *)m);
      }
    banner();
  }
void pigaleWindow::switchInputOutput()
{Tswap(InputFileName,OutputFileName);
  Tswap(InputDriver,OutputDriver);
  Tswap(GraphIndex1,GraphIndex2);
  pGraphIndex   =  &GraphIndex1;
  load(0);
}
void pigaleWindow::NewGraph()
  {setError();
    statusBar()->message("New graph");
    UndoClear();UndoSave();
    Graph G(GetMainGraph());
    G.StrictReset();
    Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
    if(debug())DebugPrintf("**** New graph");
    information();gw->update();
  }
void pigaleWindow::previous()
{load(-1);}
void pigaleWindow::reload()
{load(0);}
void pigaleWindow::next()
{load(1);}
void pigaleWindow::information()
  {if(!getError() && !MacroExecuting && !ServerExecuting)postMessageClear();
  graph_properties->update(!MacroExecuting && !ServerExecuting);
  }

void pigaleWindow::settingsHandler(int action)
  {if(action == A_SET_COLOR)
      {SetPigaleColors(); return;}
  else if(action == A_SET_DOC)
      {QString StartDico = (DirFileDoc.isEmpty()) ? "." : DirFileDoc;
      DirFileDoc = QFileDialog::
      getExistingDirectory(StartDico,this,"find",
                           tr("Choose the documentation directory <em>Doc</em>"),TRUE);
      if(!DirFileDoc.isEmpty())
          {QFileInfo fi =  QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
          if(fi.exists())
              {browser->mimeSourceFactory()->setFilePath(DirFileDoc);
              browser->setSource("manual.html");
              SaveSettings();
              QMessageBox::information(this,"Pigale Editor"
                                       ,tr("You should restart Pigale"
                                           " to get the manual loaded")
                                       ,QMessageBox::Ok);
              }
          else 
              QMessageBox::information(this,"Pigale Editor"
                                       ,tr("I cannot find the inline manual <em>manual.html</em>")
                                       ,QMessageBox::Ok);
          }
          return;
      }
  else if(action == A_SET_PAUSE_DELAY) 
      {pauseDelay() = macroSpin->value();
      SaveSettings();
      return;
      }
  else if(action == A_SET_RANDOM_SEED_CHANGE)
      {randomSetSeed() = atol((const char *)seedEdit->text()); return; 
      }
  
// Change one parameter
  menuBar()->setItemChecked(action,!menuBar()->isItemChecked(action));
  // Read parameters when one is changed
  debug()               =  menuBar()->isItemChecked(A_SET_DEBUG);
  SchnyderRect()        =  menuBar()->isItemChecked(A_SET_SCH_RECT);
  SchnyderLongestFace() =  menuBar()->isItemChecked(A_SET_LFACE);
  SchnyderColor()       =  menuBar()->isItemChecked(A_SET_SCH_COLOR);
  ShowOrientation()     =  menuBar()->isItemChecked(A_SET_ORIENT);
  ShowArrow()     =  menuBar()->isItemChecked(A_SET_ARROW);
  randomEraseMultipleEdges()  =  menuBar()->isItemChecked(A_SET_ERASE_MULT);
  randomSeed()          =  menuBar()->isItemChecked(A_SET_RANDOM_SEED);
  pauseDelay() = macroSpin->value();
  UndoEnable(menuBar()->isItemChecked(A_SET_UNDO));

  if(action == A_SET_ORIENT || action == A_SET_ARROW )
      // update the editor
      {handlerEvent *e = new handlerEvent(1,0,0);
      QApplication::postEvent(this,e);
      }
  return;
  }
void PigaleThread:: run(int _action,int  _N,int _N1,int _N2,int _M,int _delay)
  {action = _action;
  N = _N;
  N1 = _N1;
  N2 = _N2;
  M = _M;
  delay = _delay;
  start (QThread::HighPriority);
  }
void PigaleThread::run()
  {if(!action)return;
  mw->timer.start();
  int ret = 0;
  int saveType = 0;
  int drawingType = 0;
  if(action < A_AUGMENT_END)
      ret = AugmentHandler(action);
  else if(action < A_EMBED_END)
      ret = EmbedHandler(action,drawingType);
  else if(action < A_GRAPH_END)
      {ret = DualHandler(action); 
      saveType = 1;
      }
  else if(action < A_REMOVE_END)
      {ret = RemoveHandler(action);
      saveType = 2;
      }
  else if(action < A_GENERATE_END)
      {ret = GenerateHandler(action,N1,N2,M);
      saveType = 1;
      }
  else if(action < A_ALGO_END)
      ret = AlgoHandler(action,N);
  else if(action < A_ORIENT_END)
      {ret = OrientHandler(action);
      ShowOrientation() = true;
      }
//   else if(action < A_TEST_END)
//       ret  = Test(mw->GC,action - A_TEST,drawingType);
  
  handlerEvent *e = new handlerEvent(ret,drawingType,saveType);
  QApplication::postEvent(mw,e);
  }
int  pigaleWindow::getResultHandler(int value)
  {static int _value = 0;
  int value0 = _value;
  _value = value;
  return value0;
  }
int pigaleWindow::handler(int action)
// Should not have any graphic action (so can be called from a thread)
  {if(action > A_SET){settingsHandler(action);return 0;}
  if(MacroRecording)macroRecord(action);
  if(action == A_PAUSE)
      {pauseDelay() = macroSpin->value();
      qApp->processEvents();
      MacroWait = true;
      QTimer::singleShot(1000*pauseDelay(),this,SLOT(timerWait()));
      return 0;
      }
  if(action > A_TEST_END)return 0;

  blockInput(true);
  if(action < A_AUGMENT_END)
      {UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_EMBED_END)
      pigaleThread.run(action);
  else if(action < A_GRAPH_END)
      {UndoClear();UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_REMOVE_END)
      {UndoSave();
      pigaleThread.run(action);
      }
  else if(action < A_GENERATE_END)
      {UndoClear();UndoSave();
      pigaleThread.run(action,0,spin_N1->value(),spin_N2->value(),spin_M->value());
      }
  else if(action < A_ALGO_END)
      pigaleThread.run(action,spin_N->value());
  else if(action < A_ORIENT_END)
      pigaleThread.run(action);
  else if(action < A_TEST_END)
      {int drawing;
      timer.start();
      int ret = Test(GC,action - A_TEST,drawing);
      handlerEvent *e = new handlerEvent(ret,drawing,0);
      QApplication::postEvent(this,e);
      return 0;
      }
      //pigaleThread.run(action);
  else
      return 0;
  while( pigaleThread.running())
      {pigaleThread.wait(50);
      qApp->processEvents (); 
      }
  return 0;
  }

int pigaleWindow::postHandler(QCustomEvent *ev)
  {// 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
  // 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders
  handlerEvent *event = (handlerEvent  *)ev;  
  int ret = event->getAction();
   getResultHandler(ret);    // set the result
  if(ret < 0){blockInput(false);return ret;}
  int saveType = event->getSaveType();
  if(saveType == 1)
      UndoSave();
  else if(saveType == 2)
      UndoTouch(false);

  // In case we called the orienthandler
  menuBar()->setItemChecked(A_SET_ORIENT,ShowOrientation()); 
  double Time = timer.elapsed()/1000.;
  if(ret == 1)
      {if(!MacroExecuting )gw->update();}
  else if(ret == 2)
      {if(!MacroRecording)information();
      if(!MacroExecuting )gw->update();
      }
  else if(ret == 20) // Remove handler
      {if(!MacroRecording)information();
      if(!MacroExecuting ) gw->update(0);
      }
  else if(ret == 3)
      mypaint->update(event->getDrawingType()); 
  else if(ret == 4) //3d drawings
      graphgl->update(); 
  else if(ret == 5) //symetrie
      {graphgl->update(); 
      graphsym->update();
      }
  // cases 6-7-8 we need a canvas with the current graph loaded
  else if(ret == 6)
      {gw->update();gw->Spring();}
  else if(ret == 7)
      {gw->update();gw->SpringPreservingMap();}
  else if(ret == 8)
      {gw->update();gw->SpringJacquard();}

  blockInput(false);
  double TimeG = timer.elapsed()/1000.;
  if(!MacroLooping && !MacroRecording && !ServerExecuting)
      {Tprintf("Used time:%3.3f (G+I:%3.3f)",Time,TimeG);
      if(getError())
          {Tprintf("Handler Error:%s",(const char *)getErrorString());
          if(debug())Twait((const char *)getErrorString()); 
          }
      }
  return ret;
  }
void pigaleWindow::banner()
  {QString msg;  
    int NumRecords =FileIOHandler.GetNumRecords(0,(const char *)InputFileName);
    int NumRecordsOut =FileIOHandler.GetNumRecords(0,(const char *)OutputFileName);
  msg.sprintf("Input: %s %d/%d  Output: %s %d Undo:%d/%d"
	    ,(const char *)InputFileName
	    ,*pGraphIndex,NumRecords
	    ,(const char *)OutputFileName
	    ,NumRecordsOut
	    ,UndoIndex,UndoMax);
  bannerEvent *e = new bannerEvent(msg);
  QApplication::postEvent(this,e);
  //statusBar()->message(m);
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
void pigaleWindow::showLabel(int show)
  {//Now We can only show the index of a vertex or a long
  int _show = ShowVertex();
  ShowVertex() = show -1;
  if(ShowVertex() != _show && GC.nv())gw->update();
  }
void  pigaleWindow::distOption(int use)
  {useDistance() = use;
  }
void  pigaleWindow::setShowOrientation(bool val)
  {ShowOrientation() = val;
  menuBar()->setItemChecked(A_SET_ORIENT,val);
  }
void pigaleWindow::print()
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
void pigaleWindow::SetPigaleFont()
 {bool ok;
  QFont font = QFontDialog::getFont( &ok, this->font(), this );
  if(!ok)return;
  QApplication::setFont(font,true);
 }
void  pigaleWindow::setUserMenu(int i, const QString &txt)
 {userMenu->changeItem ( A_TEST+i,txt);
 }
/*
The copies are cleared when a new graph is loaded or generated, but the last graph is saved
A copy is created when edges/vertices are added/deleted outside the editor

UndoMax  : # of save graphs
UndoSave : index of graph to restore [1,UndoMax]
Should always be possible to restore 1
*/

void pigaleWindow::Undo()
  {if (UndoIndex > UndoMax) UndoSave();
  if(UndoIndex > 1)--UndoIndex;
  if(FileIOHandler.Read(0,GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  this->undoR->setEnabled(UndoMax != 0 && UndoIndex < UndoMax);
  }
void pigaleWindow::UndoTouch(bool save)
  {if (UndoIndex<=UndoMax) {UndoMax=UndoIndex; UndoIndex++;banner();}
  else if (save) { UndoSave(); UndoIndex++;banner();}
  }
void pigaleWindow::UndoSave()
  {if(!IsUndoEnable)return;
  TopologicalGraph G(GC);
  if(G.nv() == 0)return;
  int nb = FileIOHandler.GetNumRecords(0,undofile); if (nb<0) nb=0;
  int last = UndoMax>UndoIndex ? UndoIndex : UndoMax;
  if(last < nb)
      {Tgf file;
      if(file.open(undofile, Tgf::old) == 0)return;
      while (last < nb)
          {file.DeleteRecord(nb);
          nb--;
          }
      }
  if(FileIOHandler.Save(0,G,undofile) == 1)
      {handler(A_SET_UNDO);return;}
  UndoIndex=UndoMax=++nb;
  undoL->setEnabled(true);
  banner();
  }
void pigaleWindow::Redo()
  {if(UndoIndex < UndoMax)++UndoIndex;
    if(FileIOHandler.Read(0,GC,undofile,UndoMax,UndoIndex) != 0)return;
  banner();
  information(); gw->update();
  undoR->setEnabled(UndoIndex < UndoMax);
  }
void pigaleWindow::UndoClear()
  {if(!IsUndoEnable)return;
  UndoIndex = 0;
  UndoMax = 0;
  this->undoL->setEnabled(false);
  this->undoR->setEnabled(false);
  // empty the tgf file
  Tgf undo_tgf;
  undo_tgf.open(undofile,Tgf::create);
  }
void pigaleWindow::UndoEnable(bool enable)
  {this->undoS->setEnabled(enable);
  if(!enable)
      UndoClear();
  else
      {IsUndoEnable = true;UndoSave();}
  banner();
  IsUndoEnable = enable;
  }
// UndoErase is called when the program exit
void UndoErase()
  {QFileInfo fi(undofile);
  if(!fi.exists())return;
  QFile undo_tgf(undofile);
  undo_tgf.remove();
  }


