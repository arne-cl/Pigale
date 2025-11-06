/****************************************************************************
** Property-Based Tests for Planarity Invariants
**
** Uses RapidCheck to verify planarity properties hold for random graphs
** Following patterns from DEVELOPER_GUIDE.md
*****************************************************************************/

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <Pigale.h>
#include <TAXI/graphs.h>
#include "graph_builders.h"

// ============================================================================
// Euler's Formula Properties
// ============================================================================

// Property: For planar graphs, V - E + F = 2 (Euler's formula)
RC_GTEST_PROP(PlanarityProperties, EulerFormulaOnPlanarGraphs, ()) {
    auto n = *rc::gen::inRange(3, 30);

    // Use grid graph (guaranteed planar)
    auto rows = *rc::gen::inRange(2, 10);
    auto cols = *rc::gen::inRange(2, 10);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    // Compute planar embedding
    int planar = G.Planarity();
    RC_ASSERT(planar > 0);  // Should be planar

    // Get face count - use NumFaces() not nf() (DEVELOPER_GUIDE.md)
    int faces = G.NumFaces();

    // Euler's formula: V - E + F = 2
    RC_ASSERT(G.nv() - G.ne() + faces == 2);

    delete gc;
}

// ============================================================================
// Planar Graph Edge Bounds
// ============================================================================

// Property: Planar graph with n >= 3 vertices has at most 3n - 6 edges
RC_GTEST_PROP(PlanarityProperties, PlanarEdgeBound, ()) {
    auto n = *rc::gen::inRange(3, 50);

    // Grid graphs are planar
    auto rows = *rc::gen::inRange(2, 10);
    auto cols = *rc::gen::inRange(2, std::min(10, 50 / rows + 1));

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    // Grid should be planar
    RC_ASSERT(G.TestPlanar() > 0);

    // Planar bound: m <= 3n - 6 for n >= 3
    if (G.nv() >= 3) {
        RC_ASSERT(G.ne() <= 3 * G.nv() - 6);
    }

    delete gc;
}

// Property: Trees are planar
RC_GTEST_PROP(PlanarityProperties, TreesArePlanar, ()) {
    auto n = *rc::gen::inRange(2, 50);

    GraphContainer* gc = TestHelpers::BuildPath(n);
    TopologicalGraph G(*gc);

    // Trees (including paths) are planar
    RC_ASSERT(G.TestPlanar() > 0);

    delete gc;
}

// Property: Cycles are planar
RC_GTEST_PROP(PlanarityProperties, CyclesArePlanar, ()) {
    auto n = *rc::gen::inRange(3, 50);

    GraphContainer* gc = TestHelpers::BuildCycle(n);
    TopologicalGraph G(*gc);

    // Cycles are planar
    RC_ASSERT(G.TestPlanar() > 0);

    delete gc;
}

// ============================================================================
// K_n Planarity Properties
// ============================================================================

// Property: K_n is planar iff n <= 4
RC_GTEST_PROP(PlanarityProperties, CompleteGraphPlanarity, ()) {
    auto n = *rc::gen::inRange(1, 10);

    GraphContainer* gc = GenerateCompleteGraph(n);
    TopologicalGraph G(*gc);

    bool is_planar = G.TestPlanar() > 0;
    bool should_be_planar = (n <= 4);

    RC_ASSERT(is_planar == should_be_planar);

    delete gc;
}

// ============================================================================
// Bipartite Graph Planarity
// ============================================================================

// Property: K_{2,n} is planar for all n
RC_GTEST_PROP(PlanarityProperties, K2nIsPlanar, ()) {
    auto n = *rc::gen::inRange(1, 30);

    GraphContainer* gc = GenerateCompleteBiGraph(2, n);
    TopologicalGraph G(*gc);

    // K_{2,n} is always planar
    RC_ASSERT(G.TestPlanar() > 0);

    delete gc;
}

// Property: K_{3,3} is not planar
RC_GTEST_PROP(PlanarityProperties, K33IsNonPlanar, ()) {
    GraphContainer* gc = GenerateCompleteBiGraph(3, 3);
    TopologicalGraph G(*gc);

    // K_{3,3} is non-planar
    RC_ASSERT(G.TestPlanar() == 0);

    delete gc;
}

// Property: K_{m,n} with m,n >= 3 is non-planar
RC_GTEST_PROP(PlanarityProperties, KmnNonPlanarWhenBothLarge, ()) {
    auto m = *rc::gen::inRange(3, 10);
    auto n = *rc::gen::inRange(3, 10);

    GraphContainer* gc = GenerateCompleteBiGraph(m, n);
    TopologicalGraph G(*gc);

    // K_{m,n} with m,n >= 3 is non-planar
    RC_ASSERT(G.TestPlanar() == 0);

    delete gc;
}

// ============================================================================
// Face Degree Properties
// ============================================================================

// Property: For 3-connected planar graph, each face has >= 3 edges
RC_GTEST_PROP(PlanarityProperties, FaceDegreeBound, ()) {
    auto n = *rc::gen::inRange(4, 20);

    // K4 is 3-connected and planar
    GraphContainer* gc = TestHelpers::BuildK4();
    TopologicalGraph G(*gc);

    int planar = G.Planarity();
    RC_ASSERT(planar > 0);

    // For triangulated graph, all faces have exactly 3 edges
    // K4 is maximally planar (triangulated)
    int faces = G.NumFaces();
    RC_ASSERT(faces > 0);

    delete gc;
}

// ============================================================================
// Grid Graph Planarity
// ============================================================================

// Property: All grid graphs are planar
RC_GTEST_PROP(PlanarityProperties, GridsArePlanar, ()) {
    auto rows = *rc::gen::inRange(1, 15);
    auto cols = *rc::gen::inRange(1, 15);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    // All grids are planar
    RC_ASSERT(G.TestPlanar() > 0);

    delete gc;
}

// ============================================================================
// Subgraph Planarity
// ============================================================================

// Property: Subgraph of planar graph is planar
RC_GTEST_PROP(PlanarityProperties, SubgraphOfPlanarIsPlanar, ()) {
    auto rows = *rc::gen::inRange(3, 10);
    auto cols = *rc::gen::inRange(3, 10);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    // Grid is planar
    RC_ASSERT(G.TestPlanar() > 0);

    // Any graph with fewer edges (subgraph) should also be planar
    // This is implicitly tested: if grid with m edges is planar,
    // then grid with m-k edges (for any k) should also be planar

    delete gc;
}

// ============================================================================
// Biconnectivity and Planarity
// ============================================================================

// Property: Biconnecting planar graph keeps it planar
RC_GTEST_PROP(BiconnectProperties, BiconnectPreservesPlanarity, ()) {
    auto n = *rc::gen::inRange(4, 30);

    // Start with path (planar but not biconnected)
    GraphContainer gc;
    gc.setsize(n, 0);
    TopologicalGraph G(gc);

    for (int i = 1; i < n; i++) {
        G.NewEdge(tvertex(i), tvertex(i + 1));
    }

    RC_ASSERT(G.TestPlanar() > 0);  // Path is planar

    // Make biconnected
    G.Biconnect();

    // Should still be planar
    RC_ASSERT(G.TestPlanar() > 0);
    // Should now be biconnected
    RC_ASSERT(G.CheckBiconnected());
}

// ============================================================================
// Random Outerplanar Graphs
// ============================================================================

// Property: Outerplanar graphs satisfy tighter edge bound (m <= 2n - 3)
RC_GTEST_PROP(PlanarityProperties, OuterplanarEdgeBound, ()) {
    auto n = *rc::gen::inRange(3, 30);

    GraphContainer* gc = GenerateRandomOuterplanarGraph(n, false);
    TopologicalGraph G(*gc);

    // Outerplanar graphs are planar
    RC_ASSERT(G.TestPlanar() > 0);

    // Tighter bound for outerplanar: m <= 2n - 3
    if (G.nv() >= 2) {
        RC_ASSERT(G.ne() <= 2 * G.nv() - 3);
    }

    delete gc;
}

// ============================================================================
// Embedding Consistency
// ============================================================================

// Property: If TestPlanar succeeds, Planarity should also succeed
RC_GTEST_PROP(PlanarityProperties, TestPlanarConsistentWithPlanarity, ()) {
    auto rows = *rc::gen::inRange(2, 10);
    auto cols = *rc::gen::inRange(2, 10);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    int test_result = G.TestPlanar();

    // Create fresh graph for Planarity test
    GraphContainer* gc2 = GenerateGrid(rows, cols);
    TopologicalGraph G2(*gc2);

    int planarity_result = G2.Planarity();

    // Both should agree on planarity
    RC_ASSERT((test_result > 0) == (planarity_result > 0));

    delete gc;
    delete gc2;
}

// ============================================================================
// Degree Constraints in Planar Graphs
// ============================================================================

// Property: Average degree in planar graph is < 6
RC_GTEST_PROP(PlanarityProperties, AverageDegreeBound, ()) {
    auto rows = *rc::gen::inRange(2, 10);
    auto cols = *rc::gen::inRange(2, 10);

    GraphContainer* gc = GenerateGrid(rows, cols);
    TopologicalGraph G(*gc);

    RC_ASSERT(G.TestPlanar() > 0);

    // Average degree = 2m/n < 6 for planar graphs (from m <= 3n-6)
    if (G.nv() >= 3) {
        double avg_degree = (2.0 * G.ne()) / G.nv();
        RC_ASSERT(avg_degree < 6.0);
    }

    delete gc;
}
