# Library Bugs - Verification Report

**Date**: 2025-11-06
**Verified By**: Careful source code analysis and debugging
**Method**: Source code study, test programs, gdb analysis

All bugs reported in LIBRARY_BUGS_TO_REPORT.md have been independently verified.

---

## Bug #1: Kuratowski() returns -1 on K5

### Status: ✅ **CONFIRMED - Real Bug**

### Root Cause Identified

**File**: `tgraph/DFSGraph.cpp`, line 458

**The Bug**: Logic error in boolean expression
```cpp
// BUGGY CODE (line 458):
if ((nv() == 6) && ((ne() == 9) || ((nv() == 5) && (ne() == 10))))

// CORRECT CODE should be:
if ((nv() == 6 && ne() == 9) || (nv() == 5 && ne() == 10))
```

### Explanation

The buggy condition checks:
- `nv() == 6` AND (`ne() == 9` OR (`nv() == 5` AND `ne() == 10`))

For K5 (nv=5, ne=10):
- `nv() == 6` evaluates to FALSE
- The entire condition short-circuits to FALSE (AND operator)
- K5 never takes the fast path for K5 detection
- Falls through to general algorithm which fails

For K_{3,3} (nv=6, ne=9):
- `nv() == 6` is TRUE
- `ne() == 9` is TRUE
- Takes fast path correctly (accidentally works)

The inner condition `(nv() == 5 && ne() == 10)` can never be true when the outer condition requires `nv() == 6`.

### Verification

Created test program (`tests/debug_kuratowski3.cpp`) demonstrating:
- Buggy condition returns 0 for K5 (should be 1)
- Fixed condition returns 1 for K5 (correct)
- K5 Kuratowski() returns -1 (fails)
- K_{3,3} Kuratowski() returns 0 (succeeds)

### Impact

This is a **simple logic error** in operator precedence/parentheses. The fix is straightforward.

---

## Bug #2: MaxPlanar() causes segmentation fault

### Status: ✅ **CONFIRMED - Real Bug, But Misdiagnosed**

### Correction to Original Report

**Original claim**: "MaxPlanar() segfaults on NewEdge()-created graphs"
**Reality**: MaxPlanar() segfaults on **ALL** graphs, including library-generated ones

### Root Cause Identified

**File**: `tgraph/MaxPlanar.cpp`, lines 113-115

**The Bug**: Unchecked return values from Kuratowski()
```cpp
// BUGGY CODE (lines 113-115 in EraseMostMarkedEdge):
DG.CotreeCritical();// was commented
DG.Kuratowski();
Prop<bool> keep(DG.Set(tedge()),PROP_MARK);  // Uses PROP_MARK without checking if it was set
```

### Explanation

1. `CotreeCritical()` return value is ignored
2. `Kuratowski()` return value is ignored
3. For planar graphs: CotreeCritical() returns 0 (no critical cotree), Kuratowski() may fail
4. For K5: Kuratowski() returns -1 (Bug #1)
5. In both cases, PROP_MARK may not be properly set
6. Line 115 tries to use PROP_MARK anyway → undefined behavior → segfault

The comment "// was commented" suggests someone previously disabled this code due to problems.

### Verification

Created test program (`tests/debug_maxplanar2.cpp`) demonstrating:
- GenerateGrid(3,3) - library-generated planar graph → **SEGFAULT**
- GenerateCompleteGraph(4) - library-generated planar graph → **SEGFAULT**
- GenerateCompleteGraph(5) - library-generated non-planar graph → **SEGFAULT**
- BuildK5() - NewEdge-created graph → **SEGFAULT**

ALL graphs crash, not just NewEdge() graphs.

### Impact

MaxPlanar() is fundamentally broken due to unchecked error conditions. The bug affects:
- All graph types (planar and non-planar)
- All construction methods (library-generated and NewEdge())

This is more serious than originally reported.

---

## Bug #3: I/O functions lose edges with NewEdge()-created graphs

### Status: ✅ **CONFIRMED - Accurately Described**

### Root Cause: Incompatible Graph Construction Paradigms

The TGF I/O functions expect graphs constructed using the library's internal "manual property setup" pattern, not the public `NewEdge()` API.

### Verification

Created test program (`tests/debug_io.cpp`) demonstrating:

**Library-generated graphs (WORK)**:
- GenerateGrid(2,2): 4 edges → Save → Load → 4 edges ✓

**NewEdge()-created graphs (FAIL)**:
- 4-vertex cycle: 4 edges → Save → Load → 3 edges ✗ (lost 1 edge)
- 3-vertex triangle: 3 edges → Save → Load → 2 edges ✗ (lost 1 edge)

Consistently loses **exactly 1 edge** per round-trip.

### Explanation

The library has two incompatible graph construction paradigms:

1. **Manual setup** (internal, used by library):
   ```cpp
   gc.setsize(n, m);  // Pre-allocate exact edge count
   Prop<tvertex> vin(gc.PB(), PROP_VIN);
   Prop<tbrin> cir(gc.PB(), PROP_CIR);
   Prop<tbrin> acir(gc.PB(), PROP_ACIR);
   // ... manual property setup
   ```

2. **NewEdge API** (public, documented):
   ```cpp
   gc.setsize(n, 0);  // Zero edges initially
   G.NewEdge(...);    // Incremental edge addition
   ```

The I/O save/load functions assume the manual setup pattern and fail on NewEdge() graphs.

### Impact

- 4 I/O tests disabled
- Data loss on save/load cycles
- NewEdge() pattern (primary documented API) is incompatible with persistence
- Affects any application needing to save/load graphs

---

## Summary

| Bug # | Status | Severity | Diagnosis |
|-------|--------|----------|-----------|
| #1 | ✅ Confirmed | Medium | **Accurate** - Logic error in line 458 |
| #2 | ✅ Confirmed | High | **Misdiagnosed** - Affects ALL graphs, not just NewEdge() |
| #3 | ✅ Confirmed | High | **Accurate** - NewEdge() incompatible with I/O |

## Recommendations

### Bug #1: Easy Fix
Change line 458 in `tgraph/DFSGraph.cpp`:
```cpp
if ((nv() == 6 && ne() == 9) || (nv() == 5 && ne() == 10))
```

### Bug #2: Moderate Fix
Add return value checks in `tgraph/MaxPlanar.cpp`:
```cpp
int ret = DG.CotreeCritical();
if (ret == 0) return 0;  // Planar graph, no work needed

ret = DG.Kuratowski();
if (ret != 0) return ret;  // Kuratowski extraction failed
```

### Bug #3: Complex Fix
Either:
1. Fix I/O functions to handle both construction paradigms, OR
2. Document that I/O only works with library-generated graphs

---

## Verification Files

Test programs created during verification:
- `tests/debug_kuratowski.cpp` - Basic K5 test
- `tests/debug_kuratowski2.cpp` - Detailed transformation trace
- `tests/debug_kuratowski3.cpp` - Logic error demonstration
- `tests/debug_kuratowski_on_planar.cpp` - Kuratowski on planar graphs
- `tests/debug_maxplanar.cpp` - MaxPlanar on library vs NewEdge
- `tests/debug_maxplanar2.cpp` - MaxPlanar on multiple graph types
- `tests/debug_io.cpp` - I/O edge loss verification

All bugs are reproducible and verified.

---

**Conclusion**: All three bugs are real library bugs, not user misunderstandings. Bug #2's root cause was misdiagnosed but the bug itself is real and more serious than reported.
