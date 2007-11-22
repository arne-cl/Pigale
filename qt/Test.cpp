


#include "GraphWidget.h"
#include "pigaleWindow.h"
#include <QT/Action_def.h> 
#include <QT/Misc.h> 
#include <QT/pigaleCanvas.h>
#include <QT/GraphWidgetPrivate.h>

using namespace std;


// Allowed return values of Test 
// -1:error 0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
// 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders

inline double abs(double x) {if(x>=0) return x; else return -x;}
static int Test1(GraphContainer &GC,int &drawing);
static int Test2(GraphContainer &GC,int &drawing);
static int Test3(GraphContainer &GC,int &drawing);

void pigaleWindow:: initMenuTest()
  {
#ifdef VERSION_ALPHA
  setUserMenu(1,"Speed: TestPlanar/NewTestPlanar (1000x) ");
  setUserMenu(2,"1000xNewPlanarity");
  setUserMenu(3,"Test planarity algos");
#else
  setUserMenu(1,"Speed: TestPlanar/NewTestPlanar (1000x)");
  setUserMenu(2,"1000xTesNewtPlanarity");
  setUserMenu(3,"Properties");
#endif
  }
int Test(GraphContainer &GC,int action,int &drawing)
  {//cout <<"test:"<<action<<endl;
  if(debug())DebugPrintf("Executing Test:%d",action);
  int ret;
  if(action == 1)ret= Test1(GC,drawing);
  else if(action == 2)ret = Test2(GC,drawing);
  else ret =  Test3(GC,drawing);
  if(debug())DebugPrintf("    END executing Test:%d",action);
  return ret;
  }
 
int Test1(GraphContainer &GC,int &drawing)
  {drawing = 0;
  TopologicalGraph G(GC);
  bool _debug = debug();
  QTime timer;timer.start();
  for(int i = 0;i < 1000;i++){shuffleCir(G);G.TestPlanar();}
  double Time1 = timer.elapsed(); // millisec
  if(Time1 < 10)
      {Tprintf("too short time to measure");
      debug() = _debug;
      return 1;
      }
  timer.start();
  for(int i = 0;i < 1000;i++){shuffleCir(G);G.TestNewPlanar();}
  double Time2 = timer.elapsed();
  Tprintf("speedup: %2.0f %% (>0 better)",100. - 100.*Time2/Time1);
  debug() = _debug;
  return 1;
  }
int Test2(GraphContainer &GC,int &drawing)
  {TopologicalGraph G(GC);
  bool _debug = debug();
  shuffleCir(G);
  QTime timer;timer.start();
  for(int i = 0;i < 1000;i++)G.TestNewPlanar();
  Tprintf("Used time : %f",timer.elapsed()/1000.);
  debug() = _debug;
  drawing = 0;
  return 0;
  }

/******************************************************************************************************/
/******************************************************************************************************/
#ifdef VERSION_ALPHA
int Test3(GraphContainer &GC,int &drawing)
  {drawing = 0;
  TopologicalGraph G(GC);
  tvertex v1 = randomGet(G.nv());
  tvertex v2 = randomGet(G.nv());
  if(v1 != v2)G.NewEdge(v1,v2);
  shuffleCir(G);
  int ret0 = G.TestPlanar(); 
  int ret1 = G.TestNewPlanar();
  int ret2 = G.NewPlanarity(randomGet(G.ne()));
  if(ret0 != ret1)
      {setError(-1,"Error TestNewPlanar");
	cout << ret0 << " " << ret1 << endl;
	return 2;
      }
  if(ret0 != ret2)
      {setError(-1,"Error NewPlanarity");
      cout << ret0 << " " << ret1 << endl;
      return 2;
      }
  return 2;
  }
#else 

int Test3(GraphContainer &GC,int &drawing)
// display  the properties of the current graph that would be saved in a tgf file.
  {TopologicalGraph G(GC);
  Tprintf("\nVertices:");
  int i;
  for (i=G.Set(tvertex()).PStart(); i<G.Set(tvertex()).PEnd(); i++)
      if (G.Set(tvertex()).exist(i))
          if (G.Set(tvertex()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(1,i),PropDesc(1,i),
                     (1+G.nv())*G.Set(tvertex())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(1,i),PropDesc(1,i));
  Tprintf("\nEdges:");
  for (i=G.Set(tedge()).PStart(); i<G.Set(tedge()).PEnd(); i++)
      if (G.Set(tedge()).exist(i))
          if (G.Set(tedge()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(2,i),PropDesc(2,i),
                     (1+G.ne())*G.Set(tedge())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(2,i),PropDesc(2,i));
  Tprintf("\nBrins (half-edges):");
  for (i=G.Set(tbrin()).PStart(); i<G.Set(tbrin()).PEnd(); i++)
      if (G.Set(tbrin()).exist(i))
          if (G.Set(tbrin()).defined(i)) 
              Tprintf("\n%d %s \n (%s) -> %d bytes",i,PropName(3,i),PropDesc(3,i),
                     (1+2*G.ne())*G.Set(tbrin())(i)->size_elmt());
          else
              Tprintf("\n%d %s \n (%s)",i,PropName(3,i),PropDesc(3,i));
  Tprintf("\nGeneral:");
  for (i=G.Set().PStart(); i<G.Set().PEnd(); i++)
      if (G.Set().exist(i))
          Tprintf("\n%d %s \n (%s)",i,PropName(0,i),PropDesc(0,i));
  drawing = 0;
  return 0;
  }

#endif











