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

#ifndef _LEDACONV_
#define _LEDACONV_

#include <iostream.h>

#include <TAXI/color.h>
#include <LEDA/graph.h>
#include <LEDA/point.h>

#include <TAXI/graphs.h>


struct NodeInfo
    {leda_point offset;
    int    label;
    short  col;

    NodeInfo() : offset(0,0),label(0), col(Yellow) {}
    };

struct EdgeInfo
    {int   label;
    short  col;
    int    width;

    EdgeInfo() : col(Black), width(1){}
    };

inline  ostream& operator <<(ostream& out,const EdgeInfo x)
    {out << *((int *)((void *)&x)) ;return(out);}
inline  istream& operator >>(istream& in,const EdgeInfo& x)
     {in >> *((int *)((void *)&x));return(in); }

inline  ostream& operator <<(ostream& out,const NodeInfo x)
    {out << *((int *)((void *)&x)) ;return(out);}
inline  istream& operator >>(istream& in,const NodeInfo& x)
     {in >> *((int *)((void *)&x));return(in); }


class LedaGraph : public GRAPH<NodeInfo,EdgeInfo>
        {public:
                LedaGraph(){}
                LedaGraph(Graph &G);
               ~LedaGraph(){}
        };

void GraphToLedaGraph(leda_graph& ledaG, Graph &G);
void LedaGraphToGraph(GraphContainer &G,leda_graph &ledaG);
void LedaGraphToGraph(Graph &G,LedaGraph& ledaG);
void GraphToLedaGraph(LedaGraph& ledaG, Graph &G);
void ImportColorWidthFromLedaGraph(GeometricGraph &G,LedaGraph& ledaG);

template <class T>
void PastePropToLedaArray(Graph &G,leda_graph& LedaG,int PNum
                          ,leda_edge_array<T> &LedaArray, const T &defvalue)
    { Prop<T> P(G.Set(tedge()),PNum,defvalue);
    Prop<leda_edge> label(G.Set(tedge()),PROP_LABEL,leda_edge(0));
    int i;
    leda_edge e;
    forall_edges(e, LedaG)
        LedaArray[e] = defvalue;
    for (i=1; i<=G.ne(); i++)
    	if (label[i]!=edge(0)) LedaArray[label[i]]=P[i];
    }
template <class T>
void PastePropToLedaArray(Graph &G, leda_graph& LedaG,int PNum
                          ,leda_node_array<T> &LedaArray, const T &defvalue)
    { Prop<T> P(G.Set(tvertex()),PNum,defvalue);
    Prop<leda_node> label(G.Set(tvertex()),PROP_LABEL,leda_node(0));
    int i;
    leda_node v;
    forall_nodes(v, LedaG)
        LedaArray[v] = defvalue;
    for (i=1; i<=G.nv(); i++)
    	if (label[i]!=node(0)) LedaArray[label[i]]=P[i];
    }
template <class T>
void PasteVectorToLedaArray(Graph &G,leda_graph& LedaG, const svector<T> &P
                            ,leda_edge_array<T> &LedaArray, const T &defvalue)
    {Prop<leda_edge> label(G.Set(tedge()),PROP_LABEL,leda_edge(0));
    int i;
    leda_edge e;
    forall_edges(e, LedaG)
        LedaArray[e] = defvalue;
    for (i=1; i<=G.ne(); i++)
    	if (label[i]!=leda_edge(0)) LedaArray[label[i]]=P[i];
    }
template <class T>
void PasteVectorToLedaArray(Graph &G,leda_graph& LedaG, const svector<T> &P
                            , leda_node_array<T> &LedaArray, const T &defvalue)
    {Prop<leda_node> label(G.Set(tvertex()),PROP_LABEL,leda_node(0));
    int i;
    leda_node v;
    forall_nodes(e, LedaG)
        LedaArray[e] = defvalue;
    for (i=1; i<=G.nv(); i++)
    	if (label[i]!=leda_node(0)) LedaArray[label[i]]=P[i];
    }
#endif

