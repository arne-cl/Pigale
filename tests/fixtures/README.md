# Test Fixtures

This directory contains test data for the Pigale test suite.

## Graph Files

Test graphs are stored in TGF (TGRAPH) format.

### Directory Structure

- `graphs/planar/` - Planar graphs (K4, grids, trees, etc.)
- `graphs/nonplanar/` - Non-planar graphs (K5, K3,3, Petersen)
- `graphs/special/` - Edge cases (empty, single vertex, disconnected)
- `expected_outputs/` - Expected results from algorithms

### TGF Format

Basic format:
```
%!PS-TGRAPH
{n m}          # n vertices, m edges
{v1 v2}        # edge from vertex v1 to v2
...
```

## Available Test Graphs

### Planar Graphs
- `k4.tgf` - Complete graph on 4 vertices (6 edges)

### Non-Planar Graphs
- `k5.tgf` - Complete graph on 5 vertices (10 edges)
- `k33.tgf` - Complete bipartite K_{3,3} (9 edges)

### Special Cases
- `empty.tgf` - Empty graph (0 vertices, 0 edges)
- `single_vertex.tgf` - Single vertex, no edges
- `single_edge.tgf` - Minimal graph (2 vertices, 1 edge)

## Adding New Fixtures

To add new test graphs:

1. Create the TGF file in the appropriate subdirectory
2. Use the standard TGF format
3. Add comments describing the graph
4. Reference the fixture in test code

Example usage in tests:
```cpp
TopologicalGraph G;
LoadTgf(G, "fixtures/graphs/planar/k4.tgf");
```
