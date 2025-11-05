/****************************************************************************
**
** Integration tests for planarity testing pipeline
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
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

    TopologicalGraph G(gc);

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

    TopologicalGraph G(gc);

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

    TopologicalGraph G(gc);

    // K5 is non-planar
    EXPECT_FALSE(G.TestPlanar()) << "K5 should be non-planar";

    delete gc;
}

TEST_F(PlanarPipelineTest, K33IsNonPlanar) {
    GraphContainer* gc = TestHelpers::BuildK33();
    ASSERT_NE(gc, nullptr);

    TopologicalGraph G(gc);

    // K_{3,3} is non-planar
    EXPECT_FALSE(G.TestPlanar()) << "K3,3 should be non-planar";

    delete gc;
}

// This is a placeholder integration test file
// More comprehensive tests will be added as we implement the test suite
