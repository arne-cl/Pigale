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

#define CLIENT_EVENT 65432
#define TEXT_EVENT 65433
#define CLEARTEXT_EVENT 65434
#define WAIT_EVENT 65435
#define DRAWG_EVENT 65436
#define HANDLER_EVENT 65437

class clientEvent : public QCustomEvent
    {public:
      clientEvent( int action,QString  param)
          : QCustomEvent(CLIENT_EVENT ), value( action ),str(param)  {}
       QString getParamString() const { return str; }
        int getAction() const { return value; }
    private:
      int value;
      QString str;
    };

class textEvent : public QCustomEvent
    {public:
      textEvent( QString  txt)
          : QCustomEvent( TEXT_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class waitEvent : public QCustomEvent
    {public:
      waitEvent( QString  txt)
          : QCustomEvent( WAIT_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class handlerEvent : public QCustomEvent
    {public:
      handlerEvent( int action)
          : QCustomEvent(HANDLER_EVENT ), value( action )  {}
        int getAction() const { return value; }
    private:
      int value;
    };
#endif
