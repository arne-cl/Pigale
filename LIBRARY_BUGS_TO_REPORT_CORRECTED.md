# Library Bugs to Report (Verified and Corrected)

**Verification Date**: 2025-11-06
**Status**: All bugs independently verified through source code analysis and testing

These bugs were discovered during comprehensive test suite implementation and have been verified through detailed source code analysis, test programs, and debugging.

---

## Bug #1: Kuratowski() returns -1 on K5 - Logic Error

**Severity**: Medium
**Component**: Planarity algorithms
**File**: `tgraph/DFSGraph.cpp`, line 458
**Status**: ✅ Verified - Real bug with root cause identified

### Description
The `Kuratowski()` function fails to extract the Kuratowski subgraph from K5 (complete graph on 5 vertices), returning -1 due to a logic error in a boolean expression.

### Root Cause
**Logic error in line 458 of DFSGraph.cpp:**

```cpp
// BUGGY CODE:
if ((nv() == 6) && ((ne() == 9) || ((nv() == 5) && (ne() == 10))))

// SHOULD BE:
if ((nv() == 6 && ne() == 9) || (nv() == 5 && ne() == 10))
```

The buggy condition requires `nv() == 6` for the entire expression to be true, which means the inner condition `(nv() == 5 && ne() == 10)` can never be reached. K5 (with nv=5, ne=10) fails the outer test and never takes the fast path for K5 detection.

### Expected Behavior
- K5 is a well-known Kuratowski graph
- `Kuratowski()` should successfully extract K5 itself
- Should return 0 on success

### Actual Behavior
- Returns -1 (failure) on K5
- Works correctly on K_{3,3} and Petersen graph

### Steps to Reproduce
```cpp
GraphContainer* gc = GenerateCompleteGraph(5);
TopologicalGraph G(*gc);

int result = G.Kuratowski();  // Returns -1 - BUG
// Expected: 0 (success)
```

### Verification
Verified with test program `tests/debug_kuratowski3.cpp`:
- Buggy condition evaluates to 0 for K5 (incorrect)
- Correct condition evaluates to 1 for K5 (correct)
- After CotreeCritical(), K5 has nv=5, ne=10
- K_{3,3} works (nv=6, ne=9 passes the buggy condition by accident)

### Suggested Fix
Change line 458 in `tgraph/DFSGraph.cpp`:
```cpp
if ((nv() == 6 && ne() == 9) || (nv() == 5 && ne() == 10))
    {for(e = 1;e <= ne();e++)keep[e] = true;
    return 0;
    }
```

### Test Case
- Test: `tests/unit/algorithms/planarity/test_planarity.cpp`
  - `PlanarityTest.DISABLED_KuratowskiOnK5` (line ~303)
- Status: Disabled due to this bug

---

## Bug #2: MaxPlanar() segmentation fault - Unchecked Error Codes

**Severity**: High (crashes)
**Component**: Planarity algorithms
**File**: `tgraph/MaxPlanar.cpp`, lines 113-115
**Status**: ✅ Verified - More serious than initially reported

### Correction to Original Report
**Original claim**: Segfaults only on NewEdge()-created graphs
**Verified reality**: Segfaults on **ALL** graphs (library-generated and NewEdge-created, planar and non-planar)

### Description
The `MaxPlanar()` function causes segmentation faults on all graph types due to unchecked return values from `Kuratowski()`.

### Root Cause
**File**: `tgraph/MaxPlanar.cpp`, lines 113-115 in `EraseMostMarkedEdge()`:

```cpp
DG.CotreeCritical();// was commented
DG.Kuratowski();
Prop<bool> keep(DG.Set(tedge()),PROP_MARK);
```

Neither return value is checked. If `Kuratowski()` fails:
- For planar graphs: CotreeCritical() returns 0 (no critical cotree found)
- For K5: Kuratowski() returns -1 (Bug #1)
- `PROP_MARK` is not properly set
- Line 115 tries to use `PROP_MARK` anyway → undefined behavior → segfault

The comment "// was commented" suggests this code was previously disabled, likely due to these issues.

### Expected Behavior
- `MaxPlanar()` should compute maximal planar subgraph
- Should work on any valid graph
- Should return gracefully on errors

### Actual Behavior
- Segmentation fault on ALL graphs tested:
  - Grid 3x3 (library-generated, planar) → SEGFAULT
  - K4 (library-generated, planar) → SEGFAULT
  - K5 (library-generated, non-planar) → SEGFAULT
  - BuildK5() (NewEdge-created) → SEGFAULT

### Steps to Reproduce
```cpp
// ANY of these will crash:

// Test 1: Library-generated planar graph
GraphContainer* gc = GenerateGrid(3, 3);
TopologicalGraph G(*gc);
int result = G.MaxPlanar();  // SEGFAULT

// Test 2: Library-generated non-planar graph
GraphContainer* gc2 = GenerateCompleteGraph(5);
TopologicalGraph G2(*gc2);
int result2 = G2.MaxPlanar();  // SEGFAULT

// Test 3: NewEdge-created graph
GraphContainer* gc3 = new GraphContainer();
gc3->setsize(5, 0);
TopologicalGraph G3(*gc3);
// ... add edges with NewEdge() ...
int result3 = G3.MaxPlanar();  // SEGFAULT
```

### Verification
Verified with test program `tests/debug_maxplanar2.cpp`:
- Exit code 139 (SIGSEGV) on all graph types
- Crash occurs in `TopologicalGraph::MaxPlanar(svector<bool>&)`
- GDB backtrace confirms crash location

### Suggested Fix
Add error checking in `tgraph/MaxPlanar.cpp`:

```cpp
// Line 113-115:
int ret = DG.CotreeCritical();
if (ret == 0) return tedge(0);  // Planar graph, no work needed

ret = DG.Kuratowski();
if (ret != 0) {
    setPigaleError(ret, "Kuratowski extraction failed");
    return tedge(0);
}

Prop<bool> keep(DG.Set(tedge()),PROP_MARK);
```

### Impact
- MaxPlanar() is completely non-functional
- All 3 MaxPlanar tests disabled
- Function crashes on ANY graph type
- More serious than originally reported

### Test Cases
See: `tests/unit/algorithms/planarity/test_planarity.cpp`
- `DISABLED_MaxPlanarOnK5`
- `DISABLED_MaxPlanarOnK33`
- `DISABLED_MaxPlanarOnAlreadyPlanar`

---

## Bug #3: I/O functions lose edges with NewEdge()-created graphs

**Severity**: High (data loss)
**Component**: File I/O
**Files**: `tgraph/File.cpp` (SaveGraphTgf, ReadTgfGraph)
**Status**: ✅ Verified - Accurately described

### Description
The TGF file I/O functions consistently lose exactly one edge when round-tripping graphs created with the `NewEdge()` API. Library-generated graphs work correctly.

### Expected Behavior
- Save graph with N edges → Load graph with N edges
- Round-trip should preserve graph structure exactly
- Should work with graphs created via `NewEdge()`

### Actual Behavior
- NewEdge graphs lose exactly 1 edge per round-trip:
  - 4-vertex cycle (4 edges) → save/load → 3 edges
  - 3-vertex triangle (3 edges) → save/load → 2 edges
- Library-generated graphs work correctly:
  - Grid 2x2 (4 edges) → save/load → 4 edges ✓
- Vertex count preserved correctly in all cases

### Steps to Reproduce
```cpp
// NewEdge-created graph (FAILS)
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);
TopologicalGraph G(*gc);
G.NewEdge(tvertex(1), tvertex(2));
G.NewEdge(tvertex(2), tvertex(3));
G.NewEdge(tvertex(3), tvertex(4));
G.NewEdge(tvertex(4), tvertex(1));

std::cout << "Created: " << G.ne() << " edges" << std::endl;  // 4 edges

SaveGraphTgf(G, "test.tgf", 0);

GraphContainer gc2;
int numRecords, graphIndex;
ReadTgfGraph(gc2, "test.tgf", numRecords, graphIndex);
TopologicalGraph G2(gc2);

std::cout << "Loaded: " << G2.ne() << " edges" << std::endl;  // 3 edges - BUG!
```

```cpp
// Library-generated graph (WORKS)
GraphContainer* gc = GenerateGrid(2, 2);
TopologicalGraph G(*gc);
std::cout << "Created: " << G.ne() << " edges" << std::endl;  // 4 edges

SaveGraphTgf(G, "grid.tgf", 0);

GraphContainer gc2;
int numRecords, graphIndex;
ReadTgfGraph(gc2, "grid.tgf", numRecords, graphIndex);
TopologicalGraph G2(gc2);
std::cout << "Loaded: " << G2.ne() << " edges" << std::endl;  // 4 edges - correct!
```

### Verification
Verified with test program `tests/debug_io.cpp`:
- GenerateGrid(2,2): 4 edges → 4 edges ✓
- NewEdge cycle: 4 edges → 3 edges ✗ (lost 1)
- NewEdge triangle: 3 edges → 2 edges ✗ (lost 1)

### Root Cause
The library has two incompatible graph construction paradigms:

**1. Manual property setup** (used internally by library):
```cpp
// From tgraph/Generate.cpp
gc.setsize(n, m);  // Pre-allocate with EXACT edge count
Prop<tvertex> vin(gc.PB(), PROP_VIN);
Prop<tbrin> cir(gc.PB(), PROP_CIR);
Prop<tbrin> acir(gc.PB(), PROP_ACIR);
// ... manual setup of vin, cir, acir for each edge
```

**2. NewEdge API** (documented public API):
```cpp
gc.setsize(n, 0);  // Zero edges initially
G.NewEdge(...);    // Incremental edge addition
// cir/acir set up automatically by NewEdge
```

The I/O save/load functions assume the manual setup pattern and fail on NewEdge() graphs.

### Workaround
**For graphs that need to be saved/loaded:**
1. Use library generation functions (`GenerateGrid`, `GenerateCompleteGraph`, etc.)
2. Avoid `NewEdge()` for graphs that require persistence

### Impact
- 4 I/O tests disabled
- NewEdge() pattern (primary documented API) incompatible with I/O
- Data loss on save/load cycles
- Affects any application needing to persist graphs

### Test Cases
See: `tests/unit/io/test_tgf.cpp`
- Tests disabled (4 tests using NewEdge):
  - `DISABLED_RoundTripSimpleGraphWithNewEdge`
  - `DISABLED_RoundTripWithCoordinatesUsingNewEdge`
  - `DISABLED_RoundTripPathWithNewEdge`
  - `DISABLED_EdgeStructurePreservedWithNewEdge`
- Tests passing (13 tests using library generation)

### Suggested Fix
Either:
1. Fix I/O functions to detect and handle both construction patterns, OR
2. Clearly document that TGF I/O only works with library-generated graphs

---

## Additional Context

### Verification Methodology
All bugs verified through:
1. Source code analysis of library implementation
2. Test programs demonstrating each bug
3. GDB debugging where applicable
4. Comparison of working vs. broken cases

### Test Environment
- Platform: Linux
- Compiler: g++ / clang++
- Build: qmake + make (library), CMake + gtest (tests)
- Branch: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV

### Test Programs Created
- `tests/debug_kuratowski.cpp` - Basic K5 test
- `tests/debug_kuratowski2.cpp` - Transformation trace
- `tests/debug_kuratowski3.cpp` - Logic error demonstration
- `tests/debug_kuratowski_on_planar.cpp` - Kuratowski on planar graphs
- `tests/debug_maxplanar.cpp` - MaxPlanar comparison
- `tests/debug_maxplanar2.cpp` - MaxPlanar on multiple types
- `tests/debug_io.cpp` - I/O edge loss verification

### Files to Review
- Test suite: `tests/unit/algorithms/planarity/test_planarity.cpp`
- I/O tests: `tests/unit/io/test_tgf.cpp`
- Documentation: `DEVELOPER_GUIDE.md` (Known Library Bugs section)
- Test plan: `TEST_SUITE_PLAN.md` (Known Limitations section)
- Verification report: `LIBRARY_BUGS_VERIFIED.md`

---

## Summary

| Bug # | Severity | Status | Diagnosis Accuracy |
|-------|----------|--------|--------------------|
| #1 | Medium | ✅ Real Bug | ✅ Accurate - Logic error identified |
| #2 | High | ✅ Real Bug | ⚠️ Misdiagnosed - Affects ALL graphs, not just NewEdge() |
| #3 | High | ✅ Real Bug | ✅ Accurate - NewEdge() incompatible with I/O |

**All three bugs are real library bugs**, not user misunderstandings. They are ready to be reported to the library maintainers.
