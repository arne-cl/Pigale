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
#ifndef NETCUT_H
#define NETCUT_H
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <TAXI/graphs.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tpoint.h>


class SplitGraph : public TopologicalGraph
    {public :
        Prop<short> vcolor;
        Prop<long> vlabel;
        Prop<int> ewidth;
        Prop<short> ecolor;
        Prop<long> elabel;
        svector<int> ClassNumber;             // Numero classe d'un sommet
        svector<int> NumberElementsInClass;
        svector<int> degree;
        svector<Tpoint3> Coord3;
        int NumberOfClasses,MaxDimension,CurrentNumberOfClasses;
        svector<Tpoint3> BaryCoord3;
        svector<double> EigenValues;
        int **vvadj;                      // vertex/vertex sorted adjacency
        double **Distances;               // distances: carre distance euclidienne
        double **Coords;                  // Coordinates in R (n-1)  
        double MaxDistance;               // Distance max au centre


        SplitGraph(Graph &G,int numclasses,int maxdim) :
            TopologicalGraph(G),
            vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL),
            ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1),
            elabel(G.PE(),PROP_LABEL),
            NumberOfClasses(numclasses),MaxDimension(maxdim),CurrentNumberOfClasses(0)
            ,vvadj(NULL),Distances(NULL),Coords(NULL),MaxDistance(.0)
            {init();}
      ~SplitGraph() {release();}

        int ComputeAdjMatrix();
        int ComputeDistances();
        double ComputeDistance(int vertex1,int vertex2);
        void ComputeMaxDistance3d();
        
    private :
        void init();
        void release();
    };



// DIAG.CPP
int diag(double **Coords,int nb_vertex,double **Distances,svector<double>& EigenValues);
#endif
