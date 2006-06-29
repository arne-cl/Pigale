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
#include <qapplication.h>
#include <qdesktopwidget.h>
#include "pigaleWindow.h"
#include "mouse_actions.h"
#include "LineEditNum.h"
#include "gprop.h"
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <qfont.h>
#include <qpalette.h>
#include <qcolordialog.h> 
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qprinter.h>
#include <qdir.h>
#include <qtextcodec.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <Q3FileDialog>

#include "GraphGL.h"
#include "GraphSym.h"

#ifdef _WIN32
#undef PACKAGE_PATH
#define PACKAGE_PATH ".."
#elif defined(_WINDOWS)
#undef PACKAGE_PATH
#define PACKAGE_PATH "c:\\Program Files\\Pigale"
#endif


void pigaleWindow::SetDocumentPath()
  {QString StartDico = (DirFileDoc.isEmpty()) ? "." : DirFileDoc;
  DirFileDoc = Q3FileDialog::
  getExistingDirectory(StartDico,this,"find",tr("Choose the documentation directory <em>Doc</em>"),TRUE);
  if(!DirFileDoc.isEmpty())
      {QFileInfo fi =  QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
      if(fi.exists())
          {browser->setSearchPaths(QStringList(DirFileDoc));
          browser->setSource(QUrl("manual.html"));
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
void pigaleWindow:: SaveSettings()
  {QSettings setting("EHESS-CNRS","Pigale");
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  setting.writeEntry("/pigale/TgfFile input",InputFileName);
  setting.writeEntry("/pigale/TgfFile output",OutputFileName);
  setting.writeEntry("/pigale/geometry width",width());
  setting.writeEntry("/pigale/geometry height",height());
  setting.setValue("pos", pos());
  setting.writeEntry("/pigale/debug enable",debug());
  setting.writeEntry("/pigale/arrow enable",ShowArrow());
  setting.writeEntry("/pigale/orientation enable",ShowOrientation());
  setting.writeEntry("/pigale/randomSeed enable",randomSeed());
  setting.writeEntry("/pigale/randomSeed seed",(int)randomSetSeed());
  setting.writeEntry("/pigale/undo enable",IsUndoEnable);
  // limits
  setting.writeEntry("/pigale/limits slow_algo",spin_MaxNS->value());
  setting.writeEntry("/pigale/limits display",spin_MaxND->value());
  // embed
  setting.writeEntry("/pigale/embed/label show",ShowVertex());
  setting.writeEntry("/pigale/embed/schnyder rect",SchnyderRect());
  setting.writeEntry("/pigale/embed/schnyder longestface",SchnyderLongestFace());
  setting.writeEntry("/pigale/embed/schnyder color",SchnyderColor());
  setting.writeEntry("/pigale/embed/distance dist",useDistance());
  // generate
  setting.writeEntry("/pigale/generate/gen N1",spin_N1->value());
  setting.writeEntry("/pigale/generate/gen N2",spin_N2->value());
  setting.writeEntry("/pigale/generate/gen M",spin_M->value());
  setting.writeEntry("/pigale/generate/gen EraseMultiple",randomEraseMultipleEdges());
  setting.writeEntry("/pigale/generate/gen UseGeneratedCir",randomUseGeneratedCir());
  // macro
  setting.writeEntry("/pigale/macro/macroRepeat macroRepeat",macroLine->getVal());
  setting.writeEntry("/pigale/macro/macroDelay macroDelay", pauseDelay());
  setting.writeEntry("/pigale/macro/macroDir macroDir",DirFileMacro);
  // Printer
  setting.writeEntry("/pigale/printer colormode",printer->colorMode());
  setting.writeEntry("/pigale/printer orientation",printer->orientation());
  // DirFilePng 
  setting.writeEntry("/pigale/png dir",DirFilePng);
  setting.writeEntry("/pigale/Documentation dir",DirFileDoc);
  // Custom colors
  int r,g,b;
  QColor col = QColor(QColorDialog::customColor(0));
  col.rgb(&r,&g,&b);
  setting.writeEntry("/pigale/colors/Background r",r);
  setting.writeEntry("/pigale/colors/Background g",g);
  setting.writeEntry("/pigale/colors/Background b",b);
  col = QColor(QColorDialog::customColor(1));
  col.rgb(&r,&g,&b);
  setting.writeEntry("/pigale/colors/Base r",r);
  setting.writeEntry("/pigale/colors/Base g",g);
  setting.writeEntry("/pigale/colors/Base b",b);
  col = QColor(QColorDialog::customColor(2));
  col.rgb(&r,&g,&b);
  setting.writeEntry("/pigale/colors/LightBackground r",r);
  setting.writeEntry("/pigale/colors/LightBackground g",g);
  setting.writeEntry("/pigale/colors/LightBackground b",b);
  col = QColor(QColorDialog::customColor(3));
  col.rgb(&r,&g,&b);
  setting.writeEntry("/pigale/colors/GreenBackground r",r);
  setting.writeEntry("/pigale/colors/GreenBackground g",g);
  setting.writeEntry("/pigale/colors/GreenBackground b",b);
  // Font
  setting.writeEntry("/pigale/font/family",QApplication::font().family());
  setting.writeEntry("/pigale/font/size",QApplication::font().pointSize());
  }

void pigaleWindow::LoadSettings()
  {QSettings setting("EHESS-CNRS","Pigale");
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  // Screen size
  pigaleWindowInitYsize = setting.readNumEntry("/pigale/geometry height",600);
  pigaleWindowInitXsize = setting.readNumEntry("/pigale/geometry width",800);
  QPoint pos = setting.value("pos", QPoint(200, 200)).toPoint();
  move(pos);
  debug() = setting.readBoolEntry("/pigale/debug enable",false);
  ShowArrow() = setting.readBoolEntry("/pigale/arrow enable",true);
  ShowOrientation() = setting.readBoolEntry("/pigale/orientation enable",true);
  randomSeed() = setting.readBoolEntry("/pigale/randomSeed enable",false);
  randomSetSeed() = (long) setting.readNumEntry("/pigale/randomSeed seed",1);
  IsUndoEnable = setting.readBoolEntry("/pigale/undo enable",true);
  // Printer
  PrinterOrientation = setting.readNumEntry("/pigale/printer orientation",0);
  PrinterColorMode = setting.readNumEntry("/pigale/printer colormode",0);
  // limits to display and execute slow algos
  MaxNS = setting.readNumEntry("/pigale/limits slow_algo",500);
  MaxND = setting.readNumEntry("/pigale/limits display",500);

  // Embed
  ShowVertex() = setting.readNumEntry("/pigale/embed/label show",0);
  SchnyderRect() = setting.readBoolEntry("/pigale/embed/schnyder rect",false);
  SchnyderLongestFace() = setting.readBoolEntry("/pigale/embed/schnyder longestface",true);
  SchnyderColor() = setting.readBoolEntry("/pigale/embed/schnyder color",false);
  useDistance() = setting.readNumEntry("/pigale/embed/distance dist",4);
  // Macro
  pauseDelay() = setting.readNumEntry("/pigale/macro/macroDelay macroDelay",1);
  macroRepeat = setting.readNumEntry("/pigale/macro/macroRepeat macroRepeat",100);
  // Generator
  Gen_N1 = setting.readNumEntry("/pigale/generate/gen N1",10);
  Gen_N2 = setting.readNumEntry("/pigale/generate/gen N2",10);
  Gen_M  = setting.readNumEntry("/pigale/generate/gen M",30);
  randomEraseMultipleEdges() = setting.readBoolEntry("/pigale/generate/gen EraseMultiple",true);
  randomUseGeneratedCir() = setting.readBoolEntry("/pigale/generate/gen UseGeneratedCir",true);
  //DirFile 
  DirFilePng = setting.readEntry("/pigale/png dir",".");
  DirFileMacro = setting.readEntry("/pigale/macro/macroDir macroDir",QString(PACKAGE_PATH)+QDir::separator()+"macro");
  InputFileName = setting.readEntry("/pigale/TgfFile input",
                                    QString(QString(PACKAGE_PATH)+ QDir::separator() + "tgf"+QDir::separator()+ "a.tgf"));
  OutputFileName = setting.readEntry("/pigale/TgfFile output",InputFileName);
  DirFileDoc = setting.readEntry("/pigale/Documentation dir",QString(PACKAGE_PATH)+QDir::separator()+"Doc");
  // Font
  QDesktopWidget *desktop = QApplication::desktop();
  int h = desktop->height();
  int fontSize = (h > 600) ? 9 : 8;
  QString family = setting.readEntry("/pigale/font/family","Helvetica");
  fontSize = setting.readNumEntry("/pigale/font/size",fontSize);
  QFont font = QFont(family,fontSize);
  QApplication::setFont(font,true);
  setFont(font,true);
}


void pigaleWindow::EditPigaleColors()
  {QColor  initial = QColor(248,238,224);
  QColorDialog::getColor(initial,this); 
  UpdatePigaleColors();
  }
void pigaleWindow::UpdatePigaleColors()
  {QPalette Palette = QApplication::palette();
  Palette.setColor(QColorGroup::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QColorGroup::Dark,QColor(Qt::black));
  Palette.setColor(QColorGroup::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QColorGroup::Button    ,QColor(QColorDialog::customColor(2)));
  
  LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QColorGroup::Base,QColor(QColorDialog::customColor(1)));
  LightPalette.setColor(QColorGroup::Dark,QColor(Qt::black));
  LightPalette.setColor(QColorGroup::ButtonText,QColor(Qt::darkGreen));
  LightPalette.setColor(QColorGroup::WindowText,QColor(Qt::black));
  
  QApplication::setPalette(Palette);  setPalette(Palette,TRUE);
  graph_properties->setPalette(Palette); 
  gSettings->setPalette(LightPalette); gSettings->setAutoFillBackground(true); 
  mouse_actions->setPalette(LightPalette,TRUE);
  tabWidget->setPalette(LightPalette,TRUE);
  graphgl->setPalette(Palette);
  graphsym->setPalette(Palette);
  graphsym->setBackgroundColor(QColor(Qt::white));
  statusBar()->setBackgroundColor(QColor(QColorDialog::customColor(1)));
  }
void pigaleWindow::SetPigaleColorsProfile1() //gray
  {QColor col;
  //Background
  col.setRgb(180,180,180);QColorDialog::setCustomColor(0,col.rgb());
  //LightBackground
  col.setRgb(208,208,208);QColorDialog::setCustomColor(2,col.rgb());
  //Base
  col.setRgb(248,238,224);QColorDialog::setCustomColor(1,col.rgb());
  UpdatePigaleColors();
  //QApplication::setStyle(QStyleFactory::create("windows"));
  }
void pigaleWindow::SetPigaleColorsProfile2()//yellow
  {QColor col;
  //Background
  col.setRgb(163,163,127);QColorDialog::setCustomColor(0,col.rgb());
  //LightBackground
  col.setRgb(189,187,154);QColorDialog::setCustomColor(2,col.rgb());
  //Base
  col.setRgb(248,238,224);QColorDialog::setCustomColor(1,col.rgb());
  UpdatePigaleColors();
  //QApplication::setStyle(QStyleFactory::create("plastique"));
  //QApplication::setPalette(QApplication::style()->standardPalette());
  }
int GetPigaleColors()
  {QSettings setting("EHESS-CNRS","Pigale");
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  QColor col;
  int r,g,b;
  r = setting.readNumEntry("/pigale/colors/Background r",163);//180
  g = setting.readNumEntry("/pigale/colors/Background g",163);//180
  b = setting.readNumEntry("/pigale/colors/Background b",127);//180
  col.setRgb(r,g,b);QColorDialog::setCustomColor(0,col.rgb());
  r = setting.readNumEntry("/pigale/colors/Base r",248);//248
  g = setting.readNumEntry("/pigale/colors/Base g",238);//238
  b = setting.readNumEntry("/pigale/colors/Base b",224);//224
  col.setRgb(r,g,b);QColorDialog::setCustomColor(1,col.rgb());
  r = setting.readNumEntry("/pigale/colors/LightBackground r",189);//208
  g = setting.readNumEntry("/pigale/colors/LightBackground g",187);//208
  b = setting.readNumEntry("/pigale/colors/LightBackground b",154);//208
  col.setRgb(r,g,b);QColorDialog::setCustomColor(2,col.rgb());
  r = setting.readNumEntry("/pigale/colors/GreenBackground r",165);
  g = setting.readNumEntry("/pigale/colors/GreenBackground g",210);
  b = setting.readNumEntry("/pigale/colors/GreenBackground b",180);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(3,col.rgb());
  return 0;
  }
void InitPigaleColors()
  {// Set the colors of tha application
  GetPigaleColors();
  QPalette Palette = QApplication::palette();
  Palette.setColor(QColorGroup::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QColorGroup::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QColorGroup::Button    ,QColor(QColorDialog::customColor(2)));
  QApplication::setPalette(Palette);
  }

void pigaleWindow::ParseArguments()
  // if pigale was called with arguments, we may modify some values
  {if(qApp->argc() < 1)return;
  int narg = qApp->argc() -1;
  for(int i = 1; i <= narg;i++)
      {if(QString((const char *)qApp->argv()[i]) == "-fi")
	  {if(i == narg)return;
	  InputFileName = (const char *)qApp->argv()[i+1];
	  QFileInfo fi = QFileInfo(InputFileName);
	  if(!fi.exists()) 
	      qDebug("%s does not exist",(const char *)InputFileName);
	  i++;
	  }
      else if(QString((const char *)qApp->argv()[i]) == "-fo")
	  {if(i == narg)return;
	  OutputFileName =  (const char *)qApp->argv()[i+1];
	  i++;
	  }
      else if(QString((const char *)qApp->argv()[i]) == "-macro")
	  {if(i == narg)return;
	  MacroFileName =  (const char *)qApp->argv()[i+1];
	  MacroPlay = true;
	  i++;
	  }
      else if((QString((const char *)qApp->argv()[i]) == "--version") || (QString((const char *)qApp->argv()[i]) == "-v"))
	qDebug("%s version %s",PACKAGE_NAME,PACKAGE_VERSION);
      else if(QString((const char *)qApp->argv()[i]) == "-server")
	  Server = true;
      else
	  {qDebug("%s option not recognized",(const char *)qApp->argv()[i]);
	  qDebug("valid options:\n -fi input\n -fo output\n -macro macro\n -server");
	  }
      }
  if(Server || MacroPlay)IsUndoEnable = false;
  }



