# Known Limitations - Pigale Test Suite Phase 1

**Version**: 1.0 (Phase 1)
**Date**: 2025-11-05

This document describes known limitations, incomplete implementations, and placeholder code in the Phase 1 test suite.

## Test Helper Limitations

### Graph Validators - Incomplete Implementations

#### 1. `IsIsomorphic()` - Graph Isomorphism Testing

**Location**: `tests/helpers/graph_comparators.cpp:15`

**Current Implementation**:
- Only checks if vertex and edge counts match
- Does NOT actually test graph isomorphism

**Limitation**:
- Two non-isomorphic graphs with the same counts will return `true`
- This is a placeholder implementation

**Impact**:
- Tests using this function may have false positives
- Should not be relied upon for strict isomorphism testing

**Future Work**:
- Implement proper graph isomorphism algorithm (VF2, Ullmann, or similar)
- Add degree sequence comparison as intermediate improvement
- Document complexity (isomorphism testing is NP-complete for general graphs)

**Workaround**:
- Use more specific validators for now
- Manually verify graph structure in tests where isomorphism matters

---

#### 2. `HasSameEmbedding()` - Embedding Comparison

**Location**: `tests/helpers/graph_comparators.cpp:31`

**Current Implementation**:
- Only checks vertex/edge counts
- Does NOT compare circular orders

**Limitation**:
- Two different embeddings of the same graph will return `true`
- Placeholder implementation

**Impact**:
- Cannot reliably test if two graphs have identical planar embeddings

**Future Work**:
- Compare circular order (cir/acir properties)
- Check exterior face marking
- Verify face sequences match

---

#### 3. `CheckNoEdgeCrossings()` - Crossing Detection

**Location**: `tests/helpers/graph_comparators.cpp:104`

**Current Implementation**:
- Always returns `true`
- Does NOT check for edge crossings

**Limitation**:
- Geometric drawings with crossings will pass validation
- Completely non-functional placeholder

**Impact**:
- Cannot validate that a drawing is actually planar
- Drawing algorithm tests may give false positives

**Future Work**:
- Implement line segment intersection testing
- Check all pairs of non-adjacent edges
- Use robust geometric predicates to avoid numerical issues

**Algorithm Outline**:
```cpp
for each pair of edges (e1, e2):
    if e1 and e2 share a vertex:
        continue  // Adjacent edges can "cross" at shared vertex

    get endpoints: (p1, p2) for e1, (p3, p4) for e2

    if segments_intersect(p1, p2, p3, p4):
        return false  // Found crossing

return true  // No crossings found
```

---

#### 4. `ValidateFaceOrientation()` - Face Consistency

**Location**: `tests/helpers/graph_comparators.cpp:180`

**Current Implementation**:
- Always returns `true`
- Does NOT check face orientation

**Limitation**:
- Inconsistent face orientations will not be detected

**Future Work**:
- Implement face traversal
- Check that all faces have consistent clockwise/counterclockwise orientation

---

### Graph Builders - Simplified Implementations

#### 5. `BuildMaximalPlanar()` - Not Actually Maximal

**Location**: `tests/helpers/graph_builders.cpp:162`

**Current Implementation**:
- Uses simple construction: triangle + add vertices
- May not produce truly maximal planar graphs for all n

**Limitation**:
- Generated graph may not have exactly 3n-6 edges
- May not be maximal (could add more edges while staying planar)

**Impact**:
- Tests expecting maximal planar properties may fail
- Edge count may not match expected value

**Future Work**:
- Implement proper maximal planar graph generation
- Use algorithm that guarantees triangulation
- Verify edge count matches 3n-6

**Workaround**:
- Use `BuildK4()` for small maximal planar graphs
- Generate grids and triangulate them
- Document in tests that this is simplified

---

#### 6. `BuildOuterplanar()` - Limited Structure

**Location**: `tests/helpers/graph_builders.cpp:147`

**Current Implementation**:
- Just a cycle with one chord
- Very limited outerplanar structure

**Limitation**:
- Not representative of general outerplanar graphs
- May miss edge cases

**Future Work**:
- Implement more diverse outerplanar graph generation
- Add fan graphs, trees, more complex structures

---

## Test Fixture Limitations

### TGF File Format

**Location**: `tests/fixtures/graphs/*.tgf`

**Status**: UNVERIFIED

**Issue**: TGF files created based on assumed format:
```
%!PS-TGRAPH
{n m}
{v1 v2}
...
```

**Limitation**:
- Format may not match actual TGF specification
- May not load correctly with Pigale's Tgf.cpp loader
- Missing metadata and property information

**Impact**:
- File-based tests may fail to load graphs
- Need to verify format with actual TGF loader

**Future Work**:
- Test loading with actual Tgf.cpp
- Study TGF format documentation
- Add more complete TGF files with properties (coordinates, colors, etc.)

---

## Test Coverage Gaps

### What's NOT Tested in Phase 1

1. **Graph class operations**:
   - DFS/BFS algorithms
   - Connectivity testing
   - Planarity testing internals

2. **TopologicalGraph operations**:
   - Edge insertion/deletion with embedding updates
   - Edge contraction
   - Edge subdivision
   - Circular order manipulation

3. **Property system**:
   - Property persistence across operations
   - Keep/ClearKeep mechanisms
   - Property type safety

4. **Geometric operations**:
   - Coordinate computations
   - Drawing algorithms
   - Geometric transformations

5. **I/O operations**:
   - TGF file reading
   - TGF file writing
   - Round-trip testing

6. **Algorithm correctness**:
   - Planarity testing correctness
   - Embedding validity
   - Drawing quality

**Note**: These will be addressed in Phase 2 and beyond.

---

## Build System Limitations

### Dual Build System Complexity

**Issue**: Supporting both CMake and qmake

**Limitation**:
- Must maintain two build configurations
- Configurations may diverge over time
- Different compiler flags between systems

**Recommendation**:
- Choose one build system as primary
- Keep other as backup/optional

### Test Framework Dependency

**Issue**: Requires Google Test installation

**On some systems**:
- Google Test not available via package manager
- Must build from source
- CMake FetchContent may have network issues

**Workaround**:
- CMakeLists.txt uses FetchContent to auto-download
- May fail in offline environments

---

## Platform-Specific Issues

### Potential Platform Differences

1. **Property types**:
   - `short` size may vary (typically 16-bit, but not guaranteed)
   - `int` size may vary (32-bit vs 64-bit platforms)

2. **Floating point**:
   - NaN and infinity handling may differ
   - Comparison tolerances may need adjustment

3. **Memory management**:
   - Different allocators on different platforms
   - Memory leak detection tools vary

**Recommendation**:
- Test on multiple platforms (Linux, macOS, Windows)
- Use CI/CD to catch platform-specific issues

---

## Performance Limitations

### No Performance Tests Yet

**Status**: Performance testing not implemented in Phase 1

**Limitation**:
- No benchmarks for algorithm performance
- No regression testing for performance
- No comparison between different algorithms

**Future Work** (Phase 7):
- Add Google Benchmark integration
- Benchmark critical operations (planarity, DFS, embedding)
- Set up performance regression tracking

---

## Documentation Limitations

### Incomplete API Documentation

**Issue**: Test helpers not fully documented

**Missing**:
- Detailed parameter descriptions
- Return value documentation
- Usage examples for each function
- Preconditions and postconditions

**Future Work**:
- Add Doxygen-style comments
- Create usage guide with examples
- Document expected behavior more clearly

---

## Memory Management

### Manual Memory Management

**Issue**: Test helpers return raw pointers

```cpp
GraphContainer* gc = BuildK4();  // Caller must delete
```

**Limitation**:
- Easy to forget `delete`
- Risk of memory leaks
- No automatic cleanup

**Current Mitigation**:
- Tests include explicit `delete gc;` statements
- Documented in README

**Future Work**:
- Consider smart pointers (std::unique_ptr)
- Add RAII wrapper classes
- Use Google Test memory leak detection

---

## Testing Best Practices Not Yet Implemented

### Missing Test Features

1. **Parameterized tests**: Not using gtest's parameterized test features
2. **Test fixtures**: Minimal use of shared setup/teardown
3. **Death tests**: Not testing assertion failures
4. **Mock objects**: No mocking (may not be needed for this project)
5. **Property-based testing**: Not using tools like RapidCheck

**Recommendation**: Add as needed in future phases

---

## Summary of Impact

### Critical Limitations (Block Some Tests)
- None - all critical compilation issues fixed

### High Impact (Affect Test Reliability)
- `CheckNoEdgeCrossings()` - Geometric tests unreliable
- `BuildMaximalPlanar()` - May not be truly maximal
- TGF file format unverified

### Medium Impact (Reduce Coverage)
- `IsIsomorphic()` - Limited usefulness
- `HasSameEmbedding()` - Cannot compare embeddings properly
- Phase 1 only tests GraphContainer

### Low Impact (Future Improvements)
- Documentation gaps
- Manual memory management
- Build system complexity

---

## Recommendations for Using Phase 1 Code

1. **Do NOT rely on**:
   - `IsIsomorphic()` for strict isomorphism testing
   - `CheckNoEdgeCrossings()` for geometric validation
   - `BuildMaximalPlanar()` for graphs larger than ~10 vertices

2. **Safe to use**:
   - `ValidateCircularOrder()` - Fully implemented
   - `ValidatePlanarMap()` - Basic checks work
   - `ValidateCoordinates()` - NaN/infinity detection works
   - All basic graph builders (K4, K5, K3,3, grids, paths, cycles)

3. **Before production use**:
   - Verify TGF file format
   - Test on actual tgraph library (requires qmake)
   - Run full test suite
   - Fix remaining placeholders

---

## Version History

- **v1.0** (2025-11-05): Initial Phase 1 release
  - Fixed critical tcolor/PROP_VCOLOR issues
  - Documented all known limitations
  - 39 tests implemented
  - Test infrastructure complete

**Next Version** (Phase 2): Will address Graph class testing and some limitations

---

This document should be updated as limitations are addressed and new ones are discovered.
