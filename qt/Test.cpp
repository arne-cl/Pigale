#include <qapplication.h>
#include <Pigale.h> 


void Test(GraphContainer &GC,int action)
  {GeometricGraph G(GC);
  if(action == 1)
      {qDebug("Number of vertices:%d",G.nv());
      qDebug("Number of edges:%d",G.ne());
      for(tvertex v = 1; v <= G.nv();v++)
	  qDebug("vertex:%3.3d %3.0f %3.0f",v(),G.vcoord[v].x(),G.vcoord[v].y());
      }
  if(action == 2)//print incidences
      {qDebug("Number of vertices:%d",G.nv());
      qDebug("Number of edges:%d",G.ne());
      for(tedge e = 1; e <= G.ne();e++)
	  qDebug("edge:%3.3d  %3.3d %3.3d",e(),G.vin[e](),G.vin[-e]());
      return;
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
  }

