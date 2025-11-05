/****************************************************************************
**
** Test helpers for building standard test graphs - Implementation
**
****************************************************************************/

#include "graph_builders.h"
#include <Pigale.h>
#include <cmath>

namespace TestHelpers {

GraphContainer* BuildK4() {
    // Complete graph on 4 vertices
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Add all edges between 4 vertices
    for(int i = 1; i <= 4; i++) {
        for(int j = i + 1; j <= 4; j++) {
            G.NewEdge(tvertex(i), tvertex(j));
        }
    }

    return gc;
}

GraphContainer* BuildK5() {
    // Complete graph on 5 vertices (non-planar)
    GraphContainer* gc = new GraphContainer();
    gc->setsize(5, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Add all edges between 5 vertices
    for(int i = 1; i <= 5; i++) {
        for(int j = i + 1; j <= 5; j++) {
            G.NewEdge(tvertex(i), tvertex(j));
        }
    }

    return gc;
}

GraphContainer* BuildK33() {
    // Complete bipartite graph K_{3,3} (non-planar)
    GraphContainer* gc = new GraphContainer();
    gc->setsize(6, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Vertices 1,2,3 in one partition, 4,5,6 in the other
    for(int i = 1; i <= 3; i++) {
        for(int j = 4; j <= 6; j++) {
            G.NewEdge(tvertex(i), tvertex(j));
        }
    }

    return gc;
}

GraphContainer* BuildPetersenGraph() {
    // Petersen graph (non-planar, 10 vertices, 15 edges)
    GraphContainer* gc = new GraphContainer();
    gc->setsize(10, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Outer pentagon: vertices 1-5
    for(int i = 1; i <= 5; i++) {
        G.NewEdge(tvertex(i), tvertex(i % 5 + 1));
    }

    // Inner pentagon: vertices 6-10
    for(int i = 6; i <= 10; i++) {
        G.NewEdge(tvertex(i), tvertex((i - 6 + 2) % 5 + 6));
    }

    // Spokes connecting outer to inner
    for(int i = 1; i <= 5; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 5));
    }

    return gc;
}

GraphContainer* BuildWheelGraph(int spokes) {
    // Wheel graph: central vertex connected to cycle
    GraphContainer* gc = new GraphContainer();
    gc->setsize(spokes + 1, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    tvertex center(1);

    // Create cycle (vertices 2 to spokes+1)
    for(int i = 2; i <= spokes + 1; i++) {
        int next = (i - 2 + 1) % spokes + 2;
        G.NewEdge(tvertex(i), tvertex(next));
    }

    // Connect center to all cycle vertices
    for(int i = 2; i <= spokes + 1; i++) {
        G.NewEdge(center, tvertex(i));
    }

    return gc;
}

GraphContainer* BuildGrid(int rows, int cols) {
    return GenerateGrid(rows, cols);
}

GraphContainer* BuildPath(int n) {
    // Path graph with n vertices
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    for(int i = 1; i < n; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 1));
    }

    return gc;
}

GraphContainer* BuildCycle(int n) {
    // Cycle graph with n vertices
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    for(int i = 1; i <= n; i++) {
        G.NewEdge(tvertex(i), tvertex(i % n + 1));
    }

    return gc;
}

GraphContainer* BuildTree(int n) {
    // Simple tree (star-like but more balanced)
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Create a simple binary-ish tree
    for(int i = 2; i <= n; i++) {
        int parent = (i - 1) / 2 + 1;
        if(parent >= 1 && parent < i) {
            G.NewEdge(tvertex(parent), tvertex(i));
        }
    }

    return gc;
}

GraphContainer* BuildStar(int n) {
    // Star graph: one center connected to n leaves
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n + 1, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    tvertex center(1);
    for(int i = 2; i <= n + 1; i++) {
        G.NewEdge(center, tvertex(i));
    }

    return gc;
}

GraphContainer* BuildOuterplanar(int n) {
    // Simple outerplanar graph (cycle with some chords)
    GraphContainer* gc = BuildCycle(n);

    if(n >= 4) {
        TopologicalGraph G(*gc);
        // Add a chord to make it interesting but keep it outerplanar
        G.NewEdge(tvertex(1), tvertex(3));
    }

    return gc;
}

GraphContainer* BuildMaximalPlanar(int n) {
    // Triangulated planar graph
    if(n < 3) return BuildPath(n);

    // Start with a triangle and add vertices
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Initial triangle
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    // Add remaining vertices one by one
    for(int v = 4; v <= n; v++) {
        // Connect to first three vertices (simple strategy)
        G.NewEdge(tvertex(v), tvertex(1));
        G.NewEdge(tvertex(v), tvertex(2));
        G.NewEdge(tvertex(v), tvertex(v - 1));
    }

    return gc;
}

GraphContainer* Build3Connected(int n) {
    // Simple 3-connected graph (wheel is 3-connected for n >= 4)
    if(n >= 4) {
        return BuildWheelGraph(n - 1);
    }
    return BuildK4();
}

GraphContainer* BuildEmptyGraph() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(0, 0);
    return gc;
}

GraphContainer* BuildSingleVertex() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(1, 0);
    return gc;
}

GraphContainer* BuildSingleEdge() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(2, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);
    G.NewEdge(tvertex(1), tvertex(2));

    return gc;
}

GraphContainer* BuildDisconnectedGraph(int components, int verticesPerComponent) {
    int totalVertices = components * verticesPerComponent;

    GraphContainer* gc = new GraphContainer();
    gc->setsize(totalVertices, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Create each component as a path
    for(int c = 0; c < components; c++) {
        int offset = c * verticesPerComponent;
        for(int i = 1; i < verticesPerComponent; i++) {
            G.NewEdge(tvertex(offset + i), tvertex(offset + i + 1));
        }
    }

    return gc;
}

GraphContainer* BuildGraphWithMultipleEdges() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(2, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Add two edges between the same vertices
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(1), tvertex(2));

    return gc;
}

GraphContainer* BuildGraphWithSelfLoops() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(2, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Add self-loop
    G.NewEdge(tvertex(1), tvertex(1));
    G.NewEdge(tvertex(1), tvertex(2));

    return gc;
}

GraphContainer* BuildFromAdjacencyMatrix(const std::vector<std::vector<int>>& adj) {
    int n = adj.size();

    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    // Add edges (1-based indexing in Pigale)
    for(int i = 0; i < n; i++) {
        for(int j = i; j < n; j++) {
            for(int k = 0; k < adj[i][j]; k++) {
                G.NewEdge(tvertex(i + 1), tvertex(j + 1));
            }
        }
    }

    return gc;
}

GraphContainer* BuildFromEdgeList(int n, const std::vector<std::pair<int, int>>& edges) {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(n, 0);  // Don't pre-allocate edges - NewEdge will create them

    TopologicalGraph G(*gc);

    for(const auto& edge : edges) {
        // Convert from 0-based to 1-based indexing
        G.NewEdge(tvertex(edge.first + 1), tvertex(edge.second + 1));
    }

    return gc;
}

void AddGridCoordinates(GeometricGraph& G) {
    // Create Prop for coordinates
    Prop<Tpoint> vcoord(G.Set(tvertex()), PROP_COORD);

    int n = G.nv();
    int cols = static_cast<int>(std::sqrt(n));

    for(tvertex v = 1; v <= n; v++) {
        int row = (v() - 1) / cols;
        int col = (v() - 1) % cols;
        vcoord[v] = Tpoint(col * 100.0, row * 100.0);
    }
}

void AddRandomCoordinates(GeometricGraph& G) {
    // Create Prop for coordinates
    Prop<Tpoint> vcoord(G.Set(tvertex()), PROP_COORD);

    for(tvertex v = 1; v <= G.nv(); v++) {
        double x = (rand() % 1000) / 10.0;
        double y = (rand() % 1000) / 10.0;
        vcoord[v] = Tpoint(x, y);
    }
}

void DeleteGraph(GraphContainer* gc) {
    delete gc;
}

} // namespace TestHelpers
