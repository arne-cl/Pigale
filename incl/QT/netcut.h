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

#define MAX_ITERATION 10
#define SEUIL 0.00001

//in QtMisc.cpp
int & useDistance();

template <class T> class TriangularMatrix 
{private:
  svector<T> buffer;
  int Index(int i,int j) 
      {int a,b;
      if(i == j)Twait("ERROR ACCESS  TriangularMatrix ");
      if (i>j) {a=i; b=j;}
      else {a=j; b=i;}
      return a*(a-1)/2+b;
      }

public:
  TriangularMatrix(int n): buffer(n*(n-1)/2) {}
  void resize(int n) {buffer.resize(0,n*(n-1)/2-1);}
  void clear() {buffer.clear();}
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};

template <class T> class RectangularMatrix 
{
  svector<T> buffer;
  int ncols;
  int Index(int i,int j)
      {return i*ncols+j;
      }

public:
  RectangularMatrix(int nr,int nc): buffer(nr*nc), ncols(nc) {}
  void clear() {buffer.clear();}
  void resize(int nr, int nc) {buffer.resize(0,nr*nc-1); ncols=nc;}
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};

struct Classifier {
  svector<int> Class;
  svector<int> Cardinal;

  Classifier(int n, int noc): Class(n), Cardinal(noc) {}
  void clear() {Class.clear(); Cardinal.clear();}
  void affect(int i,int c) {Class[i]=c; Cardinal[c]++;}
};

struct Locals 
{
  TriangularMatrix<double> ProjectDist; // distances dans sous espace factoriel
  RectangularMatrix<double> BaryCoord; // coordonnées des barycentres
  Classifier Part;

  Locals(int n, int ms, int noc): ProjectDist(n),BaryCoord(ms,n-1),Part(n,noc)
      {ProjectDist.clear();}
};


class EmbedRnGraph : public TopologicalGraph
{public :
  Prop<short> vcolor;
  Prop<long> vlabel;
  Prop<int> ewidth;
  Prop<short> ecolor;
  Prop<long> elabel; 
  svector<int> degree;
  int **vvadj;                      // vertex/vertex sorted adjacency
  int **inList;
  int **outList;
  double **Distances;               // distances: carre distance euclidienne
  double **Coords;                  // Coordinates in R (n-1)
  bool ok;
  svector<int> indegree;
  svector<int> outdegree;
  svector<double> EigenValues;

  int ComputeDistances();
  int ComputeAdjMatrix();
  double ComputeDistance(int vertex1,int vertex2);
  int ComputeOrientDistances();
  int ComputeInOutList();
  double ComputeInDist(int vertex1,int vertex2);
  double ComputeOutDist(int vertex1,int vertex2);
  int ComputeIncidenceDistances();
  int ComputeIncidenceMDistances();
  

  EmbedRnGraph(Graph &G) :
            TopologicalGraph(G),
            vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL),
            ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1),
            elabel(G.PE(),PROP_LABEL),
	    vvadj(NULL),inList(NULL),outList(NULL),
	    Distances(NULL),Coords(NULL),ok(true)
      {init();}

  ~EmbedRnGraph() {release();}

private :
  void init();
  void release();
};

class SplitGraph : public EmbedRnGraph
    {public :
        svector<int> ClassNumber;             // Numero classe d'un sommet
        svector<int> NumberElementsInClass;
        int NumberOfClasses,MinDimension,MaxDimension,CurrentNumberOfClasses;
        SplitGraph(Graph &G,int numclasses,int maxdim) :
            EmbedRnGraph(G),
            NumberOfClasses(numclasses),MaxDimension(maxdim),CurrentNumberOfClasses(0)
            {init();}
      ~SplitGraph() {}
      
      void ComputeMaxDistance3d();
      void NewClass(int dimension,int worst);
      void SearchFarVertices(int dimension);
      void AffectExtrems(int extrem0,int extrem1);
      void BuildClasses(int dimension, double& inertie,int& worst);
      void Optimize(int dimension,int& worst,double& inertie);
      void ComputeBarycenters(int dimension);
      double TotalInertia(double& ClassVarianceNumber);
      int Segment();

    private :
      void init();
    };



// DIAG.CPP
int diag(double **Coords,int nb_vertex,double **Distances,svector<double>& EigenValues);
#endif
