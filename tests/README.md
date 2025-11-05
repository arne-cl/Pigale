# Pigale Test Suite

This directory contains the comprehensive test suite for the Pigale tgraph library.

## Directory Structure

```
tests/
├── unit/                   # Unit tests
│   ├── core/              # Core class tests (GraphContainer, Graph, etc.)
│   ├── data_structures/   # PSet, properties, circular order
│   ├── algorithms/        # Algorithm tests (DFS, planarity, embedding, drawing)
│   ├── generation/        # Graph generator tests
│   └── io/               # File I/O tests
├── integration/           # Integration tests for algorithm pipelines
├── bdd/                   # Behavior-driven tests (future)
├── fixtures/              # Test data
│   ├── graphs/           # Test graph files (TGF format)
│   └── expected_outputs/ # Expected results
├── helpers/               # Test utilities
│   ├── graph_builders.h/.cpp       # Standard graph construction
│   ├── graph_comparators.h/.cpp    # Graph validation
│   └── property_validators.h/.cpp  # Property system validation
├── performance/           # Performance benchmarks (future)
├── CMakeLists.txt        # CMake build configuration
└── README.md             # This file
```

## Building the Tests

### Prerequisites

- CMake 3.14 or higher
- C++11 compatible compiler
- Qt 5 (for building tgraph library)
- Google Test (fetched automatically by CMake)

### Build Steps

1. First, build the tgraph library:
   ```bash
   cd ../tgraph
   qmake tgraph.pro
   make
   cd ../tests
   ```

2. Build the test suite with CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

### Running Tests

Run all tests:
```bash
./unit_tests
./integration_tests
```

Or use CTest:
```bash
ctest --output-on-failure
```

Run specific tests:
```bash
./unit_tests --gtest_filter=GraphContainerTest.*
```

Run with verbose output:
```bash
./unit_tests --gtest_verbose
```

## Test Organization

### Unit Tests

Unit tests are organized by component:

- **core/**: Tests for GraphContainer, Graph, TopologicalGraph, GeometricGraph
- **data_structures/**: Tests for PSet, properties, circular order
- **algorithms/**: Algorithm-specific tests
  - traversal/: DFS, BFS, biconnect
  - planarity/: Planarity testing algorithms
  - embedding/: Schnyder, bipolar orientations
  - drawing/: Tutte, FPP, Vision
- **generation/**: Graph generator tests
- **io/**: TGF file I/O tests

### Integration Tests

Integration tests verify complete workflows:

- Planarity pipeline (generate → test → embed)
- Drawing pipeline (embed → compute coordinates)
- Graph modification sequences
- Property persistence across operations

### Test Helpers

The `helpers/` directory provides utilities:

- **graph_builders**: Construct standard test graphs (K4, K5, grids, etc.)
- **graph_comparators**: Validate graph properties and invariants
- **property_validators**: Check property system consistency

## Writing New Tests

### Using Test Helpers

```cpp
#include <gtest/gtest.h>
#include "graph_builders.h"
#include "graph_comparators.h"

TEST(MyTest, Example) {
    // Build a standard graph
    GraphContainer* gc = TestHelpers::BuildK4();
    TopologicalGraph G(gc);

    // Validate properties
    EXPECT_TRUE(TestHelpers::ValidateCircularOrder(G));

    // Cleanup
    delete gc;
}
```

### Test Naming Convention

- Test suite name: Component being tested (e.g., `GraphContainerTest`)
- Test name: Specific behavior (e.g., `SetSizeAllocatesCorrectCounts`)
- Use descriptive names that explain what is being tested

### Assertions

Use specific assertions for better error messages:

```cpp
EXPECT_EQ(G.nv(), 10);           // Good: specific comparison
EXPECT_TRUE(G.nv() == 10);       // Less informative

EXPECT_TRUE(ValidateCircularOrder(G))
    << "Circular order violated after NewEdge()";  // Good: custom message
```

## Test Coverage

Current implementation status:

- [x] Test infrastructure setup
- [x] Test helpers library
- [x] GraphContainer unit tests
- [x] Basic integration tests (planar pipeline)
- [ ] Graph class unit tests
- [ ] TopologicalGraph unit tests
- [ ] Algorithm tests (DFS, BFS, planarity, etc.)
- [ ] Full integration test suite
- [ ] Performance benchmarks

## CI/CD Integration

Tests are automatically run on every push via GitHub Actions.

See `.github/workflows/test.yml` for CI configuration.

## Code Coverage

To generate code coverage reports:

```bash
mkdir build-coverage
cd build-coverage
cmake -DENABLE_COVERAGE=ON ..
make
make check
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

Then open `coverage_html/index.html` in a browser.

## Troubleshooting

### tgraph library not found

Make sure you've built the tgraph library first:
```bash
cd ../tgraph
qmake tgraph.pro
make
```

### Google Test errors

CMake will automatically fetch Google Test. If you have network issues:
```bash
# Manually download googletest and set FETCHCONTENT_SOURCE_DIR_GOOGLETEST
```

### Linker errors

Ensure you're linking against the correct tgraph library (debug vs release):
```bash
ls ../lib/
# Should show libtgraph.a or libtgraph_debug.a
```

## Contributing

When adding new tests:

1. Follow the existing test structure
2. Use test helpers when possible
3. Add descriptive test names
4. Ensure tests are independent (no shared state)
5. Clean up resources (delete allocated graphs)
6. Update this README if adding new test categories

## References

- [Google Test Documentation](https://google.github.io/googletest/)
- [Pigale Documentation](http://pigale.sourceforge.net/)
- [Test Suite Plan](../TEST_SUITE_PLAN.md)
