/****************************************************************************
**
** Unit tests for GeometricGraph class
**
** Tests cover GeometricGraph-specific functionality:
** - Coordinate properties (vcoord)
** - Visual properties (vcolor, vlabel, ecolor, ewidth, elabel)
** - Geometric operations (FindVertex, FindEdge by coordinates)
** - NewVertex with coordinates
** - BissectEdge with coordinate interpolation
** - Label management (maxvlabel, maxelabel)
**
** Following lessons from DEVELOPER_GUIDE.md:
** - Always use setsize(n, 0) with NewEdge()
** - Test property initialization
** - Verify geometric properties after modifications
**
****************************************************************************/

#include <gtest/gtest.h>
#include <TAXI/graphs.h>
#include "../helpers/graph_builders.h"
#include <cmath>

class GeometricGraphTest : public ::testing::Test {
protected:
    GraphContainer* gc;

    void SetUp() override {
        gc = new GraphContainer();
    }

    void TearDown() override {
        delete gc;
    }

    // Helper to check if two points are approximately equal
    bool PointsEqual(const Tpoint& p1, const Tpoint& p2, double epsilon = 0.0001) {
        return std::abs(p1.x() - p2.x()) < epsilon &&
               std::abs(p1.y() - p2.y()) < epsilon;
    }
};

/**
 * Test coordinate properties initialization
 */
TEST_F(GeometricGraphTest, ConstructorInitializesCoordinates) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    // All vertices should have default coordinates (0, 0)
    for (tvertex v = 1; v <= 3; v++) {
        EXPECT_EQ(G.vcoord[v].x(), 0.0);
        EXPECT_EQ(G.vcoord[v].y(), 0.0);
    }
}

TEST_F(GeometricGraphTest, ConstructorInitializesVisualProperties) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    // Check default values
    for (tvertex v = 1; v <= 3; v++) {
        EXPECT_EQ(G.vcolor[v], 5);   // Default color
        EXPECT_EQ(G.vlabel[v], 0);   // Default label
    }
}

TEST_F(GeometricGraphTest, EdgesHaveDefaultProperties) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(G.ewidth[e], 1);   // Default width
    EXPECT_EQ(G.ecolor[e], 1);   // Default color
    EXPECT_GT(G.elabel[e], 0);   // Auto-assigned label
}

/**
 * Test coordinate setting and getting
 */
TEST_F(GeometricGraphTest, SetAndGetCoordinates) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(10.0, 20.0);
    G.vcoord[2] = Tpoint(30.0, 40.0);
    G.vcoord[3] = Tpoint(-5.0, -10.0);

    EXPECT_TRUE(PointsEqual(G.vcoord[1], Tpoint(10.0, 20.0)));
    EXPECT_TRUE(PointsEqual(G.vcoord[2], Tpoint(30.0, 40.0)));
    EXPECT_TRUE(PointsEqual(G.vcoord[3], Tpoint(-5.0, -10.0)));
}

TEST_F(GeometricGraphTest, CoordinatesIndependentPerVertex) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(1.0, 2.0);
    G.vcoord[2] = Tpoint(3.0, 4.0);

    // Modifying one shouldn't affect the other
    G.vcoord[1].x() = 10.0;

    EXPECT_NEAR(G.vcoord[1].x(), 10.0, 0.0001);
    EXPECT_NEAR(G.vcoord[2].x(), 3.0, 0.0001);
}

/**
 * Test NewVertex with coordinates
 */
TEST_F(GeometricGraphTest, NewVertexWithCoordinates) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    tvertex v = G.NewVertex(Tpoint(100.0, 200.0));

    EXPECT_EQ(G.nv(), 3);
    EXPECT_EQ(v(), 3);
    EXPECT_TRUE(PointsEqual(G.vcoord[v], Tpoint(100.0, 200.0)));
}

TEST_F(GeometricGraphTest, NewVertexAssignsLabel) {
    gc->setsize(1, 0);
    GeometricGraph G(*gc);

    int initial_max_label = G.maxvlabel;

    tvertex v = G.NewVertex(Tpoint(0.0, 0.0));

    EXPECT_EQ(G.vlabel[v], initial_max_label + 1);
    EXPECT_EQ(G.maxvlabel, initial_max_label + 1);
}

TEST_F(GeometricGraphTest, MultipleNewVerticesIncrementLabels) {
    gc->setsize(1, 0);
    GeometricGraph G(*gc);

    tvertex v1 = G.NewVertex(Tpoint(0.0, 0.0));
    tvertex v2 = G.NewVertex(Tpoint(1.0, 1.0));
    tvertex v3 = G.NewVertex(Tpoint(2.0, 2.0));

    EXPECT_EQ(G.vlabel[v2], G.vlabel[v1] + 1);
    EXPECT_EQ(G.vlabel[v3], G.vlabel[v2] + 1);
}

/**
 * Test NewEdge assigns labels
 */
TEST_F(GeometricGraphTest, NewEdgeAssignsLabel) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    int initial_max_label = G.maxelabel;

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    EXPECT_EQ(G.elabel[e], initial_max_label + 1);
    EXPECT_EQ(G.maxelabel, initial_max_label + 1);
}

TEST_F(GeometricGraphTest, MultipleNewEdgesIncrementLabels) {
    gc->setsize(4, 0);
    GeometricGraph G(*gc);

    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    tedge e2 = G.NewEdge(tvertex(2), tvertex(3));
    tedge e3 = G.NewEdge(tvertex(3), tvertex(4));

    EXPECT_EQ(G.elabel[e2], G.elabel[e1] + 1);
    EXPECT_EQ(G.elabel[e3], G.elabel[e2] + 1);
}

/**
 * Test BissectEdge with coordinate interpolation
 */
TEST_F(GeometricGraphTest, BissectEdgeInterpolatesCoordinates) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 20.0);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    tvertex v = G.BissectEdge(e);

    // New vertex should be at midpoint
    EXPECT_NEAR(G.vcoord[v].x(), 5.0, 0.0001);
    EXPECT_NEAR(G.vcoord[v].y(), 10.0, 0.0001);
}

TEST_F(GeometricGraphTest, BissectEdgeAssignsLabels) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 10.0);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));
    int v_label_before = G.maxvlabel;
    int e_label_before = G.maxelabel;

    tvertex v = G.BissectEdge(e);

    // New vertex gets incremented label
    EXPECT_EQ(G.vlabel[v], v_label_before + 1);

    // New edge gets incremented label
    EXPECT_EQ(G.maxelabel, e_label_before + 1);
}

TEST_F(GeometricGraphTest, BissectEdgePreservesEdgeProperties) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 10.0);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));
    G.ecolor[e] = 7;
    G.ewidth[e] = 3;

    tvertex v = G.BissectEdge(e);

    // Find the new edge (should be the last one)
    tedge new_edge = G.ne();

    // New edge should inherit properties from original edge
    EXPECT_EQ(G.ecolor[new_edge], 7);
    EXPECT_EQ(G.ewidth[new_edge], 3);
}

/**
 * Test color properties
 */
TEST_F(GeometricGraphTest, SetAndGetVertexColors) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vcolor[1] = 1;  // Red
    G.vcolor[2] = 2;  // Blue
    G.vcolor[3] = 3;  // Green

    EXPECT_EQ(G.vcolor[1], 1);
    EXPECT_EQ(G.vcolor[2], 2);
    EXPECT_EQ(G.vcolor[3], 3);
}

TEST_F(GeometricGraphTest, SetAndGetEdgeColors) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    G.ecolor[e] = 5;
    G.ewidth[e] = 2;

    EXPECT_EQ(G.ecolor[e], 5);
    EXPECT_EQ(G.ewidth[e], 2);
}

/**
 * Test label properties
 */
TEST_F(GeometricGraphTest, SetAndGetVertexLabels) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vlabel[1] = 100;
    G.vlabel[2] = 200;
    G.vlabel[3] = 300;

    EXPECT_EQ(G.vlabel[1], 100);
    EXPECT_EQ(G.vlabel[2], 200);
    EXPECT_EQ(G.vlabel[3], 300);
}

TEST_F(GeometricGraphTest, SetAndGetEdgeLabels) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    G.elabel[e] = 999;

    EXPECT_EQ(G.elabel[e], 999);
}

/**
 * Test FindVertex by coordinates
 */
TEST_F(GeometricGraphTest, FindVertexReturnsNearestVertex) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 0.0);
    G.vcoord[3] = Tpoint(0.0, 10.0);

    // Search near vertex 1
    tvertex v = G.FindVertex(Tpoint(0.5, 0.5), 1.0);

    EXPECT_EQ(v(), 1);
}

TEST_F(GeometricGraphTest, FindVertexReturnsZeroIfTooFar) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 10.0);

    // Search far from any vertex
    tvertex v = G.FindVertex(Tpoint(100.0, 100.0), 1.0);

    EXPECT_EQ(v(), 0);  // No vertex found
}

/**
 * Test integration with TopologicalGraph operations
 */
TEST_F(GeometricGraphTest, TopologicalOperationsPreserveCoordinates) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 0.0);
    G.vcoord[3] = Tpoint(5.0, 10.0);

    G.NewEdge(tvertex(1), tvertex(2));
    G.NewEdge(tvertex(2), tvertex(3));

    // Coordinates should remain intact after edge operations
    EXPECT_TRUE(PointsEqual(G.vcoord[1], Tpoint(0.0, 0.0)));
    EXPECT_TRUE(PointsEqual(G.vcoord[2], Tpoint(10.0, 0.0)));
    EXPECT_TRUE(PointsEqual(G.vcoord[3], Tpoint(5.0, 10.0)));
}

TEST_F(GeometricGraphTest, NewVertexIntegrationTest) {
    gc->setsize(2, 0);
    GeometricGraph G(*gc);

    // Create initial vertices with coordinates
    G.vcoord[1] = Tpoint(0.0, 0.0);
    G.vcoord[2] = Tpoint(10.0, 10.0);

    // Add edge
    G.NewEdge(tvertex(1), tvertex(2));

    // Add new vertex with coordinates
    tvertex v3 = G.NewVertex(Tpoint(5.0, 5.0));

    // Connect new vertex
    G.NewEdge(v3, tvertex(1));
    G.NewEdge(v3, tvertex(2));

    // Verify structure
    EXPECT_EQ(G.nv(), 3);
    EXPECT_EQ(G.ne(), 3);

    // Verify coordinates
    EXPECT_TRUE(PointsEqual(G.vcoord[v3], Tpoint(5.0, 5.0)));
}

/**
 * Test boundary conditions
 */
TEST_F(GeometricGraphTest, EmptyGeometricGraph) {
    gc->setsize(0, 0);
    GeometricGraph G(*gc);

    EXPECT_EQ(G.nv(), 0);
    EXPECT_EQ(G.ne(), 0);
}

TEST_F(GeometricGraphTest, SingleVertexWithCoordinates) {
    gc->setsize(1, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(42.0, 84.0);

    EXPECT_TRUE(PointsEqual(G.vcoord[1], Tpoint(42.0, 84.0)));
}

TEST_F(GeometricGraphTest, NegativeCoordinatesSupported) {
    gc->setsize(1, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(-10.5, -20.5);

    EXPECT_NEAR(G.vcoord[1].x(), -10.5, 0.0001);
    EXPECT_NEAR(G.vcoord[1].y(), -20.5, 0.0001);
}

TEST_F(GeometricGraphTest, LargeCoordinateValues) {
    gc->setsize(1, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(1000000.0, 2000000.0);

    EXPECT_NEAR(G.vcoord[1].x(), 1000000.0, 0.0001);
    EXPECT_NEAR(G.vcoord[1].y(), 2000000.0, 0.0001);
}

/**
 * Test property persistence across graph operations
 */
TEST_F(GeometricGraphTest, PropertiesPersistAfterEdgeOperations) {
    gc->setsize(3, 0);
    GeometricGraph G(*gc);

    G.vcoord[1] = Tpoint(1.0, 1.0);
    G.vcoord[2] = Tpoint(2.0, 2.0);
    G.vcoord[3] = Tpoint(3.0, 3.0);

    G.vcolor[1] = 10;
    G.vcolor[2] = 20;

    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    tedge e2 = G.NewEdge(tvertex(2), tvertex(3));

    // Vertex properties should persist
    EXPECT_TRUE(PointsEqual(G.vcoord[1], Tpoint(1.0, 1.0)));
    EXPECT_EQ(G.vcolor[1], 10);
    EXPECT_EQ(G.vcolor[2], 20);

    // Edge properties should be set
    EXPECT_GT(G.elabel[e1], 0);
    EXPECT_GT(G.elabel[e2], 0);
}
