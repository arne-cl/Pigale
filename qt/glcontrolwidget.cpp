
#include "pigaleWindow.h"
#include <TAXI/Tbase.h>
#include "glcontrolwidget.h"
#include <QGLFormat>
#include <QTimer>
#include <QMouseEvent>
#include <QT/Misc.h> 
/*! \file glcontrolwidget.cpp
\ingroup qt 
*/

GLControlWidget::GLControlWidget(QWidget *parent,QGLWidget *share)
//    : QGLWidget(QGLFormat(QGL::SampleBuffers),parent,share),
    : QGLWidget(parent,share),
  xRot(0),yRot(0),zRot(0),xTrans(0),yTrans(0),zTrans(-10.0),scale(5.0)
  ,animation(false), wasAnimated(false), delay(-1)
  {timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), SLOT(animate()) );
  setFocusPolicy(Qt::ClickFocus);
  }
void GLControlWidget::transform()
  {glTranslatef( xTrans, yTrans, zTrans );
  glScalef( scale, scale, scale );
  glRotatef( xRot, 1.0, 0.0, 0.0 );
  glRotatef( yRot, 0.0, 1.0, 0.0 );
  glRotatef( zRot, 0.0, 0.0, 1.0 );
  }
void GLControlWidget::setXRotation( double degrees )
  {xRot = (GLfloat)fmod(degrees, 360.0);
  updateGL();
  }
void GLControlWidget::setYRotation( double degrees )
  {yRot = (GLfloat)fmod(degrees, 360.0);
  updateGL();
  }
void GLControlWidget::setZRotation( double degrees )
  {zRot = (GLfloat)fmod(degrees, 360.0);
  updateGL();
  }
void GLControlWidget::setScale( double s )
  {scale = s;
  updateGL();
  }
void GLControlWidget::setXTrans( double x )
  {xTrans = x;
  updateGL();
  }
void GLControlWidget::setYTrans( double y )
  {yTrans = y;
  updateGL();
  }
void GLControlWidget::setZTrans( double z )
  {zTrans = z;
  updateGL();
  }
void GLControlWidget::setRotationImpulse( double x, double y, double z )
  {setXRotation( xRot + 180*x );
  setYRotation( yRot + 180*y );
  setZRotation( zRot - 180*z );
  }
void GLControlWidget::setTranslationImpulse( double x, double y, double z )
  {setXTrans( xTrans + 2*x );
  setYTrans( yTrans - 2*y );
  setZTrans( zTrans + 2*z );
  }
void GLControlWidget::mousePressEvent( QMouseEvent *e )
  {//e->accept();
  oldPos = e->pos();
  }
void GLControlWidget::mouseReleaseEvent( QMouseEvent *e )
  {//e->accept();
  oldPos = e->pos();
  }
void GLControlWidget::mouseMoveEvent( QMouseEvent *e )
  {//e->accept();
  double dx = e->x() - oldPos.x();
  double dy = e->y() - oldPos.y();
  oldPos = e->pos();
  double rx = dx / width();
  double ry = dy / height();
// Meta = Control
// Control = Command  
  bool None      =  e->modifiers() == Qt::NoModifier;
  bool Shift     =  e->modifiers() == Qt::ShiftModifier;
  bool Control   =  e->modifiers() == Qt::ControlModifier;
  bool Alt       =  e->modifiers() == Qt::AltModifier;
  //bool Meta      =  e->modifiers() == Qt::MetaModifier;
  //bool ShiftAlt  =  e->modifiers() == Qt::AltModifier+Qt::ShiftModifier;
  
	if(None)
		{if((e->buttons() == Qt::LeftButton))
			setRotationImpulse(ry,rx,0);
		else if(e->buttons() == Qt::RightButton) //== Contro+left
			setRotationImpulse(0,ry,rx);
       	else if((e->buttons() == Qt::MiddleButton))
			setRotationImpulse(ry,0,rx);
		}
	else if(Alt) 
    	setRotationImpulse(ry,0,rx);
	else if(Shift)
		{if((e->buttons() == Qt::LeftButton))
			setTranslationImpulse(rx,ry,0);
      	else if((e->buttons() == Qt::RightButton))
        	setTranslationImpulse(rx,0,ry);
      	else if(e->buttons() == Qt::MiddleButton)
        	setTranslationImpulse(rx,ry,0 );
    	}
    else if(Control) // Command
        setTranslationImpulse(0,rx,ry);
    else
    	return;
  }
void GLControlWidget::wheelEvent(QWheelEvent *e)
  {e->accept();
  if(scale <= ((double)e->delta()/1000))
      return;
  setScale(scale - ((double)e->delta()/1000));
  }
void GLControlWidget::keyPressEvent( QKeyEvent *k )
  {if(k->key() == Qt::Key_Up)
      {if(scale > 10.)return;
      scale *= (float)1.1;
      }
  else if(k->key() == Qt::Key_Down)
      {if(scale < .1)return;
      scale /= (float)1.1;
      }
  setScale(scale); 
  }
void GLControlWidget::mouseDoubleClickEvent(QMouseEvent *)
  {if(delay <= 0)return;
  animation = !animation;
  if(animation)
      timer->start(delay);
  else
      timer->stop();
  }
void GLControlWidget::showEvent(QShowEvent *e)
  {if(wasAnimated && !timer->isActive())
      timer->start(delay);
  QGLWidget::showEvent(e);
  }
void GLControlWidget::hideEvent(QHideEvent *e)
  {wasAnimated = timer->isActive();
  timer->stop();
  QGLWidget::hideEvent(e);
  }
void GLControlWidget::animate()
{}
void GLControlWidget::setAnimationDelay(int ms)
  {timer->stop();
  delay = ms;
  if(delay < 0)
      {wasAnimated = animation = false;return;}
  animation = true;
  wasAnimated = true;
  timer->start(delay);
  }
