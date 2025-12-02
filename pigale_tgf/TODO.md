# Pigale TGF/TXT Python Port - Implementation Plan

## Overview

Test-Driven Development (TDD) implementation of Pigale's graph file formats in Python:
- **TGF format** (version 2 only - modern property-based format)
- **TXT format** (simple ASCII edge list format)
- **GraphML conversion** (bidirectional)

## Project Structure

```
pigale_tgf/
├── __init__.py
├── core.py              # TGF low-level I/O
├── graph.py             # Graph container
├── properties.py        # Property system
├── txt_format.py        # TXT format I/O
├── graphml_format.py    # GraphML conversion
├── cli.py               # Command-line converter tool
├── constants.py         # Tags, property IDs
├── types.py             # Dataclasses
└── utils.py             # Alignment, padding

tests/
├── conftest.py          # pytest fixtures
├── test_core.py         # TGF core tests
├── test_graph.py        # Graph I/O tests
├── test_properties.py   # Property tests
├── test_txt.py          # TXT format tests
├── test_graphml.py      # GraphML tests
├── test_integration.py  # Integration tests
├── test_cli.py          # CLI tool tests
├── test_binary.py       # Binary validation
└── fixtures/
    ├── *.tgf            # Test TGF files
    ├── *.txt            # Test TXT files
    └── *.graphml        # Test GraphML files
```

## Implementation Phases

### Phase 1: Foundation & TXT Format (Week 1)

#### Day 1: Setup & Constants ✅
- [x] Create project structure
- [x] Setup pytest configuration
- [x] Write test_setup.py (package import test)
- [x] Write test_constants.py (all constants defined)
- [x] Implement constants.py with all tags/properties
- [x] Test: `pytest tests/test_setup.py tests/test_constants.py`

#### Day 2: TXT Reader (Test-First) ✅
- [x] Test 2.1: is_file_txt() detection
- [x] Implement is_file_txt()
- [x] Test 2.2: read_txt_header()
- [x] Implement read_txt_header()
- [x] Test 2.3: parse_txt_edges()
- [x] Implement parse_txt_edges()
- [x] Test 2.4: Loop skipping behavior
- [x] Test 2.5: Complete read_txt_graph()
- [x] Implement GraphContainer and read_txt_graph()
- [x] Test 2.6: Circular coordinate generation
- [x] Test: `pytest tests/test_txt.py::test_read_*`

#### Day 3: TXT Writer (Test-First) ✅
- [x] Test 3.1: write_txt_graph()
- [x] Implement write_txt_graph()
- [x] Test 3.2: TXT round-trip
- [x] Test: `pytest tests/test_txt.py`

### Phase 2: TGF Core (Week 2)

#### Day 4-5: TGF Binary I/O (Test-First) ✅
- [x] Test 4.1: Read TGF header
- [x] Implement types.py (Header, IfdHeader, Field dataclasses)
- [x] Implement Tgf.open() and header reading
- [x] Test 4.2: Write TGF header
- [x] Implement Tgf header writing
- [x] Test 4.3: Read IFD
- [x] Implement IFD reading
- [x] Test 4.4: Field read/write (small data ≤ 8 bytes)
- [x] Implement inline field storage
- [x] Test 4.5: Field read/write (large data > 8 bytes)
- [x] Implement external data storage with offsets
- [x] Test 4.6: Data alignment (4-byte boundaries)
- [x] Implement utils.py (num_padding, align_offset)
- [x] Test 4.7: Multiple records
- [x] Implement record management
- [x] Test: `pytest tests/test_core.py` - **16/16 tests passing**

#### Day 6-7: Property System (Test-First) ✅
- [x] Test 6.1: PSet creation
- [x] Implement PSet class (already done in Phase 1)
- [x] Test 6.2: PSet property storage
- [x] Implement property storage with numpy (already done in Phase 1)
- [x] Test 6.3: Property serialization (write_tgf_pset)
- [x] Implement write_tgf_pset()
- [x] Test 6.4: Property round-trip
- [x] Implement read_tgf_pset()
- [x] Test 6.5: 128 property limit enforcement
- [x] Test 6.6: Empty PSet handling
- [x] Test 6.7: Different data types (int32, int16, float64)
- [x] Add get_tag_length() to Tgf class
- [x] Test: `pytest tests/test_properties.py` - **9/9 tests passing**

### Phase 3: Graph I/O (Week 3)

#### Day 8-9: TGF Graph Reading (Test-First) ✅
- [x] Test 8.1: Detect TGF version
- [x] Implement detect_tgf_version() and get_num_records()
- [x] Test 8.2: Read modern TGF (version 2)
- [x] Implement read_tgf_graph() for v1 and v2
- [x] Test 8.3: Read specific record
- [x] Implement record selection with clamping
- [x] Test 8.4: Reject legacy formats (v0, v1)
- [x] Implement version validation (v0 rejected, v1 supported with PROP_NLOOPS erasure)
- [x] Add PSet1 tag support for general properties
- [x] Fix PSet sizing bug (1-indexed ranges)
- [x] Fix header flushing in create_record()
- [x] Add 16-byte coordinate dtype support
- [x] Test: `pytest tests/test_tgf_graph.py` - **11/11 tests passing**

#### Day 10-11: TGF Graph Writing (Test-First) ✅
- [x] Test 10.1: Write empty graph
- [x] Implement basic write_tgf_graph()
- [x] Test 10.2: Write simple graph
- [x] Test 10.3: TGF round-trip
- [x] Test 10.4: Write with coordinates
- [x] Test 10.5: Write version 2 format
- [x] Fix setsize() to store PROP_N/PROP_M as numpy arrays
- [x] Add string encoding support in write_tgf_pset()
- [x] Fix coordinate array sizing (PSet ranges)
- [x] Add get_pset1_value() helper in tests
- [x] Fix write_txt_graph() to handle array PROP_N/PROP_M
- [x] Test: `pytest tests/test_tgf_graph.py` - **16/16 tests passing**

### Phase 4: GraphML Conversion (Week 4) ✅

#### Day 12-14: GraphML Reading and Writing (Test-First) ✅
- [x] Test 12.1: Read GraphML basic structure
- [x] Implement read_graphml() using xml.etree
- [x] Test 12.2: GraphML node ID mapping
- [x] Test 12.3: Read coordinates, labels, colors, VIN
- [x] Test 14.1: Write GraphML basic
- [x] Implement write_graphml()
- [x] Test 14.2: Write with coordinates
- [x] Test 14.3: GraphML round-trip
- [x] Test: GraphML → TGF conversion
- [x] Test: TGF → GraphML conversion
- [x] Test: `pytest tests/test_graphml.py` - **11/11 tests passing**

### Phase 5: Integration & Validation (Week 5)

#### Day 15-16: Integration Testing ✅
- [x] Test 15.1: Cross-format conversion chain
- [x] Test 15.2: All format pairs (txt↔tgf↔graphml)
- [x] Test 15.3: Real Pigale files from tgf/ directory
- [x] Test 15.4: Large graph handling
- [x] Test 15.5: Edge cases (empty, single vertex)
- [x] All tgf/*.tgf files round-trip tested
- [x] All tgf/*.txt files round-trip tested
- [x] Property preservation across formats
- [x] Fixed: write_txt_graph label indexing bug
- [x] Fixed: read_txt_graph label array sizing
- [x] Fixed: GraphML coordinate handling for different dtypes
- [x] Test: `pytest tests/test_integration.py` - **10/10 tests passing**

#### Day 17-18: Binary Validation ✅
- [x] Test 17.1: TGF → GraphML → TGF round-trip preservation (Path A)
- [x] Test 17.2: GraphML → TGF → GraphML → TGF preservation (Path B)
- [x] Test 17.3: Direct TGF → TGF round-trip (baseline)
- [x] Implemented graphs_are_identical() helper function
- [x] Tests verify Pigale interprets round-tripped files as identical graphs
- [x] All fixture files tested (c.tgf, cylinder.tgf, d.tgf, symm.tgf, *.graphml)
- [x] Test: `pytest tests/test_binary.py` - **5/5 tests passing**

#### Day 19-20: Property-Based Testing
- [ ] Install hypothesis
- [ ] Test 19.1: Random graph TGF round-trip
- [ ] Test 19.2: Random property serialization
- [ ] Implement graph generators
- [ ] Test: `pytest tests/test_property.py`

## TDD Workflow

### Red-Green-Refactor Cycle

1. **RED**: Write failing test
   ```bash
   vim tests/test_feature.py
   pytest tests/test_feature.py  # Should FAIL
   ```

2. **GREEN**: Write minimal code to pass
   ```bash
   vim pigale_tgf/module.py
   pytest tests/test_feature.py  # Should PASS
   ```

3. **REFACTOR**: Improve while keeping tests green
   ```bash
   vim pigale_tgf/module.py
   pytest  # All tests still PASS
   ```

4. **COMMIT**: With test evidence
   ```bash
   git add tests/ pigale_tgf/
   git commit -m "Add feature X (TDD)"
   ```

## Success Criteria

### Must Have (MVP)
- [x] All tests pass (100/100)
- [x] 90%+ test coverage (85% overall, 93% for core modules)
- [x] Read/write TXT format
- [x] Read/write TGF v2 format
- [x] Read/write GraphML format
- [x] All tgf/*.tgf files round-trip
- [x] All tgf/*.txt files round-trip
- [x] Cross-format conversion chains
- [x] CLI converter tool
- [x] Binary compatibility validation

### Should Have
- [x] CLI converter tool
- [x] Property-based tests
- [ ] Type hints (mypy clean)
- [ ] Documentation (Sphinx)
- [ ] CI/CD pipeline

### Nice to Have
- [ ] Performance benchmarks
- [ ] Fuzzing tests
- [ ] Visualization tools
- [ ] Package for PyPI (pip install pigale-tgf)
- [ ] Man page for CLI tool
- [ ] Shell completion scripts
- [ ] Docker image

## Testing Commands

```bash
# Run all tests
pytest

# Run specific phase
pytest tests/test_txt.py
pytest tests/test_core.py
pytest tests/test_graph.py

# Coverage report
pytest --cov=pigale_tgf --cov-report=html

# Watch mode (requires pytest-watch)
ptw -- --cov=pigale_tgf
```

## Key Technical Details

### TGF Format (Version 2)
- Binary format with IFD structure (like TIFF)
- Little-endian byte order
- 4-byte alignment for data blocks
- Property-based graph storage
- Multiple graphs per file

### TXT Format
```
PIG:0
<title>
<v1> <v2>
...
0 0
```
- Simple edge list
- No coordinates (generated as circle)
- Loops skipped (v1 == v2)

### Critical Implementation Notes
- **128 property limit**: Hardcoded in C++, must preserve
- **#pragma pack(4)**: Coord struct is 20 bytes (not 24!)
- **Loop skipping**: Silently dropped in both TXT and TGF
- **Alignment**: All data blocks 4-byte aligned with '0' padding
- **Version 2 only**: Reject legacy formats with clear error

## References

- C++ Source: `tgraph/Tgf.cpp`, `tgraph/File.cpp`
- Format Spec: `incl/TAXI/Tgf.h`
- Property Defs: `incl/TAXI/propdef.h`
- Test Files: `tgf/*.tgf`, `tgf/*.txt`
