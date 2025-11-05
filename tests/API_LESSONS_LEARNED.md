# API Lessons Learned - Pigale tgraph Test Suite

**Date**: 2025-11-05
**Context**: Phase 1 Implementation and Testing

## Critical Lesson: NewEdge() and setsize()

### The Problem

When building graphs using `TopologicalGraph::NewEdge()`, we initially pre-allocated edges like this:

```cpp
// WRONG APPROACH
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 6);  // Pre-allocate 6 edges

TopologicalGraph G(*gc);
G.NewEdge(tvertex(1), tvertex(2));  // Creates edge #7, not #1!
```

**What happens:**
- `setsize(4, 6)` allocates edge slots 1-6 (uninitialized)
- `NewEdge()` creates NEW edges starting from edge #7
- Result: Edges 1-6 remain uninitialized (vin[1-6] = 0)
- Only edges 7+ have proper vertex incidence data

### The Solution

**Always set edge count to 0 when using NewEdge:**

```cpp
// CORRECT APPROACH
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);  // Edge count = 0!

TopologicalGraph G(*gc);
G.NewEdge(tvertex(1), tvertex(2));  // Creates edge #1 ✓
G.NewEdge(tvertex(2), tvertex(3));  // Creates edge #2 ✓
```

**Why this works:**
- `setsize(n, 0)` allocates n vertices, 0 edges
- `NewEdge()` creates edges 1, 2, 3, ... in order
- All edges are properly initialized with correct vin values

### Verification

Test code that confirms this behavior:

```cpp
// Test with pre-allocated edges
GraphContainer gc1;
gc1.setsize(4, 6);
TopologicalGraph G1(gc1);
tedge e1 = G1.NewEdge(tvertex(1), tvertex(2));
// e1() == 7 (not 1!)

// Test with zero edges
GraphContainer gc2;
gc2.setsize(4, 0);
TopologicalGraph G2(gc2);
tedge e2 = G2.NewEdge(tvertex(1), tvertex(2));
// e2() == 1 ✓
```

### When To Use Each Approach

#### Use `setsize(n, 0)` + `NewEdge()`:
- Building graphs programmatically
- Don't know edge count in advance
- Want automatic edge numbering
- Using high-level graph construction

```cpp
GraphContainer* BuildK4() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);  // Correct!

    TopologicalGraph G(*gc);
    for(int i = 1; i <= 4; i++) {
        for(int j = i+1; j <= 4; j++) {
            G.NewEdge(tvertex(i), tvertex(j));
        }
    }
    return gc;
}
```

#### Use `setsize(n, m)` + manual property initialization:
- Loading from file formats
- Know exact edge count
- Need specific edge numbering
- Following the approach of `GenerateGrid()` and other library functions

```cpp
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 6);  // Pre-allocate all edges

// Manually create properties
Prop<tvertex> vin(gc->PB(), PROP_VIN);
vin[0] = 0;

// Manually set edge endpoints
vin[1] = tvertex(1); vin[-1] = tvertex(2);
vin[2] = tvertex(2); vin[-2] = tvertex(3);
// ... etc
```

## Additional Lessons

### Lesson 2: Graph Object Lifetime

**Problem**: Deleting GraphContainer while Graph/TopologicalGraph still holds reference

```cpp
// DANGEROUS
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);
TopologicalGraph G(*gc);
// ... use G ...
delete gc;  // CRASH! G still holds reference to deleted gc
// G goes out of scope and tries to access deleted memory
```

**Solution**: Ensure Graph goes out of scope before deleting GraphContainer

```cpp
// SAFE
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);
{
    TopologicalGraph G(*gc);
    // ... use G ...
}  // G goes out of scope first
delete gc;  // Now safe
```

### Lesson 3: Property Types

From `propdef.h`, property types must match:

- `PROP_COLOR` → `short` (not `tcolor` - that type doesn't exist)
- `PROP_LABEL` → `int`
- `PROP_COORD` → `Tpoint`
- `PROP_VIN` → `tvertex`
- `PROP_CIR`, `PROP_ACIR`, `PROP_PBRIN` → `tbrin`

### Lesson 4: Property Existence Checking

**Wrong**: `if (G.Exist(PROP_CIR))`  // Method doesn't exist!

**Correct**: `if (G.Set(tbrin()).exist(PROP_CIR))`  // Call exist() on PSet

### Lesson 5: Use Existing Members

TopologicalGraph already has these members - don't create duplicates:

```cpp
// WRONG
Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);
tbrin next = cir[b];

// CORRECT
tbrin next = G.cir[b];  // Use existing member
```

Same for: `G.acir`, `G.pbrin`, `G.vin`, `G.extbrin`

### Lesson 6: PSet vs PSet1

- `gc.Set()` returns `PSet1` (single-value properties)
- `gc.Set(tvertex())` returns `PSet` (per-vertex properties)
- `gc.Set(tedge())` returns `PSet` (per-edge properties)
- `gc.Set(tbrin())` returns `PSet` (per-brin properties)

Or use shortcuts:
- `gc.PV()` for vertex properties
- `gc.PE()` for edge properties
- `gc.PB()` for brin properties
- `gc.PG()` for graph properties

### Lesson 7: Const-Correctness

Most Graph/TopologicalGraph methods are non-const:

```cpp
// WRONG
void ValidateCircularOrder(const TopologicalGraph& G) {
    G.Set(tbrin());  // ERROR: Set() is non-const
}

// CORRECT
void ValidateCircularOrder(TopologicalGraph& G) {  // No const
    G.Set(tbrin());  // OK
}
```

## Summary of Changes Required

From initial implementation to working tests:

1. **Changed all `setsize(n, m)` to `setsize(n, 0)`** in graph builders (15+ functions)
2. **Fixed property types**: `tcolor` → `short`, `PROP_VCOLOR` → `PROP_COLOR`
3. **Fixed property checking**: `G.Exist()` → `G.Set(...).exist()`
4. **Use existing members**: Create Prop objects → Use `G.cir`, `G.vin`, etc.
5. **Fixed pointer dereferencing**: `TopologicalGraph G(gc)` → `G(*gc)`
6. **Removed const qualifiers** from Graph/TopologicalGraph parameters
7. **Fixed object lifetime**: Ensured Graph goes out of scope before delete
8. **Fixed PSet access**: `gc.Set()` → `gc.Set(tvertex())` for Prop constructors

## Impact

- **Before fixes**: 0/27 tests passing (compilation errors)
- **After fixes**: 27/27 tests passing ✅

## References

- Working example: `UsingTgraph/main.cpp`
- Library example: `tgraph/Generate.cpp` (GenerateGrid function)
- Property definitions: `incl/TAXI/propdef.h`
- Graph classes: `incl/TAXI/graphs.h`
- Documentation: `CORRECT_API_PATTERNS.md`
