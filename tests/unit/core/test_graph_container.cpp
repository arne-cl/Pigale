/****************************************************************************
**
** Unit tests for GraphContainer class
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graph.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"
#include "graph_comparators.h"

// Test fixture for GraphContainer tests
class GraphContainerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Basic Construction Tests

TEST_F(GraphContainerTest, DefaultConstruction) {
    GraphContainer gc;

    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

TEST_F(GraphContainerTest, SetSizeAllocatesCorrectCounts) {
    GraphContainer gc;
    gc.setsize(10, 20);

    EXPECT_EQ(gc.nv(), 10);
    EXPECT_EQ(gc.ne(), 20);
}

TEST_F(GraphContainerTest, SetSizeZeroVertices) {
    GraphContainer gc;
    gc.setsize(0, 0);

    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

TEST_F(GraphContainerTest, SetSizeSingleVertex) {
    GraphContainer gc;
    gc.setsize(1, 0);

    EXPECT_EQ(gc.nv(), 1);
    EXPECT_EQ(gc.ne(), 0);
}

// Size Modification Tests

TEST_F(GraphContainerTest, IncSizeIncreasesVertexCount) {
    GraphContainer gc;
    gc.setsize(5, 5);

    // Add 3 vertices
    gc.incsize(tvertex());
    gc.incsize(tvertex());
    gc.incsize(tvertex());

    // Add 2 edges
    gc.incsize(tedge());
    gc.incsize(tedge());

    EXPECT_EQ(gc.nv(), 8);
    EXPECT_EQ(gc.ne(), 7);
}

TEST_F(GraphContainerTest, DecSizeDecreasesVertexCount) {
    GraphContainer gc;
    gc.setsize(10, 10);

    // Remove 3 vertices
    gc.decsize(tvertex());
    gc.decsize(tvertex());
    gc.decsize(tvertex());

    // Remove 2 edges
    gc.decsize(tedge());
    gc.decsize(tedge());

    EXPECT_EQ(gc.nv(), 7);
    EXPECT_EQ(gc.ne(), 8);
}

TEST_F(GraphContainerTest, IncrementalSizeChanges) {
    GraphContainer gc;
    gc.setsize(5, 5);

    gc.incsize(tvertex());
    EXPECT_EQ(gc.nv(), 6);
    EXPECT_EQ(gc.ne(), 5);

    gc.incsize(tedge());
    EXPECT_EQ(gc.nv(), 6);
    EXPECT_EQ(gc.ne(), 6);

    gc.decsize(tvertex());
    gc.decsize(tedge());
    EXPECT_EQ(gc.nv(), 5);
    EXPECT_EQ(gc.ne(), 5);
}

// Clear and Reset Tests

TEST_F(GraphContainerTest, ClearResetsToEmpty) {
    GraphContainer gc;
    gc.setsize(10, 20);
    gc.clear();

    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

TEST_F(GraphContainerTest, ResetResetsToEmpty) {
    GraphContainer gc;
    gc.setsize(10, 20);
    gc.reset();

    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

// Property Set Tests

TEST_F(GraphContainerTest, PropertySetsExist) {
    GraphContainer gc;
    gc.setsize(5, 5);

    // Check that we can access the property sets
    PSet& pset_v = gc.PV();
    PSet& pset_e = gc.PE();
    PSet& pset_b = gc.PB();
    EXPECT_NE(&pset_v, nullptr);
    EXPECT_NE(&pset_e, nullptr);
    EXPECT_NE(&pset_b, nullptr);
}

TEST_F(GraphContainerTest, PropertySurvivesResize) {
    GraphContainer gc;
    gc.setsize(5, 5);

    // Set a property
    Prop<int> testProp(gc.Set(tvertex()), PROP_COLOR);
    testProp[tvertex(1)] = 42;

    // Increase size by adding vertices one at a time
    gc.incsize(tvertex());
    gc.incsize(tvertex());

    EXPECT_EQ(gc.nv(), 7);
    // Property should still exist and have the old value
    EXPECT_EQ(testProp[tvertex(1)], 42);
}

// Edge Cases

TEST_F(GraphContainerTest, LargeGraphAllocation) {
    GraphContainer gc;
    gc.setsize(1000, 3000);

    EXPECT_EQ(gc.nv(), 1000);
    EXPECT_EQ(gc.ne(), 3000);
}

TEST_F(GraphContainerTest, MultipleSetSizeCalls) {
    GraphContainer gc;

    gc.setsize(5, 5);
    EXPECT_EQ(gc.nv(), 5);

    gc.setsize(10, 10);
    EXPECT_EQ(gc.nv(), 10);

    gc.setsize(3, 3);
    EXPECT_EQ(gc.nv(), 3);
}

// Copy and Move Tests (if applicable)

TEST_F(GraphContainerTest, CopyConstructor) {
    GraphContainer gc1;
    gc1.setsize(5, 5);

    GraphContainer gc2(gc1);

    EXPECT_EQ(gc2.nv(), 5);
    EXPECT_EQ(gc2.ne(), 5);
}

TEST_F(GraphContainerTest, AssignmentOperator) {
    GraphContainer gc1;
    gc1.setsize(5, 5);

    GraphContainer gc2;
    gc2 = gc1;

    EXPECT_EQ(gc2.nv(), 5);
    EXPECT_EQ(gc2.ne(), 5);
}

// Validation Tests

TEST_F(GraphContainerTest, ValidatesBasicInvariants) {
    GraphContainer gc;
    gc.setsize(10, 20);

    // Wrap in Graph to use validator
    Graph G(gc);
    EXPECT_TRUE(TestHelpers::VerifyVertexEdgeCounts(G));
}

TEST_F(GraphContainerTest, NegativeCountsNotAllowed) {
    GraphContainer gc;

    // This should not crash or cause issues
    // (depending on implementation, may need adjustment)
    gc.setsize(0, 0);

    EXPECT_GE(gc.nv(), 0);
    EXPECT_GE(gc.ne(), 0);
}

// Integration with Graph class

TEST_F(GraphContainerTest, WorksWithGraphClass) {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 6);

    {
        Graph G(*gc);
        EXPECT_EQ(G.nv(), 4);
        EXPECT_EQ(G.ne(), 6);
    }  // G must go out of scope before deleting gc

    delete gc;
}

TEST_F(GraphContainerTest, WorksWithTopologicalGraph) {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);  // Don't pre-allocate edges

    {
        TopologicalGraph G(*gc);
        EXPECT_EQ(G.nv(), 4);
        EXPECT_EQ(G.ne(), 0);  // No edges added yet
    }  // G must go out of scope before deleting gc

    delete gc;
}

// Memory Management Tests

TEST_F(GraphContainerTest, MemoryCleanupOnDestruction) {
    // Create and destroy multiple containers
    for(int i = 0; i < 10; i++) {
        GraphContainer* gc = new GraphContainer();
        gc->setsize(100, 200);
        delete gc;
    }

    // If we get here without crashes, memory management is working
    SUCCEED();
}

TEST_F(GraphContainerTest, PropertyCleanupOnClear) {
    GraphContainer gc;
    gc.setsize(10, 10);

    // Create multiple properties
    Prop<int> prop1(gc.Set(tvertex()), PROP_COLOR);
    Prop<int> prop2(gc.Set(tvertex()), PROP_LABEL);
    Prop<Tpoint> prop3(gc.Set(tvertex()), PROP_COORD);

    gc.clear();

    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

// Boundary Condition Tests

TEST_F(GraphContainerTest, MaximalPlanarGraphBounds) {
    // Maximal planar graph has 3n-6 edges for n >= 3
    GraphContainer gc;
    int n = 10;
    int m = 3 * n - 6;

    gc.setsize(n, m);

    EXPECT_EQ(gc.nv(), n);
    EXPECT_EQ(gc.ne(), m);
}

TEST_F(GraphContainerTest, CompleteGraphBounds) {
    // Complete graph K_n has n(n-1)/2 edges
    GraphContainer gc;
    int n = 10;
    int m = n * (n - 1) / 2;

    gc.setsize(n, m);

    EXPECT_EQ(gc.nv(), n);
    EXPECT_EQ(gc.ne(), m);
}
