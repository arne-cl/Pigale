/****************************************************************************
**
** Unit tests for Graph class - Phase 2 Core Data Structures
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include <Pigale.h>
#include "graph_builders.h"
#include "graph_comparators.h"

// Test fixture for Graph class tests
class GraphTest : public ::testing::Test {
protected:
    void SetUp() override {
        gc = new GraphContainer();
    }

    void TearDown() override {
        delete gc;
    }

    GraphContainer* gc;
};

// Basic Construction and Initialization Tests

TEST_F(GraphTest, ConstructFromGraphContainer) {
    gc->setsize(5, 0);

    {
        Graph G(*gc);
        EXPECT_EQ(G.nv(), 5);
        EXPECT_EQ(G.ne(), 0);
    }  // G destroyed before gc
}

TEST_F(GraphTest, VinPropertyInitialized) {
    gc->setsize(4, 0);

    Graph G(*gc);

    // vin[0] should be initialized to 0
    EXPECT_EQ(G.vin[0](), 0);
}

TEST_F(GraphTest, EmptyGraphProperties) {
    gc->setsize(0, 0);

    Graph G(*gc);

    EXPECT_EQ(G.nv(), 0);
    EXPECT_EQ(G.ne(), 0);
}

// Edge Count Tests

TEST_F(GraphTest, EdgeCountTracking) {
    gc->setsize(4, 0);

    Graph G(*gc);

    EXPECT_EQ(G.ne(), 0);

    // Add some edges via TopologicalGraph (Graph doesn't have NewEdge)
    TopologicalGraph TG(*gc);
    TG.NewEdge(tvertex(1), tvertex(2));
    TG.NewEdge(tvertex(2), tvertex(3));

    // Graph should see updated edge count
    EXPECT_EQ(G.ne(), 2);
}

// Vertex and Edge Iteration Tests

TEST_F(GraphTest, VertexIteration) {
    gc->setsize(5, 0);

    Graph G(*gc);

    int count = 0;
    for(tvertex v = 1; v <= G.nv(); v++) {
        count++;
        EXPECT_GE(v(), 1);
        EXPECT_LE(v(), 5);
    }

    EXPECT_EQ(count, 5);
}

TEST_F(GraphTest, EdgeIterationWithEdges) {
    gc->setsize(4, 0);

    TopologicalGraph G(*gc);
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(4));

    int count = 0;
    for(tedge e = 1; e <= G.ne(); e++) {
        count++;
        EXPECT_GE(e(), 1);
        EXPECT_LE(e(), 3);
    }

    EXPECT_EQ(count, 3);
}

// Vin Property Tests

TEST_F(GraphTest, VinStoresVertexIncidence) {
    gc->setsize(4, 0);

    TopologicalGraph G(*gc);
    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    // Positive brin should be incident to first vertex
    // Negative brin should be incident to second vertex
    EXPECT_EQ(G.vin[e](), 1);
    EXPECT_EQ(G.vin[-e](), 2);
}

TEST_F(GraphTest, VinUpdatesForMultipleEdges) {
    gc->setsize(4, 0);

    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(4));

    // Check all edges have correct vertex incidence
    EXPECT_EQ(G.vin[1](), 1);
    EXPECT_EQ(G.vin[-1](), 2);

    EXPECT_EQ(G.vin[2](), 2);
    EXPECT_EQ(G.vin[-2](), 3);

    EXPECT_EQ(G.vin[3](), 3);
    EXPECT_EQ(G.vin[-3](), 4);
}

// Brin (Half-Edge) Tests

TEST_F(GraphTest, BrinRepresentation) {
    gc->setsize(3, 0);

    TopologicalGraph G(*gc);
    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    // Edge e has two brins: e and -e
    tbrin b1 = e();
    tbrin b2 = -e();

    // They should be opposite
    EXPECT_EQ(-b1(), b2());  // Compare int values
    EXPECT_EQ(-b2(), b1());

    // And incident to different vertices
    EXPECT_NE(G.vin[b1], G.vin[b2]);
}

// Keep/ClearKeep Tests

TEST_F(GraphTest, KeepMechanismCalledInConstructor) {
    gc->setsize(4, 0);

    {
        Graph G(*gc);

        // keep() is called in constructor which calls Keep(PROP_VIN)
        // We can verify PROP_VIN exists
        EXPECT_TRUE(gc->Set(tbrin()).exist(PROP_VIN));
    }
}

// Integration with GraphContainer

TEST_F(GraphTest, SharesDataWithGraphContainer) {
    gc->setsize(5, 0);

    Graph G(*gc);

    // Both should report same counts
    EXPECT_EQ(G.nv(), gc->nv());
    EXPECT_EQ(G.ne(), gc->ne());

    // Modify through TopologicalGraph
    TopologicalGraph TG(*gc);
    TG.NewEdge(tvertex(1), tvertex(2));

    // Graph should see changes
    EXPECT_EQ(G.ne(), 1);
    EXPECT_EQ(gc->ne(), 1);
}

// Test with Standard Graph Builders

TEST_F(GraphTest, WorksWithK4) {
    GraphContainer* k4_gc = TestHelpers::BuildK4();
    ASSERT_NE(k4_gc, nullptr);

    {
        Graph G(*k4_gc);

        EXPECT_EQ(G.nv(), 4);
        EXPECT_EQ(G.ne(), 6);  // K4 has 6 edges

        // Verify all edges have valid vertex incidence
        for(tedge e = 1; e <= G.ne(); e++) {
            EXPECT_GE(G.vin[e](), 1);
            EXPECT_LE(G.vin[e](), 4);
            EXPECT_GE(G.vin[-e](), 1);
            EXPECT_LE(G.vin[-e](), 4);
        }
    }

    delete k4_gc;
}

TEST_F(GraphTest, WorksWithGrid) {
    GraphContainer* grid_gc = GenerateGrid(3, 3);
    ASSERT_NE(grid_gc, nullptr);

    {
        Graph G(*grid_gc);

        EXPECT_EQ(G.nv(), 9);   // 3x3 grid has 9 vertices
        EXPECT_EQ(G.ne(), 12);  // 3x3 grid has 12 edges

        // All edges should have valid endpoints
        for(tedge e = 1; e <= G.ne(); e++) {
            EXPECT_GE(G.vin[e](), 1);
            EXPECT_LE(G.vin[e](), 9);
        }
    }

    delete grid_gc;
}

// Boundary Conditions

TEST_F(GraphTest, SingleVertex) {
    gc->setsize(1, 0);

    Graph G(*gc);

    EXPECT_EQ(G.nv(), 1);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(GraphTest, TwoVerticesNoEdge) {
    gc->setsize(2, 0);

    Graph G(*gc);

    EXPECT_EQ(G.nv(), 2);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(GraphTest, LargeGraph) {
    gc->setsize(1000, 0);

    Graph G(*gc);

    EXPECT_EQ(G.nv(), 1000);
    EXPECT_EQ(G.ne(), 0);
}

// Memory Management

TEST_F(GraphTest, MultipleGraphsSameContainer) {
    gc->setsize(5, 0);

    Graph G1(*gc);
    Graph G2(*gc);

    // Both should access same data
    EXPECT_EQ(G1.nv(), G2.nv());
    EXPECT_EQ(G1.ne(), G2.ne());
}

TEST_F(GraphTest, SequentialGraphCreation) {
    gc->setsize(3, 0);

    {
        Graph G1(*gc);
        EXPECT_EQ(G1.nv(), 3);
    }  // G1 destroyed

    {
        Graph G2(*gc);
        EXPECT_EQ(G2.nv(), 3);
    }  // G2 destroyed
}
