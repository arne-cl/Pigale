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

#ifndef LINEEDITNUM_H
#define LINEEDTNUM_H
#include <qlineedit.h>

class LineEditNum : public QLineEdit
{
  Q_OBJECT
 private: 
  int num;
  int mul;
  int prod;
  QString prefix;
  void display();
 public:
  LineEditNum(QWidget * parent, const char * name = 0);
  ~LineEditNum(){};
  void setPrefix(const QString &t);
  int getNum();
  int getMul();
  int getVal();

 public slots:
   void setNum(int  i);
   void setMul(int  i);

};

#endif
