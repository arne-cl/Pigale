#include <qapplication.h>
#include "MyWindow.h"
#include "mouse_actions.h"
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

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView
#include <qsettings.h>
#else
#include <qtextedit.h>
#include <qsettings.h>
#endif

#if QT_VERSION < 300
void MyWindow::SaveSettings()  
  { // Save colors
  QFile settings("settings.txt");
  if(settings.open(IO_ReadWrite)) 
      {QTextStream txt(&settings);
      int r,g,b;
      txt << "colors" << endl;
      for(int i = 0;i <= 3; i++)
	  {QColor col = QColor(QColorDialog::customColor(i));
	  col.rgb(&r,&g,&b); 
	  txt << i <<" "<< r <<" " << g <<" " << b << endl; 
	  }
      settings.close();
      }
  }
void MyWindow::LoadSettings()
{}
int GetPigaleColors()
  {QFileInfo fi("settings.txt"); 
  if(!fi.exists())return -1;
  QFile settings("settings.txt");
  if(settings.open(IO_ReadWrite))
      {QString s;
      int ii,r,g,b;
      QColor col;
      QTextStream txt(&settings);
      s = txt.readLine(); 
      if(s.contains("colors") == 0)
	  {settings.close();
	  qDebug("Error reading settings.txt");
	  return -1;
	  }
      for(int i = 0;i <= 3; i++)
	  {txt >> ii >> r >>  g >> b;
	  col.setRgb(r,g,b);
	  if(col.isValid() && i < 4)
	      QColorDialog::setCustomColor(i,col.rgb());
	  else 
	      {qDebug("Error reading settings.txt");
	      settings.close();
	      return -1;
	      }
	  }
      settings.close();
      return 0;
      }
  return -1;
  }
#else
void MyWindow:: SaveSettings()
  {QSettings setting;
  setting.insertSearchPath(QSettings::Windows,"/pigale");
  setting.writeEntry("/pigale/TgfFile input",InputFileName);
  setting.writeEntry("/pigale/TgfFile output",OutputFileName);
  setting.writeEntry("/pigale/geometry width",this->width());
  setting.writeEntry("/pigale/geometry height",this->height());
  setting.writeEntry("/pigale/debug enable",debug());
  setting.writeEntry("/pigale/undo enable",IsUndoEnable);
  setting.writeEntry("/pigale/limits slow_algo",spin_MaxNS->value());
  setting.writeEntry("/pigale/limits display",spin_MaxND->value());
  setting.writeEntry("/pigale/embed/label show",ShowVertex());
  setting.writeEntry("/pigale/embed/schnyder rect",SchnyderRect());
  setting.writeEntry("/pigale/embed/schnyder longestface",SchnyderLongestFace());
  setting.writeEntry("/pigale/embed/schnyder color",SchnyderColor());
  setting.writeEntry("/pigale/embed/distance dist",useDistance());
  // Printer
  setting.writeEntry("/pigale/printer colormode",printer->colorMode());
  setting.writeEntry("/pigale/printer orientation",printer->orientation());
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
  IsUndoEnable = setting.readBoolEntry("/pigale/undo enable",true);
  ShowVertex() = setting.readNumEntry("/pigale/embed/label show",0);
  SchnyderRect() = setting.readBoolEntry("/pigale/embed/schnyder rect",false);
  SchnyderLongestFace() = setting.readBoolEntry("/pigale/embed/schnyder longestface",true);
  SchnyderColor() = setting.readBoolEntry("/pigale/embed/schnyder color",false);
  useDistance() = setting.readBoolEntry("/pigale/embed/distance dist",1);
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
  QPalette LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
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
  QApplication::setFont(font,true);
  }




