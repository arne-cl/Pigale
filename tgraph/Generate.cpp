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



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <TAXI/Tpoint.h>
#include <TAXI/Tdebug.h>
#include <TAXI/graphs.h>

GraphContainer *GenerateGrid(int a, int b)
  {if(debug())DebugPrintf("GenerateGrid");   
  GraphContainer &GC = *new GraphContainer;
  int n=a*b;
  int m=2*n-a-b;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"%d x %d Grid", a,b);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<long> vlabel(GC.PV(),PROP_LABEL);
  Prop<long> elabel(GC.PE(),PROP_LABEL);
  Prop<tbrin> cir(GC.PB(),PROP_CIR); cir[0]=0;
  Prop<tbrin> acir(GC.PB(),PROP_ACIR); acir[0]=0;
  Prop<tbrin> pbrin(GC.PV(),PROP_PBRIN); pbrin[0]=0;
  tvertex v;
  tedge e;
  for (v=0; v<=n; v++)
    vlabel[v]=v();
  for (e=0; e<=m; e++)
    elabel[e]=e();
  int aa=a-1; int bb=b-1;
  int x; int y;
  tbrin bs[4];
  for (int j=0; j<b; j++)
    {for (int i=0; i<a; i++)
      {int nb=0;
      x=i*b+j+1; y=i+j*a+aa*b+1; v=i+a*j+1;
      vcoord[v]=Tpoint(i,j);
      if (i<aa)	bs[nb++]=x;
      if (j<bb)	bs[nb++]=y;
      if (i>0)	bs[nb++]=b-x;
      if (j>0)	bs[nb++]=a-y;
      nb--;
      for (int k=0; k<nb;k++)
	{cir[bs[k]]=bs[k+1];acir[bs[k+1]]=bs[k];vin[bs[k]]=v;}
      cir[bs[nb]]=bs[0];acir[bs[0]]=bs[nb];vin[bs[nb]]=v;
      pbrin[v]=bs[0];
      }
    }
  return &GC;
}
GraphContainer *GenerateCompleteGraph(int a)
  {if(debug())DebugPrintf("GenerateCompleteGraph");   
  GraphContainer &GC = *new GraphContainer;
  int n=a;
  int m=a*(a-1)/2;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"K%d", a);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<long> vlabel(GC.PV(),PROP_LABEL);
  Prop<long> elabel(GC.PE(),PROP_LABEL);
  tvertex v;
  tedge e;
  vlabel[0]=0;
  double angle = 2.*acos(-1.)/a;
  for (v=1; v<=n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();
  tbrin b=1;
  for (v=1; v<a; v++)
    for (tvertex vv=v+1; vv<=a; vv++)
      {vin[b]=v;vin[-b]=vv; b++;}
  TopologicalGraph TG(GC);
  return &GC;
}
GraphContainer *GenerateCompleteBiGraph(int a,int b)
  {if(debug())DebugPrintf("GenerateCompleteBiGraph");   
  GraphContainer &GC = *new GraphContainer;
  int n=a+b;
  int m=a*b;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char t[256];
  sprintf(t,"K%d,%d", a,b);
  title() = t;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<long> vlabel(GC.PV(),PROP_LABEL);
  Prop<long> elabel(GC.PE(),PROP_LABEL);
  tvertex v;
  tedge e;
  vlabel[0]=0;
  for (v=1; v<=a; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(0,v()-a/2.0);
    }
  for (v=a+1; v<=a+b; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(1,v()-b/2.0-a);
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();
  tbrin bb=1;
  for (v=1; v<=a; v++)
    for (tvertex vv=a+1; vv<=a+b; vv++)
      {vin[bb]=v;vin[-bb]=vv; bb++;}
  TopologicalGraph TG(GC);
  return &GC;
}

static int random(int range) 
//returns a positve integer < range
  {return (int)( ((rand())&0x7FFFFFFF) %(long)range);
  }
GraphContainer *GenerateRandomGraph(int a,int b)
  {if(debug())DebugPrintf("GenerateRandomGraph");  
  GraphContainer &GC = *new GraphContainer;
  int n = a;
  int m = b;
  GC.setsize(n,m);
  Prop1<tstring> title(GC.Set(),PROP_TITRE);
  char titre[256];
  sprintf(titre,"Random%d,%d", a,b);
  title() = titre;
  Prop<tvertex> vin(GC.PB(),PROP_VIN); vin[0]=0;
  Prop<Tpoint> vcoord(GC.PV(),PROP_COORD);
  Prop<long> vlabel(GC.PV(),PROP_LABEL);
  Prop<long> elabel(GC.PE(),PROP_LABEL);

  tvertex v,w;
  tedge e;
  vlabel[0]=0;
 double angle = 2.*acos(-1.)/n;
  for(v=1; v <= n; v++)
    {vlabel[v]=v();
    vcoord[v]=Tpoint(cos(angle*(v()-1)),sin(angle*(v()-1)));
    }
  for (e=0; e<=m; e++)
    elabel[e]=e();
  //init random seed
  time_t time_seed;
  time(&time_seed);
  srand(time_seed);
  tbrin bb=1;
  do
      {while((v = random(n+1)) == 0);
      while((w = random(n+1)) == 0 || w == v);
      vin[bb] = v;vin[-bb] = w;
      }while(bb++ < m);
      
  TopologicalGraph TG(GC);
  return &GC;
}
