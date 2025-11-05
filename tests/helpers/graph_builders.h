/****************************************************************************
**
** Test helpers for building standard test graphs
**
****************************************************************************/

#ifndef TEST_GRAPH_BUILDERS_H
#define TEST_GRAPH_BUILDERS_H

#include <TAXI/graphs.h>
#include <vector>
#include <utility>

namespace TestHelpers {

/**
 * @brief Basic graph structures
 */

// Complete graphs
GraphContainer* BuildK4();
GraphContainer* BuildK5();
GraphContainer* BuildK33();  // Complete bipartite K_{3,3}

// Standard named graphs
GraphContainer* BuildPetersenGraph();
GraphContainer* BuildWheelGraph(int spokes);

// Regular structures
GraphContainer* BuildGrid(int rows, int cols);
GraphContainer* BuildPath(int n);
GraphContainer* BuildCycle(int n);
GraphContainer* BuildTree(int n);
GraphContainer* BuildStar(int n);  // Star with n leaves

/**
 * @brief Planar graphs
 */

// Outerplanar graph with n vertices
GraphContainer* BuildOuterplanar(int n);

// Maximal planar (triangulated) graph with n vertices
GraphContainer* BuildMaximalPlanar(int n);

// 3-connected planar graph
GraphContainer* Build3Connected(int n);

/**
 * @brief Special test cases
 */

// Edge cases
GraphContainer* BuildEmptyGraph();
GraphContainer* BuildSingleVertex();
GraphContainer* BuildSingleEdge();
GraphContainer* BuildDisconnectedGraph(int components, int verticesPerComponent);

// Graphs with special properties
GraphContainer* BuildGraphWithMultipleEdges();
GraphContainer* BuildGraphWithSelfLoops();

/**
 * @brief Parameterized construction
 */

// Build from adjacency matrix (n x n matrix, 1 = edge exists)
GraphContainer* BuildFromAdjacencyMatrix(const std::vector<std::vector<int>>& adj);

// Build from edge list (pairs of vertex indices, 0-based)
GraphContainer* BuildFromEdgeList(int n, const std::vector<std::pair<int, int>>& edges);

/**
 * @brief Graph property helpers
 */

// Add coordinates to make it a GeometricGraph (simple grid layout)
void AddGridCoordinates(GeometricGraph& G);

// Add random coordinates
void AddRandomCoordinates(GeometricGraph& G);

/**
 * @brief Cleanup
 */

// Delete a graph container (helper for test cleanup)
void DeleteGraph(GraphContainer* gc);

} // namespace TestHelpers

#endif // TEST_GRAPH_BUILDERS_H
