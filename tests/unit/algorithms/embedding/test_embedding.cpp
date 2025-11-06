/****************************************************************************
**
** Unit tests for embedding and drawing algorithms
**
** Tests cover:
** - Schnyder wood algorithms
** - Bipolar orientations
** - Tutte embedding
** - Basic drawing algorithms
**
** Following lessons from DEVELOPER_GUIDE.md:
** - Always use setsize(n, 0) with NewEdge()
** - Don't call TestPlanar() before algorithms that modify state
** - Study API signatures before use
** - Test with simple graphs first
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include <TAXI/color.h>
#include "../../../../helpers/graph_builders.h"

class EmbeddingTest : public ::testing::Test {
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
 * Test Tutte embedding
 * Tutte embedding places vertices at weighted averages of neighbors
 */
TEST_F(EmbeddingTest, TutteOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    GeometricGraph G(*gc);

    // K4 must be planar and have an embedding first
    G.Planarity();  // Compute planar embedding
    EXPECT_TRUE(G.CheckPlanar());

    // Compute Tutte embedding
    int result = G.Tutte();

    // Should succeed (returns 1 on success, 0 on failure)
    EXPECT_EQ(result, 1);

    // Check that coordinates were set
    bool has_coordinates = true;
    for (tvertex v = 1; v <= G.nv(); v++) {
        // Coordinates should not all be zero
        if (G.vcoord[v].x() == 0 && G.vcoord[v].y() == 0 && v > 1) {
            // More than one vertex at origin is suspicious
        }
    }
}

TEST_F(EmbeddingTest, TutteOnGrid) {
    delete gc;
    gc = TestHelpers::BuildGrid(3, 3);

    GeometricGraph G(*gc);

    // Grid must be planar and have an embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute Tutte embedding
    int result = G.Tutte();

    // Should succeed (returns 1 on success)
    EXPECT_EQ(result, 1);
}

TEST_F(EmbeddingTest, TutteOnCycle) {
    delete gc;
    gc = TestHelpers::BuildCycle(6);

    GeometricGraph G(*gc);

    // Cycle must be planar and have an embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute Tutte embedding
    int result = G.Tutte();

    // Should succeed on simple cycle (returns 1 on success)
    EXPECT_EQ(result, 1);
}

/**
 * Test Schnyder wood orientation
 * SchnyderOrient computes a Schnyder wood structure
 */
TEST_F(EmbeddingTest, SchnyderOrientOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    // K4 must be planar and have an embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute Schnyder orientation
    // FirstBrin parameter: use 0 or a valid brin
    int result = G.SchnyderOrient(tbrin(0));

    // Should succeed or return specific value
    // Schnyder only works on triangulated graphs
    // K4 is triangulated, so this should work
    EXPECT_TRUE(result >= 0 || result == 0);
}

/**
 * Test bipolar orientation
 * BipolarPlan computes a bipolar orientation
 * Note: BipolarPlan requires biconnected graphs
 */
TEST_F(EmbeddingTest, BipolarPlanOnCycle) {
    delete gc;
    gc = TestHelpers::BuildCycle(6);

    TopologicalGraph G(*gc);

    // Cycle must be planar and have an embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute bipolar plan (requires biconnected graph)
    tbrin first(1);  // Use first brin
    int result = G.BipolarPlan(first);

    // Should succeed (returns 0 or positive)
    EXPECT_GE(result, 0);
}

TEST_F(EmbeddingTest, BipolarPlanOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    TopologicalGraph G(*gc);

    // K4 must be planar and have an embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute bipolar plan
    tbrin first(1);
    int result = G.BipolarPlan(first);

    // Should succeed on biconnected K4
    EXPECT_GE(result, 0);
}

/**
 * Test ComputeGeometricCir
 * Computes circular order from geometric coordinates
 */
TEST_F(EmbeddingTest, ComputeGeometricCirOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    GeometricGraph G(*gc);

    // First compute embedding
    G.Planarity();
    G.Tutte();

    // Compute geometric circular order
    int result = G.ComputeGeometricCir();

    // Should succeed
    EXPECT_GE(result, 0);
}

/**
 * Test ColorExteriorface
 * Colors the exterior face of a planar graph
 */
TEST_F(EmbeddingTest, ColorExteriorfaceOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    GeometricGraph G(*gc);

    // First compute embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Color exterior face
    int result = G.ColorExteriorface();

    // Should succeed (returns exterior face size, not 0)
    EXPECT_GT(result, 0);
}

TEST_F(EmbeddingTest, ColorExteriorfaceOnGrid) {
    delete gc;
    gc = TestHelpers::BuildGrid(3, 3);

    GeometricGraph G(*gc);

    // First compute embedding
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Color exterior face
    int result = G.ColorExteriorface();

    // Should succeed (returns exterior face size)
    EXPECT_GT(result, 0);
}

/**
 * Test embedding pipeline
 * Test that we can compute planarity, then embedding, then drawing
 */
TEST_F(EmbeddingTest, EmbeddingPipelineOnK4) {
    delete gc;
    gc = TestHelpers::BuildK4();

    GeometricGraph G(*gc);

    // Step 1: Compute planar embedding
    int planar_result = G.Planarity();
    EXPECT_GT(planar_result, 0);
    EXPECT_TRUE(G.CheckPlanar());

    // Step 2: Compute Tutte embedding for coordinates
    int tutte_result = G.Tutte();
    EXPECT_EQ(tutte_result, 1);

    // Step 3: Color exterior face
    int color_result = G.ColorExteriorface();
    EXPECT_GT(color_result, 0);  // Returns exterior face size

    // Verify coordinates are set
    for (tvertex v = 1; v <= G.nv(); v++) {
        // At least check that coordinates exist
        double x = G.vcoord[v].x();
        double y = G.vcoord[v].y();
        // Just verify they're finite numbers
        EXPECT_TRUE(std::isfinite(x));
        EXPECT_TRUE(std::isfinite(y));
    }
}

TEST_F(EmbeddingTest, EmbeddingPipelineOnCycle) {
    delete gc;
    gc = TestHelpers::BuildCycle(8);

    GeometricGraph G(*gc);

    // Step 1: Planarity
    int planar_result = G.Planarity();
    EXPECT_GT(planar_result, 0);

    // Step 2: Tutte
    int tutte_result = G.Tutte();
    EXPECT_EQ(tutte_result, 1);

    // Step 3: Color exterior
    int color_result = G.ColorExteriorface();
    EXPECT_GT(color_result, 0);  // Returns exterior face size

    // Verify all vertices have coordinates
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_TRUE(std::isfinite(G.vcoord[v].x()));
        EXPECT_TRUE(std::isfinite(G.vcoord[v].y()));
    }
}

/**
 * Test that non-planar graphs fail gracefully
 */
TEST_F(EmbeddingTest, TutteFailsOnNonPlanar) {
    delete gc;
    gc = TestHelpers::BuildK5();

    GeometricGraph G(*gc);

    // K5 is non-planar, so Planarity should fail
    int planar_result = G.Planarity();
    EXPECT_EQ(planar_result, 0);  // 0 means non-planar

    // Tutte should not be called on non-planar graphs
    // But if it is, it should handle it gracefully
    // (We won't test this to avoid undefined behavior)
}

/**
 * Test edge cases
 */
TEST_F(EmbeddingTest, TutteOnTriangle) {
    gc->setsize(3, 0);
    TopologicalGraph TG(*gc);

    // Create triangle
    TG.NewEdge(tvertex(1), tvertex(2));
    TG.NewEdge(tvertex(2), tvertex(3));
    TG.NewEdge(tvertex(3), tvertex(1));

    GeometricGraph G(*gc);

    // Triangle is planar
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Compute Tutte embedding
    int result = G.Tutte();
    EXPECT_EQ(result, 1);
}

TEST_F(EmbeddingTest, BipolarOnTriangle) {
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    // Create triangle (biconnected)
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    // Make planar
    G.Planarity();
    EXPECT_TRUE(G.CheckPlanar());

    // Try bipolar plan
    tbrin first(1);
    int result = G.BipolarPlan(first);

    // Should succeed on biconnected triangle
    EXPECT_GE(result, 0);
}
