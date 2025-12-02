"""Test TXT format reading and writing."""

import pytest
from pathlib import Path


# =============================================================================
# Test 2.1: File Detection
# =============================================================================


def test_is_file_txt_valid(fixtures_dir):
    """Test TXT file detection with valid file."""
    from pigale_tgf.txt_format import is_file_txt

    assert is_file_txt(fixtures_dir / "a.txt") == True


def test_is_file_txt_invalid(fixtures_dir):
    """Test TXT file detection with TGF file."""
    from pigale_tgf.txt_format import is_file_txt

    assert is_file_txt(fixtures_dir / "c.tgf") == False


def test_is_file_txt_missing():
    """Test TXT file detection with missing file."""
    from pigale_tgf.txt_format import is_file_txt

    assert is_file_txt("missing.txt") == False


# =============================================================================
# Test 2.2: Read Header
# =============================================================================


def test_read_txt_header(fixtures_dir):
    """Test reading PIG:0 header and title."""
    from pigale_tgf.txt_format import read_txt_header

    with open(fixtures_dir / "a.txt", "r") as f:
        header, title = read_txt_header(f)

    assert header == "PIG:0"
    assert title == "Planar_1"


def test_read_txt_header_handles_different_newlines(tmp_path):
    """Test reading header with different line endings."""
    from pigale_tgf.txt_format import read_txt_header

    # Create file with Windows line endings
    test_file = tmp_path / "test.txt"
    test_file.write_text("PIG:0\r\nTest Title\r\n1 2\r\n0 0\r\n")

    with open(test_file, "r") as f:
        header, title = read_txt_header(f)

    assert header == "PIG:0"
    assert title == "Test Title"


# =============================================================================
# Test 2.3: Parse Edge List
# =============================================================================


def test_parse_txt_edges_basic():
    """Test basic edge list parsing."""
    from pigale_tgf.txt_format import parse_txt_edges

    lines = ["3 4", "1 2", "1 5", "0 0"]
    edges = parse_txt_edges(lines)

    assert edges == [(3, 4), (1, 2), (1, 5)]


def test_parse_txt_edges_empty():
    """Test parsing with no edges."""
    from pigale_tgf.txt_format import parse_txt_edges

    lines = ["0 0"]
    edges = parse_txt_edges(lines)

    assert edges == []


# =============================================================================
# Test 2.4: Loop Skipping
# =============================================================================


def test_parse_txt_edges_skip_loops():
    """Test that loops (v1 == v2) are skipped."""
    from pigale_tgf.txt_format import parse_txt_edges

    lines = ["1 2", "2 2", "3 4", "5 5", "6 7", "0 0"]
    edges = parse_txt_edges(lines)

    # Loops (2,2) and (5,5) should be skipped
    assert edges == [(1, 2), (3, 4), (6, 7)]


def test_parse_txt_edges_all_loops():
    """Test file with only loops."""
    from pigale_tgf.txt_format import parse_txt_edges

    lines = ["1 1", "2 2", "3 3", "0 0"]
    edges = parse_txt_edges(lines)

    assert edges == []


# =============================================================================
# Test 2.5: Complete Graph Reading
# =============================================================================


def test_read_txt_graph_basic(fixtures_dir):
    """Test complete TXT graph reading."""
    from pigale_tgf.graph import read_txt_graph
    from pigale_tgf.constants import PROP_TITRE, PROP_N, PROP_M

    graph = read_txt_graph(fixtures_dir / "a.txt")

    # Check basic metadata
    assert graph.general[PROP_TITRE] == "Planar_1"

    # Check graph has edges (exact count from file)
    # a.txt has edges: 3-4, 1-2, 1-5, 1-3, 1-4, 2-3, 5-6, 2-6, 2-5, 3-5, 3-7, 6-7
    assert graph.general[PROP_M] == 12  # 12 edges

    # Check vertices exist (should have at least vertices 1-7)
    assert graph.general[PROP_N] >= 7


def test_read_txt_graph_has_vin_property(fixtures_dir):
    """Test that read graph has PROP_VIN."""
    from pigale_tgf.graph import read_txt_graph
    from pigale_tgf.constants import PROP_VIN

    graph = read_txt_graph(fixtures_dir / "a.txt")

    # PROP_VIN should exist for brins
    assert graph.brin.exists(PROP_VIN)


def test_read_txt_graph_has_labels(fixtures_dir):
    """Test that vertices have labels."""
    from pigale_tgf.graph import read_txt_graph
    from pigale_tgf.constants import PROP_LABEL

    graph = read_txt_graph(fixtures_dir / "a.txt")

    # Vertices should have labels
    assert graph.vertex.exists(PROP_LABEL)


# =============================================================================
# Test 2.6: Circular Coordinates
# =============================================================================


def test_txt_generates_circular_coords(fixtures_dir):
    """Test that TXT reader generates circular layout."""
    from pigale_tgf.graph import read_txt_graph
    from pigale_tgf.constants import PROP_COORD
    import math

    graph = read_txt_graph(fixtures_dir / "a.txt")

    # Check coordinates exist
    assert graph.vertex.exists(PROP_COORD)

    coords = graph.vertex[PROP_COORD]

    # Check first vertex (index 1) is on unit circle
    x, y = coords[1]
    radius = math.sqrt(x**2 + y**2)
    assert abs(radius - 1.0) < 0.01, f"Vertex 1 not on unit circle: radius={radius}"


def test_txt_circular_coords_evenly_spaced(fixtures_dir):
    """Test that vertices are evenly spaced on circle."""
    from pigale_tgf.graph import read_txt_graph
    from pigale_tgf.constants import PROP_COORD, PROP_N
    import math

    graph = read_txt_graph(fixtures_dir / "a.txt")

    nv = graph.general[PROP_N]
    coords = graph.vertex[PROP_COORD]

    # Calculate expected angle between vertices
    expected_angle = 2 * math.pi / nv

    # Check angles between consecutive vertices
    for i in range(1, min(4, nv)):  # Check first few
        x1, y1 = coords[i]
        x2, y2 = coords[i + 1]

        angle1 = math.atan2(y1, x1)
        angle2 = math.atan2(y2, x2)

        angle_diff = abs(angle2 - angle1)
        # Handle wrap-around
        if angle_diff > math.pi:
            angle_diff = 2 * math.pi - angle_diff

        assert abs(angle_diff - expected_angle) < 0.1, \
            f"Vertices not evenly spaced: {angle_diff} vs {expected_angle}"


# =============================================================================
# Test 3.1-3.2: Writing TXT Files
# =============================================================================


def test_write_txt_graph(tmp_path, fixtures_dir):
    """Test TXT graph writing."""
    from pigale_tgf.graph import read_txt_graph, write_txt_graph
    from pigale_tgf.constants import PROP_TITRE, PROP_M

    # Read original
    g1 = read_txt_graph(fixtures_dir / "a.txt")

    # Write to temp
    output = tmp_path / "output.txt"
    write_txt_graph(g1, output)

    # Read back
    g2 = read_txt_graph(output)

    # Verify structure preserved (not coords, which are regenerated)
    assert g2.general[PROP_TITRE] == g1.general[PROP_TITRE]
    assert g2.general[PROP_M] == g1.general[PROP_M]


def test_txt_roundtrip(tmp_path, fixtures_dir):
    """Test TXT format preserves graph structure."""
    from pigale_tgf.graph import read_txt_graph, write_txt_graph
    from pigale_tgf.constants import PROP_M

    g1 = read_txt_graph(fixtures_dir / "a.txt")
    temp = tmp_path / "temp.txt"
    write_txt_graph(g1, temp)
    g2 = read_txt_graph(temp)

    # Should have same number of edges
    assert g2.general[PROP_M] == g1.general[PROP_M]


def test_write_txt_format(tmp_path):
    """Test that written file has correct format."""
    from pigale_tgf.graph import GraphContainer, write_txt_graph
    from pigale_tgf.constants import PROP_TITRE, PROP_N, PROP_M, PROP_VIN, PROP_LABEL
    import numpy as np

    # Create simple graph: two vertices, one edge
    graph = GraphContainer()
    graph.setsize(2, 1)
    graph.general[PROP_TITRE] = "Test Graph"

    # Vertex labels
    vlabels = np.array([0, 10, 20], dtype=np.int32)  # 0, then labels 10 and 20
    graph.vertex[PROP_LABEL] = vlabels

    # Edge 1: connects vertices 1 and 2
    vin = np.array([0, 0, 1, 2], dtype=np.int32)  # Index: 0, -1, 1, 2 (offset by ne=1)
    # Actually: [-1, 0, 1] with offset, stored as [0, 0, 1] for -1, [1] for 0, [2] for 1
    # Let me fix this properly
    vin = np.zeros(3, dtype=np.int32)  # -1, 0, 1
    vin[0] = 2  # brin -1 -> vertex 2
    vin[1] = 0  # brin 0 -> 0 (always)
    vin[2] = 1  # brin 1 -> vertex 1
    graph.brin[PROP_VIN] = vin

    output = tmp_path / "test.txt"
    write_txt_graph(graph, output)

    # Verify format
    with open(output, 'r') as f:
        lines = f.readlines()

    assert lines[0].strip() == "PIG:0"
    assert lines[1].strip() == "Test Graph"
    # Edge: should write labels, not indices
    assert "10 20" in lines[2] or "20 10" in lines[2]
    assert lines[3].strip() == "0 0"
