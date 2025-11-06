# TGF I/O Bug: Complete Analysis

**Date**: 2025-11-06
**Investigation**: Deep analysis of why NewEdge() graphs lose edges during save/load

---

## Executive Summary

The TGF I/O bug is **NOT** caused by NewEdge() vs manual construction differences. The real root cause is:

**SaveGraphTgf() assumes PROP_LABEL exists for vertices but doesn't initialize it if missing. NewEdge() doesn't create PROP_LABEL, so uninitialized garbage values are written to the file, corrupting the vertex-to-label mapping on load.**

---

## Key Findings

### 1. Vertex 0 and vin[0]

- **vin[0]** is explicitly set to **vertex 0** in both save and load operations
- This is a special marker/sentinel value
- Library-generated graphs initialize `vlabel[0] = 0`
- ReadTgfGraph explicitly sets `vin[0] = 0` after reading edges (line 200)

### 2. The PROP_LABEL Property

**Library-generated graphs** (e.g., GenerateGrid):
```cpp
Prop<int> vlabel(GC.PV(),PROP_LABEL);
for (v=0; v<=n; v++)
    vlabel[v]=v();  // Explicitly initialize ALL vertices including 0
```

**NewEdge()-created graphs**:
- PROP_LABEL is **NEVER** created or initialized
- Vertices have no labels

### 3. SaveGraphTgf Bug (File.cpp:316-331)

```cpp
// Line 316: Creates vlabel but doesn't initialize if it doesn't exist!
Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);

// Lines 324-329: Uses vlabel to write edge list
for(i=1;i<=m;i++)
    {elist[i].v1 = vlabel[vin[i]];   // GARBAGE if vlabel not initialized!
    elist[i].v2 = vlabel[vin[-i]];   // GARBAGE if vlabel not initialized!
    elist[i].color = (LongInt)ecolor[i];
    elist[i].width = (LongInt)ewidth[i];
    }
```

**The Problem**:
- If PROP_LABEL doesn't exist, creating a Prop object gives it **uninitialized memory**
- SaveGraphTgf writes edges using `vlabel[vin[i]]` without checking if vlabel is initialized
- For NewEdge graphs, this writes **garbage labels** to the file

### 4. ReadTgfGraph Corruption (File.cpp:176-200)

```cpp
// Read edge structures from file
e_struct *es = new e_struct[m+1];
file.FieldRead(TAG_ELIST, (char *)&es[1]);

// Build label-to-index mapping from edges
for (j = 1,i=1; j <= m; j++)
    {if (es[j].v1 == es[j].v2){++nloops;continue;}

    // Create mapping from label to vertex index
    if (map_label.ExistingIndexByKey(es[j].v1) < 0)
        {vlabel[i]=es[j].v1;  // es[j].v1 is GARBAGE from save!
        map_label[es[j].v1]=i++;
        }
    if (map_label.ExistingIndexByKey(es[j].v2) < 0)
        {vlabel[i]=es[j].v2;  // es[j].v2 is GARBAGE from save!
        map_label[es[j].v2]=i++;
        }

    vin[j] = map_label[es[j].v1];
    vin[-j] = map_label[es[j].v2];
    }
vin[0]=0;  // Explicitly set vin[0]
```

**The Problem**:
- ReadTgfGraph tries to build a mapping from vertex labels to indices
- If labels are garbage from save, the mapping is corrupted
- Edges end up pointing to wrong vertices or get lost entirely

---

## Demonstration

### Test Program Output

**Library-generated Grid** (works correctly):
```
PROP_LABEL exists for vertices
Vertex labels:
  vertex 1: label=1
  vertex 2: label=2
  vertex 3: label=3
  vertex 4: label=4
```

**NewEdge-created Cycle** (broken):
```
PROP_LABEL does NOT exist for vertices
Created PROP_LABEL. Default values:
  vertex 0: label=-1577671682  <- GARBAGE!
  vertex 1: label=21853         <- GARBAGE!
  vertex 2: label=0
  vertex 3: label=0
  vertex 4: label=4
```

### Edge Corruption Example

**Before save** (NewEdge cycle, 4 edges):
```
Edge 1: vin[1]=1, vin[-1]=2
Edge 2: vin[2]=2, vin[-2]=3
Edge 3: vin[3]=3, vin[-3]=4
Edge 4: vin[4]=4, vin[-4]=1
```

**After load** (3 edges, one lost, one corrupted):
```
Edge 1: vin[1]=1, vin[-1]=2
Edge 2: vin[2]=-3, vin[-2]=0  <- CORRUPTED! vertex 0 appears!
Edge 3: vin[3]=2, vin[-3]=3
Edge 4: MISSING!
```

---

## Root Cause Analysis

The bug has TWO components:

### 1. SaveGraphTgf Doesn't Initialize PROP_LABEL

**File**: `tgraph/File.cpp`, line 316

**Problem**:
```cpp
Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);  // No default value!
```

This should be:
```cpp
Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);
// Check if vlabel was just created and initialize it
if (!G.Set(tvertex()).exist(PROP_LABEL)) {
    for (int i = 0; i <= G.nv(); i++) {
        vlabel[i] = i;
    }
}
```

Or even better, use a default value:
```cpp
Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL, 0);  // Default to vertex index
// Then initialize
for (int i = 1; i <= G.nv(); i++) {
    if (vlabel[i] == 0) vlabel[i] = i;
}
```

### 2. NewEdge() Doesn't Create PROP_LABEL

**File**: `tgraph/Graph.cpp`, line 177-200

NewEdge() creates edges but never initializes PROP_LABEL for vertices. Library generation functions (like GenerateGrid) do this manually, but NewEdge() doesn't.

**Options**:
1. Make NewEdge() initialize PROP_LABEL when adding to vertices without labels
2. Make SaveGraphTgf defensive and initialize vlabel if missing
3. Document that users must call a function to initialize labels before saving

---

## Why Library Graphs Work

Library generation functions like `GenerateGrid()` explicitly initialize PROP_LABEL:

**From `tgraph/Generate.cpp`**:
```cpp
GraphContainer *GenerateGrid(int a, int b)
  {// ...
  Prop<int> vlabel(GC.PV(),PROP_LABEL);
  // ...
  tvertex v;
  for (v=0; v<=n; v++)
    vlabel[v]=v();  // EXPLICIT INITIALIZATION including vertex 0!
  // ...
  }
```

This is the pattern ALL library generation functions follow:
- GenerateGrid
- GenerateCompleteGraph
- GenerateRandomGraph
- etc.

They all explicitly initialize vlabel to vertex indices.

---

## Why NewEdge() Graphs Fail

NewEdge() in `tgraph/Graph.cpp` does:
```cpp
tedge TopologicalGraph::NewEdge(const tvertex &vv1,const tvertex &vv2,tedge )
  {tedge e;
  tvertex v1=vv1, v2=vv2;
  incsize(e);
  tbrin b = ne();
  InsertBrin(v1,b);
  InsertBrin(v2,-b);
  // ... property cleanup ...
  return ne();
  }
```

**What's missing**:
- No initialization of PROP_LABEL for vertices
- No initialization of vertex-related properties

Users are expected to either:
1. Use library generation functions, OR
2. Manually initialize properties before saving

But this is **not documented** and **not enforced**.

---

## The Meaning of Vertex 0

From the investigation:

1. **vin[0]** is a special entry that maps to **vertex 0**
2. Vertex 0 appears to be a **sentinel/invalid vertex marker**
3. Library functions initialize `vlabel[0] = 0`
4. ReadTgfGraph explicitly sets `vin[0] = 0` after loading edges
5. In brin indexing:
   - `vin[1..m]` = positive brins (edge i, first endpoint)
   - `vin[-1..-m]` = negative brins (edge i, second endpoint)
   - `vin[0]` = special marker for "no edge" or "invalid"

When ReadTgfGraph sets a vertex to 0, it indicates an error or invalid edge endpoint.

---

## Suggested Fixes

### Option 1: Fix SaveGraphTgf (Defensive)

**File**: `tgraph/File.cpp`, line 316

```cpp
Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);

// Initialize vlabel if it doesn't exist or has invalid values
bool need_init = !G.Set(tvertex()).exist(PROP_LABEL);
if (!need_init) {
    // Check if any labels are uninitialized (0 or garbage)
    for (int i = 1; i <= G.nv(); i++) {
        if (vlabel[i] == 0) {
            need_init = true;
            break;
        }
    }
}

if (need_init) {
    // Initialize to vertex indices
    vlabel[0] = 0;  // vertex 0 sentinel
    for (int i = 1; i <= G.nv(); i++) {
        vlabel[i] = i;
    }
}
```

### Option 2: Make NewEdge() Initialize Properties

**File**: `tgraph/Graph.cpp`, line 177

Add property initialization when vertices are first used:
```cpp
tedge TopologicalGraph::NewEdge(const tvertex &vv1,const tvertex &vv2,tedge )
  {tedge e;
  tvertex v1=vv1, v2=vv2;

  // Initialize vertex labels if not already set
  if (!Set(tvertex()).exist(PROP_LABEL)) {
      Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
      vlabel[0] = 0;
      for (int i = 1; i <= nv(); i++) {
          vlabel[i] = i;
      }
  }

  incsize(e);
  // ... rest of function
  }
```

### Option 3: Add Explicit Initialization Function

Add a new public function:
```cpp
void TopologicalGraph::InitializeLabels()
  {Prop<int> vlabel(Set(tvertex()),PROP_LABEL);
  Prop<int> elabel(Set(tedge()),PROP_LABEL);

  vlabel[0] = 0;
  for (int i = 1; i <= nv(); i++) {
      vlabel[i] = i;
  }

  for (int i = 1; i <= ne(); i++) {
      elabel[i] = i;
  }
  }
```

And document that users must call this before saving NewEdge() graphs.

---

## Test Programs

Created verification programs:
- `tests/debug_vin0.cpp` - Demonstrates vin[0] and edge corruption
- `tests/debug_vlabel.cpp` - Shows PROP_LABEL missing in NewEdge graphs
- `tests/debug_io.cpp` - Original edge loss test

All programs demonstrate the bug and prove the root cause.

---

## Conclusion

**The bug report was partially correct**:
- ✅ NewEdge() graphs do lose edges during I/O
- ✅ Library-generated graphs work correctly
- ⚠️ But the root cause is **missing PROP_LABEL initialization**, not inherent incompatibility

**The fix is straightforward**:
- Option 1 (recommended): Make SaveGraphTgf defensive and initialize vlabel if missing
- Option 2: Make NewEdge() initialize PROP_LABEL automatically
- Option 3: Document the requirement and provide an initialization function

**Impact**:
- This is a simple fix that doesn't require changing the file format
- Backward compatible - existing TGF files will still work
- Forward compatible - fixed code can save both types of graphs

The library has a design assumption that properties are pre-initialized by generation functions, but this assumption breaks for user code using the public NewEdge() API.
