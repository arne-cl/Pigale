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

#include <TAXI/graphs.h>
#include <TAXI/Tbase.h>
#include <TAXI/Tpoint.h>
#include <TAXI/embedrn.h>

#define MAX_ITERATION 10
#define SEUIL 0.00001

int & useDistance();

//! Triangular matrix handling as a symmetric square matrix.
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
  //! Constructor
  /*!
    \param n Size of the matrix
  */
  TriangularMatrix(int n): buffer(n*(n-1)/2) {}
  //! Resizes the matrix
  void resize(int n) {buffer.resize(0,n*(n-1)/2-1);}
  //! Clears the matrix entries
  void clear() {buffer.clear();}
  //! Returns a reference to an element
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  //! Returns a constant reference to an element
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};
//! Rectangular matrix
template <class T> class RectangularMatrix 
{
  svector<T> buffer;
  int ncols;
  int Index(int i,int j)
      {return i*ncols+j;
      }

public:
  //! Constructor
  /*!
    \param nr Number of rows
    \param nc Number of columns
  */
  RectangularMatrix(int nr,int nc): buffer(nr*nc), ncols(nc) {}
  //! Clears the matrix entries
  void clear() {buffer.clear();}
  //! Resizes the matrix
  void resize(int nr, int nc) {buffer.resize(0,nr*nc-1); ncols=nc;}
  //! Returns a reference to an element
  T & operator()(int i, int j) {return buffer[Index(i,j)];}
  //! Returns a constant reference to an element
  const T & operator()(int i, int j) const {return buffer[Index(i,j)];}
};

//! Class used to spread vertices in classes
struct Classifier {
  svector<int> Class; //!< Classes (\f$ V\rightarrow [1,{\rm noc}]\f$ mapping)
  svector<int> Cardinal; //!< Cardinal of the classes

  //! Constructor
  /*!
    \param n Number of vertices
    \param noc Number of classes
  */
  Classifier(int n, int noc): Class(n), Cardinal(noc) {}
  //! Clear the partition
  void clear() {Class.clear(); Cardinal.clear();}
  //! Affects a vertex to a class
  /*!
    \param i Vertex index
    \param c Class index
  */
  void affect(int i,int c) {Class[i]=c; Cardinal[c]++;}
};

//! Locals of the embedder
struct Locals 
{
  TriangularMatrix<double> ProjectDist; //!< Distances in some eigenspace
  RectangularMatrix<double> BaryCoord; //!< Barycenters coordinates
  Classifier Part; //!< Class affectation
  //! Constructor
  /*!
    \param n Number of vertices
    \param ms Maximum number of barycenters
    \param noc Number of classes
  */
  Locals(int n, int ms, int noc): ProjectDist(n),BaryCoord(ms,n-1),Part(n,noc)
  {ProjectDist.clear();}
};

//! Topological graph embedded in \f$ \mathbb{R}^n\f$ 
class EmbedRnGraph : public TopologicalGraph
{public :
  Prop<short> vcolor;  //!< colors of the vertices
 Prop<long> vlabel; //!< labels of the vertices
 Prop<int> ewidth; //!< width of the edges 
 Prop<short> ecolor; //!< colors of the edges 
 Prop<long> elabel;  //!< labels of the edges 
 svector<int> degree; //!< degrees of the vertices 
 int **vvadj;                      //!< vertex/vertex sorted adjacency 
 int **inList;  //!< incoming adjacency lists 
 int **outList; //!< outgoing adjacency lists 
 double **Distances;               //!< squared Euclidean distances 
 double **Coords;                  //!< coordinates in \f$ \mathbb{R}^{n-1}\f$ 
 bool ok; //!< computation status 
 svector<int> indegree; //!< indegrees of the vertices 
 svector<int> outdegree; //!< outdegrees of the vertices 
 svector<double> EigenValues; //!< eigenvalues 
/*!
   \f[ d^2(i,j)=1-\frac{|N(i)\cap N(j)|}{|N(i)|+|N(j)|},\f]
   where \f$ N(i) \f$ is the set including \f$ i \f$ and its neighbors. 
 */
 int ComputeCzekanovskiDistances(); //!< Computes Czekanovski-Dice distances 
 int ComputeAdjacenceMatrix(); //!< Computes the vertex/vertex adjacency matrix 
/*!
   \f[ d^2(i,j)=1-\frac{|N(i)\cap N(j)|}{|N(i)|+|N(j)|},\f]
   where \f$ N(i) \f$ is the set including \f$ i \f$ and its neighbors. 
 */
 double ComputeCzekanovskiDistance(int vertex1,int vertex2); //!< Computes Czekanovski-Dice distances between two vertices 
/*!
   \f[ d^2(i,j)=1-\frac{|N^-(i)\cap N^-(j)|}{|N^-(i)|+|N^-(j)|}-\frac{|N^+(i)\cap N^+(j)|}{|N^+(i)|+|N^+(j)|},\f]
   where \f$ N^-(i) \f$ is the set including \f$ i \f$ and its incoming neighbors, and 
   \f$ N^+(i) \f$ is the set including \f$ i \f$ and its outgoing neighbors.
 */
 int ComputeOrientDistances(); //!< Computes oriented distances 
 int ComputeInOutList(); //!< Computes incoming and outgoing neighbor sets 
/*!
   \f[ d^2(i,j)=1-\frac{|N^-(i)\cap N^-(j)|}{|N^-(i)|+|N^-(j)|},\f]
   where \f$ N^-(i) \f$ is the set including \f$ i \f$ and its incoming neighbors.
 */
 double ComputeInDist(int vertex1,int vertex2); //!< Computes the part of the oriented distance due to incoming edges
/*!
   \f[ d^2(i,j)=1-\frac{|N^+(i)\cap N^+(j)|}{|N^+(i)|+|N^+(j)|},\f]
   where \f$ N^+(i) \f$ is the set including \f$ i \f$ and its outgoing neighbors.
 */
 double ComputeOutDist(int vertex1,int vertex2); //!< Computes the part of the oriented distance due to outgoing edges
/*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$ or $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
 */
 int ComputeAdjacenceDistances(); //!< Computes adjacency distances 
 /*!
   Actually computes the bilinear form corresponding to the Laplacian distance on the complement graph:
   \f[ b(i,j)=\begin{cases}
   n-d(i),&\text{if $i=j$}\\
   0,&\text{if $i$ and $j$ are adjacent}\\
   -1,&\text{otherwise}
   \end{cases}
   \f]
   It is semi-definite positive, as \f$ B=\bar{D}\bar{D}^{\rm t}\f$ if
   \f$ \bar{D} \f$ is the oriented adjacency matrix of the complement graph for any
   arbitrary orientation.

   The distance corresponding to this bilinear form is:
   \f[ d^2(i,j)=\begin{cases}
    0,&\text{if $i=j$}\\
    2n-d(i)-d(j),&\text{if $i$ and $j$ are adjacent}\\
    2n-d(i)-d(j)+2,&\text{otherwise}
    \end{cases}
   \f]
 */   
 int ComputeLaplacianDistances(); //!< Computes Laplacian distances on the complement graph
 /*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$}\\
   1-\frac{2}{n},&\text{if $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
 */
 int ComputeAdjacenceMDistances(); //!< Computes translated adjacency distances 
 /*!
   \f[ d^2(i,j)=\begin{cases}
   0,&\text{if $i=j$}\\
   1-\frac{2}{d(i)+d(j)+2},&\text{if $i$ and $j$ are adjacent}\\
   1,&\text{otherwise}
   \end{cases}
   \f]
   where \f$ d(i) \f$ is the degree of \f$ i \f$
 */
 int ComputeBisectDistances(); //!< Computes bisection distances 
 /*!
   \f[ d^2(i,j)=x^2(i)+y^2(i),\f]
   where \f$ x(i) \f$ and \f$ y(i) \f$ are the coordinates of \f$ i \f$ in the plane.
 */
 int ComputeR2Distances(); //!< Computes distances in \f$ \mathbb{R}^2\f$ 
  
 //! Constructor
  EmbedRnGraph(Graph &G) :
    TopologicalGraph(G),
    vcolor(G.PV(),PROP_COLOR,5),vlabel(G.PV(),PROP_LABEL),
    ewidth(G.PE(),PROP_WIDTH,1), ecolor(G.PE(),PROP_COLOR,1),
    elabel(G.PE(),PROP_LABEL),
    vvadj(NULL),inList(NULL),outList(NULL),
    Distances(NULL),Coords(NULL),ok(true)
    {init();}
  
  //! Destructor
  ~EmbedRnGraph() {release();}

private :
  void init(); //!< Class initialization 
 void release(); //!< Member destructions 
};

//! Class used to partition a graph using a factorial embedding in \f$ \mathbb{R}^{n-1} \f$
class SplitGraph : public EmbedRnGraph
    {public :
      svector<int> ClassNumber;             //!< Class index of a vertex
    svector<int> NumberElementsInClass; 
    int NumberOfClasses,MinDimension,MaxDimension,CurrentNumberOfClasses;
    SplitGraph(Graph &G,int numclasses,int maxdim) :
      EmbedRnGraph(G),
      NumberOfClasses(numclasses),MaxDimension(maxdim),CurrentNumberOfClasses(0)
      {init();}
    ~SplitGraph() {}
      
    void ComputeMaxDistance3d();
    void NewClass(int dimension,int worst);
    void ComputeProjectDistance(int dimension);
    void SearchFarVertices(int dimension);
    void SearchWorst(int dimension,int &worst); 
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
int diag(double **Coords,int nb_vertex,double **Distances,svector<double>& EigenValues,bool Project = true);
#endif
