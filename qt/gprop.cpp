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
#include "pigaleWindow.h"
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
  menu = menubar;
  //QFont fnt = this->font();  fnt.setBold(true);  setFont(fnt,true);
  setMinimumHeight(180); 
  //marge,spacing
  QVBoxLayout* MainLayout = new QVBoxLayout(this,5,2,"MainLayout"); 

  QHBoxLayout *TxtLayout = new QHBoxLayout(0,0,0,"TxtLayout"); 
  QHBoxLayout *Layout_NM = new QHBoxLayout(0,0,0,"Layout_NM");
  QHBoxLayout *LayoutDegrees = new QHBoxLayout(0,0,0,"LayoutDegrees");
  QHBoxLayout *LayoutAllButtons = new QHBoxLayout(0,8,10,"LayoutAllButtons");
  
  MainLayout->addLayout(TxtLayout);
  MainLayout->addLayout(Layout_NM);
  MainLayout->addLayout(LayoutDegrees);
  MainLayout->addLayout(LayoutAllButtons);
  
  QVBoxLayout *LayoutLeftButtons  = new QVBoxLayout(0,0,0,"LayoutLeftButtons"); 
  QVBoxLayout *LayoutRightButtons = new QVBoxLayout(0,0,0,"LayoutP");
  LayoutAllButtons->addLayout(LayoutLeftButtons);
  LayoutAllButtons->addLayout(LayoutRightButtons);

  QLabel*TextLabel3 = new QLabel(this,"TextLabel3");
  TextLabel3->setText(tr("<b>Graph Properties</b>"));
  TextLabel3->setTextFormat(Qt::RichText);
  TextLabel3->setAlignment(int(QLabel::AlignCenter));
  TxtLayout->addWidget(TextLabel3);
  
  QLabel*TextLabel1 = new QLabel(this,"TextLabel1");
  TextLabel1->setText("N:  ");
  Layout_NM->addWidget(TextLabel1);
  LE_N = new QLineEdit(this,"LE_N");
  LE_N->setMaximumSize(QSize(60,32767));
  LE_N->setText("0");
  Layout_NM->addWidget(LE_N);
  QLabel *TextLabel1_2 = new QLabel(this,"TextLabel1_2");
  TextLabel1_2->setText("M:  ");
  Layout_NM->addWidget(TextLabel1_2);
  LE_M = new QLineEdit(this,"LE_M");
  LE_M->setMaximumSize(QSize(60,32767));
  LE_M->setText("0");
  Layout_NM->addWidget(LE_M);
  LE_M->setReadOnly(true);  LE_N->setReadOnly(true);
  
  QLabel* TextLabelMin = new QLabel(this,"TextLabelMin");
  TextLabelMin->setText("Min:");
  LayoutDegrees->addWidget(TextLabelMin);
  LE_Min = new QLineEdit(this,"LE_Min");
  LE_Min->setMaximumSize(QSize(60,32767));
  LE_Min->setText("0");
  LayoutDegrees->addWidget(LE_Min);
  QLabel* TextLabelMax = new QLabel(this,"TextLabelMax");
  TextLabelMax->setText("Max:");
  LayoutDegrees->addWidget(TextLabelMax);
  LE_Max = new QLineEdit(this,"LE_Max");
  LE_Max->setMaximumSize(QSize(60,32767));
  LE_Max->setText("0");
  LayoutDegrees->addWidget(LE_Max);
  LE_Min->setReadOnly(true);  LE_Max->setReadOnly(true);

  //***************************************************
  RBConnected = new RoRadioButton(this,"RBConnected");
  RBConnected->setText(tr("Connected"));
  RBConnected->setChecked(TRUE);
  LayoutLeftButtons->addWidget(RBConnected);

  RB2Connected = new RoRadioButton(this,"RB2Connected");
  RB2Connected->setText(tr("2-Connected"));
  LayoutLeftButtons->addWidget(RB2Connected);

  RB3Connected = new RoRadioButton(this,"RB3Connected");
  RB3Connected->setText(tr("3-Connected"));
  LayoutLeftButtons->addWidget(RB3Connected);

  RBBipartite = new RoRadioButton(this,"RBBipartite");
  RBBipartite->setText(tr("Bipartite"));
  LayoutLeftButtons->addWidget(RBBipartite);

  RBAcyclic = new RoRadioButton(this,"RBAcyclic");
  RBAcyclic->setText(tr("Acyclic"));
  LayoutLeftButtons->addWidget(RBAcyclic);

  //********************************************
  RBPlanar = new RoRadioButton(this,"RBPlanar");
  RBPlanar->setText(tr("Planar"));
  LayoutRightButtons->addWidget(RBPlanar);

  RBMxPlanar = new RoRadioButton(this,"RBMxPlanar");
  RBMxPlanar->setText(tr("Triangulation"));
  LayoutRightButtons->addWidget(RBMxPlanar);

  RBOuPlanar = new RoRadioButton(this,"RBOuPlanar");
  RBOuPlanar->setText(tr("Out. planar"));
  LayoutRightButtons->addWidget(RBOuPlanar);

  RBSerie = new RoRadioButton(this,"RBSerie");
  RBSerie->setText(tr("Series-//"));
  LayoutRightButtons->addWidget(RBSerie);

  RBSimple = new RoRadioButton(this,"Simple");
  RBSimple->setText(tr("Simple"));
  LayoutRightButtons->addWidget(RBSimple);
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
#ifdef TDEBUG
DebugPrintf("START INFO: n = %d m = %d",G.nv(),G.ne());
#endif
  
  int nloops = G.RemoveLoops();
  if(nloops)
      {QString t;
      t.sprintf("Graph had %d loops",nloops);
      setError(-1,(const char *)t);
      Twait((const char *)t);
      }
  S = G.CheckSimple();
  P = G.CheckPlanar();
  bool SMALL = (G.nv() < 3) ? true : false;
  bool M = (!SMALL  && (G.ne() == 3*G.nv() - 6)) ? true : false;
  T = (P && S && M) ? true : false;     //Triangulation
  A = G.CheckAcyclic(ns,nt);
  B = G.CheckBipartite();
  G.MinMaxDegree(dmin,dmax);
  R = (dmin == dmax) ? true :false;
  C1 = C2 = C3 = false;
  Outer =  Serie = false;
 
  //bool H = G.Set().exist(PROP_HYPERGRAPH);
  bool E;
  MaxBi = P && B && S && (G.ne() == (2*G.nv() - 4)  && (G.nv() >= 4));
 
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
  if(C3)C2 = true;
  if(C2)C1 = true;

  if(P && C2 && !C3) Serie = G.CheckSerieParallel();
  if(P  && !C3 && dmin <= 2 && TestOuterPlanar(G)) Outer = true;
  if(G.nv() == 2 && G.ne() == 1 || C1 && dmax == 2) 
      Serie = Outer = true;

  //Modify the enable menus
  //For slow programs or display
  bool NotBigS = (G.nv() > MaxNSlow ) ? false : true;
  bool NotBigD = (G.nv() > MaxNDisplay) ? false : true;
  menu->setItemEnabled(A_EMBED_3d,G.nv() > 3 && NotBigD);                   //Rn embedding
  menu->setItemEnabled(A_ALGO_SYM,!SMALL && NotBigS);                       //sym
  menu->setItemEnabled(A_ALGO_NETCUT,!SMALL && NotBigS);                    //partition
  menu->setItemEnabled(A_ALGO_NPSET,!P && NotBigS && S);                    //maxplanar
  menu->setItemEnabled(A_ALGO_MAX_PLANAR,!P && NotBigS && S);               //maxplanar
  menu->setItemEnabled(A_EMBED_POLAR,C1 && NotBigD);                        //polair
  //Augment
  menu->setItemEnabled(A_AUGMENT_CONNECT,(G.nv() > 1) && !C1);               //make connected 
  menu->setItemEnabled(A_AUGMENT_CONNECT_V,(G.nv() > 1) && !C1);               //make connected 
  menu->setItemEnabled(A_AUGMENT_BICONNECT,!SMALL && P && !C2);              //make 2 connected
  menu->setItemEnabled(A_AUGMENT_BICONNECT_6,!SMALL && P && !C2);            //make 2 connected opt
  menu->setItemEnabled(A_AUGMENT_BICONNECT_NP,!SMALL && !C2);                //make 2 connected NP  
  menu->setItemEnabled(A_AUGMENT_BICONNECT_NP_V, !SMALL &&!C2);                  //make 2 connected NP  
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_V,!SMALL && P && S && !T);      //vertex triangulate
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_ZZ,!SMALL && P && S && !T);     //ZigZag 
  menu->setItemEnabled(A_AUGMENT_TRIANGULATE_3C,!SMALL && P && C3 && !T);    //Tricon triangulate opt
  menu->setItemEnabled(A_AUGMENT_QUADRANGULATE_V,(G.nv() > 1) && !MaxBi);    //Quadrangulate
  menu->setItemEnabled( A_AUGMENT_BISSECT_ALL_E ,G.ne());    //Bissect all edges
  //Embed
  menu->setItemEnabled(A_EMBED_SCHNYDER_E,!SMALL && S && P && NotBigD);       //Schnyder
  menu->setItemEnabled(A_EMBED_SCHNYDER_V ,!SMALL && S && P && NotBigD);      //Schnyder V 
  menu->setItemEnabled(A_EMBED_FPP,!SMALL && S && P && NotBigD);              //FPP Fary
  menu->setItemEnabled(A_EMBED_CCD,!SMALL && S && P && C3 && NotBigD);         //Convex Compact
  menu->setItemEnabled(A_EMBED_CD,!SMALL && S && P && C3 && NotBigD);         //Convex 
  menu->setItemEnabled(A_EMBED_POLYLINE,!SMALL && S && P && NotBigD);         //Polyline
  menu->setItemEnabled(A_EMBED_TUTTE_CIRCLE,!SMALL && P && S && NotBigD);     //Tutte Circle 
  menu->setItemEnabled(A_EMBED_TUTTE,!SMALL && P && NotBigD);                 //Tutte
  menu->setItemEnabled(A_EMBED_VISION,(!SMALL || G.ne() > 1) && P && NotBigD);//Vision
  menu->setItemEnabled(A_EMBED_CONTACT_BIP,(G.nv() > 1) && B && P && NotBigD);//Biparti
  menu->setItemEnabled(A_EMBED_FPP_RECTI,!SMALL && S && P && NotBigD);        //FPP vision
  menu->setItemEnabled(A_EMBED_GVISION,!SMALL  && NotBigD);                   //Gvision
  menu->setItemEnabled(A_EMBED_T_CONTACT,!SMALL && S && P && NotBigD);        //T-contact
  menu->setItemEnabled(A_EMBED_SPRING,NotBigD);                               //spring
  menu->setItemEnabled(A_EMBED_SPRING_PM,NotBigD);                            //springPM
  menu->setItemEnabled(A_EMBED_CURVES,!SMALL && NotBigD);                               //curves
  menu->setItemEnabled(A_EMBED_JACQUARD,!SMALL && P && NotBigD);              //Jacquard
  menu->setItemEnabled(A_EMBED_3dSCHNYDER,!SMALL && S && P && NotBigD);       //Schnyder 3d
  //dual
  menu->setItemEnabled(A_GRAPH_DUAL,(G.nv() > 1) && P && C1); 
  menu->setItemEnabled(A_GRAPH_DUAL_G,(G.nv() > 1) && P && C1);
  menu->setItemEnabled(A_GRAPH_ANGLE,(G.nv() > 1) && C1 && P);
  menu->setItemEnabled(A_GRAPH_ANGLE_G,(G.nv() > 1) && C1 && P);
  //Algo
  menu->setItemEnabled(A_ALGO_KURATOWSKI,!P);
  menu->setItemEnabled(A_ALGO_COTREE_CRITICAL,!P);
  menu->setItemEnabled(A_ALGO_COLOR_BIPARTI,B);
  menu->setItemEnabled(A_ALGO_COLOR_EXT,P);
  menu->setItemEnabled(A_ALGO_COLOR_NON_CRITIC,!P);
  //Orient
  menu->setItemEnabled(A_ORIENT_TRICON,!SMALL && P && C3);       //planar 3-con 
  menu->setItemEnabled(A_ORIENT_BIPAR,(G.nv() > 1) && P && B);   //biparti 
  menu->setItemEnabled(A_ORIENT_SCHNYDER,!SMALL && P && S &C1);  //planar schnyder
  menu->setItemEnabled(A_ORIENT_BIPOLAR,(G.nv() > 1) && P && C2);//bipolar plan
  menu->setItemEnabled(A_ORIENT_BIPOLAR_NP,(G.nv() > 1) && C2);  //bipolar 

  if(!print)return;  
  // Modify the buttons
  RBSimple->setChecked(S);
  RBPlanar->setChecked(P);
  RBMxPlanar->setChecked(T);
  RBBipartite->setChecked(B);
  if(B)
      {if(MaxBi)RBBipartite->setText("Max. Bipartite");
      else     RBBipartite->setText("Bipartite");
      }
  RBAcyclic->setChecked(A);
  RBConnected->setChecked(C1);
  RB2Connected->setChecked(C2);
  // 3-connectivity
  if(!P)RB3Connected->hide();
  else  RB3Connected->show();
  RB3Connected->setChecked(C3);

  // Outerplanar
  RBOuPlanar->setChecked(Outer);
  if(Outer)
      {if(C2 && (G.ne() == 2*G.nv() - 3)) RBOuPlanar->setText(tr("Max. OuterPlanar"));
      else    RBOuPlanar->setText(tr("OuterPlanar"));
      }
  // Serie (test is only for 2-connected graphs)
  if(P && !C2)
      RBSerie->hide();
  else
      {RBSerie->show();  RBSerie->setChecked(Serie);}

  LE_N->setText(QString("%1").arg(G.nv()));
  LE_M->setText(QString("%1").arg(G.ne()));
  LE_Min->setText(QString("%1").arg(dmin));
  LE_Max->setText(QString("%1").arg(dmax));
  
  //Print informations
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
  else if(!C3 && P && S && !M && G.CheckSubdivTriconnected())
      {if(getError()){setError();Tprintf("Error CheckSubdivTriconnected");}
      else Tprintf("Subdivision of a 3-Connected");
      }

  if(G.Set().exist(PROP_MAPTYPE))
      {Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
      if(maptype() == PROP_MAPTYPE_UNKNOWN)Tprintf("Unknown origin map");
      else if(maptype() == PROP_MAPTYPE_ARBITRARY)Tprintf("Arbitrary map");
      else if(maptype() == PROP_MAPTYPE_GEOMETRIC)Tprintf("Geometric map");
      else if(maptype() == PROP_MAPTYPE_LRALGO)Tprintf("LRALGO map");
      }
  //int g = G.ComputeGenus();
  //if(g)Tprintf("Genus of the current map: %d",g);
  if(A & C1) Tprintf("Acyclic: %d sources,%d sinks",ns,nt);

  }
