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
#include <QT/Misc.h>
#include <QT/Action_def.h>

#include <qvariant.h>   // first for gcc 2.7.2
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qmenubar.h>


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
  TextLabel3->setText("<b>Graph Properties</b>");
  TextLabel3->setTextFormat(Qt::RichText);
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

RoRadioButton::RoRadioButton(QWidget * parent,const char * name)
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
  {MaxNSlow = i;//if(GetMainGraph().nv())update();
  }
void Graph_Properties::MaxNDisplayChanged(int i)
  {MaxNDisplay = i;//if(GetMainGraph().nv())update();
  }
void Graph_Properties::update(bool print)
  {GeometricGraph G(GetMainGraph());
  if(G.vin[0]() || G.cir[0]() || G.acir[0]())
      {Tprintf("vin[0]=%d,cir[0]=%d,acir[0]=%d",G.vin[0](),G.cir[0](),G.acir[0]());
      setError(-1,"vin[0] or cir[0] or acir[0] != 0");
      }
  
  int nloops = G.RemoveLoops();
  if(nloops)
      {QString t;
      t.sprintf("Graph had %d loops",nloops);
      setError(-1,(const char *)t);
      Twait((const char *)t);
      }
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
  bool MB = P && B && S && (G.ne() == (2*G.nv() - 4)  && (G.nv() >= 4));
 
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

  //Modify the enable menus
  //For slow programs or display
  bool NotBigS = (G.nv() > MaxNSlow ) ? false : true;
  bool NotBigD = (G.nv() > MaxNDisplay) ? false : true;
  menu->setItemEnabled(A_EMBED_3d,!SMALL && NotBigD);                       //Rn embedding
  menu->setItemEnabled(A_ALGO_SYM,!SMALL && NotBigS);                       //sym
  menu->setItemEnabled(A_ALGO_NETCUT,!SMALL && NotBigS);                    //partition
  menu->setItemEnabled(A_ALGO_NPSET,!P && NotBigS && S);                    //maxplanar
  menu->setItemEnabled(A_ALGO_MAX_PLANAR,!P && NotBigS && S);               //maxplanar
  menu->setItemEnabled(A_EMBED_POLAR,C1 && NotBigD);                        //polair

  //Augment
  menu->setItemEnabled(A_AUGMENT_CONNECT,(G.nv() > 1) && !C1);               //make connected 
  menu->setItemEnabled(A_AUGMENT_BICONNECT,!SMALL && P && !C2);              //make 2 connected
  menu->setItemEnabled(A_AUGMENT_BICONNECT_6,!SMALL && P && !C2);            //make 2 connected opt
  menu->setItemEnabled(A_AUGMENT_BICONNECT_NP,!SMALL && !C2);                //make 2 connected NP  
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_V,!SMALL && P && S && !T);      //vertex triangulate
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_ZZ,!SMALL && P && S && !T);     //ZigZag 
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_3C,!SMALL && P && C3 && !T);    //Tricon triangulate opt
  menu->setItemEnabled(A_AUGMENT_QUADRANGULATE_V,(G.nv() > 1) && P && B && S && !MB); //Quadrangulate
  //Embed
  menu->setItemEnabled(A_EMBED_SCHNYDER_E,!SMALL && S && P && NotBigD);       //Schnyder
  menu->setItemEnabled(A_EMBED_SCHNYDER_V ,!SMALL && S && P && NotBigD);      //Schnyder V 
  menu->setItemEnabled(A_EMBED_FPP,!SMALL && S && P && NotBigD);              //FPP Fary
  menu->setItemEnabled(A_EMBED_TUTTE_CIRCLE,!SMALL && P && S && NotBigD);     //Tutte Circle 
  menu->setItemEnabled(A_EMBED_TUTTE,!SMALL && P && NotBigD);                 //Tutte
  menu->setItemEnabled(A_EMBED_VISION,(!SMALL || G.ne() > 1) && P && NotBigD);//Vision
  menu->setItemEnabled(A_EMBED_CONTACT_BIP,(G.nv() > 1) && B && P && NotBigD);//Biparti
  menu->setItemEnabled(A_EMBED_FPP_RECTI,!SMALL && S && P && NotBigD);        //FPP vision
  menu->setItemEnabled(A_EMBED_GVISION,!SMALL  && NotBigD);                   //Gvision
  menu->setItemEnabled(A_EMBED_T_CONTACT,!SMALL && S && P && NotBigD);        //T-contact
  menu->setItemEnabled(A_EMBED_SPRING,NotBigD);                               //spring
  menu->setItemEnabled(A_EMBED_SPRING_PM,NotBigD);                            //springPM
  menu->setItemEnabled(A_EMBED_JACQUARD,!SMALL && P && NotBigD);              //Jacquard
  //dual
  menu->setItemEnabled(A_GRAPH_DUAL,(G.nv() > 1) && P); 
  menu->setItemEnabled(A_GRAPH_DUAL_G,(G.nv() > 1) && P);
  menu->setItemEnabled(A_GRAPH_ANGLE,(G.nv() > 1) && P);
  menu->setItemEnabled(A_GRAPH_ANGLE_G,(G.nv() > 1) && P);
  //Algo
  menu->setItemEnabled(A_ALGO_KURATOWSKI,!P);
  menu->setItemEnabled(A_ALGO_COTREE_CRITICAL,!P);
  menu->setItemEnabled(A_ALGO_COLOR_BIPARTI,B);
  menu->setItemEnabled(A_ALGO_COLOR_EXT,P);
  menu->setItemEnabled(A_ALGO_COLOR_NON_CRITIC,!P);
  //Orient
  menu->setItemEnabled(A_ORIENT_TRICON,!SMALL && P && C3);       //planar 3-con 
  menu->setItemEnabled(A_ORIENT_BIPAR,(G.nv() > 1) && P && B);   //biparti 
  menu->setItemEnabled(A_ORIENT_SCHNYDER,!SMALL && P && S);      //planar schnyder
  menu->setItemEnabled(A_ORIENT_BIPOLAR,(G.nv() > 1) && P && C2);//bipolar plan
  
  // Modify the buttons
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
  
  //Print informations
  if(!print)return;
  if(debug())DebugPrintf("\nn:%d m:%d",G.nv(),G.ne());
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  Tprintf("Name:%s",~title());
  if(getError())DebugPrintf("GP %s",(const char *)getErrorString());
  if(G.nv() == 0 || G.ne() == 0) return;
  if(T && G.nv() == 3)
      ;
  else if(T && G.nv() > 3)
      {E = G.CheckNoC3Sep();
      if(getError()){setError();Tprintf("Error CheckNoC3Sep");}
      else if(E) Tprintf("No C3 Separator");
      else Tprintf("C3 Separator");
      }

  else if(P && S && !M && dmin > 2  && G.CheckTriconnected())
      {if(getError()){setError();Tprintf("Error CheckTriconnected");}
      }

  else if(P && S && !M && G.CheckSubdivTriconnected())
      {if(getError()){setError();Tprintf("Error CheckSubdivTriconnected");}
      else Tprintf("Subdivision of a 3-Connected");
      }
      
  if(B && P && S && G.nv() > 3 && G.ne() == 2*G.nv()-4)
      Tprintf("Maximal planar bipartite");
  else if(H) 
      {Prop1<int> hnv(G.Set(),PROP_NV);
      Prop1<int> hne(G.Set(),PROP_NE);
      Tprintf("Hypergraph: %d hypervertices %d hyperedges",hnv(),hne());
      }

  if(dmin != dmax)
      {Tprintf("Minimum degree:%d",dmin);
      Tprintf("Maximum degree:%d",dmax);
      }
  else
      Tprintf("Regular degree:%d",dmin);

  if(G.Set().exist(PROP_MAPTYPE))
      {Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
      if(maptype() == PROP_MAPTYPE_UNKNOWN)Tprintf("Unknown origin map");
      else if(maptype() == PROP_MAPTYPE_ARBITRARY)Tprintf("Arbitrary map");
      else if(maptype() == PROP_MAPTYPE_GEOMETRIC)Tprintf("Geometric map");
      else if(maptype() == PROP_MAPTYPE_LRALGO)Tprintf("LRALGO map");
      }
  int g = G.ComputeGenus();
  if(g)Tprintf("Genus of the current map: %d",g);
  if(A & C1) Tprintf("Acyclic: %d sources,%d sinks",ns,nt);
  else if(A)Tprintf("Acyclic Orientation");
  else if(!A)Tprintf("Not Acyclic Orientation");
  }
