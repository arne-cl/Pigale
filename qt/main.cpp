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
#include <qtextcodec.h> 
#include <qtranslator.h>
#include "MyWindow.h"
#include <QT/Misc.h>

void InitPigaleColors();
void InitPigaleFont();

int main(int argc,char ** argv)
  {QApplication app(argc,argv);
  // Set the font for all widgets
  InitPigaleFont();

  // Set the colors of tha application
  InitPigaleColors();

  // translation file for Qt
  QTranslator qt( 0 );
  qt.load( QString( "qt_" ) + QTextCodec::locale(), "." );
  app.installTranslator( &qt );

  // translation file for application strings
  QTranslator myapp( 0 );
  myapp.load( QString( "pigale_" ) + QTextCodec::locale(), "." );
  //myapp.load( QString( "pigale_fr" ), "." );
  app.installTranslator( &myapp );

  MyWindow *mw = new MyWindow();
  mw->show();
  app.connect( &app, SIGNAL(lastWindowClosed()),&app,SLOT(quit()));
  int result = app.exec();
  return result;
  }
