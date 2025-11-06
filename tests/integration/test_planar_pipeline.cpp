/****************************************************************************
**
** Integration tests for planarity testing pipeline
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include <TAXI/color.h>
#include <Pigale.h>
#include "graph_builders.h"
#include "graph_comparators.h"
#include "property_validators.h"

// Test fixture for planar pipeline tests
class PlanarPipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup any allocated graphs
    }
};

// Basic Pipeline Tests

TEST_F(PlanarPipelineTest, GridGraphWorkflow) {
    // Generate → Test → Embed workflow
    GraphContainer* gc = GenerateGrid(5, 5);
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // Test planarity
    ASSERT_TRUE(G.TestPlanar()) << "5x5 grid should be planar";

    // Compute embedding
    tbrin result = G.Planarity();
    EXPECT_NE(result, 0) << "Planarity should succeed";

    // Validate planar map
    EXPECT_TRUE(TestHelpers::ValidatePlanarMap(G)) << "Planar map should be valid";

    delete gc;
}

TEST_F(PlanarPipelineTest, K4IsPlanarandEmbeddable) {
    GraphContainer* gc = TestHelpers::BuildK4();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // K4 is planar
    EXPECT_TRUE(G.TestPlanar());

    // Should be able to compute embedding
    tbrin result = G.Planarity();
    EXPECT_NE(result, 0);

    // Circular order should be valid
    EXPECT_TRUE(TestHelpers::ValidateCircularOrder(G));

    delete gc;
}

TEST_F(PlanarPipelineTest, K5IsNonPlanar) {
    GraphContainer* gc = TestHelpers::BuildK5();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // K5 is non-planar
    EXPECT_FALSE(G.TestPlanar()) << "K5 should be non-planar";

    delete gc;
}

TEST_F(PlanarPipelineTest, K33IsNonPlanar) {
    GraphContainer* gc = TestHelpers::BuildK33();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(*gc);

    // K_{3,3} is non-planar
    EXPECT_FALSE(G.TestPlanar()) << "K3,3 should be non-planar";

    delete gc;
}

/**
 * Full Embedding Pipeline Tests
 * Test complete workflows from graph creation to drawing
 */

TEST_F(PlanarPipelineTest, FullEmbeddingWorkflowOnK4) {
    // Pipeline: Create → Planarity → Tutte → Color Exterior Face
    GraphContainer* gc = TestHelpers::BuildK4();
    GeometricGraph G(*gc);

    // Step 1: Compute planar embedding
    int planar_result = G.Planarity();
    ASSERT_GT(planar_result, 0) << "K4 should be planar";
    EXPECT_TRUE(G.CheckPlanar()) << "CheckPlanar should confirm planarity";

    // Step 2: Compute Tutte embedding (assigns coordinates)
    int tutte_result = G.Tutte();
    EXPECT_EQ(tutte_result, 1) << "Tutte should succeed on K4";

    // Step 3: Verify coordinates were set
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_TRUE(std::isfinite(G.vcoord[v].x())) << "Vertex " << v() << " has invalid x coordinate";
        EXPECT_TRUE(std::isfinite(G.vcoord[v].y())) << "Vertex " << v() << " has invalid y coordinate";
    }

    // Step 4: Color exterior face
    int face_size = G.ColorExteriorface();
    EXPECT_GT(face_size, 0) << "ColorExteriorface should return face size";

    delete gc;
}

TEST_F(PlanarPipelineTest, FullEmbeddingWorkflowOnGrid) {
    // Test on more complex planar graph
    GraphContainer* gc = GenerateGrid(3, 3);
    GeometricGraph G(*gc);

    // Planarity
    int planar_result = G.Planarity();
    ASSERT_GT(planar_result, 0) << "Grid should be planar";

    // Tutte embedding
    int tutte_result = G.Tutte();
    EXPECT_EQ(tutte_result, 1) << "Tutte should succeed on grid";

    // Verify all vertices have coordinates
    int vertices_with_coords = 0;
    for (tvertex v = 1; v <= G.nv(); v++) {
        if (std::isfinite(G.vcoord[v].x()) && std::isfinite(G.vcoord[v].y())) {
            vertices_with_coords++;
        }
    }
    EXPECT_EQ(vertices_with_coords, G.nv()) << "All vertices should have coordinates";

    delete gc;
}

TEST_F(PlanarPipelineTest, BipolarOrientationWorkflow) {
    // Pipeline: Create biconnected graph → Planarity → BipolarPlan
    GraphContainer* gc = TestHelpers::BuildCycle(8);
    TopologicalGraph G(*gc);

    // Verify biconnected (required for BipolarPlan)
    ASSERT_TRUE(G.CheckBiconnected()) << "Cycle should be biconnected";

    // Compute planar embedding
    int planar_result = G.Planarity();
    ASSERT_GT(planar_result, 0) << "Cycle should be planar";

    // Compute bipolar orientation
    tbrin first(1);
    int bipolar_result = G.BipolarPlan(first);
    EXPECT_GE(bipolar_result, 0) << "BipolarPlan should succeed on biconnected cycle";

    delete gc;
}

TEST_F(PlanarPipelineTest, SchnyderWorkflow) {
    // Pipeline: Create → Planarity → SchnyderOrient
    GraphContainer* gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // Planarity
    int planar_result = G.Planarity();
    ASSERT_GT(planar_result, 0);

    // Schnyder orientation (tbrin(0) means let algorithm choose)
    int schnyder_result = G.SchnyderOrient(tbrin(0));
    EXPECT_TRUE(schnyder_result >= 0 || schnyder_result == 0) << "SchnyderOrient should succeed on K4";

    delete gc;
}

/**
 * Graph Modification and Re-testing
 */

TEST_F(PlanarPipelineTest, AddEdgeAndRetest) {
    // Start with path (planar), add edges to make K4 (still planar)
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    // Build path: 1-2-3-4
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(4));

    // Path is planar
    EXPECT_TRUE(G.TestPlanar()) << "Path should be planar";

    // Add more edges to form K4
    G.NewEdge(tvertex(1), tvertex(3));
    G.NewEdge(tvertex(1), tvertex(4));
    G.NewEdge(tvertex(2), tvertex(4));

    // K4 is still planar
    EXPECT_TRUE(G.TestPlanar()) << "K4 should be planar";

    // Can compute embedding
    int result = G.Planarity();
    EXPECT_GT(result, 0) << "Should be able to embed K4";

    delete gc;
}

TEST_F(PlanarPipelineTest, DeleteEdgePreservesPlanarity) {
    // Start with K4, delete edges, should remain planar
    GraphContainer* gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // K4 is planar
    EXPECT_TRUE(G.TestPlanar());
    int initial_edges = G.ne();

    // Delete an edge
    if (G.ne() > 0) {
        G.DeleteEdge(tedge(1));

        // Should still be planar (it's a subgraph of planar graph)
        EXPECT_TRUE(G.TestPlanar()) << "Subgraph of planar graph should be planar";
        EXPECT_EQ(G.ne(), initial_edges - 1) << "Edge count should decrease by 1";
    }

    delete gc;
}

/**
 * Property Verification Across Pipeline
 */

TEST_F(PlanarPipelineTest, PlanarityPreservesVertexCount) {
    GraphContainer* gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    int initial_vertices = G.nv();
    int initial_edges = G.ne();

    // Planarity should not change graph structure
    G.Planarity();

    EXPECT_EQ(G.nv(), initial_vertices) << "Planarity should not change vertex count";
    EXPECT_EQ(G.ne(), initial_edges) << "Planarity should not change edge count";

    delete gc;
}

TEST_F(PlanarPipelineTest, CircularOrderConsistentAfterPlanarity) {
    GraphContainer* gc = TestHelpers::BuildGrid(3, 3);
    TopologicalGraph G(*gc);

    // Compute embedding
    int result = G.Planarity();
    ASSERT_GT(result, 0);

    // Circular order should be consistent
    EXPECT_TRUE(TestHelpers::ValidateCircularOrder(G)) << "Circular order should be valid after Planarity";

    delete gc;
}

/**
 * Complex Workflow Tests
 */

TEST_F(PlanarPipelineTest, MakeConnectedThenTest) {
    // Create disconnected graph, connect it, then test planarity
    GraphContainer* gc = new GraphContainer();
    gc->setsize(6, 0);
    TopologicalGraph G(*gc);

    // Build two disconnected triangles
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    G.NewEdge(tvertex(4), tvertex(5));
    G.NewEdge(tvertex(5), tvertex(6));
    G.NewEdge(tvertex(6), tvertex(4));

    // Should be disconnected
    EXPECT_GT(G.NumberOfConnectedComponents(), 1) << "Should have multiple components";

    // Make connected
    G.MakeConnected();

    // Should be connected now
    EXPECT_EQ(G.NumberOfConnectedComponents(), 1) << "Should be connected";

    // Two triangles connected should still be planar
    EXPECT_TRUE(G.TestPlanar()) << "Connected planar graphs should be planar";

    delete gc;
}

TEST_F(PlanarPipelineTest, BiconnectThenBipolar) {
    // Create path, make it biconnected, then compute bipolar orientation
    GraphContainer* gc = TestHelpers::BuildPath(5);
    TopologicalGraph G(*gc);

    // Path is not biconnected
    EXPECT_FALSE(G.CheckBiconnected()) << "Path should not be biconnected";

    // Make biconnected (adds edges)
    G.Biconnect();

    // Should be biconnected now
    EXPECT_TRUE(G.CheckBiconnected()) << "Should be biconnected after Biconnect()";

    // Should still be planar
    EXPECT_TRUE(G.TestPlanar()) << "Biconnected planar graph should be planar";

    // Now can compute bipolar orientation
    G.Planarity();
    tbrin first(1);
    int result = G.BipolarPlan(first);
    EXPECT_GE(result, 0) << "BipolarPlan should succeed on biconnected planar graph";

    delete gc;
}

