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

#if QT_VERSION >= 0x4000

#include <qevent.h> 
#define USER_EVENT  QEvent::User
#else
#define USER_EVENT  65432
#endif


enum Event {
	CLIENT_EVENT = USER_EVENT,
	TEXT_EVENT,
	WRITE_EVENT,
	WRITEB_EVENT,
	CLEARTEXT_EVENT,
	WAIT_EVENT,
	DRAWG_EVENT,
	HANDLER_EVENT,
	PROGRESS_EVENT,
	BANNER_EVENT
};

#ifdef _QT4_
class clientEvent : public QEvent
    {public:
      clientEvent( int action,QString  param)
          :  QEvent(QEvent::User),value( action ),str(param)
          {t = CLIENT_EVENT;}
      QString getParamString() const { return str; }
      int getAction() const { return value; }
    private:
      int value;
      QString str;
    };

class textEvent : public QEvent
    {public:
      textEvent( QString  txt)
          : QEvent(QEvent::User),str(txt)  
          {t=TEXT_EVENT;}
      QString getString() const { return str; }
    private:
      QString str;
    };

class writeEvent : public QEvent
    {public:
      writeEvent( QString  txt)
          : QEvent(QEvent::User),str(txt)  
          {t = WRITE_EVENT;}
      QString getString() const { return str; }
    private:
      QString str;
    };

class writeBufEvent : public QEvent
    {public:
      writeBufEvent(char * _ptr,uint _size)
          : QEvent(QEvent::User),ptr(_ptr),size(_size)  
          {t = WRITEB_EVENT;}
      char * getPtr() const { return ptr; }
      uint   getSize() const { return size; }
    private:
      char *ptr;
      uint size;
    };

class bannerEvent : public QEvent
    {public:
      bannerEvent( QString  txt)
          : QEvent(QEvent::User),str(txt)  
          {t = BANNER_EVENT;}
      QString getString() const { return str; }
    private:
      QString str;
    };

class waitEvent : public QEvent
    {public:
      waitEvent( QString  txt)
          : QEvent(QEvent::User),str(txt)  
          {t = WAIT_EVENT;}
      QString getString() const { return str; }
    private:
      QString str;
    };

class handlerEvent : public QEvent
    {public:
      handlerEvent( int action,int drawingType,int saveType)
          :  QEvent(QEvent::User),_action( action ),_drawingType(drawingType),_saveType(saveType)  
          {t=HANDLER_EVENT;}
      int type(){return HANDLER_EVENT;}
      int getAction() const { return _action; }
      int getDrawingType() const { return _drawingType; }
      int getSaveType() const { return _saveType; }
    private:
      int _action;
      int _drawingType;
      int _saveType;
    };

class progressEvent : public QEvent
/*
action == 0   init step
action == 1   show step
action == -1  close
*/
    {public:
      progressEvent( int action,int step = 0)
          :  QEvent(QEvent::User),_action( action ),_step(step)  
          {t = PROGRESS_EVENT;}
      int getAction() const { return _action; }
      int getStep() const { return _step; }
    private:
      int _action;
      int _step;
    };
#else

class clientEvent : public QCustomEvent
    {public:
      clientEvent( int action,QString  param)
          : QCustomEvent(CLIENT_EVENT), value( action ),str(param)  {}
       QString getParamString() const { return str; }
        int getAction() const { return value; }
    private:
      int value;
      QString str;
    };

class textEvent : public QCustomEvent
    {public:
      textEvent( QString  txt)
          : QCustomEvent(TEXT_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class writeEvent : public QCustomEvent
    {public:
      writeEvent( QString  txt)
          : QCustomEvent(WRITE_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class writeBufEvent : public QCustomEvent
    {public:
      writeBufEvent(char * _ptr,uint _size)
          : QCustomEvent(WRITEB_EVENT),ptr(_ptr),size(_size)  {}
       char * getPtr() const { return ptr; }
       uint   getSize() const { return size; }
    private:
      char *ptr;
      uint size;
    };

class bannerEvent : public QCustomEvent
    {public:
      bannerEvent( QString  txt)
          : QCustomEvent(BANNER_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class waitEvent : public QCustomEvent
    {public:
      waitEvent( QString  txt)
          : QCustomEvent(WAIT_EVENT),str(txt)  {}
       QString getString() const { return str; }
    private:
      QString str;
    };

class handlerEvent : public QCustomEvent
    {public:
      handlerEvent( int action,int drawingType,int saveType)
          : QCustomEvent(HANDLER_EVENT), _action( action ),_drawingType(drawingType),_saveType(saveType)  {}
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
#endif



