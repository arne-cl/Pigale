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


#include <config.h>
#include <qapplication.h>
#include <qtextcodec.h> 
#include <qtranslator.h>
#include <qdir.h>
#include "pigaleWindow.h"
#include <QT/Misc.h>

void InitPigaleColors();

int main(int argc,char ** argv)
  {QApplication app(argc,argv);
  // Set the colors of tha application
  InitPigaleColors();

  //Translations
  QString transDict= QString(PACKAGE_PATH)+ QDir::separator()+"translations"+ QDir::separator();
  // translation file for Qt
  QTranslator qt( 0 );
  qt.load(QString("qt_") + QTextCodec::locale(),transDict);
  app.installTranslator( &qt );
  // translation file for application strings
  QTranslator myapp( 0 );

  myapp.load( QString("pigale_" ) + QTextCodec::locale(),transDict);
  app.installTranslator( &myapp );
  pigaleWindow *mw = new pigaleWindow();
  mw->show();
  app.connect( &app, SIGNAL(lastWindowClosed()),&app,SLOT(quit()));
  int result = app.exec();
  return result;
  }
