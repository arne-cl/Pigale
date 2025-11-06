/****************************************************************************
** Test Suite for Graph Generation Functions
**
** Tests for tgraph/Generate.cpp functions
** Following patterns from DEVELOPER_GUIDE.md
*****************************************************************************/

#include <gtest/gtest.h>
#include <Pigale.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"

class GenerateTest : public ::testing::Test {
protected:
    GraphContainer* gc = nullptr;

    void TearDown() override {
        if (gc) {
            delete gc;
            gc = nullptr;
        }
    }
};

// ============================================================================
// GenerateGrid Tests
// ============================================================================

TEST_F(GenerateTest, GridGraphBasicDimensions) {
    gc = GenerateGrid(5, 4);
    TopologicalGraph G(*gc);

    // Grid 5x4 should have 20 vertices
    EXPECT_EQ(G.nv(), 20);

    // Grid m x n has edges: 2*m*n - m - n
    // For 5x4: 2*20 - 5 - 4 = 31 edges
    EXPECT_EQ(G.ne(), 31);
}

TEST_F(GenerateTest, GridGraphIsConnected) {
    gc = GenerateGrid(3, 3);
    TopologicalGraph G(*gc);

    // Grid graphs should always be connected
    int m = G.ne();
    svector<tvertex> nvin(-m, m);
    svector<tbrin> tb(0, G.nv());
    svector<int> dfsnum(0, G.nv());

    int result = G.DFS(nvin, tb, dfsnum);
    EXPECT_GT(result, 0) << "Grid graph should be connected";
}

TEST_F(GenerateTest, GridGraphIsPlanar) {
    gc = GenerateGrid(4, 4);
    TopologicalGraph G(*gc);

    // Grid graphs are always planar
    EXPECT_GT(G.TestPlanar(), 0) << "Grid graph should be planar";
}

TEST_F(GenerateTest, GridGraphHasValidCoordinates) {
    gc = GenerateGrid(3, 3);
    GeometricGraph G(*gc);

    // Check that all vertices have coordinates
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_TRUE(std::isfinite(G.vcoord[v].x()))
            << "Vertex " << v() << " has invalid x coordinate";
        EXPECT_TRUE(std::isfinite(G.vcoord[v].y()))
            << "Vertex " << v() << " has invalid y coordinate";
    }
}

TEST_F(GenerateTest, GridGraphSingleVertex) {
    gc = GenerateGrid(1, 1);
    TopologicalGraph G(*gc);

    // Single vertex grid
    EXPECT_EQ(G.nv(), 1);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(GenerateTest, GridGraphPath) {
    gc = GenerateGrid(5, 1);
    TopologicalGraph G(*gc);

    // 5x1 grid is a path with 5 vertices and 4 edges
    EXPECT_EQ(G.nv(), 5);
    EXPECT_EQ(G.ne(), 4);
}

TEST_F(GenerateTest, GridGraphMaxDegree) {
    gc = GenerateGrid(4, 4);
    TopologicalGraph G(*gc);

    // In a grid, maximum degree is 4 (interior vertices)
    int maxDegree = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        int degree = G.Degree(v);
        maxDegree = std::max(maxDegree, degree);
        EXPECT_LE(degree, 4) << "Grid vertex has degree > 4";
    }
    EXPECT_EQ(maxDegree, 4) << "Grid should have vertices of degree 4";
}

// ============================================================================
// GenerateCompleteGraph Tests
// ============================================================================

TEST_F(GenerateTest, CompleteGraphEdgeCount) {
    gc = GenerateCompleteGraph(5);
    TopologicalGraph G(*gc);

    // K5 has 5 vertices and 5*(5-1)/2 = 10 edges
    EXPECT_EQ(G.nv(), 5);
    EXPECT_EQ(G.ne(), 10);
}

TEST_F(GenerateTest, CompleteGraphK4IsPlanar) {
    gc = GenerateCompleteGraph(4);
    TopologicalGraph G(*gc);

    // K4 is planar
    EXPECT_GT(G.TestPlanar(), 0) << "K4 should be planar";
}

TEST_F(GenerateTest, CompleteGraphK5IsNonPlanar) {
    gc = GenerateCompleteGraph(5);
    TopologicalGraph G(*gc);

    // K5 is not planar
    EXPECT_EQ(G.TestPlanar(), 0) << "K5 should be non-planar";
}

TEST_F(GenerateTest, CompleteGraphAllDegreesSame) {
    gc = GenerateCompleteGraph(6);
    TopologicalGraph G(*gc);

    // In Kn, all vertices have degree n-1
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_EQ(G.Degree(v), 5) << "Vertex " << v() << " should have degree 5 in K6";
    }
}

TEST_F(GenerateTest, CompleteGraphK1) {
    gc = GenerateCompleteGraph(1);
    TopologicalGraph G(*gc);

    // K1 is a single vertex with no edges
    EXPECT_EQ(G.nv(), 1);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(GenerateTest, CompleteGraphK2) {
    gc = GenerateCompleteGraph(2);
    TopologicalGraph G(*gc);

    // K2 is two vertices connected by one edge
    EXPECT_EQ(G.nv(), 2);
    EXPECT_EQ(G.ne(), 1);
}

TEST_F(GenerateTest, CompleteGraphK3) {
    gc = GenerateCompleteGraph(3);
    TopologicalGraph G(*gc);

    // K3 is a triangle
    EXPECT_EQ(G.nv(), 3);
    EXPECT_EQ(G.ne(), 3);
    EXPECT_GT(G.TestPlanar(), 0) << "K3 (triangle) should be planar";
}

TEST_F(GenerateTest, CompleteGraphHasCoordinates) {
    gc = GenerateCompleteGraph(6);
    GeometricGraph G(*gc);

    // Vertices should be arranged in a circle
    for (tvertex v = 1; v <= G.nv(); v++) {
        double x = G.vcoord[v].x();
        double y = G.vcoord[v].y();
        EXPECT_TRUE(std::isfinite(x));
        EXPECT_TRUE(std::isfinite(y));

        // Vertices should be roughly on unit circle
        double dist = std::sqrt(x*x + y*y);
        EXPECT_NEAR(dist, 1.0, 0.01) << "Vertex not on unit circle";
    }
}

// ============================================================================
// GenerateCompleteBiGraph Tests
// ============================================================================

TEST_F(GenerateTest, CompleteBipartiteBasicProperties) {
    gc = GenerateCompleteBiGraph(3, 4);
    TopologicalGraph G(*gc);

    // K(3,4) has 7 vertices and 3*4 = 12 edges
    EXPECT_EQ(G.nv(), 7);
    EXPECT_EQ(G.ne(), 12);
}

TEST_F(GenerateTest, CompleteBipartiteK33IsNonPlanar) {
    gc = GenerateCompleteBiGraph(3, 3);
    TopologicalGraph G(*gc);

    // K(3,3) is not planar
    EXPECT_EQ(G.TestPlanar(), 0) << "K(3,3) should be non-planar";
}

TEST_F(GenerateTest, CompleteBipartiteK22IsPlanar) {
    gc = GenerateCompleteBiGraph(2, 2);
    TopologicalGraph G(*gc);

    // K(2,2) is K4, which is planar
    EXPECT_EQ(G.nv(), 4);
    EXPECT_EQ(G.ne(), 4);
    EXPECT_GT(G.TestPlanar(), 0) << "K(2,2) should be planar";
}

TEST_F(GenerateTest, CompleteBipartiteK23IsPlanar) {
    gc = GenerateCompleteBiGraph(2, 3);
    TopologicalGraph G(*gc);

    // K(2,3) is planar
    EXPECT_EQ(G.nv(), 5);
    EXPECT_EQ(G.ne(), 6);
    EXPECT_GT(G.TestPlanar(), 0) << "K(2,3) should be planar";
}

TEST_F(GenerateTest, CompleteBipartiteStar) {
    gc = GenerateCompleteBiGraph(1, 5);
    TopologicalGraph G(*gc);

    // K(1,n) is a star graph
    EXPECT_EQ(G.nv(), 6);
    EXPECT_EQ(G.ne(), 5);

    // Should be planar
    EXPECT_GT(G.TestPlanar(), 0) << "Star graph should be planar";
}

// ============================================================================
// GenerateRandomGraph Tests
// ============================================================================

TEST_F(GenerateTest, RandomGraphBasicProperties) {
    gc = GenerateRandomGraph(10, 15);
    TopologicalGraph G(*gc);

    // Should have 10 vertices
    EXPECT_EQ(G.nv(), 10);

    // Random graph edge count may vary due to multiple edges being allowed
    // The second parameter appears to be "number of edge creation attempts" not final edge count
    EXPECT_GT(G.ne(), 0) << "Random graph should have some edges";
    EXPECT_LE(G.ne(), 45) << "Random graph should not exceed complete graph edge count";
}

TEST_F(GenerateTest, RandomGraphNoMultipleEdgesOption) {
    gc = GenerateRandomGraph(8, 12, true);  // true = randomEraseMultipleEdges
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 8);
    // Edge count should be <= requested, as duplicates are removed
    EXPECT_LE(G.ne(), 12);
}

TEST_F(GenerateTest, RandomGraphMaxEdges) {
    // For simple graph on n vertices, max edges is n(n-1)/2
    int n = 6;
    int maxEdges = n * (n-1) / 2;  // 15 for n=6

    gc = GenerateRandomGraph(n, maxEdges, true);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), n);
    EXPECT_LE(G.ne(), maxEdges);
}

TEST_F(GenerateTest, RandomGraphSparse) {
    gc = GenerateRandomGraph(20, 25, true);
    TopologicalGraph G(*gc);

    // Sparse graph: more vertices than edges
    EXPECT_EQ(G.nv(), 20);
    EXPECT_LE(G.ne(), 25);
}

// ============================================================================
// GenerateRandomOuterplanarGraph Tests
// ============================================================================

TEST_F(GenerateTest, RandomOuterplanarBasicProperties) {
    gc = GenerateRandomOuterplanarGraph(10, false);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 10);

    // Outerplanar graphs should be planar
    EXPECT_GT(G.TestPlanar(), 0) << "Outerplanar graph should be planar";
}

TEST_F(GenerateTest, RandomOuterplanarWithEdgeCount) {
    gc = GenerateRandomOuterplanarGraph(8, 10, false);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 8);
    // Edge count may vary but should be reasonable
    EXPECT_LE(G.ne(), 10 + 5) << "Edge count seems unreasonable";
}

TEST_F(GenerateTest, RandomOuterplanarEdgeBound) {
    gc = GenerateRandomOuterplanarGraph(15, false);
    TopologicalGraph G(*gc);

    // Outerplanar graphs have at most 2n-3 edges (for n >= 2)
    if (G.nv() >= 2) {
        EXPECT_LE(G.ne(), 2 * G.nv() - 3)
            << "Outerplanar graph exceeds edge bound";
    }
}

// ============================================================================
// Edge Cases and Error Handling
// ============================================================================

TEST_F(GenerateTest, GridGraphSmallDimensions) {
    gc = GenerateGrid(2, 2);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 4);
    EXPECT_EQ(G.ne(), 4);  // 2*4 - 2 - 2 = 4
}

TEST_F(GenerateTest, CompleteGraphSmallSizes) {
    // Test K0, K1, K2, K3
    for (int n = 0; n <= 3; n++) {
        gc = GenerateCompleteGraph(n);
        TopologicalGraph G(*gc);

        EXPECT_EQ(G.nv(), n);
        int expectedEdges = n * (n - 1) / 2;
        EXPECT_EQ(G.ne(), expectedEdges);

        if (gc) {
            delete gc;
            gc = nullptr;
        }
    }
}

TEST_F(GenerateTest, GeneratedGraphsHaveValidStructure) {
    // Test that generated graphs have valid vin arrays
    gc = GenerateGrid(3, 3);
    TopologicalGraph G(*gc);

    // All edges should have valid vertices
    for (tedge e = 1; e <= G.ne(); e++) {
        tvertex v1 = G.vin[tedge(e)];
        tvertex v2 = G.vin[tedge(-e)];

        EXPECT_GE(v1(), 1) << "Edge " << e() << " has invalid vertex";
        EXPECT_LE(v1(), G.nv());
        EXPECT_GE(v2(), 1);
        EXPECT_LE(v2(), G.nv());
    }
}

// ============================================================================
// Mathematical Properties
// ============================================================================

TEST_F(GenerateTest, HandshakingLemmaOnGeneratedGraphs) {
    // Test handshaking lemma: sum of degrees = 2 * edges

    // Test on grid
    gc = GenerateGrid(4, 4);
    {
        TopologicalGraph G(*gc);
        int degreeSum = 0;
        for (tvertex v = 1; v <= G.nv(); v++) {
            degreeSum += G.Degree(v);
        }
        EXPECT_EQ(degreeSum, 2 * G.ne()) << "Handshaking lemma violated (Grid)";
    }
    delete gc;

    // Test on complete graph
    gc = GenerateCompleteGraph(6);
    {
        TopologicalGraph G(*gc);
        int degreeSum = 0;
        for (tvertex v = 1; v <= G.nv(); v++) {
            degreeSum += G.Degree(v);
        }
        EXPECT_EQ(degreeSum, 2 * G.ne()) << "Handshaking lemma violated (Complete)";
    }
}

TEST_F(GenerateTest, GeneratedPlanarGraphsObeyEulerBound) {
    // For planar graphs: m <= 3n - 6 (when n >= 3)

    gc = GenerateGrid(5, 5);
    TopologicalGraph G(*gc);

    if (G.nv() >= 3 && G.TestPlanar() > 0) {
        EXPECT_LE(G.ne(), 3 * G.nv() - 6)
            << "Planar graph violates Euler's bound";
    }
}
