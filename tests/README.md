# Pigale Test Suite

Comprehensive test suite for the Pigale tgraph library with 297 tests covering all major components.

## Quick Start

### Building and Running Tests

```bash
# Build tests
cd tests
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run tests
./unit_tests
./integration_tests
```

### Generating Coverage Reports

```bash
# From tests/ directory
./generate_coverage.sh

# Report will be at: tests/build/coverage_html/index.html
# Open with: firefox tests/build/coverage_html/index.html
```

## Test Coverage

- **297 tests total**: 244 unit tests + 53 integration tests
- **93.3% line coverage** (3,152 of 3,380 lines)
- **91.8% function coverage** (1,483 of 1,615 functions)

### Test Categories

**Unit Tests** (244 tests):
- GraphContainer: 23 tests
- Graph: 18 tests
- TopologicalGraph: 27 tests
- GeometricGraph: 26 tests
- CircularOrder: 17 tests
- PSet: 27 tests
- Traversal: 22 tests
- Planarity: 25 tests
- Embedding: 14 tests
- Generation: 45 tests
- I/O: 13 tests (4 disabled due to library bugs)

**Integration Tests** (53 tests):
- Planar Pipeline: 14 tests
- Property Persistence: 10 tests
- Graph Invariants: 29 tests

## Coverage Report Generation

The `generate_coverage.sh` script automates coverage report generation.

See TEST_SUITE_PLAN.md for complete documentation.
