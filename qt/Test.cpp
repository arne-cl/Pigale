#include <qapplication.h>
#include <Pigale.h> 
#include "MyWindow.h" 
#include <QT/Misc.h> 


int Test(GraphContainer &GC,int action)
  {TopologicalGraph G(GC);
  GeometricGraph GG(GC);
  int err = 0;
  if(action == 1)
      {tbrin first = 1;
      if(!G.CheckConnected())G.MakeConnected();
      if(!G.CheckBiconnected())G.NpBiconnect();
      NPBipolar(G,first); 
      int ns,nt;
      G.CheckAcyclic(ns,nt);
      if(ns !=1 || nt !=1)setError(-12345,"error bipolar");
      return 0;
      }
  if(action == 2)
      {for(int i=1; i<1000;i++)
	  err = G.TestPlanar();
      if(err != 1)
	  {DebugPrintf("Test TestPlanar():%s",(const char *)getErrorString());
	  setError(-12345); 
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

