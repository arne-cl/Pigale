# Pigale TGF Python Port - FAQ & Gotchas

## Common Errors and Solutions

### Build & Setup Issues

**Q: What Python version is required?**
A: Python 3.9+ recommended. Tested on 3.9, 3.10, 3.11.

**Q: What dependencies are needed?**
A:
```bash
pip install numpy pytest hypothesis
```

---

## TGF Format Gotchas

### Binary Format Issues

**Q: Why is the Coord struct 20 bytes instead of 24?**
A: The C++ code uses `#pragma pack(4)` which forces 4-byte alignment instead of natural 8-byte alignment on 64-bit systems.
```cpp
#pragma pack(4)
struct coord {
    int label;     // 4 bytes
    double x, y;   // 8 + 8 bytes = 20 bytes total (not 24!)
};
```
**Solution**: Use `struct.pack('<idd', label, x, y)` which naturally produces 20 bytes on most systems, or explicitly handle packing.

**Q: Why does data need 4-byte alignment?**
A: The C++ implementation pads all external data blocks (len > 8) to 4-byte boundaries for efficient I/O.
```python
def num_padding(n):
    return (3 - (n+3) % 4)
```

**Q: What byte order does TGF use?**
A: Little-endian (`<` in struct format strings). This matches x86/x64 architecture.

**Q: When is data stored inline vs external?**
A:
- **Inline** (in Field.word): len ≤ 8 bytes
- **External** (at offset): len > 8 bytes

### Property System Issues

**Q: Why is there a 128 property limit?**
A: Hardcoded in C++ (`if (i==128) break;` in PropTgf.cpp:25,33). This is a bug/limitation we must preserve for compatibility.

**Q: What are the SetNum values?**
A:
- SetNum=0: General properties (graph-level) OR Vertex properties
- SetNum=1: Edge properties
- SetNum=2: Brin (half-edge) properties

**Q: How do property tags work?**
A:
```python
MAGIC_PROP = 0x1000
PSetHeaderTag(SetNum) = 0x1000 | (SetNum << 8)
PSetTag(SetNum, PNum) = 0x1000 | 0x400 | (SetNum << 8) | PNum
```

### Version Confusion

**Q: Which TGF version should we support?**
A: **Version 2 only**. This is what Pigale uses by default (SaveGraphTgf with tag=2).

**Q: What's the difference between versions?**
A:
- **Version 0** (legacy): Uses TAG_COORDLAB, TAG_ELIST, etc. Old binary structs.
- **Version 1** (transitional): Property-based but clears PROP_NLOOPS (bug).
- **Version 2** (modern): Clean property-based format. **USE THIS**.

**Q: How to detect version?**
A: Read TAG_VERSION field from first record:
```python
tgf.set_record(1)
version = tgf.field_read(TAG_VERSION, 2)  # Returns short
if version not found: version = 0 (legacy)
```

---

## TXT Format Gotchas

**Q: What's the exact TXT format?**
A:
```
PIG:0
<title up to 80 chars>
<v1> <v2>
<v1> <v2>
...
0 0
```

**Q: Are loops preserved in TXT files?**
A: **NO**. Loops (edges where v1 == v2) are silently skipped during reading (File.cpp:417).

**Q: How are coordinates handled in TXT?**
A: TXT files have **no coordinates**. On read, vertices are arranged in a circle:
```python
angle = 2*pi / nv
for i in range(1, nv+1):
    x = cos(angle * i)
    y = sin(angle * i)
```

**Q: How are vertex labels handled?**
A: TXT files use arbitrary integer labels. A mapping is created:
- `map_label`: label → sequential index (1..n)
- `map_index`: sequential index → original label

Isolated vertices are preserved via the coordinate list.

---

## GraphML Conversion Gotchas

**Q: Does GraphML round-trip preserve all properties?**
A: **YES, but only for direct GraphML → GraphML!**

- **GraphML → GraphML**: 100% property preservation ✓
- **GraphML → TGF → GraphML**: ~92% data loss (TGF can only store Pigale properties)

**Implementation details:**
```python
# GraphML reader stores ALL properties in graph._graphml_data
graph = read_graphml("input.graphml")  # Stores all keys (d0, d1, etc.)

# Direct write preserves everything
write_graphml(graph, "output.graphml")  # All 109K data elements preserved

# But TGF round-trip loses non-Pigale properties
write_tgf_graph(graph, "temp.tgf")      # Only Pigale properties survive
graph2 = read_tgf_graph("temp.tgf")     # _graphml_data is lost
write_graphml(graph2, "output.graphml") # Only structure + coordinates remain
```

**Q: How do we map Pigale properties to GraphML?**
A:
| Pigale Property | GraphML Element |
|-----------------|-----------------|
| PROP_TITRE | `<graph id="...">` |
| PROP_COORD (x,y) | `<data key="x">` `<data key="y">` or `<data key="Pigale/V/16">` |
| PROP_LABEL | `<node id="...">` or `<data key="label">` |
| PROP_VIN | `<edge source="..." target="...">` |
| PROP_COLOR | `<data key="color">` |

**Q: What about generic (non-Pigale) GraphML files?**
A: **Fully supported!** The reader now parses ALL keys, not just "Pigale/*" keys.

Example from real file:
```xml
<key id="d0" for="node" attr.name="atom_id" attr.type="string"/>
<key id="d1" for="node" attr.name="element" attr.type="string"/>
<key id="d5" for="node" attr.name="x" attr.type="double"/>
<key id="d6" for="node" attr.name="y" attr.type="double"/>
```

All these properties are preserved in GraphML → GraphML round-trips!

**Q: What about the rotation system (PROP_CIR)?**
A: GraphML doesn't have native support for rotation systems. We store as custom "Pigale/V/*" keys if present.

**Q: Is PROP_TITRE (graph title) preserved through GraphML?**
A: **NO**. Known limitation:
- TGF → GraphML → TGF: Title is lost
- The GraphML writer hardcodes `<graph id="G">` instead of using PROP_TITRE
- This is acceptable for round-trip tests as title is not part of core graph structure
- Solution: Use `graphs_are_identical(g1, g2, check_title=False)` when testing GraphML round-trips

**Q: Which GraphML dialect to support?**
A: Standard GraphML (http://graphml.graphdrawing.org/). Any valid GraphML file can be read, regardless of key naming.

---

## Graph Model Gotchas

**Q: What's a "brin"?**
A: A half-edge or dart. Each edge e has two brins: e and -e.
```
Edge 1:  brin 1 (source→target), brin -1 (target→source)
```

**Q: What's PROP_VIN?**
A: "Vertex INcidence". Maps each brin to its source vertex.
```python
vin[e] = source_vertex   # Positive brin
vin[-e] = target_vertex  # Negative brin
```

**Q: What's PROP_CIR?**
A: "CIRcular order". Rotation system around each vertex.
```python
cir[b] = next_brin_clockwise_around_vertex
```

**Q: How are graph sizes stored?**
A: In general property set:
```python
graph.general[PROP_N] = nv  # Number of vertices
graph.general[PROP_M] = ne  # Number of edges
```

**Q: Are vertices/edges 0-indexed or 1-indexed?**
A: **1-indexed** in Pigale! Arrays go from 1..n and 1..m.
```python
# PSets use (start, finish) ranges
vertex_pset = PSet(1, nv)    # Indices 1..nv
edge_pset = PSet(1, ne)      # Indices 1..ne
brin_pset = PSet(-ne, ne)    # Indices -ne..ne (excludes 0)
```

---

## Testing Gotchas

**Q: How to create test fixtures?**
A:
1. Use existing files in `tgf/*.tgf` and `tgf/*.txt`
2. Generate new ones with C++ Pigale for reference
3. Create minimal graphs programmatically

**Q: How to compare TGF files byte-for-byte?**
A: Skip the SubHeader (bytes 16-31) which may vary:
```python
def compare_tgf(f1, f2):
    # Compare header (0-15)
    assert read(f1, 16) == read(f2, 16)
    # Skip subheader (16-31)
    f1.seek(32); f2.seek(32)
    # Compare rest
    assert f1.read() == f2.read()
```

**Q: How to compare graphs for semantic equality?**
A: Use the `graphs_are_identical()` helper from `tests/test_binary.py`:
```python
from tests.test_binary import graphs_are_identical

# Compare two GraphContainer objects
graphs_are_identical(graph1, graph2)

# Skip title check for GraphML round-trips (PROP_TITRE not preserved)
graphs_are_identical(graph1, graph2, check_title=False)

# Skip coordinate check if only structure matters
graphs_are_identical(graph1, graph2, check_coords=False)
```

**What it checks:**
- Graph sizes (PROP_N, PROP_M)
- PSet ranges (vertex, edge, brin)
- VIN (vertex incidence) - critical for graph structure
- Vertex labels (PROP_LABEL)
- Coordinates (PROP_COORD) - with floating-point tolerance
- Vertex/edge colors (PROP_COLOR)
- Graph title (PROP_TITRE) - optional
- All other general properties

**Q: Why might round-trips fail?**
A:
1. **Coordinate precision**: Double rounding errors (use allclose tolerance)
2. **Property ordering**: May differ but still valid (compare values, not order)
3. **Padding bytes**: May contain garbage but don't affect data (ignore)
4. **Timestamp/metadata**: SubHeader may change (skip SubHeader)
5. **GraphML title loss**: PROP_TITRE not preserved through GraphML (known limitation)

**Solution**: Use `graphs_are_identical()` to compare graph semantics, not raw bytes.

---

## Performance Notes

**Q: Is Python fast enough for large graphs?**
A: For most use cases, yes. Bottlenecks:
- File I/O: Use buffered reads/writes
- Property arrays: Use numpy (native speed)
- Large graphs (>10k vertices): May need optimization

**Q: How to optimize?**
A:
1. Use numpy for all array operations
2. Read files in chunks, not byte-by-byte
3. Cache IFD offsets for multi-record files
4. Consider Cython for hotspots if needed

---

## Debugging Tips

**Q: How to debug binary format issues?**
A: Use hexdump to inspect files:
```bash
xxd -l 512 file.tgf  # First 512 bytes
```

**Q: How to trace property reading?**
A: Add logging:
```python
import logging
logging.basicConfig(level=logging.DEBUG)

# In read_tgf_pset:
logger.debug(f"Reading property {prop_num}, size={size}")
```

**Q: How to validate graph structure?**
A: Check invariants:
```python
def validate_graph(g):
    assert g.nv == g.general[PROP_N]
    assert g.ne == g.general[PROP_M]

    # Check VIN consistency
    vin = g.brin[PROP_VIN]
    for e in range(1, g.ne + 1):
        assert 1 <= vin[e] <= g.nv
        assert 1 <= vin[-e] <= g.nv
```

---

## C++ to Python Translation

**Q: How to translate C++ file I/O?**
A:
```cpp
// C++
stream.seekg(offset, ios::beg);
stream.read((char*)&data, size);

# Python
f.seek(offset, os.SEEK_SET)
data = f.read(size)
```

**Q: How to translate structs?**
A:
```cpp
// C++
struct Header {
    char ID[4];
    short Version;
    // ...
};

# Python
import struct
header_format = '<4sh...'
data = struct.unpack(header_format, bytes)
```

**Q: How to handle unions (UnionLongWord)?**
A:
```cpp
// C++
union UnionLongWord {
    double d;
    int i;
    long l;
};

# Python - read as raw bytes, interpret as needed
word_bytes = f.read(8)
as_int = struct.unpack('<q', word_bytes)[0]
as_double = struct.unpack('<d', word_bytes)[0]
```

---

## Known Bugs to Preserve

These are bugs in the C++ code that we must replicate for compatibility:

1. **Loop skipping**: Loops silently dropped (no warning)
2. **128 property limit**: Hardcoded, properties 128+ ignored
3. **Version 1 NLOOPS**: Version 1 files erase PROP_NLOOPS on read
4. **Coordinate scaling**: Non-LEDA files rescaled to 90x70 viewport
5. **Padding inconsistency**: `seek` flag usage in Tgf.cpp is confusing

---

## Phase 2 Gotchas

### Struct Format Strings

**Q: What's the correct struct format for TGF Header?**
A: `<4s4Hi` means:
- `<` = little-endian
- `4s` = 4-byte string (ID)
- `4H` = 4 unsigned shorts (Version, IfdNum, RecordNum, LenSubHeader)
- `i` = 1 signed int (FstIfdOffs)

Total: 4 + 2*4 + 4 = 16 bytes

**Q: Why did my struct.pack fail with "expected 7 items (got 6)"?**
A: Initial format was `<4s5Hi` which expects 7 values (4s + 5 shorts + 1 int). Should be `<4s4Hi` (6 values).

**Q: How to debug struct packing issues?**
A:
```python
import struct
# Check expected size
print(struct.calcsize('<4s4Hi'))  # Should be 16

# Test packing
data = struct.pack('<4s4Hi', b'TGF\x00', 1, 4, 4, 16, 32)
print(len(data), data.hex())
```

---

## Phase 2 Days 6-7: Property Serialization Gotchas

### PSet Array Size vs Range

**Q: Why did my dtype detection fail with uint8?**
A: PSet range must match array size! Example error:
```python
# WRONG - mismatch between range and array size
pset = PSet(1, 5)  # Range: 1..5 = 5 elements
arr = np.array([0, 1, 2, 3, 4, 5])  # 6 elements!

# This causes: element_size = 48 bytes / 5 elements = 9.6 bytes
# Result: Falls through to uint8 dtype

# CORRECT - matching range and array size
pset = PSet(0, 5)  # Range: 0..5 = 6 elements
arr = np.array([0, 1, 2, 3, 4, 5])  # 6 elements
# Now: element_size = 48 bytes / 6 elements = 8 bytes → float64
```

**Rule**: Array size must equal `(finish - start + 1)`.

### Property Limit Bug

**Q: Why are only 128 properties written/read?**
A: Hard limit in C++ code (PropTgf.cpp:25, 55):
```cpp
for (int i=X.PStart(); i< X.PEnd(); i++) {
    if (i==128) break;  // HARD LIMIT
    // ...
}
```

This means properties with ID ≥ 128 are silently ignored during I/O.

### Data Type Detection

**Q: How does read_tgf_pset know the dtype?**
A: It guesses based on element size:
- 2 bytes → int16
- 4 bytes → int32 (could also be float32)
- 8 bytes → float64 (could also be int64)
- 20 bytes → Coord struct (5 × float32 with padding)
- Other → uint8 (raw bytes)

**Limitation**: Cannot distinguish int32 from float32, or int64 from float64. Defaults to int32 and float64 respectively.

**Solution**: If you need specific types, store metadata separately or use structured dtypes.

### Empty PSet Handling

**Q: What if PSetHeaderTag is missing?**
A: read_tgf_pset() returns an empty PSet(0, -1):
```python
header_data = tgf.field_read(header_tag, 8)
if not header_data:
    return PSet(0, -1)  # Empty, invalid range
```

This matches the C++ behavior of clearing the PSet when the header is missing.

---

## Phase 3 Day 8-9: TGF Graph Reading Gotchas

### PSet vs PSet1 Tags

**Q: Why are there two types of property tags?**
A: C++ has two property set types:
- **PSet**: Regular property sets for vertex/edge/brin (tag prefix 0x1400)
- **PSet1**: Special property set for general graph properties (tag prefix 0x1800)

**Tag calculation:**
```python
# PSet tags (vertex, edge, brin)
PSetTag(SetNum, PropNum) = 0x1000 | 0x400 | (SetNum<<8) | PropNum

# PSet1 tags (general properties only)
PSet1Tag(SetNum, PropNum) = 0x1000 | 0x800 | (SetNum<<8) | PropNum
```

**SetNum mapping:**
- General: SetNum=0, uses PSet1Tag
- Vertex: SetNum=0, uses PSetTag
- Edge: SetNum=1, uses PSetTag
- Brin: SetNum=2, uses PSetTag

**Key difference:** PSet stores arrays (one value per element), PSet1 stores scalars.

### Scalar vs Array Properties

**Q: Why is graph.general[PROP_N] a scalar but graph.vertex[PROP_COORD] an array?**
A: PSet1 (general) stores each property as a single value:
```python
graph.general[PROP_N] = np.int32(14)  # Scalar
int(graph.general[PROP_N])  # Extract value
```

PSet stores properties as arrays indexed by elements:
```python
graph.vertex[PROP_COORD] = np.array([...])  # Array of 14 elements
graph.vertex[PROP_COORD][5]  # Access vertex 5's coordinate
```

### 1-Indexed Ranges

**Q: Why do vertices start at index 1, not 0?**
A: Pigale uses 1-based indexing (like FORTRAN/MATLAB):
- Vertices: 1..nv (not 0..nv-1)
- Edges: 1..ne (not 0..ne-1)
- Brins: -ne..ne excluding 0 (not -ne+1..ne-1)

**Bug caught**: Original `setsize()` used `resize(0, nv)` creating 0-indexed ranges!
```python
# WRONG
self.vertex.resize(0, nv)  # Creates 0..nv

# CORRECT
self.vertex.resize(1, nv)  # Creates 1..nv
```

### Coordinate Structure Variations

**Q: Why are coordinates sometimes 16 bytes, sometimes 20 bytes?**
A: Depends on #pragma pack() directive:

**20-byte format** (with #pragma pack(4)):
```cpp
struct coord {
    int label;   // 4 bytes
    double x, y; // 8 + 8 = 16 bytes
};  // Total: 20 bytes
```

**16-byte format** (natural alignment or no label):
```cpp
struct coord {
    double x, y; // 8 + 8 = 16 bytes
};  // Total: 16 bytes (no label stored)
```

Our code handles both:
```python
if element_size == 20:
    dtype = np.dtype([('label', '<i4'), ('x', '<f8'), ('y', '<f8')])
elif element_size == 16:
    dtype = np.dtype([('x', '<f8'), ('y', '<f8')])
```

### Header Flushing Issue

**Q: Why wasn't the header being written when I created an empty record?**
A: The `create_record()` function sets `new_ifd=True` but didn't set `new_data=True`.
When `close()` calls `_flush()`, it returns early if `new_data==False`:

```python
def _flush(self):
    if not self.new_data:
        return True  # Early return - header not written!
```

**Fix:** Set both flags in create_record():
```python
self.new_ifd = True
self.new_data = True  # Ensure flush writes header
```

### Version 1 PROP_NLOOPS Erasure

**Q: Why is PROP_NLOOPS erased when reading version 1 files?**
A: Bug preservation from C++ File.cpp:146-147:
```cpp
if(version == 1)  // as PROP_NLOOPS has changed
    G.Set().erase(PROP_NLOOPS);
```

The property definition changed between versions, so old files are sanitized.

---

## Phase 3 Days 10-11: TGF Graph Writing Gotchas

### PSet1 Property Storage Format

**Q: Why did setsize() cause "missing PROP_N/PROP_M" errors when writing?**
A: PSet1 properties must be stored as numpy arrays, not plain Python integers!

**The Problem:**
```python
# WRONG - setsize() stored as plain int
self.general[PROP_N] = nv  # nv is int

# When write_tgf_pset() tries to serialize:
arr = pset.properties[prop_num]
prop_data = arr.tobytes()  # AttributeError: 'int' has no 'tobytes'
```

**The Fix:**
```python
# CORRECT - store as 1-element numpy array
self.general[PROP_N] = np.array([nv], dtype=np.int32)
self.general[PROP_M] = np.array([ne], dtype=np.int32)
```

**Why:** PSet1 properties (general properties) are stored as single-element arrays in the binary format, not as raw scalars.

### String Property Encoding

**Q: Why did PROP_TITRE cause "string argument without an encoding" error?**
A: Strings must be explicitly encoded to bytes!

**The Problem:**
```python
# write_tgf_pset() tried:
prop_data = bytes(arr)  # When arr is string "My Graph"
# TypeError: string argument without an encoding
```

**The Fix:**
```python
if isinstance(arr, str):
    prop_data = arr.encode('utf-8')
```

**Why:** Python strings are Unicode, but TGF stores raw bytes. Must explicitly encode to UTF-8.

### Coordinate Array Sizing for PSet Ranges

**Q: Why did coordinates read back with wrong dtype (20-byte instead of 16-byte)?**
A: Array size must exactly match PSet range!

**The Problem:**
```python
# Graph has 4 vertices (range 1..4)
graph.setsize(4, 0)  # Creates vertex PSet with 4 elements

# WRONG - created 5-element array
coords = np.zeros(5, dtype=...)  # 5 * 16 = 80 bytes

# When written and read back:
# read calculates: element_size = 80 bytes / 4 elements = 20 bytes
# Detected as 20-byte format (with label) instead of 16-byte!
```

**The Fix:**
```python
# CORRECT - 4 elements for 4 vertices
coords = np.zeros(4, dtype=...)  # 4 * 16 = 64 bytes
coords[0] = (0.0, 0.0)  # Vertex 1 (numpy index 0)
coords[1] = (1.0, 0.0)  # Vertex 2 (numpy index 1)
# ...
```

**Why:** PSet with range `start..finish` has exactly `finish - start + 1` elements. The numpy array indices (0-based) map to logical vertex/edge indices (start-based).

### Extracting Scalars from PSet1 Properties

**Q: Why do I get NumPy deprecation warnings about array-to-scalar conversion?**
A: Need explicit indexing to extract scalar from 1-element array!

**The Problem:**
```python
# Triggers deprecation warning
n = int(graph.general[PROP_N])
# DeprecationWarning: Conversion of an array with ndim > 0 to a scalar...
```

**The Fix:**
```python
# Use helper function
def get_pset1_value(pset, prop_num):
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)

# Usage
n = get_pset1_value(graph.general, PROP_N)
```

**Why:** After changing setsize() to store numpy arrays, PROP_N/PROP_M are 1-element arrays. Direct `int()` conversion triggers warning. Must extract first element with `[0]` indexing.

### Side Effects of PSet1 Array Storage

**Q: Why did write_txt_graph() break after fixing setsize()?**
A: Functions accessing PROP_N/PROP_M must handle array format!

**The Problem:**
```python
ne = graph.general[PROP_M]
for e in range(1, ne + 1):  # TypeError: only integer scalar arrays can be converted
```

**The Fix:**
```python
ne_val = graph.general[PROP_M]
ne = int(ne_val[0]) if isinstance(ne_val, np.ndarray) else int(ne_val)
for e in range(1, ne + 1):  # Works!
```

**Impact:** Changing how properties are stored has ripple effects. Always check all code paths that access the changed properties.

### Version 2 Only for Writing

**Q: Can I write version 1 or version 0 TGF files?**
A: No! Only version 2 writing is supported.

```python
def write_tgf_graph(graph, filename, version=2):
    if version != 2:
        raise ValueError(f"Only version 2 writing supported, got version {version}")
```

**Why:** Version 0 is legacy format with different structure. Version 1 has the PROP_NLOOPS erasure quirk. Only version 2 (modern PSet format) is written. Reading supports v1 and v2.

### Test Helper Best Practice

**Pattern:** Create helper functions for common property access patterns:

```python
# tests/test_tgf_graph.py
def get_pset1_value(pset, prop_num):
    """Extract scalar value from PSet1 property."""
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)

# Usage in tests
nv = get_pset1_value(graph.general, PROP_N)
ne = get_pset1_value(graph.general, PROP_M)
```

**Benefits:**
- Eliminates deprecation warnings
- Makes tests more readable
- Centralizes extraction logic
- Future-proof against NumPy changes

---

## CLI Tool Usage

**Q: How do I convert between formats?**
A: Use the command-line converter:
```bash
# Basic usage (format auto-detected)
python3 pigale_tgf/cli.py input.tgf output.graphml
python3 pigale_tgf/cli.py input.txt output.tgf

# With verbose output
python3 pigale_tgf/cli.py -v input.graphml output.txt

# Select specific record from TGF
python3 pigale_tgf/cli.py --record 2 input.tgf output.graphml

# Get help
python3 pigale_tgf/cli.py --help
```

**Q: How are formats detected?**
A: Two-stage detection:
1. File extension (.tgf, .txt, .graphml, .xml)
2. Content inspection (magic bytes, headers)

**Q: Can I make it globally available?**
A: Yes, create symlink:
```bash
sudo ln -s $(pwd)/pigale_tgf/cli.py /usr/local/bin/pigale-convert
chmod +x pigale_tgf/cli.py
# Now use: pigale-convert input.tgf output.graphml
```

---

## Resources

- **C++ Source**: `tgraph/Tgf.cpp`, `tgraph/File.cpp`, `tgraph/PropTgf.cpp`
- **Headers**: `incl/TAXI/Tgf.h`, `incl/TAXI/Tfile.h`, `incl/TAXI/propdef.h`
- **Test Data**: `tgf/*.tgf`, `tgf/*.txt`
- **GraphML Spec**: http://graphml.graphdrawing.org/
- **Struct Module**: https://docs.python.org/3/library/struct.html
- **CLI Documentation**: [CLI_SUMMARY.md](CLI_SUMMARY.md)
- **Python API**: [README_PYTHON.md](README_PYTHON.md)
