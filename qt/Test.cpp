#include "MyWindow.h" 
#include <QT/Misc.h> 
#include <qapplication.h>

using namespace std;

int Test(GraphContainer &GC,int action)
  {TopologicalGraph G(GC);
  GeometricGraph GG(GC);
  int err = 0;
  
  if(action == 1)
      {GG.ComputeGeometricCir();
      NPBipolar(G,1); 
      G.FixOrientation();
      int ns,nt;
      bool acyclic = G.CheckAcyclic(ns,nt);
      ColorPoles(GG);
      if(ns !=1 || nt !=1 || !acyclic)
	  {static QString t;
	  t.sprintf("Error bipolar:s=%d t=%d a=%d",ns,nt,(int)acyclic);
	  setError(-12345,(const char *)t);
	  }
      return 0;
      }
  if(action == 2)
      {if(G.TestPlanar() != G.TestPlanar2())err = 1;
      if(err == 1)
	  {if(getError())DebugPrintf("Planar2!=Planarity:%s",(const char *)getErrorString());
	  setError(-12345,"is planar"); 
	  }
      return 0;
      }
  if(action == 3) 
      // display  the properties of the current graph that would be saved in a tgf file.
    {qDebug("Vertices:");
    for (int i=G.Set(tvertex()).PStart(); i<G.Set(tvertex()).PEnd(); i++)
      if (G.Set(tvertex()).exist(i))
	      if (G.Set(tvertex()).defined(i)) 
		  qDebug("  %d %s (%s) -> %d bytes",i,PropName(1,i),PropDesc(1,i),
			 (1+G.nv())*G.Set(tvertex())(i)->size_elmt());
	      else
		  qDebug("  %d %s (%s)",i,PropName(1,i),PropDesc(1,i));
    qDebug("Edges:");
    for (int i=G.Set(tedge()).PStart(); i<G.Set(tedge()).PEnd(); i++)
      if (G.Set(tedge()).exist(i))
	      if (G.Set(tedge()).defined(i)) 
		  qDebug("  %d %s (%s) -> %d bytes",i,PropName(2,i),PropDesc(2,i),
			 (1+G.ne())*G.Set(tedge())(i)->size_elmt());
	      else
		  qDebug("  %d %s (%s)",i,PropName(2,i),PropDesc(2,i));
    qDebug("Brins (half-edges):");
    for (int i=G.Set(tbrin()).PStart(); i<G.Set(tbrin()).PEnd(); i++)
      if (G.Set(tbrin()).exist(i))
	      if (G.Set(tbrin()).defined(i)) 
		  qDebug("  %d %s (%s) -> %d bytes",i,PropName(3,i),PropDesc(3,i),
			 (1+2*G.ne())*G.Set(tbrin())(i)->size_elmt());
	      else
		  qDebug("  %d %s (%s)",i,PropName(3,i),PropDesc(3,i));
    qDebug("General:");
    for (int i=G.Set().PStart(); i<G.Set().PEnd(); i++)
      if (G.Set().exist(i))
	  qDebug("  %d %s (%s)",i,PropName(0,i),PropDesc(0,i));
    }
  return 0;
  }

