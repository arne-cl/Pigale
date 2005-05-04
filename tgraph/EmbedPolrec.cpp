#include <Pigale.h>
#include <TAXI/MaxPath.h>

class DosGraph : public TopologicalGraph
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
  int makeDos(tbrin b0);

 public:
  DosGraph(GraphAccess &G,int _morg,tbrin b0) : TopologicalGraph(G),hmax(0),morg(_morg),ok(true)
      {init();
      if(makeDos(b0) != 0)ok = false;
      }
  ~DosGraph(){}
  void htreePR();
  int  orderPR(tbrin ee, tbrin es);
  tbrin  maxedgePR(tedge e);
  void pushPR();
  bool  stronger_constraintPR(tedge js, tedge sucje);
  void push2PR();
  void  vheightPR();
  void  lrverPR();
  int drawPR();
};
void DosGraph::init()
  {int m = ne(); int n = nv();
  nvin.resize(-m,m); nvin.SetName("DosG:nvin");
  iel.resize(-m,m); iel.SetName("DosG:iel");
  ivl.resize(1,n); ivl.SetName("DosG:ivl");
  dos.resize(1,2*m);  dos.SetName("DosG:dos");
 dosInv.resize(-m,m);  dosInv.SetName("DosG:dosInv");
 dosx.resize(1,2*m);  dosx.SetName("DosG:dosx"); 
  suc.resize(-n+1,m);  suc.SetName("DosG:suc");  suc.clear();
  foc.resize(n,m);  foc.SetName("DosG:foc");  foc.clear();
  hauteur.resize(0,m+1);  hauteur.SetName("DosG:hauteur"); hauteur.clear();
  lver.resize(1,n);  lver.SetName("DosG:lver");lver.clear();
  rver.resize(1,n);  rver.SetName("DosG:rver");
  }
int DosGraph::makeDos(tbrin b0)
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

  if (y != nv() || z != ne()+1){DebugPrintf("GDFSRenum z=%d y=%d n=%d",z(),y(),nv()); return -1;}

  // Compute dosInv  
  dosInv[0] = 0;
  for(int i = 1;i <= 2*ne();++i)       
      dosInv[dos[i]] = i;
  return 0;
  }
void  DosGraph::htreePR()
  {int hh = 0;
  tbrin last = 0;
  int ix = 1;
 
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
int  DosGraph::orderPR(tbrin ee, tbrin es)
  {return(hauteur[nvin[-es] - 1] - hauteur[nvin[ee] - 1]);
  }
tbrin  DosGraph::maxedgePR(tedge e)
  {return(dosInv[e.firsttbrin() ] > dosInv[e.secondtbrin()] ? e.firsttbrin()  : e.secondtbrin());
  }
void  DosGraph::pushPR()
  {tedge js  = 0;
  tbrin es = 0;
  for(int j = 2*ne();j >= 1;--j)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je  >= nv())     // je  cotree 
          {++foc[je];
          if(je != js)
              {if(foc[je] != 2)
                  {if((dosInv[-ee] < dosInv[-es])  && orderPR(ee, es) >= 0) // je coupe es 
                      {js = je;es = ee;}
                  else
                      {suc[je] = js;js = je;es = ee;}
                  }
              }
          else
              while(js > 0 && foc[js] == 2)
                  {js = suc[js];      es = maxedgePR(js);}
          }
      else if(ee > 0)  // je is tree
          suc[ee] = js;
      }
  }
void  DosGraph::push2PR()
// foc = 2 for all cotree edges
  {suc[0] = 0;
  tedge js = 0;
  tbrin es = 0;
  tedge je;

  for(int j = 1;j <= 2*ne();j++)
      {tbrin ee = dos[j];
      tedge je = ee.GetEdge();
      if(je >= nv())     // je is cotree
          {--foc[je];
          if(je != js)
              {if(foc[je] != 0)
                  {if(es() == 0)
                      {suc[je] = js;js = je;es = ee;}
                  else if( (dosInv[-ee] > dosInv[-es]   && orderPR(ee, es) >= 0)) // je coupe es et je doit croiser pendant la verticale de js 
                      {js = je;es = ee;}
                  else
                      {suc[je] = js;js = je;es = ee;}
                  }
              }
          else
              {while(js != 0 && foc[js] == 0)
                  {js = suc[js];es = -maxedgePR(js);}
              }
          }
      else if(ee < 0) // je is cotree
          suc[ee] = js;
      }
  }

void  DosGraph::vheightPR()
  {MaxPath *MP=new MaxPath(ne()+1,nv()+ne());
  for(tbrin  je = -nv()+1;je <= ne();je++)
      {if(je == 0)continue;
      tedge js = suc[je];
      if(js == 0)js = ne()+1;
      MP->insert(je.GetEdge()(),js(),1);
      }
  MP->solve(hauteur);
  delete  MP;
  hmax = Max(hmax,hauteur[ne()+1] +1);
  }
 void  DosGraph::lrverPR()
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
int  DosGraph::drawPR()
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
        y[ie] = (double)hauteur[je] +.5;
#ifdef TDEBUG
        if(y[ie] <= Max( y1[ie], y2[ie]))ok = false;
#endif
        }
  // delete the edges added by Connexity
  for(tedge e = ne();e > morg;e--) DeleteEdge(e);
  if(!ok)return -1;
  return 0;
  }
int EmbedPolrec(TopologicalGraph &G)
  {int morg = G.ne();
  G.MakeConnected();
  //G.Planarity();//provisoire
  tbrin b0 = G.extbrin();
  DosGraph DG(G,morg,b0);
  if(!DG.ok)return -1;
  DG.htreePR();
  DG.pushPR();
  DG.push2PR();
  DG.vheightPR();
  DG. lrverPR();
  if(DG.drawPR() != 0)return -1;
  return 0;
  }
