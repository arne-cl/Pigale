#include <qapplication.h>
#include <TAXI/graphs.h> 


void Test(GraphContainer &GC,int action)
  {GeometricGraph G(GC);
  if(action == 1)
      {qDebug("Number of vertices:%d",G.nv());
      qDebug("Number of edges:%d",G.ne());
      }
  if(action == 2)//print incidences
      {qDebug("Number of vertices:%d",G.nv());
      qDebug("Number of edges:%d",G.ne());
      for(tedge e = 1; e <= G.ne();e++)
	  qDebug("edge:%3.3d  %3.3d %3.3d",e(),G.vin[e](),G.vin[-e]());
      return;
      }
  if(action == 3) //print coordinates
      {for(tvertex v = 1; v <= G.nv();v++)
	  qDebug("vertex:%3.3d %3.0f %3.0f",v(),G.vcoord[v].x(),G.vcoord[v].y());
      }
  }
