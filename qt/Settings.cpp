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

#include <qapplication.h>
#include "MyWindow.h"
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

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView

#else
#include <qtextedit.h>
#include <qsettings.h>
#endif

#if QT_VERSION < 300
void MyWindow::SaveSettings()  
  { // Save colors
//   QFile settings("settings.txt");
//   if(settings.open(IO_ReadWrite)) 
//       {QTextStream txt(&settings);
//       int r,g,b;
//       txt << "colors" << endl;
//       for(int i = 0;i <= 3; i++)
// 	  {QColor col = QColor(QColorDialog::customColor(i));
// 	  col.rgb(&r,&g,&b); 
// 	  txt << i <<" "<< r <<" " << g <<" " << b << endl; 
// 	  }
//       settings.close();
//       }
  }
void MyWindow::LoadSettings()
{}
int GetPigaleColors()
  {
  return -1;
  }
#else
void MyWindow:: SaveSettings()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  //setting.writeEntry("/pigale/TgfFile dir",DirFile);
  setting.writeEntry("/pigale/TgfFile input",InputFileName);
  setting.writeEntry("/pigale/TgfFile output",OutputFileName);
  setting.writeEntry("/pigale/geometry width",this->width());
  setting.writeEntry("/pigale/geometry height",this->height());
  setting.writeEntry("/pigale/debug enable",debug());
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
  setting.writeEntry("/pigale/generate/gen N2",spin_N1->value());
  setting.writeEntry("/pigale/generate/gen M",spin_M->value());
  setting.writeEntry("/pigale/generate/gen EraseMultiple",randomEraseMultipleEdges());
  // macro
//setting.writeEntry("/pigale/generate/gen Repeat",spinMacro->value());
  setting.writeEntry("/pigale/macro/macroRepeat macroRepeat",macroLine->getNum());
  setting.writeEntry("/pigale/macro/macroRepeat macroMul",macroLine->getMul());
  setting.writeEntry("/pigale/macro/macroDelay macroDelay", pauseDelay());
  setting.writeEntry("/pigale/macro/macroDir macroDir",DirFileMacro);
  // Printer
  setting.writeEntry("/pigale/printer colormode",printer->colorMode());
  setting.writeEntry("/pigale/printer orientation",printer->orientation());
  // DirFilePng 
  setting.writeEntry("/pigale/png dir",DirFilePng);
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
  }
void MyWindow::LoadSettings()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
#ifndef  TDEBUG
  debug() = setting.readBoolEntry("/pigale/debug enable",false);
#else
  debug() = true;
#endif
  randomSeed() = setting.readBoolEntry("/pigale/randomSeed enable",false);
  randomSetSeed() = (long) setting.readNumEntry("/pigale/randomSeed seed",1);
  IsUndoEnable = setting.readBoolEntry("/pigale/undo enable",true);
  ShowVertex() = setting.readNumEntry("/pigale/embed/label show",0);
  SchnyderRect() = setting.readBoolEntry("/pigale/embed/schnyder rect",false);
  SchnyderLongestFace() = setting.readBoolEntry("/pigale/embed/schnyder longestface",true);
  SchnyderColor() = setting.readBoolEntry("/pigale/embed/schnyder color",false);
  useDistance() = setting.readNumEntry("/pigale/embed/distance dist",4);
  pauseDelay() = setting.readNumEntry("/pigale/macro/macroDelay macroDelay",5);
  randomEraseMultipleEdges() = setting.readBoolEntry("/pigale/generate/gen EraseMultiple",true);
  //DirFile = setting.readEntry("/pigale/TgfFile dir"," ");
  DirFilePng = setting.readEntry("/pigale/png dir",".");
  DirFileMacro = setting.readEntry("/pigale/macro/macroDir macroDir",".");
  InputFileName = setting.readEntry("/pigale/TgfFile input"
				    ,QString("!_!") 
				    + QDir::separator() 
				    + QString("a.tgf"));
  OutputFileName = setting.readEntry("/pigale/TgfFile output",InputFileName);

  // if pigale was called with arguments, we may modify some values
  if(qApp->argc() < 3)return;
  for(int i = 1; i < qApp->argc()-1;i++)
      {if(QString((const char *)qApp->argv()[i]) == "-fi")
	  {InputFileName = (const char *)qApp->argv()[i+1];
	  QFileInfo fi = QFileInfo(InputFileName);
	  if(!fi.exists()) 
	      qDebug("%s does not exist",(const char *)InputFileName);
	  i++;
	  }
      else if(QString((const char *)qApp->argv()[i]) == "-fo")
	  {OutputFileName =  (const char *)qApp->argv()[i+1];
	  i++;
	  }
      else if(QString((const char *)qApp->argv()[i]) == "-macro")
	  {MacroFileName =  (const char *)qApp->argv()[i+1];
	  MacroPlay = true;
	  i++;
	  }
      else
	  {qDebug("%s option not recognized",(const char *)qApp->argv()[i]);
	  qDebug("valid options: -fi -fo -macro");
	  }
      }
  }
int GetPigaleColors()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  QColor col;
  int r,g,b;
  r = setting.readNumEntry("/pigale/colors/Background r",170);
  g = setting.readNumEntry("/pigale/colors/Background g",187);
  b = setting.readNumEntry("/pigale/colors/Background b",203);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(0,col.rgb());
  r = setting.readNumEntry("/pigale/colors/Base r",248);
  g = setting.readNumEntry("/pigale/colors/Base g",238);
  b = setting.readNumEntry("/pigale/colors/Base b",224);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(1,col.rgb());
  r = setting.readNumEntry("/pigale/colors/LightBackground r",180);
  g = setting.readNumEntry("/pigale/colors/LightBackground g",210);
  b = setting.readNumEntry("/pigale/colors/LightBackground b",241);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(2,col.rgb());
  r = setting.readNumEntry("/pigale/colors/GreenBackground r",165);
  g = setting.readNumEntry("/pigale/colors/GreenBackground g",210);
  b = setting.readNumEntry("/pigale/colors/GreenBackground b",180);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(3,col.rgb());
  return 0;
  }
#endif

void MyWindow::SetPigaleColors()
  {QColor  initial = QColor(248,238,224);
  QColorDialog::getColor(initial,this); 
  QPalette Palette = QApplication::palette();
  Palette.setColor(QColorGroup::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QColorGroup::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QColorGroup::Button    ,QColor(QColorDialog::customColor(2)));
  QApplication::setPalette(Palette,TRUE);
  LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QColorGroup::Base,QColor(QColorDialog::customColor(1)));
  this->mouse_actions->setPalette(LightPalette,TRUE);
  this->tabWidget->setPalette(LightPalette,TRUE);
  this->statusBar()->setBackgroundColor(QColor(QColorDialog::customColor(1)));
  QBrush pb(QColorDialog::customColor(1));
  this->e->setPaper(pb); 
  }
void InitPigaleColors()
  {// Set the colors of tha application
  if(GetPigaleColors() == -1)
      {QColor BackgroundColor  = QColor(170,187,203);
      QColor Base = QColor(248,238,224);
      QColor LightBackgroundColor = QColor(180,210,241);
      QColor GreenBackgroundColor = QColor(165,210,180);
      QColorDialog::setCustomColor(0,BackgroundColor.rgb()); 
      QColorDialog::setCustomColor(1,Base.rgb()); 
      QColorDialog::setCustomColor(2,LightBackgroundColor.rgb()); 
      QColorDialog::setCustomColor(3,GreenBackgroundColor.rgb()); 
      }
  QPalette Palette = QApplication::palette();
  Palette.setColor(QColorGroup::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QColorGroup::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QColorGroup::Button    ,QColor(QColorDialog::customColor(2)));
  QApplication::setPalette(Palette,TRUE);
  }
void InitPigaleFont()
  {
#if QT_VERSION >= 300
  QRect rect = QApplication::desktop()->screenGeometry();
  int h = rect.height();
#else
  QWidget *d = QApplication::desktop();
  int h = d->height();
#endif
  int fontsize = (h > 600) ? 12 : 11;
  QFont font = QFont("helvetica",fontsize);
  font.setPixelSize(fontsize);
  QApplication::setFont(font,true);
  }




