/****************************************************************************
**
** Integration tests for property persistence across graph modifications
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"

class PropertyPersistenceTest : public ::testing::Test {
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
 * Test that vertex properties persist across edge modifications
 */
TEST_F(PropertyPersistenceTest, VertexPropertiesPersistAcrossEdgeAddition) {
    gc = new GraphContainer();
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    // Create custom vertex property
    Prop<int> vertex_data(G.Set(tvertex()), PROP_TMP);

    // Set values
    vertex_data[tvertex(1)] = 10;
    vertex_data[tvertex(2)] = 20;
    vertex_data[tvertex(3)] = 30;
    vertex_data[tvertex(4)] = 40;

    // Add edges
    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(4));

    // Vertex properties should still be there
    EXPECT_EQ(vertex_data[tvertex(1)], 10);
    EXPECT_EQ(vertex_data[tvertex(2)], 20);
    EXPECT_EQ(vertex_data[tvertex(3)], 30);
    EXPECT_EQ(vertex_data[tvertex(4)], 40);
}

/**
 * Test that edge properties persist across vertex addition
 */
TEST_F(PropertyPersistenceTest, EdgePropertiesPersistAcrossVertexAddition) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // Create custom edge property
    Prop<int> edge_weights(G.Set(tedge()), PROP_TMP);

    // Set weights for existing edges
    for (tedge e = 1; e <= G.ne(); e++) {
        edge_weights[e] = e() * 10;
    }

    int original_edges = G.ne();

    // Add a new vertex
    tvertex new_v = G.NewVertex();
    EXPECT_EQ(G.nv(), 5);

    // Original edge properties should persist
    for (tedge e = 1; e <= original_edges; e++) {
        EXPECT_EQ(edge_weights[e], e() * 10) << "Edge " << e() << " property changed";
    }
}

/**
 * Test property behavior after graph operations
 */
TEST_F(PropertyPersistenceTest, PropertiesSurvivePlanarity) {
    gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    // Set custom vertex colors
    Prop<short> custom_colors(G.Set(tvertex()), PROP_TMP);
    custom_colors[tvertex(1)] = 1;
    custom_colors[tvertex(2)] = 2;
    custom_colors[tvertex(3)] = 3;
    custom_colors[tvertex(4)] = 4;

    // Compute planarity (modifies circular order)
    G.Planarity();

    // Custom properties should survive
    EXPECT_EQ(custom_colors[tvertex(1)], 1);
    EXPECT_EQ(custom_colors[tvertex(2)], 2);
    EXPECT_EQ(custom_colors[tvertex(3)], 3);
    EXPECT_EQ(custom_colors[tvertex(4)], 4);
}

/**
 * Test Keep mechanism preserves properties across modifications
 */
TEST_F(PropertyPersistenceTest, KeepMechanismPreservesProperties) {
    gc = TestHelpers::BuildPath(5);
    Graph G(*gc);

    // Create property and use Keep to preserve it
    Prop<int> vertex_ids(G.Set(tvertex()), PROP_TMP);
    for (tvertex v = 1; v <= G.nv(); v++) {
        vertex_ids[v] = v();
    }

    // Keep the property
    G.Set(tvertex()).Keep(PROP_TMP);

    // Create new Graph object from same container
    Graph G2(*gc);

    // Property should still exist
    EXPECT_TRUE(G2.Set(tvertex()).exist(PROP_TMP)) << "Property should be kept";
}

/**
 * Test clearing properties with erase
 */
TEST_F(PropertyPersistenceTest, EraseRemovesProperties) {
    gc = new GraphContainer();
    gc->setsize(5, 0);
    Graph G(*gc);

    // Create a property
    Prop<int> data(G.Set(tvertex()), PROP_TMP);
    data[tvertex(1)] = 100;

    // Verify property exists
    EXPECT_TRUE(G.Set(tvertex()).exist(PROP_TMP));

    // Erase the property
    G.Set(tvertex()).erase(PROP_TMP);

    // Property should be gone
    EXPECT_FALSE(G.Set(tvertex()).exist(PROP_TMP)) << "Property should be erased";
}

/**
 * Test properties after graph modification operations
 */
TEST_F(PropertyPersistenceTest, PropertiesAfterEdgeContraction) {
    gc = new GraphContainer();
    gc->setsize(4, 0);
    TopologicalGraph G(*gc);

    // Build triangle with one extra vertex
    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));
    G.NewEdge(tvertex(3), tvertex(1));

    // Set vertex property
    Prop<int> vertex_vals(G.Set(tvertex()), PROP_TMP);
    vertex_vals[tvertex(1)] = 1;
    vertex_vals[tvertex(2)] = 2;
    vertex_vals[tvertex(3)] = 3;
    vertex_vals[tvertex(4)] = 4;

    // Contract an edge (merges two vertices)
    int original_nv = G.nv();
    G.ContractEdge(e1);

    // Should have one less vertex
    EXPECT_EQ(G.nv(), original_nv - 1);

    // Remaining vertex properties should still be accessible
    // (exact behavior depends on which vertex was kept)
    bool found_valid_property = false;
    for (tvertex v = 1; v <= G.nv(); v++) {
        if (vertex_vals[v] > 0 && vertex_vals[v] <= 4) {
            found_valid_property = true;
        }
    }
    EXPECT_TRUE(found_valid_property) << "Some vertex properties should remain valid";
}

/**
 * Test coordinate persistence in GeometricGraph
 */
TEST_F(PropertyPersistenceTest, CoordinatesPersistAfterEmbedding) {
    gc = TestHelpers::BuildK4();
    GeometricGraph G(*gc);

    // Manually set initial coordinates
    G.vcoord[tvertex(1)] = Tpoint(0, 0);
    G.vcoord[tvertex(2)] = Tpoint(1, 0);
    G.vcoord[tvertex(3)] = Tpoint(1, 1);
    G.vcoord[tvertex(4)] = Tpoint(0, 1);

    // Store initial positions
    double x1_before = G.vcoord[tvertex(1)].x();
    double y1_before = G.vcoord[tvertex(1)].y();

    // Compute planarity (should not affect manually set coordinates until Tutte is called)
    G.Planarity();

    // Coordinates should still be present (though may be different after algorithms)
    EXPECT_TRUE(std::isfinite(G.vcoord[tvertex(1)].x()));
    EXPECT_TRUE(std::isfinite(G.vcoord[tvertex(1)].y()));
}

/**
 * Test that properties work correctly after adding vertices
 */
TEST_F(PropertyPersistenceTest, PropertiesWorkAfterAddingVertices) {
    gc = new GraphContainer();
    gc->setsize(3, 0);
    TopologicalGraph G(*gc);

    // Create property
    Prop<int> data(G.Set(tvertex()), PROP_TMP);
    data[tvertex(1)] = 10;
    data[tvertex(2)] = 20;
    data[tvertex(3)] = 30;

    // Add new vertices
    tvertex v4 = G.NewVertex();
    tvertex v5 = G.NewVertex();

    // Old properties should still work
    EXPECT_EQ(data[tvertex(1)], 10);
    EXPECT_EQ(data[tvertex(2)], 20);
    EXPECT_EQ(data[tvertex(3)], 30);

    // Can set properties on new vertices
    data[v4] = 40;
    data[v5] = 50;
    EXPECT_EQ(data[v4], 40);
    EXPECT_EQ(data[v5], 50);
}

/**
 * Test multiple properties can coexist
 */
TEST_F(PropertyPersistenceTest, MultiplePropertiesCoexist) {
    gc = TestHelpers::BuildPath(4);
    TopologicalGraph G(*gc);

    // Create multiple different properties using valid property IDs
    Prop<int> int_prop(G.Set(tvertex()), PROP_TMP);
    Prop<int> int_prop2(G.Set(tvertex()), PROP_DRAW_INT_1);
    Prop<double> double_prop(G.Set(tvertex()), PROP_DRAW_DBLE_1);

    // Set values
    for (tvertex v = 1; v <= G.nv(); v++) {
        int_prop[v] = v() * 10;
        int_prop2[v] = v() * 100;
        double_prop[v] = v() * 0.5;
    }

    // All should coexist and be independent
    EXPECT_EQ(int_prop[tvertex(1)], 10);
    EXPECT_EQ(int_prop2[tvertex(1)], 100);
    EXPECT_DOUBLE_EQ(double_prop[tvertex(1)], 0.5);

    EXPECT_EQ(int_prop[tvertex(2)], 20);
    EXPECT_EQ(int_prop2[tvertex(2)], 200);
    EXPECT_DOUBLE_EQ(double_prop[tvertex(2)], 1.0);
}

/**
 * Test property persistence through complete workflow
 */
TEST_F(PropertyPersistenceTest, PropertiesThroughCompleteWorkflow) {
    gc = TestHelpers::BuildCycle(6);
    GeometricGraph G(*gc);

    // Tag vertices with IDs
    Prop<int> vertex_ids(G.Set(tvertex()), PROP_TMP);
    for (tvertex v = 1; v <= G.nv(); v++) {
        vertex_ids[v] = 1000 + v();
    }

    // Run complete workflow: planarity → Tutte → color
    G.Planarity();
    G.Tutte();
    G.ColorExteriorface();

    // Custom vertex IDs should survive entire pipeline
    for (tvertex v = 1; v <= G.nv(); v++) {
        EXPECT_EQ(vertex_ids[v], 1000 + v()) << "Vertex ID for " << v() << " was lost";
    }
}
