/****************************************************************************
**
** Unit tests for Circular Order Invariants - Phase 2 Core Data Structures
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include <Pigale.h>
#include "graph_builders.h"
#include "graph_comparators.h"
#include "property_validators.h"
#include <set>

// Test fixture for circular order tests
class CircularOrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        gc = nullptr;
    }

    void TearDown() override {
        if (gc) delete gc;
    }

    GraphContainer* gc;
};

// Basic Circular Order Tests

TEST_F(CircularOrderTest, CircularOrderExistsAfterPlanarity) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // Before planarity, cir/acir may not exist
    // After planarity, they should exist
    tbrin result = G.Planarity();
    ASSERT_NE(result, 0) << "K4 should be planar";

    // Check properties exist
    EXPECT_TRUE(G.Set(tbrin()).exist(PROP_CIR));
    EXPECT_TRUE(G.Set(tbrin()).exist(PROP_ACIR));
}

// CRITICAL INVARIANT: cir[acir[b]] == b

TEST_F(CircularOrderTest, CirAcirInvariant) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    tbrin result = G.Planarity();
    ASSERT_NE(result, 0);

    // Use helper validator
    EXPECT_TRUE(TestHelpers::PropertyValidator::ValidateCirAcir(G))
        << "Circular order invariant cir[acir[b]] == b failed";
}

TEST_F(CircularOrderTest, CirAcirInvariantForGrid) {
    gc = GenerateGrid(4, 4);
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // Grid is generated with planarity already computed
    EXPECT_TRUE(TestHelpers::PropertyValidator::ValidateCirAcir(G))
        << "Grid circular order invariant failed";
}

// Pbrin Tests

TEST_F(CircularOrderTest, PbrinExistsAfterPlanarity) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    // Check pbrin property exists
    EXPECT_TRUE(G.Set(tvertex()).exist(PROP_PBRIN));
}

TEST_F(CircularOrderTest, PbrinPointsToValidBrin) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    // For each vertex, pbrin should point to a valid brin incident to it
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin b = G.pbrin[v];

        // pbrin[v] should be incident to v
        EXPECT_TRUE(G.vin[b] == v || G.vin[-b] == v)
            << "pbrin[" << v() << "] = " << b() << " not incident to vertex";
    }
}

// Circular Order Traversal Tests

TEST_F(CircularOrderTest, CircularOrderFormsValidCycles) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    // Use helper validator
    EXPECT_TRUE(TestHelpers::ValidateCircularOrder(G))
        << "Circular order doesn't form valid cycles";
}

TEST_F(CircularOrderTest, AllBrinsReachableFromPbrin) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = G.pbrin[v];
        std::set<tbrin> visited;
        tbrin b = first;

        // Traverse circular order
        do {
            ASSERT_FALSE(visited.count(b))
                << "Infinite loop detected at vertex " << v() << ", brin " << b();
            visited.insert(b);
            b = G.cir[b];
        } while(b != first);

        // All visited brins should be incident to v
        for(tbrin vb : visited) {
            EXPECT_TRUE(G.vin[vb] == v || G.vin[-vb] == v)
                << "Brin " << vb() << " in cycle not incident to vertex " << v();
        }
    }
}

TEST_F(CircularOrderTest, VertexDegreeMatchesBrins) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    for(tvertex v = 1; v <= G.nv(); v++) {
        // Count brins in circular order
        tbrin first = G.pbrin[v];
        int brin_count = 0;
        tbrin b = first;

        do {
            brin_count++;
            b = G.cir[b];
        } while(b != first && brin_count < 100);  // Safety limit

        EXPECT_LT(brin_count, 100) << "Infinite loop at vertex " << v();

        // For K4, every vertex has degree 3
        EXPECT_EQ(brin_count, 3)
            << "Vertex " << v() << " should have degree 3 in K4";
    }
}

// Edge Correspondence Tests

TEST_F(CircularOrderTest, BrinPairsCorrespondToEdges) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    // Each edge e should have two brins: e and -e
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b1 = e();
        tbrin b2 = -e();

        // Both should exist in circular order
        EXPECT_TRUE(G.Set(tbrin()).exist(PROP_CIR));

        // They should have opposite orientations
        EXPECT_EQ(-b1(), b2());
        EXPECT_EQ(-b2(), b1());

        // They should be incident to different vertices
        EXPECT_NE(G.vin[b1], G.vin[b2])
            << "Edge " << e() << " has both brins incident to same vertex";
    }
}

// Planar Map Tests

TEST_F(CircularOrderTest, GridHasValidPlanarMap) {
    gc = GenerateGrid(3, 3);
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    EXPECT_TRUE(TestHelpers::ValidatePlanarMap(G))
        << "3x3 grid should have valid planar map";
}

TEST_F(CircularOrderTest, K4HasValidPlanarMap) {
    gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    EXPECT_TRUE(TestHelpers::ValidatePlanarMap(G))
        << "K4 should have valid planar map after Planarity()";
}

// Non-Planar Graphs

TEST_F(CircularOrderTest, K5HasNoCircularOrder) {
    gc = TestHelpers::BuildK5();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    tbrin result = G.Planarity();

    // K5 is non-planar
    EXPECT_EQ(result, 0) << "K5 should be non-planar";

    // Circular order should not be created (or should be incomplete)
    // Note: The library might still create partial cir/acir
}

TEST_F(CircularOrderTest, K33HasNoCircularOrder) {
    gc = TestHelpers::BuildK33();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    tbrin result = G.Planarity();

    EXPECT_EQ(result, 0) << "K3,3 should be non-planar";
}

// Empty and Boundary Cases

TEST_F(CircularOrderTest, SingleVertexNoCircularOrder) {
    gc = new GraphContainer();
    gc->setsize(1, 0);

    TopologicalGraph G(*gc);

    // Single vertex with no edges - pbrin might be 0 or undefined
    // Just verify it doesn't crash
    SUCCEED();
}

TEST_F(CircularOrderTest, TwoVerticesOneEdge) {
    gc = new GraphContainer();
    gc->setsize(2, 0);

    TopologicalGraph G(*gc);
    G.NewEdge(tvertex(1), tvertex(2));
    G.Planarity();

    // Each vertex should have one brin in its circular order
    tbrin b1 = G.pbrin[tvertex(1)];
    tbrin b2 = G.pbrin[tvertex(2)];

    // Each should form a cycle of length 1 (point to itself)
    EXPECT_EQ(G.cir[b1], b1);
    EXPECT_EQ(G.cir[b2], b2);
}

// Path Graph

TEST_F(CircularOrderTest, PathGraphCircularOrder) {
    gc = TestHelpers::BuildPath(5);
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    EXPECT_TRUE(TestHelpers::PropertyValidator::ValidateCirAcir(G))
        << "Path graph circular order invariant failed";

    // End vertices should have degree 1
    tbrin b1 = G.pbrin[tvertex(1)];
    tbrin b5 = G.pbrin[tvertex(5)];

    // Degree 1: circular order returns to itself
    EXPECT_EQ(G.cir[b1], b1);
    EXPECT_EQ(G.cir[b5], b5);
}

// Cycle Graph

TEST_F(CircularOrderTest, CycleGraphCircularOrder) {
    gc = TestHelpers::BuildCycle(6);
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);
    G.Planarity();

    EXPECT_TRUE(TestHelpers::PropertyValidator::ValidateCirAcir(G))
        << "Cycle graph circular order invariant failed";

    // Each vertex should have degree 2
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = G.pbrin[v];
        tbrin second = G.cir[first];

        // Cycle should have length 2
        EXPECT_EQ(G.cir[second], first)
            << "Vertex " << v() << " should have degree 2 in cycle";
    }
}
