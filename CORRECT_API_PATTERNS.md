# Correct API Usage Patterns for Pigale tgraph

**Learned from**: UsingTgraph/main.cpp and tgraph source code
**Date**: 2025-11-05

This document contains proven, working patterns extracted from actual code.

---

## 1. Graph Container Creation

### Pattern 1: Creating a GraphContainer

**Correct Pattern** (from Generate.cpp):
```cpp
GraphContainer &GC = *new GraphContainer;
GC.setsize(n, m);
// ... use GC
return &GC;  // Return pointer
```

**Usage Pattern** (from UsingTgraph/main.cpp):
```cpp
GraphContainer GC;  // Stack allocation (for local use)
GC.setsize(n, m);
```

**Key Point**: Can be stack or heap allocated. Generators return pointers.

---

## 2. TopologicalGraph Construction

### From GraphContainer

**Correct**:
```cpp
// If you have a reference:
GraphContainer GC;
TopologicalGraph G(GC);  // Pass by reference

// If you have a pointer:
GraphContainer* gc = GenerateGrid(5, 5);
TopologicalGraph G(*gc);  // Dereference!

// From UsingTgraph/main.cpp:
GraphContainer GC;
GC.setsize(n,m);
// ... set up edges
TopologicalGraph G(GC);  // Reference
```

**Wrong**:
```cpp
GraphContainer* gc = new GraphContainer();
TopologicalGraph G(gc);  // ERROR: expects reference, not pointer
```

---

## 3. Property Creation

### Understanding PSet Access

GraphContainer has two ways to get PSets:

**On GraphContainer** (direct access):
```cpp
GraphContainer GC;
PSet& PV = GC.PV();  // Vertex properties
PSet& PE = GC.PE();  // Edge properties
PSet& PB = GC.PB();  // Brin properties
PSet1& PG = GC.Set();  // General (single-value) properties
```

**On GraphAccess/Graph/TopologicalGraph** (type-based):
```cpp
TopologicalGraph G(...);
PSet& pv = G.Set(tvertex());  // Vertex properties
PSet& pe = G.Set(tedge());    // Edge properties
PSet& pb = G.Set(tbrin());    // Brin properties
PSet1& pg = G.Set();          // General properties
```

### Creating Prop Objects

**From Generate.cpp** (correct pattern):
```cpp
GraphContainer GC;
GC.setsize(n, m);

// Brin properties use PB():
Prop<tvertex> vin(GC.PB(), PROP_VIN);
Prop<tbrin> cir(GC.PB(), PROP_CIR);
Prop<tbrin> acir(GC.PB(), PROP_ACIR);

// Vertex properties use PV():
Prop<Tpoint> vcoord(GC.PV(), PROP_COORD);
Prop<int> vlabel(GC.PV(), PROP_LABEL);
Prop<tbrin> pbrin(GC.PV(), PROP_PBRIN);

// Edge properties use PE():
Prop<int> elabel(GC.PE(), PROP_LABEL);

// General (single-value) properties use Set():
Prop1<tstring> title(GC.Set(), PROP_TITRE);
Prop1<int> planarMap(GC.Set(), PROP_PLANARMAP);
```

**From UsingTgraph/main.cpp** (on TopologicalGraph):
```cpp
TopologicalGraph G(GC);

// Create vin property:
Prop<tvertex> vin(GC.Set(tbrin()), PROP_VIN);
// Equivalent to: Prop<tvertex> vin(GC.PB(), PROP_VIN);
```

**Initialization Idiom**:
```cpp
Prop<tvertex> vin(GC.PB(), PROP_VIN);
vin[0] = 0;  // Initialize index 0
```

---

## 4. Using Existing Graph Properties

### TopologicalGraph Has Public Members!

**From graphs.h**:
```cpp
class TopologicalGraph : public Graph {
public:
    Prop<tbrin> cir;    // Circular order
    Prop<tbrin> acir;   // Anti-circular order
    Prop<tbrin> pbrin;  // First brin per vertex
    Prop1<tbrin> extbrin;  // Exterior brin
    Prop1<int> planarMap;  // Is planar map
    // ...
};
```

**Correct Usage** (from UsingTgraph/main.cpp):
```cpp
TopologicalGraph G(GC);

// DON'T create new Prop objects, use existing ones:
tbrin first = G.pbrin[v];  // Use G.pbrin directly
tbrin b = G.cir[b];        // Use G.cir directly

// DON'T do this:
Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);  // WRONG - G already has cir!
```

**Graph Class Has vin**:
```cpp
class Graph : public GraphAccess {
public:
    Prop<tvertex> vin;  // Vertex incidence
};
```

**Usage**:
```cpp
TopologicalGraph G(GC);
tvertex v1 = G.vin[e];   // Use G.vin directly
tvertex v2 = G.vin[-e];  // Negative for other endpoint
```

---

## 5. Property Existence Checking

### Correct Pattern

**On PSet** (lowercase `exist`):
```cpp
// From TopoAlgs.cpp:
if (Set().exist(PROP_BICONNECTED))
    return true;

if (Set(tvertex()).exist(PROP_COORD))
    // Has coordinates

if (Set(tedge()).exist(PROP_MULTIPLICITY))
    // Has multiplicity property
```

**Wrong**:
```cpp
if (G.Exist(PROP_CIR))  // ERROR: No Exist() method on Graph classes
```

**Correct Full Examples**:
```cpp
TopologicalGraph G(GC);

// Check general property:
if (G.Set().exist(PROP_BICONNECTED)) { ... }

// Check vertex property:
if (G.Set(tvertex()).exist(PROP_COORD)) { ... }

// Check edge property:
if (G.Set(tedge()).exist(PROP_MULTIPLICITY)) { ... }

// Check brin property:
if (G.Set(tbrin()).exist(PROP_CIR)) { ... }
```

---

## 6. Iterating Through Graphs

### Vertices and Edges

**From UsingTgraph/main.cpp**:
```cpp
TopologicalGraph G(GC);

// Iterate vertices (1-based):
for(tvertex v = 1; v <= G.nv(); v++) {
    // Use v
    cout << v();  // v() gets the int value
}

// Iterate edges (1-based):
for(tedge e = 1; e <= G.ne(); e++) {
    cout << e() << " = [" << G.vin[e] << "," << G.vin[-e] << "]";
}
```

### Circular Order Around Vertex

**From UsingTgraph/main.cpp** (correct pattern):
```cpp
// Get first brin at vertex:
tbrin first = G.pbrin[v];
tbrin b = first;

// Loop through all incident brins:
do {
    cout << b() << " ";  // Print brin
    cout << G.vin[-b]() << " ";  // Print opposite vertex
    b = G.cir[b];  // Next in circular order
} while(b != first);
```

**From tgraph source**:
```cpp
// Standard idiom:
tbrin b0 = G.pbrin[v];
tbrin b = b0;
do {
    // Process b
} while((b = G.cir[b]) != b0);
```

---

## 7. Creating Edges

### NewEdge Methods

**Signatures** (from graphs.h):
```cpp
// Insert edge between two vertices (arbitrary order):
tedge NewEdge(const tvertex &v1, const tvertex &v2, tedge e0=0);

// Insert edge between two brins (specific position in circular order):
tedge NewEdge(const tbrin &ref1, const tbrin &ref2);
```

**Usage from UsingTgraph/main.cpp**:
```cpp
// Manual edge creation using vin:
Prop<tvertex> vin(GC.Set(tbrin()), PROP_VIN);
vin[1] = 1; vin[-1] = 2;  // Edge 1 from vertex 1 to 2
vin[2] = 1; vin[-2] = 3;  // Edge 2 from vertex 1 to 3
```

**Using NewEdge** (from tgraph source):
```cpp
// Between vertices:
G.NewEdge(tvertex(1), tvertex(2));

// Between brins (preserving circular order):
G.NewEdge(b1, G.cir[b2]);  // Insert after b1, before cir[b2]
```

---

## 8. Planarity Testing

### TestPlanar vs Planarity

**From UsingTgraph/main.cpp**:
```cpp
// Planarity() returns 0 for non-planar, non-zero for planar:
if(G.Planarity() == 0) {
    cout << "not planar" << endl;
} else {
    cout << "planar graph" << endl;
    // G now has planar embedding in cir/acir/pbrin
}
```

**From BipPlanarize.cpp**:
```cpp
// TestPlanar() returns bool:
if (G.TestPlanar()) {
    // Planar
} else {
    // Non-planar
}
```

**Key Difference**:
- `Planarity()`: Computes embedding, returns 0 or non-zero
- `TestPlanar()`: Just tests, returns bool

---

## 9. Type Conversions

### Getting Integer Values

**Pattern**:
```cpp
tvertex v = 5;
int n = v();  // Get int value

tedge e = 3;
int m = e();  // Get int value

// In output:
cout << v() << endl;  // Print as int
```

### Vector Indexing

**Wrong**:
```cpp
std::vector<int> data;
data[v];  // ERROR: can't index with tvertex
```

**Correct**:
```cpp
std::vector<int> data;
data[v()];  // Use v() to get int index
```

---

## 10. Const-Correctness

### Important: Most Methods Are Non-Const!

**From compilation errors**:
```cpp
// This DOESN'T work:
void MyFunction(const TopologicalGraph& G) {
    G.Set(...);  // ERROR: Set() is non-const
    G.nv();      // OK: nv() is const
}
```

**Correct**:
```cpp
// Don't use const for Graph classes:
void MyFunction(TopologicalGraph& G) {
    G.Set(...);  // OK
    Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);  // OK
}
```

**Exception**: Getters like `nv()`, `ne()` are const.

---

## 11. Complete Working Example

### Building and Testing a Graph

**From UsingTgraph/main.cpp** (full pattern):
```cpp
#include <Pigale.h>

int main() {
    // 1. Create container
    GraphContainer GC;
    GC.setsize(4, 5);  // 4 vertices, 5 edges

    // 2. Create vin property
    Prop<tvertex> vin(GC.Set(tbrin()), PROP_VIN);

    // 3. Set up edges manually
    vin[1] = 1; vin[-1] = 2;  // Edge 1: v1-v2
    vin[2] = 1; vin[-2] = 3;  // Edge 2: v1-v3
    vin[3] = 2; vin[-3] = 3;  // Edge 3: v2-v3
    vin[4] = 3; vin[-4] = 4;  // Edge 4: v3-v4
    vin[5] = 2; vin[-5] = 4;  // Edge 5: v2-v4

    // 4. Create topological graph
    TopologicalGraph G(GC);

    // 5. Print edges using G.vin
    for(tedge e = 1; e <= G.ne(); e++) {
        cout << e() << " = [" << G.vin[e] << "," << G.vin[-e] << "]" << endl;
    }

    // 6. Test planarity and compute embedding
    if(G.Planarity() == 0) {
        cout << "not planar" << endl;
        return -1;
    }

    // 7. Print planar map using G.pbrin and G.cir
    for(tvertex v = 1; v <= G.nv(); v++) {
        cout << v() << " -> ";
        tbrin first = G.pbrin[v];
        tbrin b = first;
        do {
            cout << b() << " ";
        } while((b = G.cir[b]) != first);
        cout << endl;
    }

    return 0;
}
```

---

## 12. Generator Functions

### Using GenerateGrid, etc.

**From Pigale.h** (signatures):
```cpp
GraphContainer *GenerateGrid(int a, int b);
GraphContainer *GenerateCompleteGraph(int a);
GraphContainer *GenerateCompleteBiGraph(int a, int b);
GraphContainer *GenerateRandomGraph(int a, int b);
```

**Correct Usage**:
```cpp
// Get pointer from generator:
GraphContainer* gc = GenerateGrid(10, 10);

// Use with TopologicalGraph (dereference):
TopologicalGraph G(*gc);

// Test and use:
if (G.Planarity() != 0) {
    // Process planar embedding
}

// Clean up:
delete gc;
```

---

## 13. Common Mistakes to Avoid

### Mistake 1: Creating Duplicate Properties

**Wrong**:
```cpp
TopologicalGraph G(GC);
Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);  // G already has cir!
```

**Right**:
```cpp
TopologicalGraph G(GC);
// Just use G.cir directly:
tbrin next = G.cir[b];
```

### Mistake 2: Wrong Exist() Call

**Wrong**:
```cpp
if (G.Exist(PROP_CIR))  // No such method
```

**Right**:
```cpp
if (G.Set(tbrin()).exist(PROP_CIR))  // exist() on PSet
```

### Mistake 3: Using Const

**Wrong**:
```cpp
void Process(const TopologicalGraph& G)  // Most methods non-const!
```

**Right**:
```cpp
void Process(TopologicalGraph& G)  // No const
```

### Mistake 4: Pointer vs Reference

**Wrong**:
```cpp
GraphContainer* gc = GenerateGrid(5, 5);
TopologicalGraph G(gc);  // Expects reference!
```

**Right**:
```cpp
GraphContainer* gc = GenerateGrid(5, 5);
TopologicalGraph G(*gc);  // Dereference
```

### Mistake 5: Vector Indexing

**Wrong**:
```cpp
std::vector<int> data;
data[v];  // v is tvertex, not int
```

**Right**:
```cpp
data[v()];  // Get int value
```

---

## 14. Summary Checklist

When writing code using tgraph:

- [ ] Use `GC.PV()`, `GC.PE()`, `GC.PB()` for properties
- [ ] Use `G.Set(tvertex())`, `G.Set(tedge())`, `G.Set(tbrin())` on GraphAccess
- [ ] Use existing `G.vin`, `G.cir`, `G.acir`, `G.pbrin` members
- [ ] Check properties with `G.Set(...).exist(PROP_*)`
- [ ] Dereference pointers when constructing: `TopologicalGraph G(*gc)`
- [ ] Don't use `const` on Graph/TopologicalGraph parameters
- [ ] Use `v()`, `e()`, `b()` to get int values
- [ ] Initialize arrays: `vin[0] = 0`
- [ ] Iterate: `for(tvertex v = 1; v <= G.nv(); v++)`
- [ ] Circular order: `do { ... } while((b = G.cir[b]) != first)`

---

## References

- **UsingTgraph/main.cpp**: Basic usage example
- **tgraph/Generate.cpp**: Graph construction patterns
- **tgraph/TopoAlgs.cpp**: Property checking patterns
- **incl/TAXI/graphs.h**: Class definitions and public members
- **incl/TAXI/graph.h**: GraphAccess and GraphContainer

---

**Next**: Apply these patterns to rewrite test helpers correctly.
