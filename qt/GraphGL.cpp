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

#include "MyWindow.h"
#include "GraphGL.h"
#include "glcontrolwidget.h"
#include "mouse_actions.h"
#include <QT/GLWindow.h>
#include <QT/MyQcolors.h>
#include <TAXI/netcut.h>
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <GL/glut.h>

//in QtMisc.cpp
int & ShowVertex();

class GraphGLPrivate
{public:
  GraphGLPrivate()
      {is_init = false;
      isHidden = true;
      pGG = 0;
      editor = 0;
      idelay = 1;
      edge_width = 2.;  // -> *2 
      vertex_width = 1.;// -> *5
      }
  ~GraphGLPrivate()
      {delete editor;}
  bool is_init; 
  bool isHidden; 
  int delay;
  int idelay;
  bool rX,rY,rZ;
  double edge_width;
  double vertex_width;
  QCheckBox* bt_facet;
  QCheckBox* bt_label;
  QCheckBox* bt_color;
  MyWindow *mywindow;
  GeometricGraph* pGG;
  GraphContainer *pGC;
  EmbedRnGraph   *pSG;
  GraphGL  *GL;
  GLWindow *editor;
};
//*****************************************************
GraphGL::GraphGL(QWidget *parent,const char *name,MyWindow *mywindow)
    : QWidget( parent, name )
  {d = new GraphGLPrivate;
  d->pGC = new GraphContainer;
  d->mywindow = mywindow;
  d->GL = this;
  d->delay = -1;
  d->rX = d->rY = false;
  d->rZ = true;
  }
GraphGL::~GraphGL()
  {delete d->pGG; delete d;}
int GraphGL::update()
  {if(!d->is_init)
      {QVBoxLayout* vb = new QVBoxLayout(this,2,0);
      d->editor = new GLWindow(d,this);
      vb->addWidget(d->editor);
      QHBoxLayout* hb = new QHBoxLayout(vb);
      QHButtonGroup* bt_group = new QHButtonGroup(this);
      bt_group->setFrameShape(QFrame::NoFrame); 
      bt_group->setMaximumHeight(45);
      d->bt_facet = new QCheckBox("Facet",this);
      d->bt_facet->setChecked(false);
      d->bt_label = new QCheckBox("Label",this);
      d->bt_label->setChecked(false);
      d->bt_color = new QCheckBox("Color",this);
      d->bt_color->setChecked(false);
      hb->addWidget(d->bt_facet);
      hb->addWidget(d->bt_label);
      hb->addWidget(d->bt_color);
      //hb->addWidget(bt_group);
      QSpinBox *spin_Edge = new QSpinBox(1,10,1,this,"spinEdge");
      spin_Edge->setValue((int)d->edge_width*2);     spin_Edge->setPrefix("Ew: ");
      QSpinBox *spin_Vertex = new QSpinBox(1,20,1,this,"spinVertex");
      spin_Vertex->setValue((int)d->vertex_width*5);     spin_Vertex->setPrefix("Vw: ");
      hb->addWidget(spin_Edge);hb->addWidget(spin_Vertex);
      hb->addWidget(bt_group);
      QRadioButton* rb_x = new QRadioButton("X",bt_group,"x");
      QRadioButton* rb_y = new QRadioButton("Y",bt_group,"y");
      QRadioButton* rb_z = new QRadioButton("Z",bt_group,"z");
      rb_x->setChecked(FALSE);
      rb_y->setChecked(FALSE);
      rb_z->setChecked(TRUE);
      spin_X = new QSpinBox(1,100,1,this,"spinX");
      spin_Y = new QSpinBox(1,100,1,this,"spinY");
      spin_Z = new QSpinBox(1,100,1,this,"spinZ");
      spin_X->setValue(1);     spin_X->setPrefix("X: ");
      spin_Y->setValue(2);     spin_Y->setPrefix("Y: ");
      spin_Z->setValue(3);     spin_Z->setPrefix("Z: ");
      hb->addWidget(spin_X); hb->addWidget(spin_Y); hb->addWidget(spin_Z);
      d->is_init = true;
      connect(spin_Edge,SIGNAL(valueChanged(int)),SLOT(EdgeWidth(int)));
      connect(spin_Vertex,SIGNAL(valueChanged(int)),SLOT(VertexWidth(int)));
      connect(spin_X,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(spin_Y,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(spin_Z,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(d->bt_facet,SIGNAL(clicked()),SLOT(Reload()));
      connect(d->bt_color,SIGNAL(clicked()),SLOT(Reload()));
      connect(d->bt_label,SIGNAL(clicked()),SLOT(Reload()));
      connect(bt_group,SIGNAL(clicked(int)),SLOT(axisChanged(int)));
      }
  else
      {delete d->pSG; delete d->pGG;}
  
  //Copy the graph
  *(d->pGC) = d->mywindow->GC;
  d->pGG = new GeometricGraph(*(d->pGC));

  //Compute the embedding
  GeometricGraph & GeoG = *(d->pGG);
  if(GeoG.nv() < 3 || GeoG.ne() < 2)return -1;
  d->bt_facet->setEnabled(GeoG.TestPlanar());
  d->pSG = new EmbedRnGraph(GeoG);
  if(!d->pSG->ok){Tprintf("DIAG ERROR (Complete Graph?)");return -1;}
  d->mywindow->tabWidget->showPage(this);
#if QT_VERSION < 300
  d->mywindow->tabWidget->changeTab(this,"3-d Embedding");
#else
  d->mywindow->tabWidget->setTabLabel(this,"3-d Embedding");
#endif
  spin_X->setValue(1);  spin_Y->setValue(2);  spin_Z->setValue(3);
  spin_X->setMaxValue(GeoG.nv()-1);  spin_Y->setMaxValue(GeoG.nv()-1);  spin_Z->setMaxValue(GeoG.nv()-1);
  Reload(1);
  return 0;
  }
void GraphGL:: EdgeWidth(int i)
  {d->edge_width = i /2.;
  d->editor->initialize(false);
  }
void GraphGL:: VertexWidth(int i)
  {d->vertex_width = i/5.;
  d->editor->initialize(false);
  }
void GraphGL::Reload()
  {//if Qt < 3.0 Reload(int i =0) does not work
  d->editor->initialize(false);
  }
void GraphGL::Reload(int i)
  {if(i == 0){d->editor->initialize(false);return;}
  GeometricGraph & GeoG = *(d->pGG);
  int i1,i2,i3;
  i1 = spin_X->value(); i1 = Min(i1,GeoG.nv()-1);
  i2 = spin_Y->value(); i2 = Min(i2,GeoG.nv()-1);
  i3 = spin_Z->value(); i3 = Min(i3,GeoG.nv()-1);
  // Compute min,max on three coord;
  double min1,min2,min3,max1,max2,max3;
  min1 = max1 = d->pSG->Coords[1][i1];
  min2 = max2 = d->pSG->Coords[1][i2];
  min3 = max3 = d->pSG->Coords[1][i3];
  
  for(int i = 2;i <= d->pSG->nv();i++)
      {min1 = Min(min1, d->pSG->Coords[i][i1]); max1 = Max(max1,d->pSG->Coords[i][i1]);
       min2 = Min(min2, d->pSG->Coords[i][i2]); max2 = Max(max2,d->pSG->Coords[i][i2]);
       min3 = Min(min3, d->pSG->Coords[i][i3]); max3 = Max(max3,d->pSG->Coords[i][i3]);
      }
  double min123 = min1; min123 = Min(min123,min2);min123 = Min(min123,min3);
  double max123 = max1; max123 = Max(max123,max2);max123 = Max(max123,max3);
  double alpha = Max(-min123,max123);
  alpha = 1./alpha;
  Prop<Tpoint3> Coord3(GeoG.Set(tvertex()),PROP_COORD3);
  for(int i = 1;i <= GeoG.nv();i++)
      {Coord3[i].x() = alpha * d->pSG->Coords[i][i1];
       Coord3[i].y() = alpha * d->pSG->Coords[i][i2];
       Coord3[i].z() = alpha * d->pSG->Coords[i][i3];
      }
  d->editor->initialize(true);
  }
void GraphGL::resizeEvent(QResizeEvent* e)
  {if(d->editor)d->editor->initialize(false);
  QWidget::resizeEvent(e);
  }
void GraphGL::delayChanged(int i)
  {if(!d->is_init || d->isHidden)return;
  d->idelay = i;
  d->delay = (d->idelay <= 2) ? -1 : 101 - d->idelay;
  d->editor->setAnimationDelay(d->delay);
  }
void GraphGL::axisChanged(int i)
  {if(i == 0)    // Rotate around X
      {d->rX = true;d->rY = d->rZ = false;}
  else if(i == 1)// Rotate around Y
      {d->rY = true;d->rX = d->rZ = false;}
  else           // Rotate around Z
      {d->rZ = true;d->rX = d->rY = false;}
  }
//*********************************************************

GLWindow::GLWindow(GraphGLPrivate *g,QWidget * parent,const char * name)
    : GLControlWidget(parent,name)
    ,glp(g)
  {is_init = false;
  object = 0;
  setAnimationDelay(-1);
  xRot = yRot = zRot = 0.0;
  scale = 1.;	
  }
GLWindow::~GLWindow()
  {makeCurrent();
  glDeleteLists(object,1);
  }
void GLWindow::initializeGL()
  {setAutoBufferSwap(true); 
  glDepthFunc(GL_LESS);//hide features
  glEnable(GL_DEPTH_TEST);//hide features
  glEnable(GL_LINE_SMOOTH);//pas grand chose
  qglClearColor(color[Black]);
  glShadeModel(GL_FLAT);
  //fog
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY,.15);
  glFogf(GL_FOG_START,8.);  glFogf(GL_FOG_END,12.);
  GLfloat fog_color[] = {0.25, 0.25, 0.25, 1.0};
  glFogfv(GL_FOG_COLOR,fog_color);
  CharSize = glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char *)"0");
  }
void GLWindow::initialize(bool init) 
  {if(!is_init){is_init=true;initializeGL();return;}
  if(object){makeCurrent();glDeleteLists(object,1);}
  object = load(init);
  updateGL();
  }
GLuint GLWindow::load(bool init)
  {GLfloat ds = (1./40.)*glp->vertex_width;
  if(init)
      {xRot = yRot = zRot = 0.0;
      xTrans = yTrans = 0.0;
      zTrans = -10.0;
      scale = 1.41;
      }
  GeometricGraph & G = *(glp->pGG);
  Prop<Tpoint3> Coord3(G.Set(tvertex()),PROP_COORD3);
  GLuint list;
  list = glGenLists(1);
  glNewList(list,GL_COMPILE);

  bool WithFaces = G.TestPlanar() && glp->bt_facet->isChecked();
  glLineWidth(glp->edge_width);
  glBegin(GL_LINES);
  tvertex v0,v1;
  GLfloat x0,y0,z0,x1,y1,z1;
  for(tedge e = 1;e <= G.ne();e++)
      {v0 = G.vin[e]; v1 = G.vin[-e];
      x0 = (GLfloat)Coord3[v0].x();y0 = (GLfloat)Coord3[v0].y();z0 = (GLfloat)Coord3[v0].z();
      x1 = (GLfloat)Coord3[v1].x();y1 = (GLfloat)Coord3[v1].y();z1 = (GLfloat)Coord3[v1].z();

      if(G.ecolor[e] == Black && WithFaces)glColor3f(.5,.5,.5);
      else if(G.ecolor[e] == Black)glColor3f(1.,1.,.8);
      else qglColor(color[G.ecolor[e]]);
      glVertex3f(x0,y0,z0);      glVertex3f(x1,y1,z1);
      }
  glEnd();
  glColor3f(1.,1.,.0);
  if(WithFaces)loadFaces();

  drawCube(.0,.0,.0, .5*ds);
  if(glp->bt_color->isChecked())
      for(tvertex  v = 1;v <= G.nv();v++)
	  drawCube((GLfloat)Coord3[v].x(),(GLfloat)Coord3[v].y(),(GLfloat)Coord3[v].z(),ds
		   ,color[G.vcolor[v]]);
  else
      for(tvertex  v = 1;v <= G.nv();v++)
	  drawCube((GLfloat)Coord3[v].x(),(GLfloat)Coord3[v].y(),(GLfloat)Coord3[v].z(),ds);
  
  if(glp->bt_label->isChecked())
      { glLineWidth(1.0);
      for(tvertex  v = 1;v <= G.nv();v++)
	  {if(ShowVertex() == -1)
	      drawInt(v(),(GLfloat)Coord3[v].x(),(GLfloat)Coord3[v].y(),(GLfloat)Coord3[v].z(),ds);
	  else
	      drawInt((int)G.vlabel[v],(GLfloat)Coord3[v].x(),(GLfloat)Coord3[v].y()
		      ,(GLfloat)Coord3[v].z(),ds);
	  }
      }

 glLineWidth(1.0);
 glBegin(GL_LINES);
 qglColor(red);   glVertex3f(.0,.0,.0);   glVertex3f(.25,0.,0.);
 qglColor(green); glVertex3f(.0,.0,.0);   glVertex3f(.0,.25,0.);
 qglColor(blue);  glVertex3f(.0,.0,.0);   glVertex3f(.0,0.,.25);
 glEnd();
 glEndList();
 return list;
  }
void GLWindow::loadFaces()
  {GeometricGraph & G = *(glp->pGG);
  Prop<Tpoint3> Coord3(G.Set(tvertex()),PROP_COORD3);
  int m = G.ne();
  G.ZigZagTriangulate();
  svector<tbrin> & Fpbrin = G.ComputeFpbrin();
  GLfloat x1,y1,z1,x2,y2,z2,x3,y3,z3;
  tbrin b;
  for (int i=1; i <= Fpbrin.n(); i++)
      {if(G.FaceWalkLength(Fpbrin[i]) != 3)continue;
      // add a triangle
      b = Fpbrin[i];
      x1 = (GLfloat)Coord3[G.vin[b]].x();
      y1 = (GLfloat)Coord3[G.vin[b]].y();
      z1 = (GLfloat)Coord3[G.vin[b]].z();
      b = -b;
      x2 = (GLfloat)Coord3[G.vin[b]].x();
      y2 = (GLfloat)Coord3[G.vin[b]].y();
      z2 = (GLfloat)Coord3[G.vin[b]].z();
      b = -G.cir[b];
      x3 = (GLfloat)Coord3[G.vin[b]].x();
      y3 = (GLfloat)Coord3[G.vin[b]].y();
      z3 = (GLfloat)Coord3[G.vin[b]].z();
      
      glBegin(GL_TRIANGLES);
      glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2); glVertex3f(x3,y3,z3);
      glEnd();
      }
  delete &Fpbrin;
  for(tedge e = G.ne();e >m;e--)
      G.DeleteEdge(e);
  }
void GLWindow::setAnimationDelay(int ms)
  {GLControlWidget::setAnimationDelay(ms);
  }
void GLWindow::animate()
  {
  if(glp->rX)xRot -= 1.5;
  if(glp->rY)yRot -= 1.5;
  if(glp->rZ)zRot -= 1.5;
  updateGL();
  }
void GLWindow::paintGL()
  {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//hide features
  if(!object)return;
  glLoadIdentity();
  transform();
  glCallList(object);
  }
void GLWindow::paintEvent(QPaintEvent *e)
  {//initialize the QLWindow
  if(!is_init){is_init = true;initialize(true);}
  QGLWidget::paintEvent(e);
  } 
void GLWindow::hideEvent(QHideEvent*)
  {
  glp->isHidden = true;
  }
void GLWindow::showEvent(QShowEvent*)
  {if(glp->isHidden)
      {glp->mywindow->mouse_actions->LCDNumber->display(glp->idelay);
      glp->mywindow->mouse_actions->Slider->setValue(glp->idelay);
      }
  glp->isHidden = false;
  }
QSize GLWindow::sizeHint() const
  {return QSize(glp->GL->width(),glp->GL->height());
  }
void GLWindow::resizeGL(int w,int h)
  {glViewport(0,0,(GLint)w,(GLint)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //left,right,bottom,top,near,far
  glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
  glMatrixMode(GL_MODELVIEW);
  }
void GLWindow::drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size)
  {GLfloat sat = .1  ;
  glColor3f(sat,sat,1.);
  glBegin(GL_POLYGON);//bas
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x+size,y+size,z-size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  glColor3f(sat,sat,.7);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glEnd();
  glColor3f(1.,sat,sat);//sides
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x-size,y+size,z-size);
  glEnd();
  glColor3f(.7,sat,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x+size,y-size,z-size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  glColor3f(sat,1.,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  glColor3f(sat,.7,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  }
void GLWindow::drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size,QColor &col)
  {int r,g,b;
  col.rgb(&r,&g,&b);
  GLfloat xr = (GLfloat)r/255.;
  GLfloat xg = (GLfloat)g/255.;
  GLfloat xb = (GLfloat)b/255.;
  GLfloat sat =1.;
  glColor3f(xr*sat,xg*sat,xb*sat);
  //qglColor(col);
  glBegin(GL_POLYGON);//bas
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x+size,y+size,z-size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  sat = 1.;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glEnd();
  sat = .5;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x-size,y+size,z-size);
  glEnd();
  sat =.5;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x+size,y-size,z-size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  sat = .8;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  sat =.8;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  }
void GLWindow::drawInt(int vlabel,GLfloat x,GLfloat y,GLfloat z,GLfloat size)
  {QString t;
  t.sprintf("%2.2d",vlabel);
  int len = glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char *)((const char *)t));
  qglColor(red);
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y-size*.8,z+size*1.01);
  //xm=.003
  //double xm = .9*size/CharSize;
  double xm = .9*size*2/len;
  glScalef(xm,xm*1.2,1);
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();

  //Second cube face
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y+size*.8,z-size*1.01);
  glScalef(xm,xm*1.2,1);
  glRotatef(180., 1.0, 0.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();
  
  //Third cube face
  qglColor(blue);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*1.01,y-size*.8,z-size*.85);
  glScalef(xm,xm*1.2,xm);
  glRotatef(-90., 0.0, 1.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Fourth cube face
  qglColor(blue);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x+size*1.01,y-size*.8,z+size*.85);
  glScalef(xm,xm*1.2,xm);
  glRotatef(90., 0.0, 1.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Fith cube face
  qglColor(black);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y-size*1.1,z-size*.8);
  glScalef(xm,xm,1.2*xm);
  glRotatef(90., 1.0, 0.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Sith cube face
  qglColor(yellow);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y+size*1.1,z+size*.8);
  glScalef(xm,xm,1.2*xm);
  glRotatef(-90., 1.0, 0.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t);
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();
  }
void GLWindow::drawText(void * font,const char *txt)
  {for(int i = 0; txt[i];i++)
      glutStrokeCharacter(font,txt[i]);
  }
