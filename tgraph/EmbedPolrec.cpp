#include <Pigale.h>
#include <TAXI/MaxPath.h>

class DoccGraph : public TopologicalGraph
{public:
  svector<tbrin> iel,dos;
  svector<tedge> suc;
  svector<tvertex> nvin,ivl;  
  svector<int> dosInv,dosx,foc,hauteur,lver,rver;
  int hmax;
  int morg;
  bool ok;

 private:
  void init();
  MaxPath *MPATH; 
  int numConstraint;
  int makeDosDFS();
  int makeDosLR();

 public:
  DoccGraph(GraphAccess &G,int _morg,int type) : TopologicalGraph(G),hmax(0),morg(_morg),ok(true)
      {init();
      if(type == 0)
          makeDosDFS();
      else
          makeDosLR();
      }
  ~DoccGraph(){}

  void htreePR();
  bool  horderPR(tbrin ee, tbrin es);
  tbrin  rightBrinPR(tedge e);
  void insertConstraint(tedge je,tedge js);
  void pushSPR();
  void push1PR();
  bool  stronger_constraintPR(tedge js, tedge sucje);
  void push2PR();
  void  vheightPR();
  void  lrverPR();
  int drawPR();
};
void DoccGraph::init()
  {int m = ne(); int n = nv();
  nvin.resize(-m,m); nvin.SetName("DosG:nvin");
  iel.resize(-m,m); iel.SetName("DosG:iel");
  ivl.resize(1,n); ivl.SetName("DosG:ivl");
  dos.resize(1,2*m);  dos.SetName("DosG:dos");
 dosInv.resize(-m,m);  dosInv.SetName("DosG:dosInv");
 dosx.resize(1,2*m);  dosx.SetName("DosG:dosx"); 
  suc.resize(-n+1,m);  suc.SetName("DosG:suc");  
  foc.resize(n,m);  foc.SetName("DosG:foc"); 
  hauteur.resize(0,m+1);  hauteur.SetName("DosG:hauteur");
  lver.resize(1,n);  lver.SetName("DosG:lver");lver.clear();
  rver.resize(1,n);  rver.SetName("DosG:rver");
  }
int DoccGraph::makeDosLR()
  {Prop<bool> isTree(Set(tedge()),PROP_ISTREE_LR);
#ifdef TDEBUG
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<long> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<long> elabel(Set(tedge()),PROP_LABEL);
  Prop<int> ewidth(Set(tedge()),PROP_WIDTH);
  for(tedge a =1; a <= ne();a++) 
      {ecolor[a] = Black;
      if(isTree(a))ewidth[a] = 3;
      else ewidth[a] = 1;
      }
#endif
  tbrin b0 = acir[extbrin()];
  svector<int> dfsnum(1,nv()); dfsnum.SetName("dfsnum");

 // Renum the vertices as they are discovered
  dfsnum.clear();
  tvertex v = vin[b0];
  int num = 1;
  dfsnum[v] = 1;  ivl[1] = v; 
  tbrin b = b0;
  do
      {tedge je = b.GetEdge();
      tvertex w=vin[-b];
      if(!isTree(je))
          ;
      else if (dfsnum[w] )  // Backtraking on tree
          {b.cross();  v = w;
          }
      else  // Climbing on  tree
          {dfsnum[w] = ++num;  ivl[num] = w;
          b.cross(); v = w;
          }
      b = acir[b];
      } while(b != b0);
 
#ifdef TDEBUG
  for(tvertex v = 1; v <= nv();v++)
      vlabel[v] = (long) dfsnum[v];
 #endif

  // Relabel thes edges and compute Dos
  svector<bool> visited(1,nv()); visited.SetName("visited");
  svector<tbrin> nel(-ne(),ne());  nel.SetName("nel");
  iel[0] = nel[0] = 0;
  nvin[0]=0;
  int occ = 0;
  tbrin y = 1; 
  tbrin z = nv();
  nel.clear();
 visited.clear();
 visited[v] = true;
 b = b0;
  do
      {tedge je = b.GetEdge();
      tvertex w=vin[-b];
      if(!isTree(je))  
          {if (nel[b] != 0)                                                                                                    // cotree -> 2 occ
              dos[++occ] = nel[b];
          else                                                                                                                   // cotree -> 1 occ
              {if (dfsnum[v] < dfsnum[w]) // lower cotree
                  {iel[z] = b;                             nel[b] = z;
                  iel[-z] = -b ;                           nel[-b] = -z;
                  nvin[z] = dfsnum[v]; nvin[-z] = dfsnum[w]; 
                  }
              else // upper cotree
                  {iel[-z] = b ;                           nel[b] = -z;
                  iel[z] = -b;                             nel[-b] = z;
                  nvin[z] = dfsnum[w]; nvin[-z] = dfsnum[v]; 
                  }
              dos[++occ] = nel[b];
              z++;
              }
          }
      else if (visited[w])                                                                                                      // tree 2 occ
          {dos[++occ] = nel[b];
          b.cross();
          v = w;
          }
      else                                                                                                                             // tree 1 occ
          {iel[y]=b;                 nel[b] = y; 
           iel[-y] = -b;             nel[-b] = -y;  
          dos[++occ] = nel[b];
          nvin[y]=dfsnum[v];  nvin[-y] = dfsnum[w] ;
          visited[w] = true;
          b.cross();
          y ++; 
          v = w;
#ifdef TDEBUG
          ecolor[b.GetEdge()] = Red;
#endif
          }
      b = acir[b];
      } while(b != b0);

#ifdef TDEBUG
  // elabel < 0 <=> 1 occ up cotree
  for(tedge je = 1; je <= ne();je++)
      elabel[je] = (long) nel[je].GetEdge()();
      
  for(tedge je = 1; je <= ne();je++)
      if(nvin[je] > nvin[-je])ok = false;
 if (y != nv() || occ != 2*ne()){printf("makeDosPR error\n"); ok = false;}
  if(!ok)return -1;
#endif
  if (y != nv() || occ != 2*ne()){DebugPrintf("makeDosPR error"); ok = false;return -1;}
  // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }

int DoccGraph::makeDosDFS()
  {svector<tbrin> tb(0,nv()); tb.clear(); tb.SetName("tb");
  svector<int> dfsnum(0,nv());  dfsnum.SetName("dfsnum");
  svector<tbrin> nel(-ne(),ne());  nel.SetName("nel");
#ifdef TDEBUG
  Prop<short> ecolor(Set(tedge()),PROP_COLOR);
  Prop<long> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<long> elabel(Set(tedge()),PROP_LABEL);
#endif
  iel[0] = nel[0] = 0;
  nvin[0]=0;
  tbrin b0 = acir[extbrin()];
  tbrin b = b0;
  tbrin y = 1;
  tbrin z = nv();
  tvertex w;
  tvertex v = vin[b0];
  ivl[1] = v;  
#ifdef TDEBUG
  vlabel[v] = 1L;
#endif
  tb[v]= b0;
  dfsnum[v]=1;
  int occ = 0;
  do
      {w=vin[-b];
      if(tb[w]!=0)            // w deja connu ?
          {if (b==tb[v])    // on descend sur l'abre 
              {dos[++occ] = nel[b];
              b.cross();
              v=w;
              }
          else if (dfsnum[v]<dfsnum[w]) // coarbre bas 
              {dos[++occ] = nel[b];
              }
          else // coarbre haut decouvert en 1
              {iel[-z] = b;                             nel[b] = -z;
              iel[z()] = -b ;                           nel[-b] = z;
              nvin[z]=dfsnum[w];              nvin[-z]=dfsnum[v];
              dos[++occ] = nel[b];
#ifdef TDEBUG
              elabel[b.GetEdge()] = (long)z();
#endif
              z++;
              }
          }
      else   // arbre bas 
          {iel[y]=b;                nel[b] = y; 
          nel[-b] = -y;            iel[-y] = -b;
          dos[++occ] = nel[b];
          b.cross();
          tb[w]=b;
          nvin[y]=dfsnum[v];
          y = nvin[-y]() = dfsnum[w] = y()+1;
          ivl[y] = (tvertex) w;  
#ifdef TDEBUG
          vlabel[w] = (long) y(); 
          elabel[b.GetEdge()] = (long)nel[b]();          
          ecolor[b.GetEdge()] = Red;
#endif
          v = w;
          }
      b = acir[b];
      } while(b != b0);

  if (y != nv() || z != ne()+1){DebugPrintf("GDFSRenum z=%d y=%d n=%d",z(),y(),nv());ok = false; return -1;}

  // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }
void  DoccGraph::htreePR()
  {int hh = 0;
  tbrin last = 0;
  int ix = 1;
  hauteur.clear();
  for(int i = 1;i <= 2*ne();++i)
      {tedge je = dos[i].GetEdge();
      if(je < nv())
          {if(dos[i] > 0)
              {hh += 2;hmax = max(hmax,hh);}
          else
              {hauteur[je] = hh;hh -= 2;}
          }
      if(( (last < 0)||(last >= nv()) ) && ( (dos[i] >= 0) || (dos[i] <= -nv())) )++ix;
      dosx[i] = ix;
      last = dos[i];
      }
  hmax += 2;
  }
bool  DoccGraph::horderPR(tbrin ee, tbrin es)
  {return (hauteur[nvin[ee] - 1] <  hauteur[nvin[es] - 1]);
  }
tbrin  DoccGraph::rightBrinPR(tedge e)
// si e a son brin bas a gauche -> brin haut
// si e a son brin bas a droite     -> brin bas
  {return(dosInv[e.firsttbrin() ] > dosInv[e.secondtbrin()] ? e.firsttbrin()  : e.secondtbrin());
  }
void DoccGraph::insertConstraint(tedge je,tedge js)
  { if(js == 0)js = ne()+1;
//   else if(je() >= nv())
//       printf("END  %d pushes %d (%d %d)\n",je(),js(),nvin[js](),nvin[-js]());
  MPATH->insert(je(),js(),1);
  ++numConstraint;
  }
void  DoccGraph::push1PR()
  {tedge js  = 0;
  tbrin es = 0; // right brin of js
  foc.clear();suc.clear();
  MPATH=new MaxPath(ne()+1,2*ne());
  numConstraint = 0;
  for(int j = 2*ne();j >= 1;--j)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      //qDebug("dos[%d]:%d",j,ee());
      if(je  >= nv())     // je  cotree 
          {++foc[je];
          if(je != js)
              {if(foc[je] != 2)  // premiere occurrence (en partant de la droite)
                  {if(dosInv[-ee] < dosInv[-es]  && horderPR(ee,-es) )
                      {//printf("P1 je:%d coupe es:%d (js=%d)\n",je(),es(),js());
                          js = je;es = ee;
                      }
                  else
                      {//printf("P1   %d pushes %d\n",je(),js());
                      suc[je] = js;
                      insertConstraint(je,js);
                      js = je; es = ee;
                      }
                  }
              }
          else  // deuxieme occurrence (en partant de la droite) et je == js
              {while(js > 0 && foc[js] == 2)
                  js = suc[js];
              es = rightBrinPR(js);
              }
          }
      else if(ee > 0) // je is tree
          {suc[ee] = js;
          insertConstraint(je,js);
          }
      }
  //for(tedge je = nv();je <= ne();je++)  if(suc[je]())printf("END1  %d pushes %d\n",je(),suc[je]());
  //vheightPR();
  //MPATH->solve(hauteur);
  //printf("END PUSH\n");
  }
bool  DoccGraph::stronger_constraintPR(tedge a, tedge b)
  {return (Max(hauteur[nvin[a] - 1], hauteur[nvin[-a] - 1]) <  Max(hauteur[nvin[b] - 1], hauteur[nvin[-b] - 1]));
  }
void  DoccGraph::push2PR()
// foc = 2 for all cotree edges
// je ne reinitialise pas suc pour verifier l'absence de circuits
  {suc[0] = 0;
  tedge js = 0;
  tbrin es = 0; // left  brin of js
  tedge je;
  for(int j = 1;j <= 2*ne();j++)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je >= nv())     // je is cotree
          {--foc[je];
          if(je != js)
              {//qDebug("   je:%d sucje:%d js:%d",je(),suc[je](),js());
              if(foc[je] != 0)  // premiere occurrence
                  {if(es() && dosInv[-ee] > dosInv[-es] 
                      && horderPR(es, ee) && horderPR(ee, -es) && horderPR(-es,- ee))
                      {
//                       printf("ee=%d  (%d %d) es=%d (%d %d) js=%d sucjs=%d\n"
//                              ,ee(),nvin[ee](),nvin[-ee](),es(),nvin[es](),nvin[-es](),js(),suc[js]());
//                       if(suc[js]() && Max(dosInv[suc[js]] ,dosInv[-suc[js]]) > j 
//                          && Max(dosInv[suc[js]] ,dosInv[-suc[js]])  < dosInv[-je]) 
//                           {insertConstraint(je,suc[js]);
//                           }
                      //insertConstraint(js,je);
                      insertConstraint(je,js); //ok
                      suc[js] = je; 
                      //suc[je] = js; 
                      js = je;es = ee;
                      }
//                   else if(js() && suc[je]() &&  dosInv[-js] > dosInv[-ee] && stronger_constraintPR(js, suc[je]))
//                       {//suc[je] = js;
//                       js = je;es = ee;
//                       }
                  else
                      {insertConstraint(je,js);
                      suc[je] = js;
                      js = je;es = ee;
                      }
                  }
              }
          else // deuxieme occurrence
              {while(js != 0 && foc[js] == 0)
                  js = suc[js];
              es = -rightBrinPR(js); 
              }
          }
      else if(ee < 0) // je is tree
          {suc[ee] = js;
          insertConstraint(je,js);
          }
      }

 //  for(tedge je = nv();je <= ne();je++)  
//       if(suc[je]())printf("END  %d pushes %d (%d %d)\n",je(),suc[je](),nvin[suc[je]](),nvin[-suc[je]]());
  MPATH->solve(hauteur);
  delete  MPATH;
  hmax = Max(hmax,hauteur[ne()+1] +1);
  //vheightPR();
  // printf("end  Maxpath:%d (m=%d)---------------------------\n",numConstraint,ne());
  }
 void  DoccGraph::lrverPR()
  {for(int i = 1;i <= 2*ne();++i)
      {tbrin ee = dos[i];
      tedge je = ee.GetEdge();
      if(iel[je].GetEdge() > morg)continue;
      tvertex v = nvin[ee];
      if(lver[v] == 0)
          {if(je() < nv())
              lver[v] = dosInv[je];
          else
              lver[v] = i;
          rver[v] = lver[v];
          }
      else
          {if(je() < nv())
              rver[v] = Max(rver[v],dosInv[je]);
          else
              rver[v] = i;
          }
      }
  // for vertices originally isolated
for(int i = 1;i <= 2*ne();++i)
      {tbrin ee = dos[i];
      tedge je = ee.GetEdge();
      tvertex v = nvin[ee];
      if(lver[v] != 0)continue;
      if(lver[v] == 0)
          {if(je() < nv())
              lver[v] = dosInv[je];
          else
              lver[v] = i;
          rver[v] = lver[v];
          }
      }
  }
int  DoccGraph::drawPR()
  {Prop1<Tpoint> pmin(Set(),PROP_POINT_MIN);
  Prop1<Tpoint> pmax(Set(),PROP_POINT_MAX);
  pmin() = Tpoint(.0,-1.5);
  pmax() = Tpoint(dosx[2*ne()]+1.,hmax+.5);

  Prop<Tpoint> p1(Set(tvertex()),PROP_DRAW_POINT_1); p1.SetName("p1");
  Prop<Tpoint> p2(Set(tvertex()),PROP_DRAW_POINT_2); p2.SetName("p2");
  Prop<double> x1(Set(tedge()),PROP_DRAW_DBLE_1); x1.SetName("x1");
  Prop<double> x2(Set(tedge()),PROP_DRAW_DBLE_2); x2.SetName("x2");
  Prop<double> y1(Set(tedge()),PROP_DRAW_DBLE_3); y1.SetName("y1");
  Prop<double> y2(Set(tedge()),PROP_DRAW_DBLE_4); y2.SetName("y2");
  Prop<double> y(Set(tedge()),PROP_DRAW_DBLE_5); y.SetName("y");
  Prop<bool> isTree(Set(tedge()),PROP_ISTREE); isTree.SetName("isTree");
  isTree.clear();

  // vertex plotting
  for(tvertex v = 1;v <= nv();++v)
        {double xv1 = (double)dosx[lver[v]] - .4;
        double yv1 =  (double)hauteur[v-1] + .5; // point haut-gauche du sommet
        double xv2 = (double)dosx[rver[v]] + .4;
        p1[ivl[v]] = Tpoint(xv1,yv1);        p2 [ivl[v]] = Tpoint(xv2,yv1);
        }
  // tree plotting 
  for(tedge je = 1;je <= nv()-1 ;++je)
      {tedge ie = iel[je].GetEdge();
      isTree[ie] = true;
      x1[ie] = (double)dosx[dosInv[je]]; x2[ie] = -1;
      y2[ie] = (double)hauteur[je] -.5;
      y1[ie] =  y2[ie] - 1.;
      }
  // cotree plotting (x1,y1) -> (x1,y) -> (x2,y) -> (x2,y2)
  for(tedge je = nv();je <= ne();++je)
        {tedge ie = iel[je].GetEdge();
        x1[ie] = (double)dosx[dosInv[je]];
        x2[ie] = (double)dosx[dosInv[-je]];
        y1[ie] = (double)hauteur[nvin[dos[dosInv[je]]] - 1] +.5;
        y2[ie] = (double)hauteur[nvin[dos[dosInv[-je]]] - 1] +.5;
        y[ie] = (double)hauteur[je] +.5-.3;
        if(y[ie] <= Max( y1[ie], y2[ie]))ok = false;
        }
  // delete the edges added by Connexity
  for(tedge e = ne();e > morg;e--) DeleteEdge(e);
  return 0;
  }

int EmbedPolrecGeneral(TopologicalGraph &G,int type)
// type == 0 -> polerc    
// type == 1 -> polercLR
  {int morg = G.ne();
  G.MakeConnected();
  svector<tbrin> cir0,acir0;
  if(type == 1) // save the cir
      {cir0 = G.cir; acir0 = G.acir;
      G.Planarity();
      }
  DoccGraph DG(G,morg,type);
  if(!DG.ok)
      {setError(-1,"ERROR: polrec construction");printf("ERROR: polrec");
      if(type == 1) {G.cir.vector() = cir0; G.acir.vector() = acir0;}
      return -1;
      }
  DG.htreePR();
  DG.push1PR();
  DG.push2PR();
  DG.lrverPR();
  DG.drawPR();
  if(type == 1) {G.cir.vector() = cir0; G.acir.vector() = acir0;}
#ifndef TDEBUG
  if(!DG.ok){setError(-1,"ERROR: polrec");printf("ERROR: polrec");return -1;}
#endif
  return 0;
  }    
int EmbedPolrec(TopologicalGraph &G)
  {return  EmbedPolrecGeneral(G,0);
  }
int EmbedPolrecLR(TopologicalGraph &G)
  {return  EmbedPolrecGeneral(G,1);
  }

// not used now
void  DoccGraph::pushSPR()
  {tedge js  = 0;
  foc.clear();suc.clear();
 
  for(int j = 2*ne();j >= 1;--j)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je  >= nv())     // je  cotree 
          {++foc[je];
          if(je != js)
              {if(foc[je] != 2)  // premiere occurrence (en partant de la droite)
                  {suc[je] = js;js = je;}
              }
          else  // deuxieme occurrence (en partant de la droite) et je == js
              while(js > 0 && foc[js] == 2)js = suc[js];
          }
      else  // je is tree
          suc[ee] = js;
      }
  vheightPR();
  }
void  DoccGraph::vheightPR()
  {MaxPath *MP=new MaxPath(ne()+1,nv()+ne());
  for(tbrin  je = -nv()+1;je <= ne();je++)
      {if(je == 0)continue;
      tedge js = suc[je];
      if(js == 0)js = ne()+1;
      MP->insert(je.GetEdge()(),js(),1);
      //if(je.GetEdge()() > nv())qDebug("MP %d pushes %d",je.GetEdge()(),js());
      }
  MP->solve(hauteur);
  delete  MP;
  hmax = Max(hmax,hauteur[ne()+1] +1);
  }
