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

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView
#include <qtextstream.h>
#else
#include <qtextedit.h>
#include <qsettings.h>
#endif

#if QT_VERSION < 300
   // Save colors
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
  
void pigaleWindow::SaveSettings()
  {QFile settings("settings.txt");
  if(!settings.open(IO_WriteOnly))return; 
  QTextStream txt(&settings);
  txt << "GeometryHeight=" << this->height() << endl;
  txt << "GeometryWidth=" << this->width() << endl;
  txt << "InputFileName=" << InputFileName << endl;
  txt << "OutputFileName=" << OutputFileName << endl;
  txt << "Debug=" << debug() << endl;
  txt << "IsUndoEnable=" << IsUndoEnable << endl;
  txt << "RandomSeedEnable=" << randomSeed() << endl;
  txt << "RandomSeed=" << randomSetSeed() << endl;
  txt << "LimitsSlow=" << spin_MaxNS->value() << endl;
  txt << "LimitsDisplay=" << spin_MaxND->value() << endl;
  txt << "EmbedShowLabels=" << ShowVertex() << endl;
  txt << "EmbedRect=" << SchnyderRect() << endl;
  txt << "EmbedLongestFace=" << SchnyderLongestFace() << endl;
  txt << "EmbedSchnyderColor=" << SchnyderColor() << endl;
  txt << "EmbedDistance=" << useDistance() << endl;
  txt << "GenerateN1=" << spin_N1->value() << endl;
  txt << "GenerateN2=" << spin_N2->value() << endl;
  txt << "GenerateM=" << spin_M->value() << endl;
  txt << "GenerateMultiple=" << randomEraseMultipleEdges() << endl;
  txt << "MacroRepeat=" << macroLine->getNum() << endl;
  txt << "MacroRepeatMul=" << macroLine->getMul() << endl;
  txt << "MacroDelay=" << pauseDelay() << endl;
  txt << "MacroDir=" << DirFileMacro << endl;
  txt << "PngDir=" << DirFilePng << endl;
  txt << "DocDir=" << DirFileDoc << endl; 
  txt << "PrinterColorMode=" << printer->colorMode() << endl;
  txt << "PrinterOrientation=" << printer->orientation() << endl;
  settings.close();
  }  

void pigaleWindow::LoadSettings()
  {// define default values
  pigaleWindowInitYsize = 600;
  pigaleWindowInitXsize = 800;
  InputFileName = OutputFileName = "a.txt";
  IsUndoEnable = true;
  debug() = false;
  randomSeed() = true;
  randomSetSeed() = 1;
  pauseDelay() = 1;
  macroRepeat = 1;
  macroMul = 0;
  MaxNS = 500;
  MaxND = 500;
  Gen_N1 = Gen_N2 = 10;
  Gen_M = 60;
  PrinterOrientation = 1;
  PrinterColorMode = 1;
  DirFileMacro = ".";
  DirFilePng = ".";
  DirFileDoc = "Doc";
 
  // Read settings
  QFile settings("settings.txt");
  if(!settings.open(IO_ReadOnly))return; 
  QTextStream txt(&settings);
  QString str,arg,param;
  int pos;
  bool ok;
  int val;
  while(!txt.atEnd())
      {str = txt.readLine();
       pos = str.find('=');
       arg = str.left(pos); param = str.right(str.length() - 1 - pos);
       ok = true;
       if(arg == "GeometryHeight")
          {val = param.toInt(&ok); if(ok)pigaleWindowInitYsize = val;}
       else if(arg == "GeometryWidth")
          {val = param.toInt(&ok); if(ok)pigaleWindowInitXsize = val;}
       else if(arg == "InputFileName")
          {if(!param.isEmpty())InputFileName = param;}
       else if(arg == "OutputFileName")
          {if(!param.isEmpty())OutputFileName = param;}
       else if(arg == "Debug")
          {val = param.toInt(&ok); if(ok)debug() = (bool)val;}
       else if(arg == "IsUndoEnable")
          {val = param.toInt(&ok); if(ok)IsUndoEnable = (bool)val;}
       else if(arg == "RandomSeedEnable")
          {val = param.toInt(&ok); if(ok)randomSeed() = (bool)val;}
       else if(arg == "RandomSeed")
          {val = param.toInt(&ok); if(ok)randomSetSeed() = val;}
       else if(arg == "LimitsSlow")
          {val = param.toInt(&ok); if(ok)MaxNS = val;}
       else if(arg == "LimitsDisplay")
          {val = param.toInt(&ok); if(ok)MaxND = val;}
       else if(arg == "EmbedShowLabels")
          {val = param.toInt(&ok); if(ok)ShowVertex() = val;}
       else if(arg == "EmbedLongestFace")
          {val = param.toInt(&ok); if(ok)SchnyderLongestFace() = (bool)val;}
       else if(arg == "EmbedSchnyderColor")
          {val = param.toInt(&ok); if(ok)SchnyderColor() = (bool)val;}
       else if(arg == "EmbedRect")
          {val = param.toInt(&ok); if(ok)SchnyderRect() = (bool)val;}
       else if(arg == "EmbedDistance")
          {val = param.toInt(&ok); if(ok)useDistance() = val;}
       else if(arg == "GenerateN1")
          {val = param.toInt(&ok); if(ok)Gen_N1 = val;}
       else if(arg == "GenerateN2")
          {val = param.toInt(&ok); if(ok)Gen_N2 = val;}
       else if(arg == "GenerateM")
          {val = param.toInt(&ok); if(ok)Gen_M = val;}
       else if(arg == "GenerateMultiple")
          {val = param.toInt(&ok); if(ok)randomEraseMultipleEdges() = (bool)val;}
       else if(arg == "MacroRepeat")
          {val = param.toInt(&ok); if(ok)macroRepeat  = val;}
       else if(arg == "MacroRepeatMul")
          {val = param.toInt(&ok); if(ok)macroMul  = val;}
       else if(arg == "MacroDelay")
          {val = param.toInt(&ok); if(ok)pauseDelay()  = val;}
      else if(arg == "MacroDir")
          {if(!param.isEmpty())DirFileMacro = param;}
      else if(arg == "PngDir")
          {if(!param.isEmpty())DirFilePng = param;}
      else if(arg == "DocDir")
          {if(!param.isEmpty())DirFileDoc = param;}
       else if(arg == "PrinterOrientation")
          {val = param.toInt(&ok); if(ok)PrinterOrientation = val;}
       else if(arg == "PrinterColorMode")
          {val = param.toInt(&ok); if(ok)PrinterColorMode = val;}
      else
         LogPrintf("could not interpret:\n%s\n",(const char *)str);
      if(!ok)LogPrintf("ERROR SETTINGS:%s -> %s\n",(const char *)arg,(const char *)param);
      } 
  settings.close();
  LogPrintf("language:-%s-\n",(const char *)QTextCodec::locale());
  }
int GetPigaleColors()
  {
  return -1;
  }
#else
void pigaleWindow:: SaveSettings()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
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
  setting.writeEntry("/pigale/generate/gen N2",spin_N2->value());
  setting.writeEntry("/pigale/generate/gen M",spin_M->value());
  setting.writeEntry("/pigale/generate/gen EraseMultiple",randomEraseMultipleEdges());
  // macro
  setting.writeEntry("/pigale/macro/macroRepeat macroRepeat",macroLine->getNum());
  setting.writeEntry("/pigale/macro/macroRepeat macroMul",macroLine->getMul());
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
  setting.writeEntry("/pigale/font/family",this->font().family());
  setting.writeEntry("/pigale/font/size",this->font().pointSize());

  }

void pigaleWindow::LoadSettings()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  // Screen size
  pigaleWindowInitYsize = setting.readNumEntry("/pigale/geometry height",600);
  pigaleWindowInitXsize = setting.readNumEntry("/pigale/geometry width",800);
  debug() = setting.readBoolEntry("/pigale/debug enable",false);
  randomSeed() = setting.readBoolEntry("/pigale/randomSeed enable",false);
  randomSetSeed() = (long) setting.readNumEntry("/pigale/randomSeed seed",1);
  IsUndoEnable = setting.readBoolEntry("/pigale/undo enable",true);
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
  pauseDelay() = setting.readNumEntry("/pigale/macro/macroDelay macroDelay",5);
  macroRepeat = setting.readNumEntry("/pigale/macro/macroRepeat macroRepeat",100);
  macroMul = setting.readNumEntry("/pigale/macro/macroRepeat macroMul",0);
  // Generator
  Gen_N1 = setting.readNumEntry("/pigale/generate/gen N1",10);
  Gen_N2 = setting.readNumEntry("/pigale/generate/gen N2",10);
  Gen_M  = setting.readNumEntry("/pigale/generate/gen M",30);
  randomEraseMultipleEdges() = setting.readBoolEntry("/pigale/generate/gen EraseMultiple",true);
  //DirFile 
  DirFilePng = setting.readEntry("/pigale/png dir",".");
  DirFileMacro = setting.readEntry("/pigale/macro/macroDir macroDir",".");
  InputFileName = setting.readEntry("/pigale/TgfFile input"
				    ,QString("../tgf")//,QString("!_!")
				    + QDir::separator()
				    + QString("a.tgf"));
  OutputFileName = setting.readEntry("/pigale/TgfFile output",InputFileName);
  DirFileDoc = setting.readEntry("/pigale/Documentation dir",QString(PACKAGE_PATH)+QDir::separator()+"Doc");
  // Font
#if QT_VERSION >= 300
  QRect rect = QApplication::desktop()->screenGeometry();
  int h = rect.height();
#else
  QWidget *d = QApplication::desktop();
  int h = d->height();
#endif
  int fontSize = (h > 600) ? 12 : 10;
  QString family = setting.readEntry("/pigale/font/family","Helvetica");
  fontSize = setting.readNumEntry("/pigale/font/size",fontSize);
  QFont font = QFont(family,fontSize);
  QApplication::setFont(font,true);
}

int GetPigaleColors()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  QColor col;
  int r,g,b;
  r = setting.readNumEntry("/pigale/colors/Background r",180);
  g = setting.readNumEntry("/pigale/colors/Background g",180);
  b = setting.readNumEntry("/pigale/colors/Background b",180);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(0,col.rgb());
  r = setting.readNumEntry("/pigale/colors/Base r",248);
  g = setting.readNumEntry("/pigale/colors/Base g",238);
  b = setting.readNumEntry("/pigale/colors/Base b",224);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(1,col.rgb());
  r = setting.readNumEntry("/pigale/colors/LightBackground r",208);
  g = setting.readNumEntry("/pigale/colors/LightBackground g",208);
  b = setting.readNumEntry("/pigale/colors/LightBackground b",208);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(2,col.rgb());
  r = setting.readNumEntry("/pigale/colors/GreenBackground r",165);
  g = setting.readNumEntry("/pigale/colors/GreenBackground g",210);
  b = setting.readNumEntry("/pigale/colors/GreenBackground b",180);
  col.setRgb(r,g,b);QColorDialog::setCustomColor(3,col.rgb());
  return 0;
  }
#endif
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
void pigaleWindow::SetPigaleColors()
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





