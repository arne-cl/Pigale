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

#include "gprop.h"
#include "MyWindow.h"
#include <TAXI/graphs.h>
#include <TAXI/Tmessage.h>
#include <QT/Misc.h>

#include <qvariant.h>   // first for gcc 2.7.2
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qmenubar.h>

int TestOuterPlanar(TopologicalGraph &G);

Graph_Properties::Graph_Properties(QWidget* parent,QMenuBar *menubar
				   ,const char* name,WFlags fl)
    : QWidget( parent,name,fl)
  {if(!name)setName("Graph_Properties");
  //resize(227,166); //166
  menu = menubar;
  //QFont font = QFont("helvetica",12);
  QFont fnt = this->font();
  fnt.setBold(true);
  setFont(fnt,true);
  // 10 left margin,0 top margin,200 width (total width=220),165 hight 
  //this->setGeometry(QRect(10,0,200,185)); 
  this->setMinimumSize(QSize(220,180));
  this->setMaximumSize(QSize(220,180));
  //parent,0 marge,0 spacing,name
  MainLayout = new QVBoxLayout(this,0,0,"MainLayout"); 

  QHBoxLayout *TxtLayout = new QHBoxLayout(0,5,0,"TxtLayout"); 
  MainLayout->addLayout(TxtLayout);
  TextLabel3 = new QLabel(this,"TextLabel3");
  TextLabel3->setText("Graph Properties");
  TextLabel3->setAlignment(int(QLabel::AlignCenter));
  TxtLayout->addWidget(TextLabel3);

  Layout_NM = new QHBoxLayout(0,0,0,"Layout_NM"); 

  TextLabel1 = new QLabel(this,"TextLabel1");
  TextLabel1->setText("N:");
  Layout_NM->addWidget(TextLabel1);

  LE_N = new QLineEdit(this,"LE_N");
  LE_N->setMaximumSize(QSize(60,32767));
  LE_N->setText("0");
  Layout_NM->addWidget(LE_N);

  TextLabel1_2 = new QLabel(this,"TextLabel1_2");
  TextLabel1_2->setText("M:");
  Layout_NM->addWidget(TextLabel1_2);

  LE_M = new QLineEdit(this,"LE_M");
  LE_M->setMaximumSize(QSize(60,32767));
  LE_M->setText("0");
  Layout_NM->addWidget(LE_M);
  MainLayout->addLayout(Layout_NM);

  Layout_CP = new QHBoxLayout(0,10,12,"Layout_CP"); //marge,space entre deux groupes
  //Layout_C  = new QVBoxLayout(0,12,-6,"Layout_C"); 
  Layout_C  = new QVBoxLayout(0,0,0,"Layout_C"); 

  RBConnected = new RoRadioButton(this,"RBConnected");
  RBConnected->setText("Connected");
  RBConnected->setChecked(TRUE);
  Layout_C->addWidget(RBConnected);

  RB2Connected = new RoRadioButton(this,"RB2Connected");
  RB2Connected->setText("2-Connected");
  Layout_C->addWidget(RB2Connected);

  RB3Connected = new RoRadioButton(this,"RB3Connected");
  RB3Connected->setText("3-Connected");
  Layout_C->addWidget(RB3Connected);

  RBBipartite = new RoRadioButton(this,"RBBipartite");
  RBBipartite->setText("Bipartite");
  Layout_C->addWidget(RBBipartite);

  RBRegular = new RoRadioButton(this,"RBRegular");
  RBRegular->setText("Regular");
  Layout_C->addWidget(RBRegular);

  Layout_CP->addLayout(Layout_C);
  //********************************************

  LayoutP = new QVBoxLayout(0,0,0,"LayoutP"); 

  RBPlanar = new RoRadioButton(this,"RBPlanar");
  RBPlanar->setText("Planar");
  LayoutP->addWidget(RBPlanar);

  RBMxPlanar = new RoRadioButton(this,"RBMxPlanar");
  RBMxPlanar->setText("Triangulation");
  LayoutP->addWidget(RBMxPlanar);

  RBOuPlanar = new RoRadioButton(this,"RBOuPlanar");
  RBOuPlanar->setText("Out. planar");
  LayoutP->addWidget(RBOuPlanar);

  RBSerie = new RoRadioButton(this,"RBSerie");
  RBSerie->setText("Series-//");
  LayoutP->addWidget(RBSerie);

  RBSimple = new RoRadioButton(this,"Simple");
  RBSimple->setText("Simple");
  LayoutP->addWidget(RBSimple);

  Layout_CP->addLayout(LayoutP);
  MainLayout->addLayout(Layout_CP);

  LE_M->setReadOnly(true);
  LE_N->setReadOnly(true);
  }

RoRadioButton::RoRadioButton(QWidget * parent,const char * name = 0)
    :QRadioButton(parent,name)
  {}
void RoRadioButton::mousePressEvent(QMouseEvent* e)
  {
#if QT_VERSION >= 300
  e->accept();
#endif
  }
void RoRadioButton::mouseReleaseEvent(QMouseEvent* e)
  {
#if QT_VERSION >= 300
  e->accept();
#endif
  }
Graph_Properties::~Graph_Properties()
  { }
void Graph_Properties::MaxNSlowChanged(int i)
  {MaxNSlow = i;update();
  }
void Graph_Properties::MaxNDisplayChanged(int i)
  {MaxNDisplay = i;update();
  }
void Graph_Properties::update()
  {GeometricGraph G(GetMainGraph());
  int ns,nt;
  bool S = G.CheckSimple();
  bool P = G.CheckPlanar();
  bool SMALL = (G.nv() < 3) ? true : false;
  bool M = (!SMALL  && (G.ne() == 3*G.nv() - 6)) ? true : false;
  bool T = (P && S && M) ? true : false;     //Triangulation
  bool A = G.CheckAcyclic(ns,nt);
  bool B = G.CheckBipartite();
  int dmin,dmax;  G.MinMaxDegree(dmin,dmax);
  bool R = (dmin == dmax) ? true :false;
  bool C1,C2,C3;
  C1 = C2 = C3 = false;
  bool Outer = false;
  bool Serie = false;
 
  bool H = G.Set().exist(PROP_HYPERGRAPH);
  bool E;
  bool MB = P && B && S && (G.ne() == (2*G.nv() - 4));
 
  if(S && G.nv() == 2 && G.ne() == 1) //One edge graph
      C1 = true;
  if(T && G.nv() == 3)
      C1 = C2 = true;
  else if(T && G.nv() > 3) // MaxPlanar
      C3 = true;
  else if(P && S && !M && dmin > 2  && G.CheckTriconnected())
      C3 = true;
  else if(!T)
      {if(G.CheckBiconnected())
	  C1 = C2 = true;
      else if(G.CheckConnected())
	  C1 = true;
      }
  if(P && C2 && !C3)
      {Serie = G.CheckSerieParallel();
      if(Serie && dmin == 2 && TestOuterPlanar(G))Outer = true;
      }
  else if(G.nv() == 2 && G.ne() == 1)
      Serie = Outer = true;
  if(C3)C2 = true;
  if(C2)C1 = true;

  RBSimple->setChecked(S);
  RBPlanar->setChecked(P);
  RBMxPlanar->setChecked(T);
  RBBipartite->setChecked(B);
  RBRegular->setChecked(R);
  RBConnected->setChecked(C1);
  RB2Connected->setChecked(C2);
  if(!P)RB3Connected->setEnabled(false);
  else  {RB3Connected->setEnabled(true);RB3Connected->setChecked(C3);}
  RBOuPlanar->setChecked(Outer);
  RBSerie->setChecked(Serie);
  QString m;
  m.sprintf("%d",G.nv());
  LE_N->setText(m);
  m.sprintf("%d",G.ne());
  LE_M->setText(m);
  //Modify the enable menus
  //For slow programs or display
  bool NotBigS = (G.nv() > MaxNSlow ) ? false : true;
  bool NotBigD = (G.nv() > MaxNDisplay) ? false : true;
  menu->setItemEnabled(298,!SMALL && NotBigD);                     //Rn embedding
  menu->setItemEnabled(609,!SMALL && NotBigS);                     //sym
  menu->setItemEnabled(606,!SMALL && NotBigS);                     //partition
  menu->setItemEnabled(603,!P && NotBigS && S);                    //maxplanar
  menu->setItemEnabled(604,!P && NotBigS && S);                    //maxplanar
  menu->setItemEnabled(252,C1 && NotBigD);                         //polair

  //Augment
  menu->setItemEnabled(101,(G.nv() > 1) && !C1);                   //make connected 
  menu->setItemEnabled(102,(G.nv() > 1) && P && !C2);              //make 2 connected
  menu->setItemEnabled(103,!SMALL && P && !C2);                    //make 2 connected opt
  menu->setItemEnabled(104,(G.nv() > 1) && !C2);                   //make 2 connected NP  
  menu->setItemEnabled(105,!SMALL && P && S && !T);                //vertex triangulate
  menu->setItemEnabled(106,(G.nv() > 1) && P && S && !T);          //ZigZag 
  menu->setItemEnabled(107,(G.nv() > 1) && P && C3 && !T);         //Tricon triangulate opt
  menu->setItemEnabled(108,(G.nv() > 1) && P && B && S && !MB);    //Quadrangulate
  //Embed
  menu->setItemEnabled(201,!SMALL && P && NotBigD);                 //Schnyder
  menu->setItemEnabled(202,!SMALL && P && NotBigD);                 //Schnyder V 
  menu->setItemEnabled(203,!SMALL && P && NotBigD);                 //Tutte
  menu->setItemEnabled(204,!SMALL && P && S && NotBigD);            //Tutte Circle 
  menu->setItemEnabled(205,!SMALL && P && NotBigD);                 //FPP Fary
  menu->setItemEnabled(250,(!SMALL || G.ne() > 1) && P && NotBigD); //Vision
  menu->setItemEnabled(251,(G.nv() > 1) && B && P && NotBigD);      //Biparti
  menu->setItemEnabled(253,!SMALL && P && NotBigD);                 //FPP vision
  //dual
  menu->setItemEnabled(301,(G.nv() > 1) && P); 
  menu->setItemEnabled(302,(G.nv() > 1) && P);
  menu->setItemEnabled(303,(G.nv() > 1) && P);
  menu->setItemEnabled(304,(G.nv() > 1) && P);
  //Algo
  menu->setItemEnabled(601,!P);
  menu->setItemEnabled(602,!P);
  menu->setItemEnabled(610,B);
  menu->setItemEnabled(611,P);
  menu->setItemEnabled(612,!P);
  //Orient
  menu->setItemEnabled(703,!SMALL && P && C3);                        //planar 3-con 
  menu->setItemEnabled(704,(G.nv() > 1) && P && B);                   //biparti 
  menu->setItemEnabled(705,!SMALL && P);                              //planar schnyder
  menu->setItemEnabled(706,(G.nv() > 1) && P && C2);                  //bipolar 

  //Print informations
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  Tprintf("Name:%s",~title());
  if(G.nv() ==0 || G.ne() == 0) return;
  if(G.vin[0]() || G.cir[0]() || G.acir[0]())
      Tprintf("vin[0]=%d,cir[0]=%d,acir[0]=%d"
              ,G.vin[0](),G.cir[0](),G.acir[0]());

  if(T && G.nv() == 3)
      ;
  else if(T && G.nv() > 3)
      {E = G.CheckNoC3Sep();
      if(Error()){Error() = 0;Tprintf("Error CheckNoC3Sep");}
      else if(E) Tprintf("No C3 Separator");
      else Tprintf("C3 Separator");
      }

  else if(P && S && !M && dmin > 2  && G.CheckTriconnected())
      {if(Error()){Error() = 0;Tprintf("Error CheckTriconnected");}
      }

  else if(P && S && !M && G.CheckSubdivTriconnected())
      {if(Error()){Error() = 0;Tprintf("Error CheckSubdivTriconnected");}
      else Tprintf("Subdivision of a 3-Connected");
      }
      
  if(B && P && S && G.nv() > 3 && G.ne() == 2*G.nv()-4)
      Tprintf("Maximal planar bipartite");
  else if(H) {
    Prop1<int> hnv(G.Set(),PROP_NV);
    Prop1<int> hne(G.Set(),PROP_NE);
    Tprintf("Hypergraph: %d hypervertices %d hyperedges",hnv(),hne());
  }

  if(dmin != dmax)
      {Tprintf("Minimum degree:%d",dmin);
      Tprintf("Maximum degree:%d",dmax);
      }

  if(G.Set().exist(PROP_MAPTYPE))
      {Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
      if(maptype() == PROP_MAPTYPE_UNKNOWN)Tprintf("Unknown origin map");
      else if(maptype() == PROP_MAPTYPE_ARBITRARY)Tprintf("Arbitrary map");
      else if(maptype() == PROP_MAPTYPE_GEOMETRIC)Tprintf("Geometric map");
      else if(maptype() == PROP_MAPTYPE_LRALGO)Tprintf("LRALGO map");
      }
  int g = G.ComputeGenus();
  if(g)Tprintf("Genus of the current map: %d",g);
  if(A) Tprintf("Acyclic: %d sources,%d sinks",ns,nt);
  }
