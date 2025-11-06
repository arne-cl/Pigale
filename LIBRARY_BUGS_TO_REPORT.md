# Library Bugs to Report

These bugs were discovered during comprehensive test suite implementation (Phases 1-7).

---

## Bug #1: Kuratowski() returns -1 on K5 graph

**Severity**: Medium
**Component**: Planarity algorithms
**File**: `tgraph/Planarity.cpp` (DFSGraph::MarkKuratowski)

### Description
The `Kuratowski()` function fails to extract the Kuratowski subgraph from K5 (complete graph on 5 vertices), returning -1 instead of successfully identifying K5 as a Kuratowski subgraph.

### Expected Behavior
- K5 is a well-known Kuratowski graph (forbidden minor for planarity)
- `Kuratowski()` should successfully extract K5 itself as the Kuratowski subgraph
- Should return 0 on success

### Actual Behavior
- `Kuratowski()` returns -1 (failure) on K5
- The function works correctly on K_{3,3} and Petersen graph

### Steps to Reproduce
```cpp
GraphContainer* gc = GenerateCompleteGraph(5);
TopologicalGraph G(*gc);

// Verify K5 is non-planar
int planar = G.TestPlanar();  // Returns 0 (non-planar) - correct

// Try to extract Kuratowski subgraph
int result = G.Kuratowski();  // Returns -1 (failure) - BUG
// Expected: 0 (success)
```

### Root Cause
Error occurs in `DFSGraph::MarkKuratowski()` specifically for K5. K_{3,3} and Petersen graph work fine, suggesting the issue is specific to K5's structure.

### Test Case
See: `tests/unit/algorithms/planarity/test_planarity.cpp`
- Test: `PlanarityTest.DISABLED_KuratowskiOnK5` (line ~145)
- Status: Disabled due to this bug

### Workaround
Use alternative non-planar graphs (K_{3,3}, Petersen) for Kuratowski extraction testing.

### References
- Test suite commit: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV
- Documentation: DEVELOPER_GUIDE.md (Known Library Bugs #1)

---

## Bug #2: MaxPlanar() causes segmentation fault

**Severity**: High (crashes)
**Component**: Planarity algorithms
**File**: `tgraph/Planarity.cpp` (MaxPlanar implementation)

### Description
The `MaxPlanar()` function causes a segmentation fault when called on graphs created using the `NewEdge()` API pattern.

### Expected Behavior
- `MaxPlanar()` should compute a maximal planar subgraph
- Should work on graphs created with `NewEdge()`
- Should return gracefully on any valid graph

### Actual Behavior
- Segmentation fault when called on `NewEdge()`-created graphs
- Crash occurs during execution, not at function entry

### Steps to Reproduce
```cpp
GraphContainer* gc = new GraphContainer();
gc->setsize(5, 0);

TopologicalGraph G(*gc);
// Create K5 using NewEdge
G.NewEdge(tvertex(1), tvertex(2));
G.NewEdge(tvertex(1), tvertex(3));
G.NewEdge(tvertex(1), tvertex(4));
G.NewEdge(tvertex(1), tvertex(5));
G.NewEdge(tvertex(2), tvertex(3));
G.NewEdge(tvertex(2), tvertex(4));
G.NewEdge(tvertex(2), tvertex(5));
G.NewEdge(tvertex(3), tvertex(4));
G.NewEdge(tvertex(3), tvertex(5));
G.NewEdge(tvertex(4), tvertex(5));

int result = G.MaxPlanar();  // SEGFAULT
```

### Root Cause Analysis
MaxPlanar() may expect graphs to be initialized using the manual property setup pattern (like `GenerateGrid()` uses with `setsize(n,m)` and manual vin/cir/acir setup), not the `NewEdge()` pattern.

The library's own generation functions use:
- `setsize(n, m)` with exact edge count
- Manual `Prop<tvertex> vin` setup
- Manual circular order (cir/acir) setup

NewEdge() creates graphs differently:
- `setsize(n, 0)` with zero initial edges
- Incremental edge addition
- Different internal state

### Test Cases
See: `tests/unit/algorithms/planarity/test_planarity.cpp`
- Tests: All `MaxPlanar*` tests disabled (3 tests)
- Status: Disabled due to segfaults

### Workaround
1. Use alternative planarity functions (`TestPlanar()`, `Planarity()`)
2. OR: Initialize graphs using manual property setup like GenerateGrid does
3. Avoid MaxPlanar() on NewEdge()-created graphs

### Impact
- All MaxPlanar tests disabled
- Function effectively unusable with standard NewEdge() API pattern

### References
- Test suite commit: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV
- Documentation: DEVELOPER_GUIDE.md (Known Library Bugs #2)
- Related: See Bug #3 for similar NewEdge() incompatibility with I/O

---

## Bug #3: I/O functions lose edges with NewEdge()-created graphs

**Severity**: High (data loss)
**Component**: File I/O
**Files**: `tgraph/File.cpp` (SaveGraphTgf, ReadTgfGraph)

### Description
The TGF file I/O functions (`SaveGraphTgf()` and `ReadTgfGraph()`) consistently lose one edge when round-tripping graphs created with the `NewEdge()` API. Saving works, but reading back the file produces a graph with fewer edges.

### Expected Behavior
- Save graph with N edges → Load graph with N edges
- Round-trip should preserve graph structure exactly
- Should work with graphs created via `NewEdge()`

### Actual Behavior
- Save 4-vertex cycle (4 edges) → Load back only 3 edges
- Save triangle (3 edges) → Load back only 2 edges
- Consistently loses exactly 1 edge per round-trip
- Vertex count preserved correctly

### Steps to Reproduce
```cpp
// Create a 4-vertex cycle using NewEdge
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);

TopologicalGraph G(*gc);
G.NewEdge(tvertex(1), tvertex(2));
G.NewEdge(tvertex(2), tvertex(3));
G.NewEdge(tvertex(3), tvertex(4));
G.NewEdge(tvertex(4), tvertex(1));

std::cout << "Created: " << G.ne() << " edges" << std::endl;  // 4 edges

// Save to TGF
SaveGraphTgf(G, "test.tgf", 0);

// Load back
GraphContainer gc2;
int numRecords, graphIndex;
ReadTgfGraph(gc2, "test.tgf", numRecords, graphIndex);

TopologicalGraph G2(gc2);
std::cout << "Loaded: " << G2.ne() << " edges" << std::endl;  // 3 edges - BUG!
```

**Output:**
```
Created: 4 edges
Loaded: 3 edges
```

### Graphs That Work Correctly
Graphs created by library generation functions work fine:
```cpp
// GenerateGrid works correctly
GraphContainer* gc = GenerateGrid(3, 3);
TopologicalGraph G(*gc);
std::cout << "Grid: " << G.ne() << " edges" << std::endl;  // 12 edges

SaveGraphTgf(G, "grid.tgf", 0);

GraphContainer gc2;
ReadTgfGraph(gc2, "grid.tgf", numRecords, graphIndex);
TopologicalGraph G2(gc2);
std::cout << "Loaded: " << G2.ne() << " edges" << std::endl;  // 12 edges - correct!
```

### Root Cause
The I/O functions expect graphs to be initialized using the **manual property setup pattern** that the library's own generation functions use:

**Library generation functions (e.g., GenerateGrid):**
```cpp
// From tgraph/Generate.cpp
gc.setsize(n, m);  // Pre-allocate with EXACT edge count
Prop<tvertex> vin(gc.PB(), PROP_VIN);
Prop<tbrin> cir(gc.PB(), PROP_CIR);
Prop<tbrin> acir(gc.PB(), PROP_ACIR);
// ... manual setup of vin, cir, acir for each edge
```

**User code with NewEdge():**
```cpp
gc.setsize(n, 0);  // Zero edges initially
G.NewEdge(...);    // Incremental edge addition
// cir/acir set up automatically by NewEdge
```

The I/O save format likely depends on the specific memory layout and property initialization used by the manual setup approach.

### Test Cases
See: `tests/unit/io/test_tgf.cpp`
- Tests disabled: 4 tests using NewEdge()
  - `DISABLED_RoundTripSimpleGraphWithNewEdge`
  - `DISABLED_RoundTripWithCoordinatesUsingNewEdge`
  - `DISABLED_RoundTripPathWithNewEdge`
  - `DISABLED_EdgeStructurePreservedWithNewEdge`
- Tests passing: 13 tests using library generation functions

### Workaround
**For graphs that need to be saved/loaded:**
1. Use library generation functions (`GenerateGrid`, `GenerateCompleteGraph`, etc.)
2. OR: Manually set up properties like GenerateGrid does (complex, not recommended)
3. Avoid NewEdge() for graphs that need I/O

**Example:**
```cpp
// BAD: NewEdge graph won't round-trip correctly
gc->setsize(n, 0);
G.NewEdge(...);  // DON'T use this if you need I/O

// GOOD: Use library generation
gc = GenerateGrid(3, 3);  // I/O works correctly
```

### Impact
- 4 I/O tests disabled
- NewEdge() pattern (recommended in other contexts) incompatible with I/O
- Data loss on save/load cycles
- Affects any application needing to persist graphs

### Related Issues
- Similar incompatibility in Bug #2 (MaxPlanar segfault with NewEdge)
- Suggests broader issue: library has two incompatible graph construction paradigms
  1. Manual setup (used by library internally)
  2. NewEdge API (documented as primary API)

### Suggested Fix
I/O functions should detect and handle both graph construction patterns, or documentation should clearly state I/O only works with manually-constructed graphs.

### References
- Test suite commit: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV
- Documentation: DEVELOPER_GUIDE.md (Known Library Bugs #3)
- Detailed test cases: tests/unit/io/test_tgf.cpp

---

## Additional Context

All three bugs were discovered during systematic test suite implementation covering:
- 297 tests total (244 unit + 53 integration)
- 9 tests disabled due to these bugs
- Comprehensive testing following DEVELOPER_GUIDE.md patterns

### Test Environment
- Platform: Linux
- Compiler: g++ / clang++
- Build: qmake + make (library), CMake + gtest (tests)
- Branch: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV

### Files to Review
- Test suite: `tests/unit/algorithms/planarity/test_planarity.cpp`
- I/O tests: `tests/unit/io/test_tgf.cpp`
- Documentation: `DEVELOPER_GUIDE.md` (Known Library Bugs section)
- Test plan: `TEST_SUITE_PLAN.md` (Known Limitations section)
