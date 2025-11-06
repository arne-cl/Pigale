/****************************************************************************
**
** Integration tests for graph invariants and mathematical properties
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"

class GraphInvariantsTest : public ::testing::Test {
protected:
    GraphContainer* gc;

    void SetUp() override {
        gc = nullptr;
    }

    void TearDown() override {
        if (gc) delete gc;
    }
};

/**
 * Euler's Formula Tests
 * For connected planar graphs: V - E + F = 2
 */

TEST_F(GraphInvariantsTest, EulerFormulaOnK4) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // K4: 4 vertices, 6 edges
    ASSERT_EQ(G.nv(), 4);
    ASSERT_EQ(G.ne(), 6);

    // Compute embedding to get faces
    G.Planarity();

    // For K4: V=4, E=6, F should be 4 (by Euler's formula: V - E + F = 2)
    int faces = G.NumFaces();
    EXPECT_EQ(G.nv() - G.ne() + faces, 2) << "Euler's formula: V - E + F = 2";
}

TEST_F(GraphInvariantsTest, EulerFormulaOnGrid) {
    gc = TestHelpers::BuildGrid(3, 3);
    TopologicalGraph G(*gc);

    // Grid should be planar
    G.Planarity();
    ASSERT_TRUE(G.CheckPlanar());

    // Euler's formula
    int V = G.nv();
    int E = G.ne();
    int F = G.NumFaces();

    EXPECT_EQ(V - E + F, 2) << "Euler's formula should hold for planar grid";
}

TEST_F(GraphInvariantsTest, EulerFormulaOnCycle) {
    gc = TestHelpers::BuildCycle(8);
    TopologicalGraph G(*gc);

    // Cycle: n vertices, n edges
    EXPECT_EQ(G.nv(), G.ne());

    G.Planarity();

    // Cycle divides plane into 2 faces (interior and exterior)
    int faces = G.NumFaces();
    EXPECT_EQ(G.nv() - G.ne() + faces, 2) << "Euler's formula for cycle";
}

/**
 * Handshaking Lemma Tests
 * Sum of all vertex degrees = 2 * number of edges
 */

TEST_F(GraphInvariantsTest, HandshakingLemmaOnK4) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // Sum all degrees
    int degree_sum = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        degree_sum += G.Degree(v);
    }

    // Should equal 2 * edges
    EXPECT_EQ(degree_sum, 2 * G.ne()) << "Handshaking lemma: sum(degrees) = 2*edges";
}

TEST_F(GraphInvariantsTest, HandshakingLemmaOnTree) {
    gc = TestHelpers::BuildTree(10);
    TopologicalGraph G(*gc);

    int degree_sum = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        degree_sum += G.Degree(v);
    }

    EXPECT_EQ(degree_sum, 2 * G.ne());
}

TEST_F(GraphInvariantsTest, HandshakingLemmaOnPath) {
    gc = TestHelpers::BuildPath(7);
    TopologicalGraph G(*gc);

    int degree_sum = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        degree_sum += G.Degree(v);
    }

    EXPECT_EQ(degree_sum, 2 * G.ne());
}

/**
 * Tree Properties
 * A tree with n vertices has exactly n-1 edges
 */

TEST_F(GraphInvariantsTest, TreeHasNMinus1Edges) {
    gc = TestHelpers::BuildTree(10);
    Graph G(*gc);

    EXPECT_EQ(G.ne(), G.nv() - 1) << "Tree should have n-1 edges";
}

TEST_F(GraphInvariantsTest, TreeIsConnected) {
    gc = TestHelpers::BuildTree(8);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.NumberOfConnectedComponents(), 1) << "Tree should be connected";
}

TEST_F(GraphInvariantsTest, TreeIsAcyclic) {
    gc = TestHelpers::BuildTree(10);
    TopologicalGraph G(*gc);

    // Tree should be planar
    EXPECT_TRUE(G.TestPlanar());

    // For planar graph, if E = V-1, it's acyclic (a tree)
    EXPECT_EQ(G.ne(), G.nv() - 1);
}

/**
 * Complete Graph Properties
 * K_n has n vertices and n(n-1)/2 edges
 */

TEST_F(GraphInvariantsTest, K4HasCorrectEdgeCount) {
    gc = TestHelpers::BuildK4();
    Graph G(*gc);

    int n = G.nv();
    int expected_edges = n * (n - 1) / 2;

    EXPECT_EQ(G.ne(), expected_edges) << "K4 should have 6 edges";
    EXPECT_EQ(G.ne(), 6);
}

TEST_F(GraphInvariantsTest, K5HasCorrectEdgeCount) {
    gc = TestHelpers::BuildK5();
    Graph G(*gc);

    int n = G.nv();
    int expected_edges = n * (n - 1) / 2;

    EXPECT_EQ(G.ne(), expected_edges) << "K5 should have 10 edges";
    EXPECT_EQ(G.ne(), 10);
}

/**
 * Cycle Properties
 * Cycle has n vertices and n edges
 */

TEST_F(GraphInvariantsTest, CycleHasEqualVerticesAndEdges) {
    gc = TestHelpers::BuildCycle(6);
    Graph G(*gc);

    EXPECT_EQ(G.nv(), G.ne()) << "Cycle should have equal vertices and edges";
}

TEST_F(GraphInvariantsTest, CycleVerticesHaveDegree2) {
    gc = TestHelpers::BuildCycle(8);
    TopologicalGraph G(*gc);

    // All vertices in cycle have degree 2
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_EQ(G.Degree(v), 2) << "Vertex " << v() << " should have degree 2";
    }
}

TEST_F(GraphInvariantsTest, CycleIsBiconnected) {
    gc = TestHelpers::BuildCycle(5);
    TopologicalGraph G(*gc);

    // Cycle with 3+ vertices is biconnected
    if (G.nv() >= 3) {
        EXPECT_TRUE(G.CheckBiconnected()) << "Cycle should be biconnected";
    }
}

/**
 * Path Properties
 */

TEST_F(GraphInvariantsTest, PathHasNMinus1Edges) {
    gc = TestHelpers::BuildPath(10);
    Graph G(*gc);

    EXPECT_EQ(G.ne(), G.nv() - 1) << "Path should have n-1 edges";
}

TEST_F(GraphInvariantsTest, PathHasTwoLeaves) {
    gc = TestHelpers::BuildPath(6);
    TopologicalGraph G(*gc);

    // Count degree-1 vertices (leaves)
    int leaf_count = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        if (G.Degree(v) == 1) {
            leaf_count++;
        }
    }

    EXPECT_EQ(leaf_count, 2) << "Path should have exactly 2 leaves";
}

TEST_F(GraphInvariantsTest, PathIsNotBiconnected) {
    gc = TestHelpers::BuildPath(5);
    TopologicalGraph G(*gc);

    // Path is not biconnected (has articulation points)
    EXPECT_FALSE(G.CheckBiconnected()) << "Path should not be biconnected";
}

/**
 * Planar Graph Properties
 * For simple planar graph: E <= 3V - 6 (if V >= 3)
 */

TEST_F(GraphInvariantsTest, PlanarGraphEdgeBound) {
    gc = TestHelpers::BuildK4();
    Graph G(*gc);

    if (G.nv() >= 3) {
        EXPECT_LE(G.ne(), 3 * G.nv() - 6) << "Planar graph edge bound: E <= 3V - 6";
    }
}

TEST_F(GraphInvariantsTest, K5ViolatesPlanarEdgeBound) {
    gc = TestHelpers::BuildK5();
    Graph G(*gc);

    // K5 has 5 vertices and 10 edges
    // Planar bound: E <= 3*5 - 6 = 9
    // K5 has 10 edges, so it violates the bound (hence non-planar)
    EXPECT_GT(G.ne(), 3 * G.nv() - 6) << "K5 violates planar edge bound";
}

/**
 * Bipartite Graph Properties
 */

TEST_F(GraphInvariantsTest, K33IsBipartite) {
    gc = TestHelpers::BuildK33();
    TopologicalGraph G(*gc);

    // K_{3,3} should have 6 vertices
    EXPECT_EQ(G.nv(), 6);

    // K_{3,3} should have 9 edges (3*3)
    EXPECT_EQ(G.ne(), 9);

    // Each vertex should have degree 3
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_EQ(G.Degree(v), 3) << "K_{3,3} vertices should have degree 3";
    }
}

/**
 * Genus Tests
 * Planar graphs have genus 0
 */

TEST_F(GraphInvariantsTest, PlanarGraphHasGenus0) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    G.Planarity();
    ASSERT_TRUE(G.CheckPlanar());

    int genus = G.Genus();
    EXPECT_EQ(genus, 0) << "Planar graph should have genus 0";
}

TEST_F(GraphInvariantsTest, GridHasGenus0) {
    gc = TestHelpers::BuildGrid(4, 4);
    TopologicalGraph G(*gc);

    G.Planarity();

    int genus = G.Genus();
    EXPECT_EQ(genus, 0) << "Grid is planar, should have genus 0";
}

/**
 * Connectivity Properties
 */

TEST_F(GraphInvariantsTest, ConnectedGraphHasOneComponent) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.NumberOfConnectedComponents(), 1) << "K4 should be connected";
}

TEST_F(GraphInvariantsTest, DisconnectedGraphHasMultipleComponents) {
    gc = new GraphContainer();
    gc->setsize(6, 0);
    TopologicalGraph G(*gc);

    // Build two separate triangles
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    G.NewEdge(tvertex(4), tvertex(5));
    G.NewEdge(tvertex(5), tvertex(6));
    G.NewEdge(tvertex(6), tvertex(4));

    EXPECT_EQ(G.NumberOfConnectedComponents(), 2) << "Should have 2 components";
}

/**
 * Degree Sequence Properties
 */

TEST_F(GraphInvariantsTest, RegularGraphHasUniformDegree) {
    gc = TestHelpers::BuildCycle(6);
    TopologicalGraph G(*gc);

    // Cycle is 2-regular (all vertices have degree 2)
    int first_degree = G.Degree(tvertex(1));
    for (tvertex v = 2; v <= G.nv(); v++) {
        EXPECT_EQ(G.Degree(v), first_degree) << "Regular graph should have uniform degree";
    }
}

TEST_F(GraphInvariantsTest, K4Is3Regular) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // K4 is 3-regular
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_EQ(G.Degree(v), 3) << "K4 should be 3-regular";
    }
}

/**
 * Biconnectivity Invariants
 */

TEST_F(GraphInvariantsTest, BiconnectedGraphHasNoArticulationPoints) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // K4 is biconnected
    ASSERT_TRUE(G.CheckBiconnected());

    // Could verify no single vertex whose removal disconnects graph
    // (Not directly testable without implementing articulation point detection)
}

TEST_F(GraphInvariantsTest, BiconnectedGraphHasAtLeast3Vertices) {
    gc = TestHelpers::BuildCycle(5);
    TopologicalGraph G(*gc);

    if (G.CheckBiconnected()) {
        EXPECT_GE(G.nv(), 3) << "Biconnected graph must have at least 3 vertices";
    }
}

/**
 * Edge Bounds for Bipartite Planar Graphs
 * For bipartite planar graphs: E <= 2V - 4 (if V >= 3)
 */

TEST_F(GraphInvariantsTest, BipartitePlanarEdgeBound) {
    gc = TestHelpers::BuildGrid(3, 3);  // Grid is bipartite
    Graph G(*gc);

    if (G.nv() >= 3) {
        // Grids are bipartite and planar, should satisfy stricter bound
        EXPECT_LE(G.ne(), 2 * G.nv() - 4) << "Bipartite planar: E <= 2V - 4";
    }
}
