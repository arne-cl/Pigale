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

#include <LEDA/panel.h>
#include <LEDA/ps_file.h>


class Interface {
public:
  virtual void init(double,double,double)=0;
  virtual void draw_box(double,double,double,double,short)=0;
  virtual void draw_rectangle(double,double,double,double,short)=0;
  virtual void draw_segment(double,double,double,double,short)=0;
  virtual void draw_ctext(double,double,char *,short)=0;
};
class InterfaceWindow : public Interface {
  leda_window *p;
public:
  InterfaceWindow(double x, double y,char *title) {p= new leda_window(x,y,title);}
  InterfaceWindow(leda_window *q) {p=q;}
  void init(double a,double b ,double c ) {p->init(a,b,c);}
  void draw_box(double a,double b ,double c,double d,short e) {p->draw_box(a,b,c,d,e);}
  void draw_rectangle(double a,double b ,double c,double d,short e) {p->draw_rectangle(a,b,c,d,e);}
  void draw_segment(double a,double b ,double c,double d,short e) {p->draw_segment(a,b,c,d,e);}
  void draw_ctext(double a,double b ,char *c,short e) {p->draw_ctext(a,b,c,e);}
};
class InterfacePS : public Interface {
  ps_file *p;
public:
  InterfacePS(double x, double y,char *title) {p= new ps_file(x,y,title);}
  InterfacePS(ps_file *q) {p=q;}
  void init(double a,double b ,double c ) {p->init(a,b,c);}
  void draw_box(double a,double b ,double c,double d,short e) {p->draw_box(a,b,c,d,e);}
  void draw_rectangle(double a,double b ,double c,double d,short e) {p->draw_rectangle(a,b,c,d,e);}
  void draw_segment(double a,double b ,double c,double d,short e) {p->draw_segment(a,b,c,d,e);}
  void draw_ctext(double a,double b ,char *c,short e) {p->draw_ctext(a,b,c,e);}
}; 

