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

#include <qapplication.h>
//#include <TAXI/Tbase.h>
#include "MyWindow.h" 
#include "GraphWidget.h" 

#if QT_VERSION < 300
#undef QTextEdit
#include <qtextview.h>
#define QTextEdit QTextView
#else
#include <qtextedit.h>
#include <qtextbrowser.h>
#endif

#include <qtabwidget.h> 
#include <qstatusbar.h> 
#include <qcolor.h> 
#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <stdarg.h>
#include <stdio.h> //pour vsprintf !!!!
#include <iostream.h>

void DrawGraph(Graph &G) {}


static GraphContainer* gC;
static MyWindow* mw;
MyWindow* GetMyWindow()
  {return mw;}
void DefineMyWindow(MyWindow *father) 
  {mw = father;}
void DefineGraphContainer(GraphContainer *GC0)
  {gC = GC0;}
GraphContainer& GetMainGraph()
  {return *gC;}
void DrawGraph()
  {if(!mw)return;
  mw->gw->update();
  }
int Twait(const char *s)
  {if(!mw)return 0;
  int rep = QMessageBox::information (mw,"Wait",s,"","CANCEL","EXIT",0,0);
  if(rep == 2)mw->close();
  return (rep == 1) ? 0:1;
  }
void Tmessage(const char *string)
  {if(!mw)return;
  mw->Message(string);
  }
void Tclear()
  {if(!mw)return;
  mw->MessageClear();
  }
void Tprintf(const char *fmt,...)
  {if(!mw)return;
  va_list arg_ptr;
  char texte_print[200];
  va_start(arg_ptr,fmt);
  vsprintf(texte_print,fmt,arg_ptr);
  va_end(arg_ptr);
  mw->Message((const char *)texte_print);
  }
QString  getErrorString()
  {QString m;
  if(getErrorMsg() &&  strlen(getErrorMsg()))
      m.sprintf("Error:%d '%s' in %s:%d",getError(),getErrorMsg(),getErrorFile(),getErrorLine());
  else
      m.sprintf("Error:%d in %s:%d",getError(),getErrorFile(),getErrorLine());
  return m;
  }
bool & ShowOrientation()
  {static bool showorientation = false;
  return showorientation;
  }
int & ShowVertex()
// -1:index 0:label
  {static int _show = 0; 
  return _show;
  }
QColor OppCol(QColor & col)
  {int hue,sat,val;
  col.hsv(&hue,&sat,&val);
  val = (val <= 192) ? 255 : 0;
  col.setHsv((hue+180)%360,sat/2,val); 
  /*
  int r,g,b;
  col.rgb(&r,&g,&b);
  r = (r > 128) ? 0 : 255;
  g = (g > 128) ? 0 : 255;
  b = (b > 128) ? 0 : 255;
  col.setRgb(r,g,b);
  */
  return col;
  }
QColor Desaturate(QColor & col)
  {int hue,sat,val;
  col.hsv(&hue,&sat,&val);
  int val0 = 192;
  val = val0 +(int)(val*((double)(255.-val0)/255.));
  col.setHsv(hue,sat/4,val); 
  //col.setHsv((hue+180)%360,sat/4,val); 
  return col;
  }
//*************************************
void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);

static bool Equal(double x, double y);
static bool Less(double x, double y);
static int Cmp(int i,int j);
static double Epsilon = 2.;
static double *xcoord, *ycoord;

bool Equal(double x, double y)
  {if(fabs(x-y) <= Epsilon)return true;
  return false;
  }
bool Less(double x, double y)
  {if(!Equal(x,y) && x < y)return true;
  return false;
  }
int Cmp(int i,int j)
  {double ax = xcoord[i];
  double ay = ycoord[i];
  double bx = xcoord[j];
  double by = ycoord[j];
  if(Less(ax,bx))return 1;
  if(Equal(ax,bx) && Less(ay,by))return 1;
  return 0;
  }
bool CheckCoordNotOverlap(GeometricGraph & G)
  {bool ok = true;
  int n = G.nv();
  int *heap = new int[n+1];xcoord = new double[n+1];ycoord = new double[n+1];
  for(int i = 1;i <= n;i++)
      {xcoord[i] = G.vcoord[i].x();
      ycoord[i] = G.vcoord[i].y();
      }
  HeapSort (Cmp,1,n,heap);
  double pos,prevpos;
  prevpos = G.vcoord[heap[0] + 1].x();
  int i0 = 0;
  for(int i = 1;i < n;i++)
      {pos = G.vcoord[heap[i] + 1].x();
      if(Equal(pos,prevpos) &&  G.vcoord[heap[i] + 1].y() == G.vcoord[heap[i0] + 1].y())
	  {ok = false;   
	  break;
	  }
      prevpos = pos;
      i0 = i;
      }
  delete [] heap;delete [] xcoord;delete [] ycoord;
  return ok;
  }

struct OrderedBrin
{double angle;
  tbrin brin;
  OrderedBrin & operator=(const OrderedBrin &x)
      {angle=x.angle; brin=x.brin;return *this;}
};
static int compare(const void *b1, const void *b2)
  {if (((OrderedBrin *)b1)->angle > ((OrderedBrin *)b2)->angle) return -1;
  else if (((OrderedBrin *)b1)->angle == ((OrderedBrin *)b2)->angle) return 0;
  else return 1;
  }

void ComputeGeometricCir(GeometricGraph &G,svector<tbrin> &cir)
  {tvertex v;
  tbrin b0,b,opp;
  Tpoint p;
  int degree;
  
  for(v = 1;v <=G. nv();v++)
      {degree = G.Degree(v);
      svector<OrderedBrin> ob(degree);
      
      if((b0 = G.pbrin[v]) == 0)continue;
      // Put adjacent brins in the array.
      int i = 0; b = b0;
      p = G.vcoord[v];
      do
          {ob[i].brin = b;
          ob[i].angle = Angle(G.vcoord[G.vin[-b]]-p);
          b = G.cir[b]; i++;
          }while (b != b0);

      if(!i)continue;
      qsort(ob.begin(), degree, sizeof(OrderedBrin), compare);

      // rewrite cir which is COUNTERCLOCKWISE.
      for (i = 0; i < degree - 1; i++)
          cir[ob[i+1].brin] = ob[i].brin;
      cir[ob[0].brin] = ob[degree - 1].brin;
      }
  cir[0] = 0;
  }
