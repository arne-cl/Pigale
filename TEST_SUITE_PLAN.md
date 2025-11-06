# Comprehensive Test Suite Plan for Pigale tgraph Library

## Executive Summary

This document outlines a comprehensive testing strategy for the Pigale tgraph library, a C++ graph theory toolkit specializing in planar graphs. The library contains ~22,000 lines of code across 54+ implementation files with no existing formal test framework.

**Prepared**: 2025-11-05
**Target Directory**: `/tgraph/`
**Current Status**: No formal test framework exists

---

## 1. Testing Strategy Overview

### 1.1 Test Philosophy

The testing approach will follow a **layered pyramid strategy**:

```
              ┌─────────────┐
              │   BDD/E2E   │  (10% - High-level behaviors)
              ├─────────────┤
              │ Integration │  (30% - Algorithm combinations)
              ├─────────────┤
              │ Unit Tests  │  (60% - Core components)
              └─────────────┘
```

### 1.2 Test Framework Selection

**Recommended**: Google Test (gtest/gmock)

**Rationale**:
- Industry standard for C++ testing
- Excellent assertion macros and test organization
- Built-in mocking support (gmock)
- Good integration with CI/CD pipelines
- Compatible with existing Qt/qmake build system

**Alternatives Considered**:
- Catch2: Modern, header-only (good alternative)
- Qt Test: Already available but less feature-rich
- Boost.Test: Heavyweight dependency

### 1.3 Directory Structure

```
Pigale/
├── tgraph/                    # Existing library code
│   ├── Graph.cpp
│   ├── ...
│   └── tgraph.pro
├── tests/                     # NEW: Test suite root
│   ├── unit/                  # Unit tests (mirrors tgraph structure)
│   │   ├── core/
│   │   │   ├── test_graph_container.cpp
│   │   │   ├── test_graph_access.cpp
│   │   │   ├── test_graph.cpp
│   │   │   ├── test_topological_graph.cpp
│   │   │   └── test_geometric_graph.cpp
│   │   ├── data_structures/
│   │   │   ├── test_pset.cpp
│   │   │   ├── test_properties.cpp
│   │   │   ├── test_brin_representation.cpp
│   │   │   └── test_circular_order.cpp
│   │   ├── algorithms/
│   │   │   ├── traversal/
│   │   │   │   ├── test_dfs.cpp
│   │   │   │   ├── test_bfs.cpp
│   │   │   │   └── test_biconnect.cpp
│   │   │   ├── planarity/
│   │   │   │   ├── test_planar.cpp
│   │   │   │   ├── test_lr_algo.cpp
│   │   │   │   ├── test_max_planar.cpp
│   │   │   │   └── test_single_pass_planar.cpp
│   │   │   ├── embedding/
│   │   │   │   ├── test_schnyder.cpp
│   │   │   │   ├── test_schnyder_wood.cpp
│   │   │   │   ├── test_bipolar.cpp
│   │   │   │   └── test_st_list.cpp
│   │   │   └── drawing/
│   │   │       ├── test_tutte.cpp
│   │   │       ├── test_fpp.cpp
│   │   │       ├── test_embed_vision.cpp
│   │   │       └── test_barycenter.cpp
│   │   ├── generation/
│   │   │   ├── test_generate.cpp
│   │   │   └── test_schaeffer.cpp
│   │   └── io/
│   │       ├── test_tgf.cpp
│   │       └── test_file.cpp
│   ├── integration/           # Integration tests
│   │   ├── test_planar_pipeline.cpp
│   │   ├── test_drawing_pipeline.cpp
│   │   ├── test_graph_modifications.cpp
│   │   └── test_property_persistence.cpp
│   ├── bdd/                   # Behavior-driven tests
│   │   ├── features/
│   │   │   ├── planarity_detection.feature
│   │   │   ├── graph_embedding.feature
│   │   │   └── graph_drawing.feature
│   │   └── step_definitions/
│   │       └── graph_steps.cpp
│   ├── fixtures/              # Test data
│   │   ├── graphs/
│   │   │   ├── planar/
│   │   │   │   ├── k4.tgf
│   │   │   │   ├── petersen.tgf
│   │   │   │   └── grid_5x5.tgf
│   │   │   ├── nonplanar/
│   │   │   │   ├── k5.tgf
│   │   │   │   └── k33.tgf
│   │   │   └── special/
│   │   │       ├── empty.tgf
│   │   │       ├── single_vertex.tgf
│   │   │       └── disconnected.tgf
│   │   └── expected_outputs/
│   ├── helpers/               # Test utilities
│   │   ├── graph_builders.h
│   │   ├── graph_comparators.h
│   │   ├── property_validators.h
│   │   └── test_fixtures.h
│   ├── performance/           # Performance benchmarks
│   │   ├── bench_planarity.cpp
│   │   ├── bench_dfs_bfs.cpp
│   │   └── bench_embedding.cpp
│   ├── CMakeLists.txt         # CMake build for tests
│   └── tests.pro              # Qt project file for tests
└── .github/
    └── workflows/
        └── test.yml           # CI integration

```

---

## 2. Test Categories

### 2.1 Unit Tests (Priority: HIGH)

**Goal**: Test individual components in isolation

#### 2.1.1 Core Data Structures

**File**: `tests/unit/core/test_graph_container.cpp`

Test Coverage:
- ✓ Constructor/destructor behavior
- ✓ `setsize()`, `incsize()`, `decsize()` operations
- ✓ Property set initialization (General, V, E, B)
- ✓ `clear()` and `reset()` functionality
- ✓ Vertex/edge count tracking (`nv()`, `ne()`)
- ✓ Memory management and resource cleanup
- ✓ Copy constructor and assignment operator

Example Tests:
```cpp
TEST(GraphContainerTest, DefaultConstruction) {
    GraphContainer gc;
    EXPECT_EQ(gc.nv(), 0);
    EXPECT_EQ(gc.ne(), 0);
}

TEST(GraphContainerTest, SetSizeAllocatesProperties) {
    GraphContainer gc;
    gc.setsize(10, 20);
    EXPECT_EQ(gc.nv(), 10);
    EXPECT_EQ(gc.ne(), 20);
    // Verify property sets are allocated
}

TEST(GraphContainerTest, IncrementalSizeChanges) {
    GraphContainer gc;
    gc.setsize(5, 5);
    gc.incsize(3, 2);
    EXPECT_EQ(gc.nv(), 8);
    EXPECT_EQ(gc.ne(), 7);
}
```

---

**File**: `tests/unit/data_structures/test_pset.cpp`

Test Coverage:
- ✓ Property allocation and deallocation
- ✓ Type safety (int, bool, double, Tpoint, tstring)
- ✓ Sparse array behavior
- ✓ `Prop<T>` vs `Prop1<T>` semantics
- ✓ Property persistence across graph modifications
- ✓ Keep/ClearKeep mechanisms
- ✓ Out-of-bounds access handling

Example Tests:
```cpp
TEST(PSetTest, IntPropertyGetSet) {
    PSet pset;
    Prop<int> prop(pset, PROP_COLOR);
    prop[1] = 42;
    EXPECT_EQ(prop[1], 42);
}

TEST(PSetTest, PropertySparse) {
    PSet pset;
    Prop<int> prop(pset, PROP_LABEL);
    prop[1000] = 99;  // Should not allocate 1000 entries
    EXPECT_EQ(prop[1000], 99);
}
```

---

**File**: `tests/unit/data_structures/test_circular_order.cpp`

Test Coverage:
- ✓ Circular order invariant: `cir[acir[b]] == b`
- ✓ `pbrin[v]` initialization
- ✓ Brin traversal around vertices
- ✓ Edge/brin correspondence: `vin[e]` vs `vin[-e]`
- ✓ Circular order updates during edge insertion/deletion
- ✓ `MoveBrin()` correctness

**CRITICAL INVARIANTS TO TEST**:
```cpp
TEST(CircularOrderTest, CircularOrderInvariant) {
    TopologicalGraph G;
    // Build graph...
    Prop<tbrin> &cir = G.ComputeCir();
    Prop<tbrin> &acir = G.ComputeAcir();

    ForAllBrins(b, G) {
        EXPECT_EQ(cir[acir[b]], b);
        EXPECT_EQ(acir[cir[b]], b);
    }
}

TEST(CircularOrderTest, BrinCycleCompleteness) {
    TopologicalGraph G;
    // Build graph...
    ForAllVertices(v, G) {
        tbrin first = G.pbrin[v];
        std::set<tbrin> visited;
        tbrin b = first;
        do {
            ASSERT_FALSE(visited.count(b)) << "Cycle detected";
            visited.insert(b);
            b = G.cir[b];
        } while (b != first);

        // All brins incident to v should be visited
        EXPECT_EQ(visited.size(), G.Degree(v));
    }
}
```

---

#### 2.1.2 Graph Algorithms

**File**: `tests/unit/algorithms/traversal/test_dfs.cpp`

Test Coverage:
- ✓ Tree edge classification
- ✓ Back edge detection
- ✓ Component identification
- ✓ DFS numbering correctness
- ✓ Parent relationships
- ✓ Starting vertex variations
- ✓ Disconnected graph handling

Example Tests:
```cpp
TEST(DFSTest, TreeEdgesFormForest) {
    Graph G;
    // Build connected graph...
    svector<tvertex> parent;
    G.DFS(parent, 1);

    // Verify tree properties
    int treeEdges = 0;
    ForAllVertices(v, G) {
        if (parent[v] != 0) treeEdges++;
    }
    EXPECT_EQ(treeEdges, G.nv() - 1);  // Connected graph
}

TEST(DFSTest, BackEdgesDetectCycles) {
    Graph G;
    // Build graph with cycle...
    bool hasCycle = false;
    // ... DFS implementation checks for back edges ...
    EXPECT_TRUE(hasCycle);
}
```

---

**File**: `tests/unit/algorithms/planarity/test_planar.cpp`

Test Coverage:
- ✓ Correctly identifies planar graphs (K4, grid graphs, trees)
- ✓ Correctly identifies non-planar graphs (K5, K3,3)
- ✓ Kuratowski subgraph extraction (for non-planar)
- ✓ Embedding computation on planar graphs
- ✓ Circular order validity after embedding
- ✓ Handles disconnected graphs
- ✓ Edge cases: empty, single vertex, single edge

**Test Graph Suite**:
```cpp
class PlanarityTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Pre-build test graphs
        k4 = BuildK4();          // Planar
        k5 = BuildK5();          // Non-planar
        k33 = BuildK33();        // Non-planar
        grid5x5 = BuildGrid(5,5);// Planar
        petersen = BuildPetersenGraph();  // Non-planar
    }

    GraphContainer *k4, *k5, *k33, *grid5x5, *petersen;
};

TEST_F(PlanarityTestFixture, K4IsPlanar) {
    Graph G(k4);
    EXPECT_TRUE(G.TestPlanar());
}

TEST_F(PlanarityTestFixture, K5IsNonPlanar) {
    Graph G(k5);
    EXPECT_FALSE(G.TestPlanar());
}

TEST_F(PlanarityTestFixture, EmbeddingComputedForPlanar) {
    TopologicalGraph G(k4);
    tbrin result = G.Planarity();
    EXPECT_NE(result, 0);  // Success
    EXPECT_TRUE(G.CheckPlanarMap());
}
```

---

**File**: `tests/unit/algorithms/embedding/test_schnyder.cpp`

Test Coverage:
- ✓ Schnyder wood construction on triangulated planar graphs
- ✓ Valid 3-orientation (exactly 3 outgoing edges per vertex)
- ✓ Region property (3 disjoint spanning trees)
- ✓ Barycentric coordinates computation
- ✓ Coordinate validity (non-overlapping for maximal planar)

---

**File**: `tests/unit/algorithms/drawing/test_tutte.cpp`

Test Coverage:
- ✓ Barycentric embedding convergence
- ✓ Coordinates within unit circle
- ✓ No edge crossings for planar graphs
- ✓ Fixed exterior face vertices
- ✓ Interior vertex placement

---

#### 2.1.3 Graph Generation

**File**: `tests/unit/generation/test_generate.cpp`

Test Coverage:
- ✓ Grid graphs have correct dimensions
- ✓ Complete graphs have n(n-1)/2 edges
- ✓ Bipartite graphs respect partition
- ✓ Random graphs respect edge count constraints
- ✓ Generated graphs are valid (no self-loops unless allowed)

---

#### 2.1.4 I/O Operations

**File**: `tests/unit/io/test_tgf.cpp`

Test Coverage:
- ✓ Round-trip: write → read → compare
- ✓ Property serialization (coordinates, colors, labels)
- ✓ Circular order preservation
- ✓ Malformed file handling
- ✓ Large file handling

---

### 2.2 Integration Tests (Priority: MEDIUM)

**Goal**: Test algorithm combinations and workflows

#### 2.2.1 Planarity Pipeline

**File**: `tests/integration/test_planar_pipeline.cpp`

Test Scenarios:
1. **Generate → Test → Embed**
   ```cpp
   TEST(PlanarPipeline, GridGraphWorkflow) {
       auto* gc = GenerateGrid(10, 10);
       TopologicalGraph G(gc);
       ASSERT_TRUE(G.TestPlanar());
       tbrin result = G.Planarity();
       EXPECT_NE(result, 0);
       EXPECT_TRUE(G.CheckPlanarMap());
   }
   ```

2. **Read → Planarity → Schnyder → Tutte Drawing**
   ```cpp
   TEST(PlanarPipeline, FileToDrawing) {
       TopologicalGraph G;
       LoadTgf(G, "fixtures/graphs/planar/k4.tgf");

       ASSERT_TRUE(G.TestPlanar());
       G.Planarity();
       G.Triangulate();

       GeometricGraph GG(&G);
       GG.SchnyderOrient(GG.extbrin());
       GG.Tutte();

       EXPECT_TRUE(ValidateCoordinates(GG));
   }
   ```

3. **Modify → Retest**
   - Add edges → test planarity again
   - Delete edges → verify still planar
   - Contract edges → check biconnectivity

---

#### 2.2.2 Graph Modification Pipeline

**File**: `tests/integration/test_graph_modifications.cpp`

Test Scenarios:
1. **Edge Operations Preserve Invariants**
   ```cpp
   TEST(GraphModification, NewEdgePreservesCircularOrder) {
       TopologicalGraph G;
       // Build initial graph...

       tbrin b1 = ..., b2 = ...;
       tedge newEdge = G.NewEdge(b1, b2);

       EXPECT_TRUE(ValidateCircularOrder(G));
       EXPECT_EQ(G.ne(), initialEdgeCount + 1);
   }
   ```

2. **Vertex Deletion Updates Properties**
3. **Edge Contraction Preserves Planarity**

---

#### 2.2.3 Property Persistence

**File**: `tests/integration/test_property_persistence.cpp`

Test Scenarios:
- Properties survive graph modifications
- Properties cleared appropriately
- Keep/ClearKeep behavior across operations

---

### 2.3 Behavior-Driven Tests (Priority: LOW)

**Goal**: High-level user scenarios in readable format

**Framework**: Cucumber-cpp or custom Gherkin parser

**File**: `tests/bdd/features/planarity_detection.feature`

```gherkin
Feature: Planarity Detection
  As a graph theorist
  I want to test if a graph is planar
  So that I can determine if it can be drawn without edge crossings

  Scenario: Testing a planar graph
    Given I have a graph K4
    When I test planarity
    Then the result should be "planar"
    And I can compute an embedding

  Scenario: Testing a non-planar graph
    Given I have a graph K5
    When I test planarity
    Then the result should be "non-planar"
    And I can extract a Kuratowski subgraph

  Scenario: Generating random planar graphs
    Given I generate a random planar graph with 100 vertices
    When I test planarity
    Then the result should be "planar"
```

**File**: `tests/bdd/step_definitions/graph_steps.cpp`

```cpp
#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

GIVEN("^I have a graph (K\\d+|K\\d+,\\d+|grid_\\d+x\\d+)$") {
    REGEX_PARAM(std::string, graphName);
    currentGraph = BuildGraphByName(graphName);
}

WHEN("^I test planarity$") {
    isPlanar = currentGraph->TestPlanar();
}

THEN("^the result should be \"(planar|non-planar)\"$") {
    REGEX_PARAM(std::string, expected);
    if (expected == "planar")
        EXPECT_TRUE(isPlanar);
    else
        EXPECT_FALSE(isPlanar);
}
```

---

### 2.4 Performance Tests (Priority: LOW)

**Goal**: Regression testing for algorithm performance

**Framework**: Google Benchmark

**File**: `tests/performance/bench_planarity.cpp`

```cpp
#include <benchmark/benchmark.h>

static void BM_PlanarityTestLR(benchmark::State& state) {
    int n = state.range(0);
    auto* gc = GenerateGrid(n, n);

    for (auto _ : state) {
        TopologicalGraph G(gc);
        benchmark::DoNotOptimize(G.TestPlanar());
    }
}
BENCHMARK(BM_PlanarityTestLR)->Range(10, 1000);

static void BM_SchnyderEmbedding(benchmark::State& state) {
    // ...
}
BENCHMARK(BM_SchnyderEmbedding)->Range(10, 1000);
```

---

## 3. Test Fixtures and Helpers

### 3.1 Graph Builders

**File**: `tests/helpers/graph_builders.h`

```cpp
#ifndef TEST_GRAPH_BUILDERS_H
#define TEST_GRAPH_BUILDERS_H

#include <TAXI/graphs.h>

namespace TestHelpers {

// Basic structures
GraphContainer* BuildK4();
GraphContainer* BuildK5();
GraphContainer* BuildK33();
GraphContainer* BuildPetersenGraph();
GraphContainer* BuildGrid(int rows, int cols);
GraphContainer* BuildPath(int n);
GraphContainer* BuildCycle(int n);
GraphContainer* BuildTree(int n);

// Planar graphs
GraphContainer* BuildOuterplanar(int n);
GraphContainer* BuildMaximalPlanar(int n);
GraphContainer* Build3Connected(int n);

// Special cases
GraphContainer* BuildEmptyGraph();
GraphContainer* BuildSingleVertex();
GraphContainer* BuildSingleEdge();
GraphContainer* BuildDisconnectedGraph(int components);

// Parameterized
GraphContainer* BuildFromAdjacencyMatrix(const std::vector<std::vector<int>>& adj);
GraphContainer* BuildFromEdgeList(int n, const std::vector<std::pair<int, int>>& edges);

} // namespace TestHelpers

#endif
```

---

### 3.2 Graph Validators

**File**: `tests/helpers/graph_comparators.h`

```cpp
namespace TestHelpers {

// Structural comparisons
bool IsIsomorphic(const Graph& G1, const Graph& G2);
bool HasSameEmbedding(const TopologicalGraph& G1, const TopologicalGraph& G2);

// Property validators
bool ValidateCircularOrder(const TopologicalGraph& G);
bool ValidatePlanarMap(const TopologicalGraph& G);
bool ValidateCoordinates(const GeometricGraph& G);
bool CheckNoEdgeCrossings(const GeometricGraph& G);

// Invariants
bool VerifyVertexEdgeCounts(const Graph& G);
bool VerifyBrinIncidence(const TopologicalGraph& G);
bool VerifyDegreeSequence(const Graph& G, const std::vector<int>& expectedDegrees);

// Assertions for gtest
#define ASSERT_VALID_GRAPH(G) \
    ASSERT_TRUE(TestHelpers::ValidateCircularOrder(G)) << "Circular order invalid"

#define ASSERT_PLANAR_MAP(G) \
    ASSERT_TRUE(TestHelpers::ValidatePlanarMap(G)) << "Planar map invalid"

} // namespace TestHelpers
```

---

### 3.3 Property Validators

**File**: `tests/helpers/property_validators.h`

```cpp
namespace TestHelpers {

class PropertyValidator {
public:
    // Check property consistency
    static bool ValidateProperty(const PSet& pset, int propId);

    // Check all properties
    static bool ValidateAllProperties(const GraphContainer& gc);

    // Property-specific validators
    static bool ValidateCirAcir(const TopologicalGraph& G);
    static bool ValidateVinPbrin(const TopologicalGraph& G);
    static bool ValidateCoordinates(const GeometricGraph& G);
};

} // namespace TestHelpers
```

---

## 4. Test Data Management

### 4.1 Graph Fixtures

**Directory**: `tests/fixtures/graphs/`

#### Standard Test Graphs (TGF format):

**Planar Graphs**:
- `k4.tgf` - Complete graph on 4 vertices
- `petersen_plane.tgf` - Planar embedding of Petersen graph (if exists)
- `grid_5x5.tgf` - 5x5 grid
- `grid_10x10.tgf` - 10x10 grid
- `wheel_8.tgf` - Wheel graph (8 spokes)
- `outerplanar_10.tgf` - Outerplanar graph
- `tree_binary_15.tgf` - Binary tree with 15 nodes
- `maximal_planar_20.tgf` - Maximal planar (triangulated)

**Non-Planar Graphs**:
- `k5.tgf` - Complete graph on 5 vertices
- `k33.tgf` - Complete bipartite K₃,₃
- `petersen.tgf` - Petersen graph

**Special Cases**:
- `empty.tgf` - Empty graph (n=0, m=0)
- `single_vertex.tgf` - Single vertex
- `single_edge.tgf` - Single edge (2 vertices)
- `disconnected_3.tgf` - 3 disconnected components
- `multigraph.tgf` - Graph with multiple edges
- `self_loop.tgf` - Graph with self-loops

**Large Graphs** (for performance testing):
- `grid_100x100.tgf`
- `random_planar_1000.tgf`
- `tree_1000.tgf`

---

### 4.2 Expected Outputs

**Directory**: `tests/fixtures/expected_outputs/`

For each test graph, store expected results:
- `k4_embedding.tgf` - Expected embedding of K4
- `k4_tutte_coords.txt` - Expected Tutte coordinates
- `k5_kuratowski.txt` - Kuratowski subgraph of K5

---

## 5. Build System Integration

### 5.1 CMake Configuration

**File**: `tests/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(PigaleTests)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/incl)

# Link tgraph library
add_library(tgraph STATIC IMPORTED)
set_target_properties(tgraph PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libtgraph.a
)

# Unit tests
file(GLOB_RECURSE UNIT_TEST_SOURCES unit/**/*.cpp)
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests gtest_main tgraph)

# Integration tests
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES integration/**/*.cpp)
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests gtest_main tgraph)

# Register tests
enable_testing()
add_test(NAME UnitTests COMMAND unit_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)

# Test helpers library
add_library(test_helpers STATIC
    helpers/graph_builders.cpp
    helpers/graph_comparators.cpp
    helpers/property_validators.cpp
)
target_link_libraries(test_helpers tgraph)
target_link_libraries(unit_tests test_helpers)
target_link_libraries(integration_tests test_helpers)
```

---

### 5.2 Qt Project File

**File**: `tests/tests.pro`

```qmake
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread

INCLUDEPATH += ../incl

LIBS += -L../lib -ltgraph

# Google Test
INCLUDEPATH += /usr/include/gtest
LIBS += -lgtest -lgtest_main -lpthread

SOURCES += \
    unit/core/test_graph_container.cpp \
    unit/core/test_graph.cpp \
    unit/algorithms/traversal/test_dfs.cpp \
    unit/algorithms/planarity/test_planar.cpp \
    helpers/graph_builders.cpp

HEADERS += \
    helpers/graph_builders.h \
    helpers/graph_comparators.h
```

---

## 6. CI/CD Integration

### 6.1 GitHub Actions

**File**: `.github/workflows/test.yml`

```yaml
name: Test Suite

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        compiler: [gcc, clang]

    steps:
    - uses: actions/checkout@v3

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y qt5-default libgtest-dev cmake

    - name: Build tgraph library
      run: |
        cd tgraph
        qmake tgraph.pro
        make

    - name: Build tests
      run: |
        cd tests
        mkdir build && cd build
        cmake ..
        make

    - name: Run unit tests
      run: |
        cd tests/build
        ./unit_tests --gtest_output=xml:unit_test_results.xml

    - name: Run integration tests
      run: |
        cd tests/build
        ./integration_tests --gtest_output=xml:integration_test_results.xml

    - name: Publish test results
      uses: EnricoMi/publish-unit-test-result-action@v2
      if: always()
      with:
        files: tests/build/*_test_results.xml
```

---

## 7. Implementation Roadmap

### Phase 1: Foundation (Weeks 1-2) ✅ COMPLETE
- [x] Set up test directory structure
- [x] Integrate Google Test framework
- [x] Create CMakeLists.txt and tests.pro
- [x] Implement test helper library (graph_builders.h)
- [x] Write first unit test (test_graph_container.cpp)
- [x] Set up CI/CD pipeline
- **Result**: 27 tests passing (23 GraphContainer + 4 integration)

### Phase 2: Core Data Structures (Weeks 3-4) 🔄 IN PROGRESS
- [x] Complete core class tests for Graph class (18 tests)
- [x] Test circular order invariants (17 tests)
- [x] Implement graph validators (reused from Phase 1)
- [x] Test PSet property system thoroughly (27 tests, 1 disabled)
- [ ] Complete TopologicalGraph-specific tests (TODO)
- [ ] Complete GeometricGraph tests (TODO)
- **Current Status**: 89 tests passing (85 unit + 4 integration), 1 disabled
- **Note**: tstring property test disabled due to segfault (requires investigation)

### Phase 3: Traversal Algorithms (Week 5)
- [ ] Test DFS/BFS implementations
- [ ] Test biconnected component detection
- [ ] Test component numbering

### Phase 4: Planarity (Weeks 6-7)
- [ ] Test all planarity algorithms (LR, single-pass)
- [ ] Test Kuratowski subgraph extraction
- [ ] Test maximal planar subgraph
- [ ] Create comprehensive planar/non-planar test suite

### Phase 5: Embedding & Drawing (Weeks 8-9)
- [ ] Test Schnyder wood algorithms
- [ ] Test bipolar orientations
- [ ] Test Tutte embedding
- [ ] Test FPP and other drawing algorithms

### Phase 6: Integration & BDD (Week 10)
- [ ] Write integration test scenarios
- [ ] Implement BDD features (if desired)
- [ ] Test full pipelines

### Phase 7: Performance & Documentation (Week 11-12)
- [ ] Add performance benchmarks
- [ ] Document test coverage
- [ ] Identify gaps and add missing tests
- [ ] Generate coverage reports

---

## 8. Code Coverage Goals

### Target Coverage:
- **Overall**: 80%+
- **Core classes** (Graph, TopologicalGraph): 90%+
- **Critical algorithms** (planarity, DFS/BFS): 85%+
- **I/O operations**: 75%+
- **Drawing algorithms**: 70%+

### Tools:
- **gcov/lcov**: Generate coverage reports
- **Codecov**: Online coverage tracking
- **Coveralls**: Alternative coverage service

### Integration:
```yaml
# Add to .github/workflows/test.yml
- name: Generate coverage
  run: |
    cd tests/build
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info
    lcov --list coverage.info

- name: Upload coverage
  uses: codecov/codecov-action@v3
  with:
    files: tests/build/coverage.info
```

---

## 9. Testing Best Practices

### 9.1 Test Naming Convention

```cpp
// Format: TEST(TestSuiteName, TestName)
// TestSuiteName: Component being tested
// TestName: Specific behavior being tested

TEST(GraphContainer, DefaultConstructorCreatesEmptyGraph)
TEST(CircularOrder, InvariantHoldsAfterEdgeInsertion)
TEST(PlanarityLR, K5DetectedAsNonPlanar)
TEST(Schnyder, ValidOrientationOnMaximalPlanar)
```

### 9.2 Assertion Guidelines

```cpp
// Prefer specific assertions
EXPECT_EQ(G.nv(), 10);           // ✓ Good
EXPECT_TRUE(G.nv() == 10);       // ✗ Less informative

// Use descriptive failure messages
EXPECT_TRUE(ValidateCircularOrder(G))
    << "Circular order violated after NewEdge()";

// Test boundary conditions
EXPECT_EQ(G.nv(), 0);  // Empty graph
EXPECT_EQ(G.nv(), 1);  // Single vertex
EXPECT_EQ(G.ne(), 0);  // No edges
```

### 9.3 Test Independence

```cpp
// Each test should be independent
// ✓ Good: Use SetUp/TearDown
class GraphTest : public ::testing::Test {
protected:
    void SetUp() override {
        G = new TopologicalGraph();
        // Initialize...
    }

    void TearDown() override {
        delete G;
    }

    TopologicalGraph* G;
};

// ✗ Bad: Tests depend on each other
TEST(Bad, Test1) { globalGraph.AddEdge(...); }
TEST(Bad, Test2) { /* Assumes Test1 ran */ }
```

### 9.4 Test Documentation

```cpp
// Document complex test scenarios
TEST(Planarity, KuratowskiSubgraphExtractionK5) {
    // K5 is non-planar because it contains K5 as a minor
    // This test verifies:
    // 1. K5 is correctly identified as non-planar
    // 2. The Kuratowski subgraph is K5 itself
    // 3. The subgraph is properly extracted with valid properties

    GraphContainer* gc = BuildK5();
    TopologicalGraph G(gc);

    ASSERT_FALSE(G.TestPlanar());
    // ... rest of test
}
```

---

## 10. Maintenance and Evolution

### 10.1 Adding New Tests

When adding new algorithm implementations:
1. Write unit tests first (TDD approach)
2. Add integration tests for algorithm combinations
3. Update test fixtures if needed
4. Update this document

### 10.2 Test Review Process

Before merging code:
- [ ] All tests pass locally
- [ ] CI/CD pipeline passes
- [ ] Code coverage doesn't decrease
- [ ] New code has tests (>80% coverage)
- [ ] Tests are documented

### 10.3 Regression Testing

When bugs are found:
1. Write a failing test that reproduces the bug
2. Fix the bug
3. Verify test passes
4. Add test to regression suite

---

## 11. Open Questions and Future Work

### 11.1 Questions to Resolve:
1. Should we use Catch2 instead of Google Test? (More modern, header-only)
2. Do we need BDD tests, or are integration tests sufficient?
3. What's the target code coverage percentage?
4. Should we integrate with SonarQube for code quality?
5. Do we need mutation testing (e.g., using Stryker)?

### 11.2 Future Enhancements:
- [ ] Property-based testing (e.g., using RapidCheck)
- [ ] Fuzzing for robustness (e.g., using libFuzzer)
- [ ] Visual regression testing for drawing algorithms
- [ ] Performance regression detection
- [ ] Test parallelization for faster CI/CD

### 11.3 Advanced Testing Techniques:

#### Property-Based Testing Example:
```cpp
// Using RapidCheck
RC_GTEST_PROP(PlanarityProperty,
              ConnectedPlanarGraphHasFewerEdges,
              (int n)) {
    RC_PRE(n >= 3 && n <= 100);

    auto* gc = GenerateRandomPlanarGraph(n);
    TopologicalGraph G(gc);

    // Euler's formula: m ≤ 3n - 6 for connected planar graphs
    RC_ASSERT(G.ne() <= 3 * G.nv() - 6);
}
```

---

## 12. Risk Assessment

### High Risk Areas:
1. **Circular order operations**: Complex pointer manipulation
2. **Property system**: Memory management, type safety
3. **Planarity algorithms**: High algorithmic complexity
4. **Concurrent modifications**: If multi-threading added

### Mitigation Strategies:
- Extensive unit tests for circular order invariants
- Memory sanitizers (AddressSanitizer, MemorySanitizer)
- Valgrind integration for leak detection
- Stress tests with large graphs
- Randomized testing with property-based tests

---

## 13. Success Criteria

The test suite will be considered comprehensive when:

✓ **Coverage**: >80% code coverage across tgraph library
✓ **Completeness**: All public APIs have unit tests
✓ **Robustness**: Edge cases and error conditions tested
✓ **Integration**: Key workflows have integration tests
✓ **Performance**: Benchmarks established for critical algorithms
✓ **Automation**: CI/CD pipeline runs all tests on every commit
✓ **Documentation**: Test suite is documented and maintainable
✓ **Speed**: Full test suite completes in <5 minutes

---

## Appendix A: Test Prioritization Matrix

| Component | Priority | Complexity | Risk | Estimated Effort |
|-----------|----------|------------|------|------------------|
| GraphContainer | HIGH | Low | Medium | 2 days |
| Graph | HIGH | Medium | High | 3 days |
| TopologicalGraph | HIGH | High | High | 5 days |
| GeometricGraph | MEDIUM | Medium | Medium | 3 days |
| PSet/Properties | HIGH | High | High | 4 days |
| Circular Order | HIGH | High | Critical | 4 days |
| DFS/BFS | HIGH | Low | Medium | 2 days |
| Biconnect | HIGH | Medium | High | 3 days |
| Planarity (LR) | HIGH | High | Critical | 5 days |
| Planarity (Single-Pass) | MEDIUM | High | Medium | 3 days |
| MaxPlanar | MEDIUM | High | Medium | 3 days |
| Schnyder | MEDIUM | High | Medium | 4 days |
| Tutte | MEDIUM | Medium | Low | 2 days |
| FPP/Vision | LOW | High | Low | 4 days |
| Generate | HIGH | Low | Low | 2 days |
| TGF I/O | HIGH | Medium | Medium | 3 days |
| Integration Tests | MEDIUM | Medium | High | 5 days |

**Total Estimated Effort**: ~55-60 developer days (~12 weeks for 1 person)

---

## Appendix B: Sample Test Output

Expected test execution output:

```
$ ./unit_tests
[==========] Running 127 tests from 24 test suites.
[----------] Global test environment set-up.
[----------] 8 tests from GraphContainer
[ RUN      ] GraphContainer.DefaultConstruction
[       OK ] GraphContainer.DefaultConstruction (0 ms)
[ RUN      ] GraphContainer.SetSizeAllocatesProperties
[       OK ] GraphContainer.SetSizeAllocatesProperties (1 ms)
...
[----------] 8 tests from GraphContainer (12 ms total)

[----------] 15 tests from CircularOrder
[ RUN      ] CircularOrder.CircularOrderInvariant
[       OK ] CircularOrder.CircularOrderInvariant (3 ms)
...
[----------] 15 tests from CircularOrder (45 ms total)

[----------] 22 tests from Planarity
[ RUN      ] Planarity.K4IsPlanar
[       OK ] Planarity.K4IsPlanar (5 ms)
[ RUN      ] Planarity.K5IsNonPlanar
[       OK ] Planarity.K5IsNonPlanar (8 ms)
...
[----------] 22 tests from Planarity (156 ms total)

[----------] Global test environment tear-down
[==========] 127 tests from 24 test suites ran. (2847 ms total)
[  PASSED  ] 127 tests.
```

---

## Appendix C: References

### Testing Resources:
- Google Test Documentation: https://google.github.io/googletest/
- Modern C++ Testing: https://www.modernescpp.com/
- Test-Driven Development (Beck): ISBN 978-0321146533

### Graph Theory Testing:
- "Graph Algorithms" (Sedgewick): ISBN 978-0321573513
- Planarity testing algorithms: Hopcroft-Tarjan, Booth-Lueker
- Pigale documentation: http://pigale.sourceforge.net/

### CI/CD:
- GitHub Actions: https://docs.github.com/en/actions
- Codecov: https://about.codecov.io/
- CMake Testing: https://cmake.org/cmake/help/latest/manual/ctest.1.html

---

**Document Version**: 1.0
**Last Updated**: 2025-11-05
**Author**: Test Planning Team
**Next Review**: After Phase 1 completion
