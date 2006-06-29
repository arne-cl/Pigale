#include <Pigale.h>


int main (int argc, char** argv)
  {GraphContainer GCC;
  
  if(argc<2) 
      {cout << "usage: " << argv[0] << " <filename.txt>\n";
      exit(0); 
      } 
 
  if(ReadGraphAscii(GCC,argv[1]) != 0)
      {cout << "Error in reading file" << endl;return 1;}
  TopologicalGraph G(GCC); // create a topological access
  cout<<"Reading file a.txt"<<endl;
  cout << "Nodes: " << G.nv() << "\tEdges: " << G.ne()<< endl;
  cout << "Simple:" << G.CheckSimple() << endl;
  cout << "Connected:" << G.CheckConnected() << endl;
  cout << "BiConnected:" << G.CheckBiconnected() << endl;
  G.Simplify();
  G.Biconnect();
  cout << "After making 2-connected"<<endl;
  cout << "Nodes: " << G.nv() << "\tEdges: " << G.ne()<< endl;
  cout << "BiConnected:" << G.CheckBiconnected() << endl;
  cout << "TesTplanar:" << G.TestPlanar() << endl; 

  
  GraphContainer *GC=(GraphContainer *)0;
  {cout<<"\nGenerate a 3x3 grid"<<endl;
  GC = GenerateGrid(3,3); // returns a pointer on a graph container
  GCC.Tswap(*GC);          // swap the pointer
  delete GC;
  GeometricGraph GG(GCC); // create a geometric access
  cout << "Nodes: " << GG.nv() << "\tEdges: " << GG.ne()<< endl;
  cout << "Coordinates of vertices" << endl;
  for(tvertex v = 1; v <= GG.nv();v++)
      cout << "vertex " << v()<< " -> (" << GG.vcoord[v].x() <<","<<  GG.vcoord[v].x() <<")"<<endl;
  }
   
  // see Qt/Handler.cpp to see the parameters of GenerateSchaeffer
  {cout <<"\nGenerate a 3-connected planar graph with around 30 edges" <<endl;
  GC = GenerateSchaeffer(30,1,3);  
  GCC.Tswap(*GC);         
  delete GC;    
  TopologicalGraph GG(GCC);
  cout << "Nodes: " << GG.nv() << "\tEdges: " << GG.ne()<< endl;
  cout << "BiConnected:" << GG.CheckBiconnected() << endl;
  cout << "TriConnected:" << GG.CheckTriconnected() << endl;
  cout << "TesTplanar:" << GG.TestPlanar2() << endl; 
  }


  return 0;      
  }


