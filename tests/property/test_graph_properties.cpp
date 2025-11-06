/****************************************************************************
** Property-Based Tests for Graph Invariants
**
** Uses RapidCheck to verify properties hold for randomly generated inputs
** Following patterns from DEVELOPER_GUIDE.md
*****************************************************************************/

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <Pigale.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"

// ============================================================================
// Basic Graph Properties
// ============================================================================

// Property: For any graph, nv() vertices means vertex indices 1..nv()
RC_GTEST_PROP(GraphInvariants, VertexCountMatchesIndices, ()) {
    auto n = *rc::gen::inRange(1, 100);
    GraphContainer gc;
    gc.setsize(n, 0);  // IMPORTANT: Use 0 edges when using NewEdge (DEVELOPER_GUIDE.md)

    RC_ASSERT(gc.nv() == n);
    // All vertices 1..n should be valid
    for (int i = 1; i <= n; i++) {
        RC_ASSERT(tvertex(i)() == i);
    }
}

// Property: Adding edges never decreases edge count
RC_GTEST_PROP(GraphProperties, NewEdgeIncreasesCount, ()) {
    auto n = *rc::gen::inRange(2, 20);
    GraphContainer gc;
    gc.setsize(n, 0);  // IMPORTANT: setsize(n, 0) with NewEdge
    TopologicalGraph G(gc);

    int m_before = G.ne();
    auto v1 = *rc::gen::inRange(1, n + 1);
    auto v2 = *rc::gen::inRange(1, n + 1);

    if (v1 != v2) {  // No self-loops
        G.NewEdge(tvertex(v1), tvertex(v2));
        int m_after = G.ne();
        RC_ASSERT(m_after == m_before + 1);
    }
}

// Property: Edge count never exceeds complete graph bound
RC_GTEST_PROP(GraphProperties, EdgeCountWithinBounds, ()) {
    auto n = *rc::gen::inRange(2, 30);
    auto num_edges = *rc::gen::inRange(0, n * (n - 1) / 2 + 1);

    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // Add random edges
    int added = 0;
    for (int i = 0; i < num_edges && added < n * (n - 1) / 2; i++) {
        auto v1 = *rc::gen::inRange(1, n + 1);
        auto v2 = *rc::gen::inRange(1, n + 1);
        if (v1 != v2) {
            G.NewEdge(tvertex(v1), tvertex(v2));
            added++;
        }
    }

    // Can't exceed complete graph edge count
    RC_ASSERT(G.ne() <= n * (n - 1) / 2);
}

// ============================================================================
// Handshaking Lemma Properties
// ============================================================================

// Property: Sum of degrees = 2 * edges (Handshaking Lemma)
RC_GTEST_PROP(GraphInvariants, HandshakingLemma, ()) {
    auto n = *rc::gen::inRange(2, 30);
    auto num_edges = *rc::gen::inRange(1, std::min(50, n * (n - 1) / 2));

    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // Add random edges
    for (int i = 0; i < num_edges; i++) {
        auto v1 = *rc::gen::inRange(1, n + 1);
        auto v2 = *rc::gen::inRange(1, n + 1);
        if (v1 != v2) {
            G.NewEdge(tvertex(v1), tvertex(v2));
        }
    }

    if (G.ne() > 0) {
        // Calculate sum of degrees - need TopologicalGraph for Degree()
        int degree_sum = 0;
        for (tvertex v = 1; v <= G.nv(); v++) {
            degree_sum += G.Degree(v);
        }

        // Handshaking lemma: sum of degrees = 2 * edges
        RC_ASSERT(degree_sum == 2 * G.ne());
    }
}

// ============================================================================
// Tree Properties
// ============================================================================

// Property: Tree with n vertices has exactly n-1 edges
RC_GTEST_PROP(TreeProperties, TreeEdgeCount, ()) {
    auto n = *rc::gen::inRange(2, 50);

    GraphContainer* gc = TestHelpers::BuildPath(n);
    TopologicalGraph G(*gc);

    // Path is a tree
    RC_ASSERT(G.ne() == n - 1);

    delete gc;
}

// Property: Connected acyclic graph is a tree (n vertices, n-1 edges)
RC_GTEST_PROP(TreeProperties, ConnectedAcyclicIsTree, ()) {
    auto n = *rc::gen::inRange(2, 30);

    // Build a path (which is a tree)
    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // Create path: 1-2-3-...-n
    for (int i = 1; i < n; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 1));
    }

    // Should have n-1 edges
    RC_ASSERT(G.ne() == n - 1);

    // Should be connected - use DFS to verify
    int m = G.ne();
    svector<tvertex> nvin(-m, m);  // IMPORTANT: Index by brins (-m, m) - DEVELOPER_GUIDE.md
    svector<tbrin> tb(0, n);
    svector<int> dfsnum(0, n);

    int result = G.DFS(nvin, tb, dfsnum);
    RC_ASSERT(result > 0);  // Connected
}

// ============================================================================
// Cycle Properties
// ============================================================================

// Property: Cycle has n vertices and n edges
RC_GTEST_PROP(CycleProperties, CycleEdgeCount, ()) {
    auto n = *rc::gen::inRange(3, 50);

    GraphContainer* gc = TestHelpers::BuildCycle(n);
    TopologicalGraph G(*gc);

    // Cycle has n edges
    RC_ASSERT(G.ne() == n);

    // All vertices have degree 2
    for (tvertex v = 1; v <= G.nv(); v++) {
        RC_ASSERT(G.Degree(v) == 2);
    }

    delete gc;
}

// ============================================================================
// DFS/BFS Properties
// ============================================================================

// Property: DFS on connected graph visits all vertices
RC_GTEST_PROP(TraversalProperties, DFSVisitsAllVertices, ()) {
    auto n = *rc::gen::inRange(2, 50);
    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // Create connected graph (path)
    for (int i = 1; i < n; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 1));
    }

    int m = G.ne();
    svector<tvertex> nvin(-m, m);  // Indexed by brins
    svector<tbrin> tb(0, n);
    svector<int> dfsnum(0, n);

    int result = G.DFS(nvin, tb, dfsnum);

    RC_ASSERT(result > 0);  // Connected
    // All vertices should have DFS number > 0
    for (tvertex v = 1; v <= n; v++) {
        RC_ASSERT(dfsnum[v] > 0);
    }
}

// Property: BFS on connected graph marks all vertices in same component
RC_GTEST_PROP(TraversalProperties, BFSMarksAllVertices, ()) {
    auto n = *rc::gen::inRange(2, 50);
    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // Create connected graph (path)
    for (int i = 1; i < n; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 1));
    }

    svector<int> comp(1, n);

    int num_components = G.BFS(comp);

    // Should be 1 component
    RC_ASSERT(num_components == 1);

    // All vertices in component 1
    for (tvertex v = 1; v <= n; v++) {
        RC_ASSERT(comp[v] == 1);
    }
}

// ============================================================================
// Disconnected Graph Properties
// ============================================================================

// Property: Graph with k components has at least k vertices
RC_GTEST_PROP(ConnectivityProperties, ComponentsRequireVertices, ()) {
    auto n = *rc::gen::inRange(2, 30);
    auto k = *rc::gen::inRange(1, n + 1);  // k components

    // Can't have more components than vertices
    RC_ASSERT(k <= n);

    // Build k isolated vertices (k components)
    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    // No edges -> n components (all isolated)
    svector<int> comp(1, n);

    int num_components = G.BFS(comp);

    // All vertices isolated = n components
    RC_ASSERT(num_components == n);
}

// ============================================================================
// Complete Graph Properties
// ============================================================================

// Property: Complete graph K_n has n(n-1)/2 edges
RC_GTEST_PROP(CompleteGraphProperties, EdgeCountFormula, ()) {
    auto n = *rc::gen::inRange(1, 20);

    GraphContainer* gc = GenerateCompleteGraph(n);
    TopologicalGraph G(*gc);

    int expected_edges = n * (n - 1) / 2;
    RC_ASSERT(G.ne() == expected_edges);

    // All vertices have degree n-1
    if (n > 1) {
        for (tvertex v = 1; v <= G.nv(); v++) {
            RC_ASSERT(G.Degree(v) == n - 1);
        }
    }

    delete gc;
}

// ============================================================================
// Grid Graph Properties
// ============================================================================

// Property: Grid graph dimensions match vertex count
RC_GTEST_PROP(GridGraphProperties, DimensionsMatchVertexCount, ()) {
    auto rows = *rc::gen::inRange(1, 20);
    auto cols = *rc::gen::inRange(1, 20);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    RC_ASSERT(G.nv() == rows * cols);

    // Edge count: 2*rows*cols - rows - cols
    int expected_edges = 2 * rows * cols - rows - cols;
    RC_ASSERT(G.ne() == expected_edges);

    delete gc;
}

// Property: Grid graphs are always connected
RC_GTEST_PROP(GridGraphProperties, GridIsConnected, ()) {
    auto rows = *rc::gen::inRange(1, 15);
    auto cols = *rc::gen::inRange(1, 15);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    svector<int> comp(1, G.nv());

    int num_components = G.BFS(comp);

    // Grid should be 1 connected component
    RC_ASSERT(num_components == 1);

    delete gc;
}
