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
#define PROGRESS_EVENT 65438
#define BANNER_EVENT 65439

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

class bannerEvent : public QCustomEvent
    {public:
      bannerEvent( QString  txt)
          : QCustomEvent( BANNER_EVENT),str(txt)  {}
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
      handlerEvent( int action,int drawingType,int saveType)
          : QCustomEvent(HANDLER_EVENT ), _action( action ),_drawingType(drawingType),_saveType(saveType)  {}
        int getAction() const { return _action; }
        int getDrawingType() const { return _drawingType; }
        int getSaveType() const { return _saveType; }
    private:
      int _action;
      int _drawingType;
      int _saveType;
    };

class progressEvent : public QCustomEvent
/*
action == 0   init step
action == 1   show step
action == -1  close
*/
    {public:
      progressEvent( int action,int step = 0)
          : QCustomEvent(PROGRESS_EVENT ), _action( action ),_step(step)  {}
        int getAction() const { return _action; }
        int getStep() const { return _step; }
    private:
      int _action;
      int _step;
    };

#endif
