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

#ifdef QT
#undef QT
#endif

#include <Pigale.h> 
#include <QT/Handler.h>


// in Misc.cpp
GraphContainer& GetMainGraph();
//in Generate.cpp
GraphContainer *GenerateGrid(int a, int b);
GraphContainer *GenerateCompleteGraph(int a);
GraphContainer *GenerateCompleteBiGraph(int a,int b);
GraphContainer *GenerateRandomGraph(int a,int b);
//Schaeffer algo
GraphContainer *GenerateSchaeffer(int n_ask,int type,int e_connectivity);

// in MyCanvas.cpp
bool & ShowOrientation();
// in Generate.cpp
bool & EraseMultipleEdges();

// Those handlers return:
// -1 if error
// O nothing
// 1 need redraw
// 2 need info redraw
// 20 need info redraw without any recomputing
// 3 draw (paint)
// 4 3d drawing
// 5 symetrie

int OrientHandler(int action)
  {GeometricGraph G(GetMainGraph());
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
  int ret = 0;
  int i;
  tedge e;
  tbrin first;
  switch(action)
      {case 701:
	   {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,true);
	   eoriented.definit(true);
	   for(e=1;e <= G.ne();e++)eoriented[e] = true;
	   }
	   ShowOrientation() = true;
	   break;
      case 702:
	  i = G.InfOrient();
          G.FixOrientation();
	  ShowOrientation() = true;
	  Tprintf("G is %d inf-oriented",i-1);
          break;
      case 703:
          ret = G.Tricon3orient();
          G.FixOrientation();
	  ShowOrientation() = true;
          if(ret != 0)Tprintf("ret=%d",ret);
          break;
      case 704:
	  ret = DecompMaxBip(G);
          if(ret != 0){Tprintf("ret=%d",ret);return -1;}
          G.FixOrientation();
	  ShowOrientation() = true;
          break;
      case 705:
	  ret = G.SchnyderOrient(0);
          G.FixOrientation();
	  ShowOrientation() = true;
          if(ret != 0)Tprintf("ret=%d",ret);
          break;
      case 706:
	  first = 1;
	  ret = G.BipolarPlan(first);
	  if(ret != 0){Tprintf("ret=%d",ret);return -1;}
	  G.FixOrientation();
	  ShowOrientation() = true;
	  if(first.GetEdge() <= G.ne())
	      {G.ecolor(first.GetEdge()) = Red;
	      G.ewidth(first.GetEdge()) = 2;
	      }
	  Tprintf("st=%d  (%d %d)",first(),G.vin[first](),G.vin[-first]());
	  break;
      default:
	    return 0;
      }
  if(ret)return -1;
  return 1;
  }
int AlgoHandler(int action,int nn)
  {GeometricGraph G(GetMainGraph());
  int ret = 0;
  int i;
  tedge e; tvertex v;
  switch(action)
      {case 601:
	   ret = G.Kuratowski();
	   Tprintf("Kuratowski");
	   break;
      case 602:
	  G.CotreeCritical(i);
	  Tprintf("Cotre Critical case:%d",i);
	  //if(ret){Tprintf("Error Cotre Critical");return -1;}
          // Compute cycle dimension of the Cotree Critical
          {int nblack ,mblack;
          tbrin b,b0;
          tvertex v;
          bool noblack;
          Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
          nblack = mblack = 0;
          ForAllVertices(v,G)
              {b = b0 = G.FirstBrin(v);
              if(!b)continue; // Isolated vertex
              noblack = true;
              do
                  {if(ecolor[b.GetEdge()] == Black)
                      {++mblack;noblack=false;}
                  }while((b = G.cir[b]) != b0);
              if(noblack == false)++nblack;
              }
          mblack /= 2;
          Tprintf("Critical cycle dimension=%d",mblack-nblack+1);
          }
	  return 1;
	  break;
      case 603:
	  //G.Simplify();
	  i = FindNPSet(G);
	  if(i == 0)return 0;
	  Tprintf("Max.planar (fast)");
	  Tprintf("Need to erase %d edges",i);
	  return 1;
	  break;
      case 604:
	  //G.Simplify();
	  i = G.MaxPlanar();
	  if(i == 0)return 0;
	  if(i < 0){Tprintf("Max.planar (slow) Error:%d",i);return i;}
	  Tprintf("Max.planar (slow)");
	  Tprintf("Need to erase %d edges",i);
	  return 1;
	  break;
      case 605:
	  {short ecol; G.ecolor.getinit(ecol);
	  short vcol;  G.vcolor.getinit(vcol);
	  int width; G.ewidth.getinit(width);
          ForAllEdges(e,G) {G.ecolor[e] = ecol; G.ewidth[e] = width;}
          ForAllVertices(v,G) G.vcolor[v] = vcol;
	  }
          break;
      case 606: // Netcut
	  G.Simplify();
	  ret = split(G,nn);
          break;
      case 607:
	  if(G.ComputeGeometricCir() == 0)
              {G.extbrin() = G.FindExteriorFace();
	      Prop1<int> map(G.Set(),PROP_PLANARMAP);
              Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
              maptype() = PROP_MAPTYPE_GEOMETRIC;
              }
	  break;
      case 608:
          if(G.Planarity() == 1)
              {Prop1<int> map(G.Set(),PROP_PLANARMAP);
              Prop1<int> maptype(G.Set(),PROP_MAPTYPE);
              maptype() = PROP_MAPTYPE_LRALGO;
              }
	  return 0;
	  break;
      case 609://symetrie
	  ret = Embed3d(G);
	  if(ret >=0)return 5;
	  break;
      case 610://color bipartite
	  G.CheckBipartite(true);
	  break;
      case 611://color exterior face
	  i = G.ColorExteriorface();
	  ret = (i > 0) ? 0:1;
	  break;
      case 612://show non critical edges
	  {tedge e;
          ForAllEdges(e,G)
              {G.ecolor[e] = Black; G.ewidth[e] = 1;}
	  tvertex v0,v1;
	  int planar,m;
	  m = G.ne();
	  for(tedge e = m; e >= 1;e--)
	      {v0 = G.vin[e]; v1 = G.vin[-e];
	      G.DeleteEdge(e);
	      planar = G.TestPlanar();
	      G.NewEdge(v0,v1);
	      if(!planar)
		  {G.ecolor[m] = Red; G.ewidth[m] = 2;}
	      }
	  }
	  return 1;
	  break;
      default: 
	    return 0;
      }
  if(ret)return -1;
  return 1;
  }
int EmbedHandler(int action,int &drawing)
  {TopologicalGraph G(GetMainGraph());
  GeometricGraph G0(G);
  int ret;
  int err = 0;
  drawing = 0;
  switch(action)
      {case 201:ret = 1;
	   err = G.Schnyder(0);
	   Tprintf("Schnyder");
	   if(err)Tprintf("ret=%d",err);
	  break;
      case 202:ret = 1;
          err = G.SchnyderV(0);
	  Tprintf("Schnyder V");
          if(err)Tprintf("ret=%d",err);
	  break;
      case 203:ret = 1;
	  err = EmbedFPP(G);
	  if(err)Tprintf("ret=%d",err);
	  break;
      case 204:ret = 1;
          EmbedTutteCircle(G);
	  Tprintf("Tutte circle");
	  break;
      case 205:ret = 1;
          G0.Tutte();
	  Tprintf("Tutte");
	  break;
      case 250:ret = 3;
	  err = EmbedVision(G);
	  drawing = 0;
	  break;
      case 251:ret = 3;
	  err = EmbedContactBip(G0);
	  drawing = 3;
	  break;
      case 252:ret = 3;
	  err = Polar(G);
	  drawing = 4;
	  break;
      case 253:ret = 3;
	  err = EmbedFPPRecti(G);
	  drawing = 1;
	  break;
      case 254:ret = 3;
	  err = Vision(G);
	  if(err)Tprintf("Vision err=%d",err);
	  drawing = 2;
	  break;
      case 255:ret = 3;
	  err = EmbedTContact(G);
	  if(err)Tprintf("TContact err=%d",err);
	  drawing = 5;
	  break;
      case 298:ret = 4; //Embed3d
	  break;
      default:
	    return 0;
      }
  if(err)return -1;
  return ret;
  }
int AugmentHandler(int action)
  {GeometricGraph  G(GetMainGraph());
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  int ret = 0;
  int i;
  switch(action)
      {case 101: 
	   i = G.MakeConnected();
	   Tprintf("Added %d Edges",i);
	   title() = "K-" + title();
	   break;
      case 102:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.Biconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2K-" + title();
	  break;
      case 103:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.Opt6Biconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2Kopt-" + title();
	  break;
      case 104:
          G.Set().erase(PROP_BICONNECTED);
          ret = G.NpBiconnect();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "2KNP-" + title();
	  break;
      case 105:
	  ret = G.VertexTriangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "VT-" + title();
	  break;
      case 106:
	  ret = G.ZigZagTriangulate();
          if(ret < 0){Tprintf("ret=%d",ret);break;}
          title() = "ZZ-" + title();
	  break;
      case 107:
	  ret = G.TriconTriangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "3CT-" + title();
	  break;
      case 108:
	  ret =   G.VertexQuadrangulate();
          if(ret != 0){Tprintf("ret=%d",ret);break;}
          title() = "VQ-" + title();
	  break;
      case 109:
	  {int m = G.ne();
	  for(tedge e = 1;e <= m;e++)G.BissectEdge(e);
	  }
	  break;
      default:
	  return 0;
      }
  if(ret)return -1;
  return 2;
  }
int DualHandler(int action)
  {GeometricGraph G(GetMainGraph());
  GraphContainer *pGC;
  switch(action)
      {case 301:
	   pGC = G.DualGraph();
	   if (pGC) G.swap(*pGC);
	   delete pGC;
	   Tprintf("Dual");
	   break;
      case 302:
	   pGC = G.GeometricDualGraph();
	   if (pGC) G.swap(*pGC);
	   delete pGC;
	   Tprintf("Dual geometric");
	   break;
      case 303:
	   pGC = G.AngleGraph();
	   if (pGC) G.swap(*pGC);
	   delete pGC;
	   Tprintf("Angle");
	   break;
      case 304:
	   pGC = G.GeometricAngleGraph();
	   if (pGC) G.swap(*pGC);
	   delete pGC;
	   Tprintf("Angle geometric");
	   break;
      default:
	    break;
      }
  {TopologicalGraph GG(GetMainGraph()); GG.RemoveLoops();}
  return 2;
  }
int RemoveHandler(int action)
  {TopologicalGraph G(GetMainGraph());
  GeometricGraph GG(GetMainGraph());
  int n;
  switch(action)
      {case 401:
	   n = G.RemoveIsolatedVertices();
	   break;
      case 402:
	  G.RemoveLoops();
	  break;
      case 403:
	  G.Simplify();
	  break;
      case 404:
	  G.RemoveIsthmus();
	  break;
      case 405://Erase Color Vertices
	  short vcol; GG.vcolor.getinit(vcol);
	  for(tvertex v= GG.nv() ;v > 0;v--)
	      if(GG.vcolor[v] == vcol)GG.DeleteVertex(v);
	  break;
	  break;
      case 406://Erase Color Edges
	  short ecol; GG.ecolor.getinit(ecol);
	  for(tedge e = GG.ne() ;e > 0;e--)
	      if(GG.ecolor[e] == ecol)GG.DeleteEdge(e);
	  break;
      case 407://Erase Thick Edges
	  int ewidth; GG.ewidth.getinit(ewidth);
	  for(tedge e = GG.ne() ;e > 0;e--)
	      if(GG.ewidth[e] == ewidth)GG.DeleteEdge(e);
	  break;
      default:
	  return 0;
      }
  return 20;
  }
int GenerateHandler(int action,int n1_gen,int n2_gen,int m_gen)
  {GraphContainer *GC=(GraphContainer *)0;
  switch(action)
      {case 501:
	   GC = GenerateGrid(n1_gen,n2_gen);
	   break;
      case 502:
	   GC = GenerateCompleteGraph(n1_gen);
	   break;
      case 503:
	   GC = GenerateCompleteBiGraph(n1_gen,n2_gen);
	   break;
      case 504:
	  GC = GenerateRandomGraph(n1_gen,m_gen);
	   break;
	   // Schaeffer generator
      case 505:
	   GC = GenerateSchaeffer(m_gen,1,1);
	   break;
      case 506:
	   GC = GenerateSchaeffer(m_gen,1,2);
	   break;
      case 507:
	   GC = GenerateSchaeffer(m_gen,1,3);
	   break;
      case 510:
	   GC = GenerateSchaeffer(m_gen,3,2);
	   break;
      case 511:
	   GC = GenerateSchaeffer(m_gen,3,3);
	   break;
      case 512:
	   GC = GenerateSchaeffer(m_gen,3,4);
	   break;
      case 513:
	   GC = GenerateSchaeffer(m_gen,4,2);
	   break;
      case 514:
	   GC = GenerateSchaeffer(m_gen,4,4);
	   break;
      case 515:
	   GC = GenerateSchaeffer(m_gen,4,6);
	   break;
      case 516:
	   GC = GenerateSchaeffer(m_gen,4,0);
	   break;
      case 517:
	   GC = GenerateSchaeffer(m_gen,2,1);
	   break;
      case 518:
	   GC = GenerateSchaeffer(m_gen,2,2);
	   break;
      case 519:
	   GC = GenerateSchaeffer(m_gen,2,3);
	   break;
      default:
	  return 0;
      }
  if(GC)
      {GetMainGraph().swap(*GC);
      GeometricGraph GG(GetMainGraph());
      }
  else {Tprintf("Error generating");LogPrintf("Error generating:%d",action);}
  delete GC;
  return 2;
  }
