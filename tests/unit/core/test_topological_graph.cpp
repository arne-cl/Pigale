/****************************************************************************
**
** Unit tests for TopologicalGraph class
**
** Tests cover TopologicalGraph-specific functionality:
** - Edge operations (NewEdge, DeleteEdge)
** - Vertex operations (NewVertex, DeleteVertex)
** - Graph modifications (ContractEdge, BissectEdge, MoveBrin)
** - Degree calculations (Degree, InDegree, OutDegree)
** - Topological properties (genus, faces)
** - Graph structure operations
**
** Following lessons from DEVELOPER_GUIDE.md:
** - Always use setsize(n, 0) with NewEdge()
** - Test edge operations thoroughly
** - Verify circular order after modifications
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include "../helpers/graph_builders.h"
#include "../helpers/property_validators.h"

class TopologicalGraphTest : public ::testing::Test {
protected:
    GraphContainer* gc;

    void SetUp() override {
        gc = new GraphContainer();
    }

    void TearDown() override {
        delete gc;
    }
};

/**
 * Test edge operations - NewEdge and DeleteEdge
 */
TEST_F(TopologicalGraphTest, NewEdgeCreatesEdge) {
    gc->setsize(4, 0);  // LESSON: Use 0 edges with NewEdge()
    TopologicalGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(e(), 1);  // First edge created
    EXPECT_EQ(G.ne(), 1);
    EXPECT_EQ(G.vin[e](), 1);
    EXPECT_EQ(G.vin[-e](), 2);
}

TEST_F(TopologicalGraphTest, MultipleNewEdges) {
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    tedge e2 = G.NewEdge(tvertex(2), tvertex(3));
    tedge e3 = G.NewEdge(tvertex(3), tvertex(4));

    EXPECT_EQ(G.ne(), 3);
    EXPECT_EQ(e1(), 1);
    EXPECT_EQ(e2(), 2);
    EXPECT_EQ(e3(), 3);
}

TEST_F(TopologicalGraphTest, DeleteEdgeRemovesEdge) {
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    tedge e2 = G.NewEdge(tvertex(2), tvertex(3));

    EXPECT_EQ(G.ne(), 2);

    G.DeleteEdge(e1);

    EXPECT_EQ(G.ne(), 1);
}

TEST_F(TopologicalGraphTest, DeleteEdgeUpdatesCircularOrder) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);
    int initial_edges = G.ne();

    tedge e1 = G.FirstEdge(tvertex(1));
    G.DeleteEdge(e1);

    EXPECT_EQ(G.ne(), initial_edges - 1);
    // Circular order should still be valid
    EXPECT_TRUE(TestHelpers::PropertyValidator::ValidateCirAcir(G));
}

/**
 * Test vertex operations - NewVertex and DeleteVertex
 */
TEST_F(TopologicalGraphTest, NewVertexIncreasesCount) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 3);

    tvertex v = G.NewVertex();

    EXPECT_EQ(G.nv(), 4);
    EXPECT_EQ(v(), 4);
}

TEST_F(TopologicalGraphTest, NewVertexWithEdges) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));

    tvertex v = G.NewVertex();  // v = 4

    tedge e = G.NewEdge(v, tvertex(1));

    EXPECT_EQ(G.nv(), 4);
    EXPECT_EQ(G.ne(), 2);
    EXPECT_EQ(G.vin[e](), 4);
    EXPECT_EQ(G.vin[-e](), 1);
}

TEST_F(TopologicalGraphTest, DeleteVertexRemovesVertex) {
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(3), tvertex(4));

    EXPECT_EQ(G.nv(), 4);

    G.DeleteVertex(tvertex(4));

    EXPECT_EQ(G.nv(), 3);
}

/**
 * Test ContractEdge - merges two vertices
 */
TEST_F(TopologicalGraphTest, ContractEdgeReducesVertexCount) {
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(G.nv(), 4);
    EXPECT_EQ(G.ne(), 1);

    tvertex v = G.ContractEdge(e);

    EXPECT_EQ(G.nv(), 3);  // One vertex removed
    EXPECT_EQ(G.ne(), 0);  // Edge removed
    EXPECT_NE(v(), 0);     // Returns merged vertex
}

TEST_F(TopologicalGraphTest, ContractEdgeInTriangle) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    // Create triangle: 1-2, 2-3, 3-1
    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    EXPECT_EQ(G.nv(), 3);
    EXPECT_EQ(G.ne(), 3);

    tvertex v = G.ContractEdge(e1);

    EXPECT_EQ(G.nv(), 2);  // 3 vertices -> 2
    EXPECT_EQ(G.ne(), 2);  // 3 edges -> 2 (contracted edge + self-loop removed)
}

/**
 * Test BissectEdge - splits an edge by inserting a vertex
 */
TEST_F(TopologicalGraphTest, BissectEdgeIncreasesVertexCount) {
    gc->setsize(2, 0);
    TopologicalGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(G.nv(), 2);
    EXPECT_EQ(G.ne(), 1);

    tvertex v = G.BissectEdge(e);

    EXPECT_EQ(G.nv(), 3);  // New vertex added
    EXPECT_EQ(G.ne(), 2);  // Edge split into two
    EXPECT_EQ(v(), 3);     // New vertex is #3
}

TEST_F(TopologicalGraphTest, BissectEdgePreservesEndpoints) {
    gc->setsize(2, 0);
    TopologicalGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));
    tvertex v1 = G.vin[e];
    tvertex v2 = G.vin[-e];

    tvertex v = G.BissectEdge(e);

    // Original edge should be modified, new edge added
    // One edge connects v1 to v, another connects v to v2
    EXPECT_EQ(G.ne(), 2);

    // Verify endpoints
    bool found_v1_v = false;
    bool found_v_v2 = false;

    for (tedge edge = 1; edge <= G.ne(); edge++) {
        tvertex a = G.vin[edge];
        tvertex b = G.vin[-edge];

        if ((a == v1 && b == v) || (a == v && b == v1)) found_v1_v = true;
        if ((a == v && b == v2) || (a == v2 && b == v)) found_v_v2 = true;
    }

    EXPECT_TRUE(found_v1_v);
    EXPECT_TRUE(found_v_v2);
}

/**
 * Test Degree calculations
 */
TEST_F(TopologicalGraphTest, DegreeOfIsolatedVertex) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.Degree(tvertex(1)), 0);
    EXPECT_EQ(G.Degree(tvertex(2)), 0);
}

TEST_F(TopologicalGraphTest, DegreeAfterAddingEdges) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(1), tvertex(3));

    EXPECT_EQ(G.Degree(tvertex(1)), 2);
    EXPECT_EQ(G.Degree(tvertex(2)), 1);
    EXPECT_EQ(G.Degree(tvertex(3)), 1);
}

TEST_F(TopologicalGraphTest, DegreeInK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    // K4: all vertices have degree 3
    for (tvertex v = 1; v <= 4; v++) {
        EXPECT_EQ(G.Degree(v), 3);
    }
}

/**
 * Test InDegree and OutDegree for directed edges
 */
TEST_F(TopologicalGraphTest, InOutDegreeUndirectedGraph) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(1), tvertex(3));

    // For undirected graph, InDegree + OutDegree == Degree
    EXPECT_EQ(G.Degree(tvertex(1)),
              G.InDegree(tvertex(1)) + G.OutDegree(tvertex(1)));
}

/**
 * Test RemoveLoops
 */
TEST_F(TopologicalGraphTest, RemoveLoopsOnGraphWithoutLoops) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));

    int loops_removed = G.RemoveLoops();

    EXPECT_EQ(loops_removed, 0);
    EXPECT_EQ(G.ne(), 2);
}

TEST_F(TopologicalGraphTest, CheckNoLoopsOnCleanGraph) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    EXPECT_TRUE(G.CheckNoLoops());
}

/**
 * Test RemoveIsolatedVertices
 */
TEST_F(TopologicalGraphTest, RemoveIsolatedVerticesEmptyGraph) {
    gc->setsize(5, 0);
    TopologicalGraph G(*gc);

    // All 5 vertices are isolated
    int removed = G.RemoveIsolatedVertices();

    EXPECT_EQ(removed, 5);
    EXPECT_EQ(G.nv(), 0);
}

TEST_F(TopologicalGraphTest, RemoveIsolatedVerticesPartialGraph) {
    gc->setsize(5, 0);
    TopologicalGraph G(*gc);

    // Connect only vertices 1-2-3
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));

    // Vertices 4 and 5 are isolated
    int removed = G.RemoveIsolatedVertices();

    EXPECT_EQ(removed, 2);
    EXPECT_EQ(G.nv(), 3);
}

/**
 * Test face and genus calculations on planar graphs
 */
TEST_F(TopologicalGraphTest, NumFacesK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    tbrin result = G.Planarity();
    ASSERT_NE(result, 0);  // K4 is planar

    int faces = G.NumFaces();

    // K4: 4 vertices, 6 edges, 4 faces (including outer face)
    // Euler's formula: V - E + F = 2
    // 4 - 6 + F = 2, so F = 4
    EXPECT_EQ(faces, 4);
}

TEST_F(TopologicalGraphTest, GenusK4IsZero) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    G.Planarity();

    int genus = G.Genus();

    // K4 is planar, so genus = 0
    EXPECT_EQ(genus, 0);
}

TEST_F(TopologicalGraphTest, GenusGridIsZero) {
    delete gc;
    gc = TestHelpers::BuildGrid(3, 3);

    TopologicalGraph G(*gc);

    G.Planarity();

    int genus = G.Genus();

    // Grid graphs are planar, genus = 0
    EXPECT_EQ(genus, 0);
}

/**
 * Test graph structure after modifications
 */
TEST_F(TopologicalGraphTest, SequentialEdgeAdditions) {
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    EXPECT_EQ(G.ne(), 1);

    G.NewEdge(tvertex(2), tvertex(3));
    EXPECT_EQ(G.ne(), 2);

    G.NewEdge(tvertex(3), tvertex(4));
    EXPECT_EQ(G.ne(), 3);

    G.NewEdge(tvertex(4), tvertex(1));
    EXPECT_EQ(G.ne(), 4);

    // Verify all vertices are connected
    EXPECT_GT(G.Degree(tvertex(1)), 0);
    EXPECT_GT(G.Degree(tvertex(2)), 0);
    EXPECT_GT(G.Degree(tvertex(3)), 0);
    EXPECT_GT(G.Degree(tvertex(4)), 0);
}

TEST_F(TopologicalGraphTest, EdgeAndVertexOperationsMixed) {
    gc->setsize(2, 0);
    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    EXPECT_EQ(G.nv(), 2);
    EXPECT_EQ(G.ne(), 1);

    tvertex v = G.NewVertex();
    EXPECT_EQ(G.nv(), 3);

    G.NewEdge(v, tvertex(1));
    EXPECT_EQ(G.ne(), 2);

    G.NewEdge(v, tvertex(2));
    EXPECT_EQ(G.ne(), 3);

    // Now we have triangle: 1-2, 1-3, 2-3
    EXPECT_EQ(G.Degree(v), 2);
    EXPECT_EQ(G.Degree(tvertex(1)), 2);
    EXPECT_EQ(G.Degree(tvertex(2)), 2);
}

/**
 * Test boundary conditions
 */
TEST_F(TopologicalGraphTest, SingleEdgeGraph) {
    gc->setsize(2, 0);
    TopologicalGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(G.nv(), 2);
    EXPECT_EQ(G.ne(), 1);
    EXPECT_EQ(G.Degree(tvertex(1)), 1);
    EXPECT_EQ(G.Degree(tvertex(2)), 1);
}

TEST_F(TopologicalGraphTest, EmptyTopologicalGraph) {
    gc->setsize(0, 0);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 0);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(TopologicalGraphTest, SingleVertexGraph) {
    gc->setsize(1, 0);
    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 1);
    EXPECT_EQ(G.ne(), 0);
    EXPECT_EQ(G.Degree(tvertex(1)), 0);
}
