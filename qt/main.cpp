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

/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : sam oct 13 10:05:44 CEST 2001
    copyright            : (C) 2001 by Hubert de Fraysseix
                                     &  Patrice Ossona de Mendez
    email                : hf@ehess.fr
 ***************************************************************************/

#include <qapplication.h>
#include "MyWindow.h"
#include <QT/Misc.h>
#include <qcolordialog.h> 
#include <qcolor.h> 
#include <qpalette.h>

int main(int argc,char ** argv)
  {QApplication a(argc,argv);
  QFont font = QFont("helvetica",13);
  QApplication::setFont(font,TRUE);
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
  MyWindow *mw = new MyWindow();
  mw->show();
  a.connect( &a, SIGNAL(lastWindowClosed()),&a,SLOT(quit()));
  int result = a.exec();
  return result;
  }

