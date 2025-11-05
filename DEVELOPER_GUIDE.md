# Pigale Developer Guide - Lessons from Test Suite Implementation

**Author**: Claude (AI Assistant)
**Date**: 2025-11-05
**Context**: Test suite implementation for tgraph library
**Branch**: claude/plan-test-suite-011CUqLN1JHrgWipL88rcpwV

## Purpose

This guide documents all the mistakes made during the Phase 1 test suite implementation and provides clear guidance on how to avoid them. This is a **post-mortem learning document** that captures hard-won lessons about the Pigale/tgraph API.

---

## Table of Contents

1. [The Cardinal Rule](#the-cardinal-rule)
2. [Critical Mistake #1: The NewEdge() Trap](#critical-mistake-1-the-newedge-trap)
3. [Mistake #2: Assuming API Behavior](#mistake-2-assuming-api-behavior)
4. [Mistake #3: Creating Duplicate Properties](#mistake-3-creating-duplicate-properties)
5. [Mistake #4: Non-Existent Methods](#mistake-4-non-existent-methods)
6. [Mistake #5: Wrong Property Types](#mistake-5-wrong-property-types)
7. [Mistake #6: Pointer vs Reference Confusion](#mistake-6-pointer-vs-reference-confusion)
8. [Mistake #7: Object Lifetime Issues](#mistake-7-object-lifetime-issues)
9. [Mistake #8: PSet vs PSet1 Confusion](#mistake-8-pset-vs-pset1-confusion)
10. [Mistake #9: Const-Correctness Assumptions](#mistake-9-const-correctness-assumptions)
11. [Mistake #10: GitHub Actions Permissions](#mistake-10-github-actions-permissions)
12. [Best Practices](#best-practices)
13. [Code Review Checklist](#code-review-checklist)

---

## The Cardinal Rule

### ⚠️ **NEVER GUESS API BEHAVIOR - ALWAYS TEST**

**What happened**: Initial implementation wrote 39 tests based on assumptions about how the API worked. None compiled.

**User feedback that changed everything**: *"don't guess, actually run the tests!"*

**The lesson**:
- Read the headers
- **BUT ALSO** study working examples (UsingTgraph/main.cpp, tgraph/Generate.cpp)
- **AND** actually compile and run code to verify understanding
- Never assume that patterns from other libraries apply here

**How to follow this rule**:
1. Before writing any code, find working examples
2. Create small test programs to verify API behavior
3. Compile and run early and often
4. Don't write 1000+ lines before testing compilation

---

## Critical Mistake #1: The NewEdge() Trap

### ❌ **The Mistake**

**What I did**:
```cpp
GraphContainer* BuildK4() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 6);  // "K4 has 6 edges, so pre-allocate 6 edges"

    TopologicalGraph G(*gc);

    // Add edges
    G.NewEdge(tvertex(1), tvertex(2));  // Expect to create edge #1
    G.NewEdge(tvertex(1), tvertex(3));  // Expect to create edge #2
    // ... 4 more edges

    return gc;
}
```

**What actually happened**:
```cpp
// After setsize(4, 6):
// - Edges 1-6 exist but are UNINITIALIZED (vin[1-6] = 0)

// NewEdge() doesn't fill these slots!
tedge e1 = G.NewEdge(tvertex(1), tvertex(2));  // e1() == 7  (not 1!)
tedge e2 = G.NewEdge(tvertex(1), tvertex(3));  // e2() == 8  (not 2!)
// ...
tedge e6 = G.NewEdge(tvertex(3), tvertex(4));  // e6() == 12 (not 6!)

// Result:
// - Edges 1-6: uninitialized (vin = 0, breaks everything)
// - Edges 7-12: properly initialized
// - Tests crash when accessing vin[1-6]
```

**Why this happens**:
`NewEdge()` ALWAYS creates NEW edges beyond the current count. It increments `m` (edge count) and creates edge `m+1`. It does NOT fill in pre-allocated slots.

### ✅ **The Solution**

**ALWAYS use `setsize(n, 0)` when using NewEdge()**:

```cpp
GraphContainer* BuildK4() {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);  // Edge count = 0!  ✓

    TopologicalGraph G(*gc);

    G.NewEdge(tvertex(1), tvertex(2));  // Creates edge #1 ✓
    G.NewEdge(tvertex(1), tvertex(3));  // Creates edge #2 ✓
    // ...

    return gc;
}
```

### 📋 **When to Use Each Approach**

#### Use `setsize(n, 0)` + `NewEdge()`:
- ✅ Building graphs programmatically
- ✅ Don't know exact edge count in advance
- ✅ Want automatic edge numbering
- ✅ High-level graph construction

#### Use `setsize(n, m)` + manual property setup:
- ✅ Loading from file formats
- ✅ Know exact edge count and structure
- ✅ Need specific edge numbering
- ✅ Following library patterns (like `GenerateGrid()`)

**Example of manual approach**:
```cpp
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 6);  // Pre-allocate all edges

// Manually create properties
Prop<tvertex> vin(gc->PB(), PROP_VIN);
vin[0] = 0;

// Manually set edge endpoints
vin[1] = tvertex(1); vin[-1] = tvertex(2);  // Edge 1: 1-2
vin[2] = tvertex(1); vin[-2] = tvertex(3);  // Edge 2: 1-3
// etc...
```

### 🔍 **How to Verify**

Create a test file:
```cpp
#include <Pigale.h>
#include <iostream>

int main() {
    // Test 1: Pre-allocated edges
    GraphContainer gc1;
    gc1.setsize(4, 6);
    TopologicalGraph G1(gc1);
    tedge e1 = G1.NewEdge(tvertex(1), tvertex(2));
    std::cout << "With setsize(4,6): NewEdge created edge #" << e1() << std::endl;

    // Test 2: Zero edges
    GraphContainer gc2;
    gc2.setsize(4, 0);
    TopologicalGraph G2(gc2);
    tedge e2 = G2.NewEdge(tvertex(1), tvertex(2));
    std::cout << "With setsize(4,0): NewEdge created edge #" << e2() << std::endl;

    return 0;
}
```

Expected output:
```
With setsize(4,6): NewEdge created edge #7
With setsize(4,0): NewEdge created edge #1
```

### 💥 **Impact**

This single mistake affected:
- ✗ 17 graph builder functions
- ✗ All integration tests (K4, K5, K3,3 tests crashed)
- ✗ Multiple unit tests
- ✗ Prevented any tests from running successfully

**Time to discover**: ~4 hours of debugging after studying working code

**Time to fix**: 15 minutes (change all `setsize(n, m)` to `setsize(n, 0)`)

---

## Mistake #2: Assuming API Behavior

### ❌ **The Mistake**

**What I did**:
- Read header files (graphs.h, propdef.h)
- Made assumptions about how the API should work
- Wrote 1000+ lines of test code based on these assumptions
- Tried to compile → 50+ errors

**Why this failed**:
- Headers don't show implementation details
- No comments explaining NewEdge() behavior
- Assumed patterns from other graph libraries would apply
- Didn't look at working examples first

### ✅ **The Solution**

**ALWAYS study working code BEFORE implementing**:

1. **Start with working examples**:
   - `UsingTgraph/main.cpp` - Simple, complete example
   - `tgraph/Generate.cpp` - Library implementation patterns
   - `tgraph/*.cpp` - Algorithm implementations

2. **Create verification tests**:
   ```cpp
   // Quick test to verify understanding
   GraphContainer gc;
   gc.setsize(4, 0);
   TopologicalGraph G(gc);
   tedge e = G.NewEdge(tvertex(1), tvertex(2));
   std::cout << "Edge created: " << e() << std::endl;
   std::cout << "Edge count: " << G.ne() << std::endl;
   ```

3. **Compile small pieces first**:
   - Don't write 1000+ lines before compiling
   - Test each helper function individually
   - Verify API understanding incrementally

### 📋 **Process to Follow**

```
❌ WRONG Process:
1. Read headers
2. Write all code
3. Try to compile → fails
4. Debug for hours
5. Rewrite everything

✅ CORRECT Process:
1. Read headers
2. Study working examples
3. Write small test program
4. Compile and run test
5. Verify behavior matches expectation
6. Document findings
7. Implement actual code
8. Compile early and often
```

---

## Mistake #3: Creating Duplicate Properties

### ❌ **The Mistake**

**What I did**:
```cpp
bool ValidateCircularOrder(TopologicalGraph& G) {
    // Create Prop objects for circular order
    Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);
    Prop<tbrin> acir(G.Set(tbrin()), PROP_ACIR);

    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();
        if(cir[acir[b]] != b) {  // Check invariant
            return false;
        }
    }
    return true;
}
```

**Why this is wrong**:
- `TopologicalGraph` **already has** `cir` and `acir` as public members!
- Creating new `Prop` objects adds overhead
- The existing members are already properly initialized
- This is inefficient and confusing

**How to discover what exists**:
```cpp
// Look at incl/TAXI/graphs.h:
class TopologicalGraph : public Graph {
public:
    Prop<tbrin> cir;        // Circular order - ALREADY EXISTS!
    Prop<tbrin> acir;       // Reverse circular order - ALREADY EXISTS!
    Prop<tbrin> pbrin;      // First brin at vertex - ALREADY EXISTS!
    Prop<tbrin> extbrin;    // External brin for faces
    Prop1<int>  planarMap;  // 0/1 if planar map exists
    // ...
};
```

### ✅ **The Solution**

**Use existing members directly**:

```cpp
bool ValidateCircularOrder(TopologicalGraph& G) {
    // Check if properties exist first
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tbrin()).exist(PROP_ACIR)) {
        return false;
    }

    // Use G.cir and G.acir directly - they already exist!
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();
        if(G.cir[G.acir[b]] != b) {  // Use G.cir, not new Prop
            return false;
        }
    }
    return true;
}
```

### 📋 **Members That Already Exist**

**Graph class has**:
```cpp
Prop<tvertex> vin;  // Vertex incidence (which vertex for each brin)
```

**TopologicalGraph class has**:
```cpp
Prop<tbrin> cir;      // Use G.cir
Prop<tbrin> acir;     // Use G.acir
Prop<tbrin> pbrin;    // Use G.pbrin
Prop<tbrin> extbrin;  // Use G.extbrin
Prop1<int> planarMap; // Use G.planarMap()
```

**GeometricGraph class has**:
```cpp
// Check GeometricGraph header for coordinate/drawing properties
```

### 💥 **Impact**

- Compilation errors from duplicate definitions
- Confusion about which property to use
- Potential bugs from using wrong property instance
- Affected graph_comparators.cpp and property_validators.cpp

---

## Mistake #4: Non-Existent Methods

### ❌ **The Mistake**

**What I did**:
```cpp
if(G.Exist(PROP_CIR)) {  // Check if property exists
    // Use circular order
}
```

**Compiler error**:
```
error: 'class TopologicalGraph' has no member named 'Exist'
```

**Why**: I assumed there would be an `Exist()` method on Graph/TopologicalGraph because it seemed logical.

### ✅ **The Solution**

**Use the correct API**:

```cpp
// WRONG:
if(G.Exist(PROP_CIR))  // Method doesn't exist!

// CORRECT:
if(G.Set(tbrin()).exist(PROP_CIR))  // Call exist() on PSet
```

**The pattern**:
1. `G.Set(tbrin())` returns a `PSet&` (property set for brins)
2. `PSet` has an `exist(int property_id)` method
3. Call `exist()` on the returned PSet

**For different property types**:
```cpp
// Brin properties (cir, acir, vin)
if(G.Set(tbrin()).exist(PROP_CIR))

// Vertex properties (coordinates, colors)
if(G.Set(tvertex()).exist(PROP_COORD))

// Edge properties
if(G.Set(tedge()).exist(PROP_LABEL))

// Graph properties (single-value)
if(G.Set().exist(PROP_PLANAR))
```

### 🔍 **How to Find Correct Methods**

When you think a method should exist but get "no member named X":

1. **Check the header**: Look at actual method signatures in graphs.h
2. **Search working code**: `grep -r "methodName" tgraph/`
3. **Look for similar patterns**: How do other functions check properties?
4. **Check base classes**: Graph inherits from GraphAccess, which uses GraphContainer

**Example search**:
```bash
# Find how existing code checks properties
grep -r "PROP_CIR" tgraph/*.cpp

# Find uses of "exist"
grep -r "\.exist(" tgraph/*.cpp
```

### 💥 **Impact**

- Multiple compilation errors in validators
- Affected 8+ functions across graph_comparators.cpp and property_validators.cpp

---

## Mistake #5: Wrong Property Types

### ❌ **The Mistake**

**What I did**:
```cpp
Prop<tcolor> vcolor(G.Set(tvertex()), PROP_VCOLOR);
```

**Compiler errors**:
```
error: 'tcolor' does not name a type
error: 'PROP_VCOLOR' was not declared in this scope
```

**Why**: Assumed types/constants that don't exist in the library.

### ✅ **The Solution**

**Check incl/TAXI/propdef.h for correct types**:

```cpp
// WRONG:
Prop<tcolor> vcolor(G.Set(tvertex()), PROP_VCOLOR);

// CORRECT:
Prop<short> vcolor(G.Set(tvertex()), PROP_COLOR);
```

### 📋 **Correct Property Types**

From `incl/TAXI/propdef.h`:

| Property Constant | Type | Usage |
|------------------|------|-------|
| `PROP_COLOR` | `short` | Vertex/edge colors |
| `PROP_LABEL` | `int` | Vertex/edge labels |
| `PROP_COORD` | `Tpoint` | Vertex coordinates |
| `PROP_VIN` | `tvertex` | Vertex incidence |
| `PROP_CIR` | `tbrin` | Circular order |
| `PROP_ACIR` | `tbrin` | Reverse circular order |
| `PROP_PBRIN` | `tbrin` | First brin at vertex |
| `PROP_PLANAR` | `int` | Planarity flag |

**How to find property types**:
```bash
# Look at property definitions
cat incl/TAXI/propdef.h | grep "define PROP_"

# Find usage examples
grep -r "Prop<.*> .*PROP_COLOR" tgraph/*.cpp
```

### 🔍 **How to Verify Types**

Create a test:
```cpp
#include <Pigale.h>

int main() {
    GraphContainer gc;
    gc.setsize(4, 0);

    // Try each property type
    Prop<short> color(gc.PV(), PROP_COLOR);      // ✓
    Prop<int> label(gc.PV(), PROP_LABEL);         // ✓
    Prop<Tpoint> coord(gc.PV(), PROP_COORD);      // ✓
    Prop<tvertex> vin(gc.PB(), PROP_VIN);         // ✓

    // These would fail:
    // Prop<tcolor> color(gc.PV(), PROP_VCOLOR);  // ✗ No such type/const

    return 0;
}
```

---

## Mistake #6: Pointer vs Reference Confusion

### ❌ **The Mistake**

**What I did**:
```cpp
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);

TopologicalGraph G(gc);  // Pass pointer
```

**Compiler error**:
```
error: no matching function for call to 'TopologicalGraph::TopologicalGraph(GraphContainer*&)'
note: candidate: 'TopologicalGraph::TopologicalGraph(GraphContainer&)'
note:   no known conversion for argument 1 from 'GraphContainer*' to 'GraphContainer&'
```

**Why**: `TopologicalGraph` constructor takes a **reference**, not a pointer.

### ✅ **The Solution**

**Dereference the pointer**:

```cpp
GraphContainer* gc = new GraphContainer();
gc->setsize(4, 0);

TopologicalGraph G(*gc);  // Dereference! ✓
```

**Or use reference directly**:
```cpp
GraphContainer gc;  // Stack allocation
gc.setsize(4, 0);

TopologicalGraph G(gc);  // Pass reference ✓
```

### 📋 **Constructor Signatures**

From `incl/TAXI/graphs.h`:

```cpp
// Graph constructors
Graph(GraphContainer& G);  // Takes REFERENCE
Graph(GraphAccess& G);

// TopologicalGraph constructors
TopologicalGraph(GraphContainer& G);  // Takes REFERENCE
TopologicalGraph(GraphAccess& G);
TopologicalGraph(Graph& G, int);

// GeometricGraph constructors
GeometricGraph(GraphContainer& G);  // Takes REFERENCE
```

**Pattern**: All graph class constructors take **references**, not pointers.

### 💥 **Impact**

- Affected 10+ locations across test files
- Every graph builder function had this error
- Integration tests all failed to compile

---

## Mistake #7: Object Lifetime Issues

### ❌ **The Mistake**

**What I did**:
```cpp
TEST_F(GraphContainerTest, WorksWithTopologicalGraph) {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);

    TopologicalGraph G(*gc);

    EXPECT_EQ(G.nv(), 4);

    delete gc;  // Delete while G still holds reference!
}
// G destructor runs → accesses deleted gc → SEGFAULT
```

**What happens**:
1. `TopologicalGraph G(*gc)` stores a reference to `*gc`
2. `delete gc` deallocates the GraphContainer
3. Test ends, G's destructor runs
4. G tries to access the deleted GraphContainer
5. **Segmentation fault** 💥

**Why this is dangerous**:
- Graph/TopologicalGraph/GeometricGraph store **references** to GraphContainer
- They expect the GraphContainer to outlive them
- Deleting GraphContainer first = dangling reference

### ✅ **The Solution**

**Ensure Graph goes out of scope before deleting GraphContainer**:

```cpp
TEST_F(GraphContainerTest, WorksWithTopologicalGraph) {
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);

    {
        TopologicalGraph G(*gc);
        EXPECT_EQ(G.nv(), 4);
    }  // G destructor runs here - gc still valid ✓

    delete gc;  // Now safe to delete ✓
}
```

**Or use stack allocation**:
```cpp
TEST_F(GraphContainerTest, WorksWithTopologicalGraph) {
    GraphContainer gc;
    gc.setsize(4, 0);

    TopologicalGraph G(gc);  // Both on stack
    EXPECT_EQ(G.nv(), 4);

    // Both destructed in correct order (G first, then gc)
}
```

### 📋 **Lifetime Rules**

1. **GraphContainer must outlive all Graph objects using it**
2. **Graph must outlive all TopologicalGraph objects using it**
3. **Use scope blocks to control destruction order**
4. **Prefer stack allocation when possible**

**Memory management patterns**:

```cpp
// Pattern 1: Stack allocation (safest)
GraphContainer gc;
gc.setsize(n, 0);
TopologicalGraph G(gc);
// Both destructed automatically in correct order

// Pattern 2: Heap with scoping
GraphContainer* gc = new GraphContainer();
{
    TopologicalGraph G(*gc);
    // Use G
}  // G destroyed
delete gc;  // Now safe

// Pattern 3: Return GraphContainer, not Graph
GraphContainer* BuildGraph() {
    GraphContainer* gc = new GraphContainer();
    {
        TopologicalGraph G(*gc);
        // Build graph using G
    }  // G destroyed
    return gc;  // Caller owns gc
}
```

### 💥 **Impact**

- Tests ran but crashed with segfault
- Hard to debug (crash happens in destructor after test "passes")
- Affected multiple unit tests

---

## Mistake #8: PSet vs PSet1 Confusion

### ❌ **The Mistake**

**What I did**:
```cpp
GraphContainer gc;
gc.setsize(4, 0);

Prop<int> prop(gc.Set(), PROP_COLOR);  // gc.Set() returns PSet1
```

**Compiler error**:
```
error: no matching function for call to 'Prop<int>::Prop(PSet1&, int)'
note: candidate: 'Prop<T>::Prop(PSet&, int)'
note:   no known conversion for argument 1 from 'PSet1' to 'PSet&'
```

**Why**:
- `gc.Set()` returns `PSet1` (for single-value graph properties)
- `Prop<T>` constructor needs `PSet` (for per-element properties)
- These are different types!

### ✅ **The Solution**

**Use type-specific Set() methods**:

```cpp
// WRONG:
Prop<int> vprop(gc.Set(), PROP_COLOR);  // PSet1, won't work

// CORRECT:
Prop<int> vprop(gc.Set(tvertex()), PROP_COLOR);  // PSet ✓
```

### 📋 **PSet Access Methods**

**From GraphContainer**:

```cpp
// Per-element properties (returns PSet&)
gc.Set(tvertex())  // Vertex properties
gc.Set(tedge())    // Edge properties
gc.Set(tbrin())    // Brin properties

// Shortcuts (returns PSet&)
gc.PV()  // Vertex properties (same as Set(tvertex()))
gc.PE()  // Edge properties (same as Set(tedge()))
gc.PB()  // Brin properties (same as Set(tbrin()))

// Single-value properties (returns PSet1&)
gc.Set()   // Graph-level properties
gc.PG()    // Graph properties (same as Set())
```

**Usage examples**:

```cpp
// Vertex properties
Prop<Tpoint> vcoord(gc.PV(), PROP_COORD);        // ✓
Prop<int> vcolor(gc.Set(tvertex()), PROP_COLOR); // ✓

// Edge properties
Prop<int> elabel(gc.PE(), PROP_LABEL);           // ✓
Prop<int> ecolor(gc.Set(tedge()), PROP_COLOR);   // ✓

// Brin properties
Prop<tvertex> vin(gc.PB(), PROP_VIN);            // ✓
Prop<tbrin> cir(gc.Set(tbrin()), PROP_CIR);      // ✓

// Single-value properties (use Prop1, not Prop)
Prop1<tstring> title(gc.Set(), PROP_TITRE);      // ✓
Prop1<int> planar(gc.PG(), PROP_PLANAR);         // ✓
```

### 🔍 **How to Remember**

```
If the property is...
├─ Per-vertex: gc.PV() or gc.Set(tvertex())  → Prop<T>
├─ Per-edge:   gc.PE() or gc.Set(tedge())    → Prop<T>
├─ Per-brin:   gc.PB() or gc.Set(tbrin())    → Prop<T>
└─ Graph-wide: gc.PG() or gc.Set()           → Prop1<T>
```

### 💥 **Impact**

- Compilation errors in multiple test files
- Affected property creation in tests
- Confusing error messages

---

## Mistake #9: Const-Correctness Assumptions

### ❌ **The Mistake**

**What I did**:
```cpp
bool ValidateCircularOrder(const TopologicalGraph& G) {
    if(!G.Set(tbrin()).exist(PROP_CIR)) {  // Call non-const method
        return false;
    }
    // ...
}
```

**Compiler error**:
```
error: passing 'const TopologicalGraph' as 'this' argument discards qualifiers
note: in call to 'PSet& GraphAccess::Set(const tbrin&)'
```

**Why**: Most methods on Graph/TopologicalGraph are **non-const**, even if they don't modify the graph. This is a design choice in the library.

### ✅ **The Solution**

**Remove const qualifiers from Graph parameters**:

```cpp
// WRONG:
bool ValidateCircularOrder(const TopologicalGraph& G)

// CORRECT:
bool ValidateCircularOrder(TopologicalGraph& G)  // No const
```

### 📋 **Methods That Are Non-Const**

Most Graph/TopologicalGraph methods are non-const:

```cpp
// All non-const:
G.Set(...)        // Access property sets
G.nv()            // Get vertex count
G.ne()            // Get edge count
G.TestPlanar()    // Test planarity
G.Planarity()     // Compute embedding
// etc...
```

**Why**: The property system allows lazy initialization and caching, which requires non-const access.

### 🔍 **When to Use Const**

```cpp
// DON'T use const for Graph/TopologicalGraph parameters
void ProcessGraph(TopologicalGraph& G);  // ✓

// DO use const for simple types
void ProcessValue(const int& value);     // ✓

// DO use const for data structures you control
void ProcessData(const std::vector<int>& data);  // ✓
```

### 💥 **Impact**

- Compilation errors in all validator functions
- Had to remove const from 10+ function signatures

---

## Mistake #10: GitHub Actions Permissions

### ❌ **The Mistake**

**What I did**: Created GitHub Actions workflow without explicit permissions.

**Error**:
```
Request POST /repos/arne-cl/Pigale/check-runs failed with 403: Forbidden
Resource not accessible by integration
```

**Why**: When using actions that need to create check runs (like `publish-unit-test-result-action`), the workflow needs explicit `checks: write` permission. Without it, the action can't publish test results.

### ✅ **The Solution**

**Add permissions block to workflow**:

```yaml
name: Test Suite

on:
  push:
    branches: [ main, master, 'claude/**' ]
  pull_request:
    branches: [ main, master ]

# ADD THIS:
permissions:
  contents: read
  checks: write
  pull-requests: write

jobs:
  test-linux:
    # ... rest of workflow
```

### 📋 **GitHub Actions Permissions**

**Common permissions needed**:

```yaml
permissions:
  contents: read          # Read repository contents
  checks: write           # Create/update check runs
  pull-requests: write    # Comment on PRs
  statuses: write         # Update commit statuses
  issues: write           # Create/update issues
```

**When you need each**:
- `contents: read` - Almost always (to checkout code)
- `checks: write` - When publishing test results, creating check runs
- `pull-requests: write` - When commenting on PRs with results
- `statuses: write` - When updating commit status
- `issues: write` - When creating issues from failures

### 🔍 **How to Debug Permission Issues**

When you see 403 Forbidden errors:

1. **Check workflow permissions**: Add explicit permissions block
2. **Check action requirements**: Read action documentation for required permissions
3. **Check organization settings**: Ensure organization allows required permissions
4. **Test with minimal permissions**: Start with read-only, add permissions as needed

**Common 403 error patterns**:
```
"Resource not accessible by integration" → Missing checks: write
"Not Found" (on public repo) → Missing contents: read
"Forbidden" (on PR comment) → Missing pull-requests: write
```

### 💥 **Impact**

- GitHub Actions workflow completed but couldn't publish results
- Test results not visible in PR/commit checks
- Had to manually check build logs to see test pass/fail

---

## Best Practices

### 1. Study Working Code First

**Before implementing any feature**:

```bash
# Find examples of what you want to do
cd Pigale
grep -r "NewEdge" tgraph/*.cpp
grep -r "Planarity" tgraph/*.cpp

# Study the example program
cat UsingTgraph/main.cpp

# Study library implementations
cat tgraph/Generate.cpp  # Graph generation
cat tgraph/Planarity.cpp # Planarity testing
```

**Create a learning test**:
```cpp
// test_learn_api.cpp
#include <Pigale.h>
#include <iostream>

int main() {
    // Test specific API behavior
    GraphContainer gc;
    gc.setsize(4, 0);

    TopologicalGraph G(gc);
    tedge e = G.NewEdge(tvertex(1), tvertex(2));

    std::cout << "Created edge: " << e() << std::endl;
    std::cout << "Graph now has: " << G.ne() << " edges" << std::endl;

    return 0;
}
```

### 2. Compile Early and Often

**Don't write 1000+ lines before compiling**:

```bash
# Write one function
vim graph_builders.cpp

# Compile immediately
g++ -c graph_builders.cpp -I../incl

# Fix errors
# Repeat
```

**Incremental development**:
1. Write one helper function
2. Compile it
3. Test it
4. Write next function
5. Repeat

### 3. Use Test-Driven Development

**Write tests that verify API understanding**:

```cpp
// First, verify you understand the API
TEST(APIVerification, NewEdgeCreatesSequentialEdges) {
    GraphContainer gc;
    gc.setsize(4, 0);
    TopologicalGraph G(gc);

    tedge e1 = G.NewEdge(tvertex(1), tvertex(2));
    EXPECT_EQ(e1(), 1);  // Should create edge 1

    tedge e2 = G.NewEdge(tvertex(2), tvertex(3));
    EXPECT_EQ(e2(), 2);  // Should create edge 2
}
```

**Then implement using verified patterns**:
```cpp
GraphContainer* BuildK4() {
    // Now confident this is correct
    GraphContainer* gc = new GraphContainer();
    gc->setsize(4, 0);  // Verified this is needed

    TopologicalGraph G(*gc);
    G.NewEdge(tvertex(1), tvertex(2));  // Verified this works
    // ...
    return gc;
}
```

### 4. Check Method Existence

**Before assuming a method exists**:

```bash
# Search headers
grep "methodName" incl/TAXI/*.h

# Search implementations
grep "methodName" tgraph/*.cpp

# If not found, find similar patterns
grep "similar" tgraph/*.cpp
```

**Example**:
```bash
# Want to check if property exists
# Is there an Exist() method?
grep "Exist" incl/TAXI/graphs.h  # Not found

# How do others check property existence?
grep -r "exist" tgraph/*.cpp
# Found: Set(...).exist(PROP_...)
```

### 5. Understand Object Lifetimes

**Always ensure GraphContainer outlives Graph**:

```cpp
// SAFE: Stack allocation
void SafeFunction() {
    GraphContainer gc;
    TopologicalGraph G(gc);
    // Both destructed in correct order
}

// SAFE: Scoped heap allocation
void SafeHeapFunction() {
    GraphContainer* gc = new GraphContainer();
    {
        TopologicalGraph G(*gc);
        // Use G
    }  // G destroyed first
    delete gc;  // Then gc
}

// DANGEROUS: Return Graph by value
TopologicalGraph DangerousFunction() {
    GraphContainer gc;
    TopologicalGraph G(gc);
    return G;  // G holds reference to local gc - DANGLING!
}

// SAFE: Return GraphContainer
GraphContainer* SafeFunction() {
    GraphContainer* gc = new GraphContainer();
    {
        TopologicalGraph G(*gc);
        // Build graph
    }
    return gc;  // Caller owns gc
}
```

### 6. Use Existing Members

**Check what Graph/TopologicalGraph already provides**:

```cpp
// DON'T create duplicates
Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);  // ✗

// DO use existing members
tbrin next = G.cir[b];  // ✓ G already has cir

// Check what exists:
// - Graph: vin
// - TopologicalGraph: cir, acir, pbrin, extbrin, planarMap
// - GeometricGraph: (check header)
```

### 7. Document Surprises

**When you discover surprising behavior, document it**:

```cpp
// DON'T just fix and move on
gc->setsize(4, 0);  // Fixed: was 6

// DO document why
gc->setsize(4, 0);  // IMPORTANT: Must be 0 when using NewEdge()
                     // NewEdge() creates NEW edges beyond current count

// Even better: Add to guide
// See DEVELOPER_GUIDE.md#mistake-1-the-newedge-trap
```

### 8. Verify Types from Headers

**Don't assume property types**:

```bash
# Check property definitions
cat incl/TAXI/propdef.h | grep PROP_COLOR
# #define PROP_COLOR 1

# Find type in usage
grep "Prop<.*>.*PROP_COLOR" tgraph/*.cpp
# Prop<short> vcolor(G.Set(tvertex()), PROP_COLOR);
```

**Create type verification test**:
```cpp
TEST(TypeVerification, PropertyTypes) {
    GraphContainer gc;
    gc.setsize(4, 0);

    // Verify each type compiles
    Prop<short> color(gc.PV(), PROP_COLOR);      // ✓
    Prop<int> label(gc.PV(), PROP_LABEL);         // ✓
    Prop<Tpoint> coord(gc.PV(), PROP_COORD);      // ✓
    Prop<tvertex> vin(gc.PB(), PROP_VIN);         // ✓

    SUCCEED();
}
```

---

## Code Review Checklist

Before submitting code that uses Pigale/tgraph API:

### ✅ **Graph Construction**

- [ ] Using `setsize(n, 0)` when using NewEdge()?
- [ ] Using `setsize(n, m)` with manual property setup if not using NewEdge?
- [ ] Dereferencing pointers: `TopologicalGraph G(*gc)` not `G(gc)`?

### ✅ **Property Access**

- [ ] Using existing members (G.cir, G.vin) instead of creating new Prop?
- [ ] Using correct PSet access: `gc.PV()` or `gc.Set(tvertex())` for vertex properties?
- [ ] Using `Prop<T>` for per-element properties?
- [ ] Using `Prop1<T>` for single-value properties?
- [ ] Property types match propdef.h definitions?

### ✅ **Method Calls**

- [ ] Using `G.Set(...).exist(PROP_X)` not `G.Exist(PROP_X)`?
- [ ] No const on Graph/TopologicalGraph parameters?
- [ ] All method calls actually exist (checked headers/examples)?

### ✅ **Memory Management**

- [ ] GraphContainer outlives all Graph objects using it?
- [ ] Using scope blocks to control destruction order?
- [ ] Not returning Graph objects that reference local GraphContainer?

### ✅ **Testing**

- [ ] Studied working examples before implementing?
- [ ] Created small verification tests?
- [ ] Compiled early and often?
- [ ] Actually ran tests, not just assumed they work?

### ✅ **GitHub Actions**

- [ ] Added permissions block to workflow?
- [ ] Tested workflow on actual commit?
- [ ] Checked test results published correctly?

---

## Summary

### Top 3 Mistakes to Avoid

1. **NewEdge() requires `setsize(n, 0)`** - Most critical, hardest to debug
2. **Never guess API behavior** - Always study working examples first
3. **GraphContainer must outlive Graph** - Memory safety issue

### Top 3 Best Practices

1. **Study working code** (UsingTgraph/main.cpp, tgraph/*.cpp) before implementing
2. **Compile early and often** - Don't write 1000+ lines before testing
3. **Create verification tests** - Test API understanding before implementing

### Key Resources

- **Working Examples**: `UsingTgraph/main.cpp`, `tgraph/Generate.cpp`
- **Headers**: `incl/TAXI/graphs.h`, `incl/TAXI/propdef.h`
- **API Patterns**: `tests/CORRECT_API_PATTERNS.md`
- **Lessons Learned**: `tests/API_LESSONS_LEARNED.md`

### Timeline

- **Initial Implementation**: 2-3 hours (all wrong)
- **First Fixes**: 1 hour (still wrong)
- **Studying Working Code**: 2 hours (learned correct patterns)
- **Complete Rewrite**: 3 hours (fixed everything)
- **Final Discovery (NewEdge)**: 1 hour (enabled 100% pass rate)
- **Total Time**: ~9 hours (could have been 3 hours with this guide)

---

## Contributing to This Guide

If you discover new mistakes or API quirks:

1. **Document the mistake** with before/after examples
2. **Explain why it happened** and how to discover it
3. **Provide the solution** with clear code examples
4. **Estimate impact** (how many files affected, time to debug)
5. **Add to checklist** if it's a common error

**This guide should grow** as we discover more patterns and pitfalls in the Pigale/tgraph API.

---

**Last Updated**: 2025-11-05
**Status**: Phase 1 Complete - 27/27 Tests Passing
**Next Update**: After Phase 2 implementation
