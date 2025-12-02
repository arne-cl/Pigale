"""Test TGF graph reading and writing."""

import pytest
import struct
import numpy as np
from pathlib import Path


# =============================================================================
# Helper Functions
# =============================================================================


def get_pset1_value(pset, prop_num):
    """
    Extract scalar value from PSet1 property.

    PSet1 properties (general) are stored as single-element arrays.
    This helper extracts the scalar value properly.
    """
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)


# =============================================================================
# Test 8.1: Detect TGF Version
# =============================================================================


def test_detect_tgf_version(fixtures_dir):
    """Test detecting TGF version from file."""
    from pigale_tgf.graph import detect_tgf_version

    # Test with existing TGF file
    tgf_file = fixtures_dir / "c.tgf"
    version = detect_tgf_version(tgf_file)

    # c.tgf should be version 2 (modern format)
    assert version == 2


def test_detect_version_missing_tag(tmp_path):
    """Test version detection when TAG_VERSION is missing (version 0)."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.graph import detect_tgf_version

    # Create TGF file without TAG_VERSION
    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()
    # Don't write TAG_VERSION - simulates version 0
    tgf.close()

    version = detect_tgf_version(output)
    assert version == 0


# =============================================================================
# Test 8.2: Read Modern TGF (Version 2)
# =============================================================================


def test_read_tgf_graph_version_2(fixtures_dir):
    """Test reading version 2 TGF file."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    tgf_file = fixtures_dir / "c.tgf"

    # Read first record
    graph = read_tgf_graph(tgf_file, record_num=1)

    # Should have graph structure
    assert PROP_N in graph.general.properties
    assert PROP_M in graph.general.properties

    # PSet1 properties are scalars, not arrays
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    assert nv > 0
    assert ne >= 0


def test_read_tgf_graph_has_vin(fixtures_dir):
    """Test that VIN property is loaded."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import PROP_VIN, PROP_M

    tgf_file = fixtures_dir / "c.tgf"
    graph = read_tgf_graph(tgf_file, record_num=1)

    # Should have VIN property for brins
    assert PROP_VIN in graph.brin.properties

    ne = get_pset1_value(graph.general, PROP_M)
    vin = graph.brin[PROP_VIN]

    # VIN should have entries for all brins (-ne..ne)
    # Note: stored as array with offset, so index 0 = brin -ne
    assert len(vin) == 2 * ne + 1


def test_read_tgf_graph_coordinates(fixtures_dir):
    """Test that coordinates are loaded."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import PROP_COORD

    tgf_file = fixtures_dir / "c.tgf"
    graph = read_tgf_graph(tgf_file, record_num=1)

    # Should have coordinates
    if PROP_COORD in graph.vertex.properties:
        coords = graph.vertex[PROP_COORD]
        # Coords should be structured array with x, y fields
        assert coords.dtype.names is not None
        assert 'x' in coords.dtype.names
        assert 'y' in coords.dtype.names


# =============================================================================
# Test 8.3: Read Specific Record
# =============================================================================


def test_read_tgf_multiple_records(fixtures_dir):
    """Test reading different records from multi-record file."""
    from pigale_tgf.graph import read_tgf_graph, get_num_records

    tgf_file = fixtures_dir / "c.tgf"

    # Get number of records
    num_records = get_num_records(tgf_file)
    assert num_records > 0

    # Read first record
    graph1 = read_tgf_graph(tgf_file, record_num=1)

    # If multiple records, read second
    if num_records > 1:
        graph2 = read_tgf_graph(tgf_file, record_num=2)
        # Graphs might have different sizes
        # Just verify both loaded successfully
        assert graph1 is not None
        assert graph2 is not None


def test_read_default_record(fixtures_dir):
    """Test reading without specifying record number (defaults to 1)."""
    from pigale_tgf.graph import read_tgf_graph

    tgf_file = fixtures_dir / "c.tgf"

    # Default should read record 1
    graph = read_tgf_graph(tgf_file)
    assert graph is not None


def test_read_invalid_record_number(fixtures_dir):
    """Test reading with invalid record number."""
    from pigale_tgf.graph import read_tgf_graph, get_num_records

    tgf_file = fixtures_dir / "c.tgf"
    num_records = get_num_records(tgf_file)

    # Record number too high - should clamp to max
    graph = read_tgf_graph(tgf_file, record_num=num_records + 100)
    assert graph is not None  # Should read last record

    # Record number too low - should clamp to 1
    graph = read_tgf_graph(tgf_file, record_num=0)
    assert graph is not None  # Should read first record


# =============================================================================
# Test 8.4: Reject Legacy Formats (Version 0)
# =============================================================================


def test_read_version_0_legacy_format(tmp_path):
    """Test reading version 0 (legacy format) - not supported."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import TAG_N, TAG_M

    # Create version 0 file (no TAG_VERSION)
    output = tmp_path / "v0.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write old-style tags
    tgf.field_write(TAG_N, struct.pack('<i', 5))
    tgf.field_write(TAG_M, struct.pack('<i', 4))

    tgf.close()

    # Reading version 0 should raise error or return None
    with pytest.raises(ValueError, match="Version 0.*not supported"):
        read_tgf_graph(output)


def test_read_version_1_legacy_format(tmp_path):
    """Test that version 1 files can be read (but PROP_NLOOPS erased)."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import TAG_VERSION, PROP_N, PROP_M, PROP_NLOOPS
    from pigale_tgf.properties import write_tgf_pset, PSet

    # Create version 1 file
    output = tmp_path / "v1.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write version 1
    tgf.field_write(TAG_VERSION, struct.pack('<h', 1))

    # Write minimal graph
    pset = PSet(0, -1)
    pset[PROP_N] = np.array([3], dtype=np.int32)
    pset[PROP_M] = np.array([2], dtype=np.int32)
    pset[PROP_NLOOPS] = np.array([1], dtype=np.int32)  # This should be erased
    write_tgf_pset(pset, tgf, set_num=0, use_pset1=True)  # General uses PSet1

    # Write empty vertex/edge/brin sets (use PSet tags)
    write_tgf_pset(PSet(1, 3), tgf, set_num=0, use_pset1=False)  # vertex
    write_tgf_pset(PSet(1, 2), tgf, set_num=1, use_pset1=False)  # edge
    write_tgf_pset(PSet(-2, 2), tgf, set_num=2, use_pset1=False)  # brin

    tgf.close()

    # Read version 1
    graph = read_tgf_graph(output)

    # PROP_NLOOPS should be erased (C++ File.cpp:147)
    assert PROP_NLOOPS not in graph.general.properties


# =============================================================================
# Test 8.5: Graph Structure Validation
# =============================================================================


def test_read_tgf_graph_sizes(fixtures_dir):
    """Test that graph sizes are set correctly."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    tgf_file = fixtures_dir / "c.tgf"
    graph = read_tgf_graph(tgf_file, record_num=1)

    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    # Vertex PSet should have range 1..nv
    assert graph.vertex.start == 1
    assert graph.vertex.finish == nv

    # Edge PSet should have range 1..ne
    assert graph.edge.start == 1
    assert graph.edge.finish == ne

    # Brin PSet should have range -ne..ne
    assert graph.brin.start == -ne
    assert graph.brin.finish == ne


# =============================================================================
# Test 10: TGF Graph Writing
# =============================================================================


def test_write_empty_graph(tmp_path):
    """Test writing an empty graph to TGF."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    output = tmp_path / "empty.tgf"

    # Create empty graph
    graph = GraphContainer()
    graph.setsize(0, 0)

    # Write to file
    write_tgf_graph(graph, output)

    # Read back
    graph2 = read_tgf_graph(output)

    # Verify
    assert get_pset1_value(graph2.general, PROP_N) == 0
    assert get_pset1_value(graph2.general, PROP_M) == 0


def test_write_simple_graph(tmp_path):
    """Test writing a simple graph to TGF."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN, PROP_TITRE

    output = tmp_path / "simple.tgf"

    # Create simple graph: triangle (3 vertices, 3 edges)
    graph = GraphContainer()
    graph.setsize(3, 3)

    # Set title
    graph.general[PROP_TITRE] = "Triangle"

    # Set VIN (vertex incidence)
    # Edge 1: 1->2, Edge 2: 2->3, Edge 3: 3->1
    vin = np.zeros(7, dtype=np.int32)  # -3..3
    vin[3+1] = 1; vin[3-1] = 2  # Edge 1
    vin[3+2] = 2; vin[3-2] = 3  # Edge 2
    vin[3+3] = 3; vin[3-3] = 1  # Edge 3
    graph.brin[PROP_VIN] = vin

    # Write to file
    write_tgf_graph(graph, output)

    # Read back
    graph2 = read_tgf_graph(output)

    # Verify sizes
    assert get_pset1_value(graph2.general, PROP_N) == 3
    assert get_pset1_value(graph2.general, PROP_M) == 3

    # Verify VIN
    vin2 = graph2.brin[PROP_VIN]
    assert vin2[3+1] == 1 and vin2[3-1] == 2
    assert vin2[3+2] == 2 and vin2[3-2] == 3
    assert vin2[3+3] == 3 and vin2[3-3] == 1


def test_tgf_roundtrip(fixtures_dir, tmp_path):
    """Test TGF round-trip: read existing file, write it, read again."""
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN

    original_file = fixtures_dir / "c.tgf"
    output_file = tmp_path / "roundtrip.tgf"

    # Read original
    graph1 = read_tgf_graph(original_file, record_num=1)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)

    # Write to new file
    write_tgf_graph(graph1, output_file)

    # Read back
    graph2 = read_tgf_graph(output_file)
    nv2 = get_pset1_value(graph2.general, PROP_N)
    ne2 = get_pset1_value(graph2.general, PROP_M)

    # Verify sizes match
    assert nv2 == nv1
    assert ne2 == ne1

    # Verify VIN property preserved
    if PROP_VIN in graph1.brin.properties:
        assert PROP_VIN in graph2.brin.properties
        np.testing.assert_array_equal(graph2.brin[PROP_VIN], graph1.brin[PROP_VIN])


def test_write_with_coordinates(tmp_path):
    """Test writing graph with coordinate properties."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_COORD

    output = tmp_path / "with_coords.tgf"

    # Create graph with 4 vertices
    graph = GraphContainer()
    graph.setsize(4, 0)

    # Set coordinates (using 16-byte format: x, y)
    # PSet with range 1..4 needs 4 elements (indexed 0-3 in numpy array)
    coords = np.zeros(4, dtype=np.dtype([('x', '<f8'), ('y', '<f8')]))
    coords[0] = (0.0, 0.0)  # Vertex 1
    coords[1] = (1.0, 0.0)  # Vertex 2
    coords[2] = (1.0, 1.0)  # Vertex 3
    coords[3] = (0.0, 1.0)  # Vertex 4
    graph.vertex[PROP_COORD] = coords

    # Write
    write_tgf_graph(graph, output)

    # Read back
    graph2 = read_tgf_graph(output)

    # Verify coordinates preserved
    assert PROP_COORD in graph2.vertex.properties
    coords2 = graph2.vertex[PROP_COORD]

    # Check a few points (PSet arrays are 0-indexed)
    assert coords2[0]['x'] == 0.0 and coords2[0]['y'] == 0.0  # Vertex 1
    assert coords2[1]['x'] == 1.0 and coords2[1]['y'] == 0.0  # Vertex 2


def test_write_version_2(tmp_path):
    """Test that write_tgf_graph writes version 2 format."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, detect_tgf_version

    output = tmp_path / "version2.tgf"

    # Create simple graph
    graph = GraphContainer()
    graph.setsize(2, 1)

    # Write
    write_tgf_graph(graph, output)

    # Check version
    version = detect_tgf_version(output)
    assert version == 2
