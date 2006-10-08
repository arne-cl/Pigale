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

#undef QT3_SUPPORT

#include "LineEditNum.h"

#define MULT 100
LineEditNum::LineEditNum(QWidget * parent)
    :QLineEdit(parent),num(1),mul(1)
  {}
void LineEditNum::display()
  {int i = (mul == 0) ? 1 : mul*MULT;
  QLineEdit::setText(QString("(%1x%2) %3").arg(i).arg(num).arg(prod));
  }
void LineEditNum::setNum(int  i)
  {num = i;
  prod = (mul == 0) ? num : num*mul*MULT;
  display();
  }
void LineEditNum::setPrefix(const QString &t)
  {prefix = t;
  display();
  }
void LineEditNum::setMul(int i)
  {mul = i;
  prod = (mul == 0) ? num : num*mul*MULT;
  display();
  }
int LineEditNum::getNum()
  {return num;
  }
int LineEditNum::getMul()
  {return mul;
  }
int LineEditNum::getVal()
  {return prod;
  }
