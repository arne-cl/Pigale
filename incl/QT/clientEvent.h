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

#ifndef CLIENTEVENT_H 
#define CLIENTEVENT_H

#define CLIENTEVENT 65432
#define TEXTEVENT 65433

class clientEvent : public QCustomEvent
    {public:
      clientEvent( int action,QString  param)
          : QCustomEvent(CLIENTEVENT ), value( action ),str(param)  {}
       QString getParamString() const { return str; }
        int getAction() const { return value; }
    private:
      int value;
      QString str;
    };

class textEvent : public QCustomEvent
    {public:
      textEvent( QString  txt)
          : QCustomEvent( TEXTEVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

#endif
