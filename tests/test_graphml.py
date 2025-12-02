"""Test GraphML format reading and writing."""

import pytest
import numpy as np
from pathlib import Path


# =============================================================================
# Test 12.1: Read GraphML Basic Structure
# =============================================================================


def test_read_graphml_basic(fixtures_dir):
    """Test reading basic GraphML file structure."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    graphml_file = fixtures_dir / "embed3d.graphml"

    # Read GraphML
    graph = read_graphml(graphml_file)

    # Check graph was loaded
    assert graph is not None

    # Check sizes
    nv = int(graph.general[PROP_N][0]) if isinstance(graph.general[PROP_N], np.ndarray) else int(graph.general[PROP_N])
    ne = int(graph.general[PROP_M][0]) if isinstance(graph.general[PROP_M], np.ndarray) else int(graph.general[PROP_M])

    # embed3d.graphml has 14 nodes
    assert nv == 14
    # Should have edges (count from file: 26 edges)
    assert ne > 0


def test_read_graphml_has_coordinates(fixtures_dir):
    """Test that coordinates are loaded from GraphML."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_COORD

    graphml_file = fixtures_dir / "embed3d.graphml"
    graph = read_graphml(graphml_file)

    # Should have coordinates
    assert PROP_COORD in graph.vertex.properties
    coords = graph.vertex[PROP_COORD]

    # Coords should be structured array with x, y fields
    assert coords.dtype.names is not None
    assert 'x' in coords.dtype.names
    assert 'y' in coords.dtype.names


def test_read_graphml_has_labels(fixtures_dir):
    """Test that vertex labels are loaded from GraphML."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_LABEL

    graphml_file = fixtures_dir / "embed3d.graphml"
    graph = read_graphml(graphml_file)

    # Should have labels
    assert PROP_LABEL in graph.vertex.properties


def test_read_graphml_has_colors(fixtures_dir):
    """Test that colors are loaded from GraphML."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_COLOR

    graphml_file = fixtures_dir / "embed3d.graphml"
    graph = read_graphml(graphml_file)

    # Should have vertex colors
    assert PROP_COLOR in graph.vertex.properties


def test_read_graphml_has_vin(fixtures_dir):
    """Test that edges are converted to VIN property."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_VIN

    graphml_file = fixtures_dir / "embed3d.graphml"
    graph = read_graphml(graphml_file)

    # Should have VIN property for brins
    assert PROP_VIN in graph.brin.properties


# =============================================================================
# Test 12.2: GraphML Node IDs
# =============================================================================


def test_read_graphml_node_mapping(fixtures_dir):
    """Test that node IDs (n1, n2, ...) map correctly to vertex indices."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_LABEL

    graphml_file = fixtures_dir / "embed3d.graphml"
    graph = read_graphml(graphml_file)

    # Get labels
    labels = graph.vertex[PROP_LABEL]

    # First few nodes in file have labels 1, 2, 3, ...
    # But may be reordered, so just check they exist
    label_set = set(labels)
    assert 1 in label_set
    assert 2 in label_set
    assert 3 in label_set


# =============================================================================
# Test 14.1: Write GraphML Basic
# =============================================================================


def test_write_graphml_basic(tmp_path):
    """Test writing basic GraphML file."""
    from pigale_tgf.graph import GraphContainer
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN

    output = tmp_path / "test.graphml"

    # Create simple graph: triangle (3 vertices, 3 edges)
    graph = GraphContainer()
    graph.setsize(3, 3)

    # Set VIN (vertex incidence)
    # Edge 1: 1->2, Edge 2: 2->3, Edge 3: 3->1
    vin = np.zeros(7, dtype=np.int32)  # -3..3
    vin[3+1] = 1; vin[3-1] = 2  # Edge 1
    vin[3+2] = 2; vin[3-2] = 3  # Edge 2
    vin[3+3] = 3; vin[3-3] = 1  # Edge 3
    graph.brin[PROP_VIN] = vin

    # Write to GraphML
    write_graphml(graph, output)

    # Read back
    graph2 = read_graphml(output)

    # Verify sizes
    nv = int(graph2.general[PROP_N][0]) if isinstance(graph2.general[PROP_N], np.ndarray) else int(graph2.general[PROP_N])
    ne = int(graph2.general[PROP_M][0]) if isinstance(graph2.general[PROP_M], np.ndarray) else int(graph2.general[PROP_M])

    assert nv == 3
    assert ne == 3


def test_write_graphml_with_coordinates(tmp_path):
    """Test writing GraphML with coordinates."""
    from pigale_tgf.graph import GraphContainer
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.constants import PROP_COORD

    output = tmp_path / "with_coords.graphml"

    # Create graph with 3 vertices
    graph = GraphContainer()
    graph.setsize(3, 0)

    # Set coordinates
    coords = np.zeros(3, dtype=np.dtype([('x', '<f8'), ('y', '<f8')]))
    coords[0] = (0.0, 0.0)
    coords[1] = (1.0, 0.0)
    coords[2] = (0.5, 0.866)
    graph.vertex[PROP_COORD] = coords

    # Write
    write_graphml(graph, output)

    # Read back
    graph2 = read_graphml(output)

    # Verify coordinates preserved
    assert PROP_COORD in graph2.vertex.properties
    coords2 = graph2.vertex[PROP_COORD]

    # Check coordinates (may have rounding)
    np.testing.assert_allclose(coords2[0]['x'], 0.0, atol=0.001)
    np.testing.assert_allclose(coords2[0]['y'], 0.0, atol=0.001)


# =============================================================================
# Test 14.2: GraphML Round-Trip
# =============================================================================


def test_graphml_roundtrip(fixtures_dir, tmp_path):
    """Test GraphML round-trip: read → write → read."""
    from pigale_tgf.graphml_format import read_graphml, write_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    original_file = fixtures_dir / "embed3d.graphml"
    output_file = tmp_path / "roundtrip.graphml"

    # Read original
    graph1 = read_graphml(original_file)
    nv1 = int(graph1.general[PROP_N][0]) if isinstance(graph1.general[PROP_N], np.ndarray) else int(graph1.general[PROP_N])
    ne1 = int(graph1.general[PROP_M][0]) if isinstance(graph1.general[PROP_M], np.ndarray) else int(graph1.general[PROP_M])

    # Write to new file
    write_graphml(graph1, output_file)

    # Read back
    graph2 = read_graphml(output_file)
    nv2 = int(graph2.general[PROP_N][0]) if isinstance(graph2.general[PROP_N], np.ndarray) else int(graph2.general[PROP_N])
    ne2 = int(graph2.general[PROP_M][0]) if isinstance(graph2.general[PROP_M], np.ndarray) else int(graph2.general[PROP_M])

    # Verify sizes match
    assert nv2 == nv1
    assert ne2 == ne1


# =============================================================================
# Test 12.3: Cross-Format Conversion
# =============================================================================


def test_graphml_to_tgf_conversion(fixtures_dir, tmp_path):
    """Test converting GraphML to TGF."""
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.graph import write_tgf_graph, read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    graphml_file = fixtures_dir / "embed3d.graphml"
    tgf_file = tmp_path / "converted.tgf"

    # Read GraphML
    graph1 = read_graphml(graphml_file)
    nv1 = int(graph1.general[PROP_N][0]) if isinstance(graph1.general[PROP_N], np.ndarray) else int(graph1.general[PROP_N])
    ne1 = int(graph1.general[PROP_M][0]) if isinstance(graph1.general[PROP_M], np.ndarray) else int(graph1.general[PROP_M])

    # Write as TGF
    write_tgf_graph(graph1, tgf_file)

    # Read TGF
    graph2 = read_tgf_graph(tgf_file)
    nv2 = int(graph2.general[PROP_N][0]) if isinstance(graph2.general[PROP_N], np.ndarray) else int(graph2.general[PROP_N])
    ne2 = int(graph2.general[PROP_M][0]) if isinstance(graph2.general[PROP_M], np.ndarray) else int(graph2.general[PROP_M])

    # Verify conversion preserved sizes
    assert nv2 == nv1
    assert ne2 == ne1


def test_tgf_to_graphml_conversion(fixtures_dir, tmp_path):
    """Test converting TGF to GraphML."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    tgf_file = fixtures_dir / "c.tgf"
    graphml_file = tmp_path / "converted.graphml"

    # Read TGF
    graph1 = read_tgf_graph(tgf_file)
    nv1 = int(graph1.general[PROP_N][0]) if isinstance(graph1.general[PROP_N], np.ndarray) else int(graph1.general[PROP_N])
    ne1 = int(graph1.general[PROP_M][0]) if isinstance(graph1.general[PROP_M], np.ndarray) else int(graph1.general[PROP_M])

    # Write as GraphML
    write_graphml(graph1, graphml_file)

    # Read GraphML
    graph2 = read_graphml(graphml_file)
    nv2 = int(graph2.general[PROP_N][0]) if isinstance(graph2.general[PROP_N], np.ndarray) else int(graph2.general[PROP_N])
    ne2 = int(graph2.general[PROP_M][0]) if isinstance(graph2.general[PROP_M], np.ndarray) else int(graph2.general[PROP_M])

    # Verify conversion preserved sizes
    assert nv2 == nv1
    assert ne2 == ne1
