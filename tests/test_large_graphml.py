"""
Tests for large GraphML files round-trip conversion.

Tests that large real-world GraphML files can be converted to TGF
and back without data loss.
"""

import tempfile
from pathlib import Path
import numpy as np
import pytest

from pigale_tgf.graphml_format import read_graphml, write_graphml
from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN, PROP_COORD, PROP_LABEL


# Helper to extract scalar from PSet1
def get_pset1_value(pset, prop_num):
    """Extract scalar value from PSet1 property."""
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)


def test_original_mixed_model_layout_exists():
    """Test that original_MixedModelLayout_nx.graphml exists."""
    graphml_file = Path(__file__).parent / "fixtures" / "original_MixedModelLayout_nx.graphml"
    assert graphml_file.exists(), f"File not found: {graphml_file}"

    # Check file size (should be ~44KB)
    size = graphml_file.stat().st_size
    assert size > 40000, f"File too small: {size} bytes"
    assert size < 50000, f"File unexpectedly large: {size} bytes"


def test_bm_plus_mixed_model_layout_exists():
    """Test that BM+_MixedModelLayout_nx.graphml exists."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"
    assert graphml_file.exists(), f"File not found: {graphml_file}"

    # Check file size (should be ~4.9MB)
    size = graphml_file.stat().st_size
    assert size > 4_500_000, f"File too small: {size} bytes"
    assert size < 6_000_000, f"File unexpectedly large: {size} bytes"


def test_read_original_mixed_model_layout():
    """Test that original_MixedModelLayout_nx.graphml can be read."""
    graphml_file = Path(__file__).parent / "fixtures" / "original_MixedModelLayout_nx.graphml"

    # Read the graph
    graph = read_graphml(graphml_file)

    # Get graph stats
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    # Should have vertices and edges
    assert nv > 0, f"Graph has no vertices"
    assert ne > 0, f"Graph has no edges"

    print(f"Original graph: {nv} vertices, {ne} edges")

    # Should have VIN property
    assert graph.brin.exists(PROP_VIN), "Missing VIN property"

    # Check VIN consistency
    vin = graph.brin[PROP_VIN]
    for e in range(1, min(ne + 1, 10)):  # Check first 10 edges
        src = vin[ne + e]
        dst = vin[ne - e]
        assert 1 <= src <= nv, f"Invalid source vertex {src} for edge {e}"
        assert 1 <= dst <= nv, f"Invalid target vertex {dst} for edge {e}"


def test_read_bm_plus_mixed_model_layout():
    """Test that BM+_MixedModelLayout_nx.graphml can be read."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    # Read the graph
    graph = read_graphml(graphml_file)

    # Get graph stats
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    # Should have many vertices and edges
    assert nv > 1000, f"Expected large graph, got {nv} vertices"
    assert ne > 1000, f"Expected large graph, got {ne} edges"

    print(f"Large graph: {nv} vertices, {ne} edges")

    # Should have VIN property
    assert graph.brin.exists(PROP_VIN), "Missing VIN property"

    # Check VIN consistency (sample first 10 edges)
    vin = graph.brin[PROP_VIN]
    for e in range(1, min(ne + 1, 10)):
        src = vin[ne + e]
        dst = vin[ne - e]
        assert 1 <= src <= nv, f"Invalid source vertex {src} for edge {e}"
        assert 1 <= dst <= nv, f"Invalid target vertex {dst} for edge {e}"


def test_original_graphml_to_tgf_roundtrip():
    """Test GraphML → TGF → GraphML round-trip for original file."""
    graphml_file = Path(__file__).parent / "fixtures" / "original_MixedModelLayout_nx.graphml"

    # Read original GraphML
    graph1 = read_graphml(graphml_file)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)

    # Get original properties
    has_coords1 = graph1.vertex.exists(PROP_COORD)
    has_labels1 = graph1.vertex.exists(PROP_LABEL)

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # GraphML → TGF
        write_tgf_graph(graph1, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        # Check structure preserved
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)
        assert nv2 == nv1, f"Vertex count changed: {nv1} → {nv2}"
        assert ne2 == ne1, f"Edge count changed: {ne1} → {ne2}"

        # TGF → GraphML
        write_graphml(graph2, graphml_file2)
        graph3 = read_graphml(graphml_file2)

        # Check final structure
        nv3 = get_pset1_value(graph3.general, PROP_N)
        ne3 = get_pset1_value(graph3.general, PROP_M)
        assert nv3 == nv1, f"Final vertex count changed: {nv1} → {nv3}"
        assert ne3 == ne1, f"Final edge count changed: {ne1} → {ne3}"

        # Check properties preserved
        has_coords3 = graph3.vertex.exists(PROP_COORD)
        has_labels3 = graph3.vertex.exists(PROP_LABEL)

        if has_coords1:
            assert has_coords3, "Coordinates lost in round-trip"

        # VIN should be preserved
        assert graph3.brin.exists(PROP_VIN), "VIN lost in round-trip"


def test_bm_plus_graphml_to_tgf_roundtrip():
    """Test GraphML → TGF → GraphML round-trip for large file."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    # Read original GraphML
    print("Reading large GraphML file...")
    graph1 = read_graphml(graphml_file)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)
    print(f"  Loaded: {nv1} vertices, {ne1} edges")

    # Get original properties
    has_coords1 = graph1.vertex.exists(PROP_COORD)
    has_labels1 = graph1.vertex.exists(PROP_LABEL)

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test_large.tgf"
        graphml_file2 = Path(tmpdir) / "test_large.graphml"

        # GraphML → TGF
        print("Writing TGF file...")
        write_tgf_graph(graph1, tgf_file)
        tgf_size = tgf_file.stat().st_size
        print(f"  TGF file size: {tgf_size:,} bytes")

        print("Reading TGF file back...")
        graph2 = read_tgf_graph(tgf_file)

        # Check structure preserved
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)
        assert nv2 == nv1, f"Vertex count changed: {nv1} → {nv2}"
        assert ne2 == ne1, f"Edge count changed: {ne1} → {ne2}"
        print(f"  Structure preserved: {nv2} vertices, {ne2} edges")

        # TGF → GraphML
        print("Writing GraphML file back...")
        write_graphml(graph2, graphml_file2)
        graphml_size2 = graphml_file2.stat().st_size
        print(f"  GraphML file size: {graphml_size2:,} bytes")

        print("Reading final GraphML...")
        graph3 = read_graphml(graphml_file2)

        # Check final structure
        nv3 = get_pset1_value(graph3.general, PROP_N)
        ne3 = get_pset1_value(graph3.general, PROP_M)
        assert nv3 == nv1, f"Final vertex count changed: {nv1} → {nv3}"
        assert ne3 == ne1, f"Final edge count changed: {ne1} → {ne3}"
        print(f"  Final structure: {nv3} vertices, {ne3} edges")

        # Check properties preserved
        has_coords3 = graph3.vertex.exists(PROP_COORD)
        has_labels3 = graph3.vertex.exists(PROP_LABEL)

        if has_coords1:
            assert has_coords3, "Coordinates lost in round-trip"

        # VIN should be preserved
        assert graph3.brin.exists(PROP_VIN), "VIN lost in round-trip"


def test_original_coordinate_precision():
    """Test that coordinates are preserved with good precision."""
    graphml_file = Path(__file__).parent / "fixtures" / "original_MixedModelLayout_nx.graphml"

    # Read original
    graph1 = read_graphml(graphml_file)

    if not graph1.vertex.exists(PROP_COORD):
        pytest.skip("No coordinates in original file")

    coords1 = graph1.vertex[PROP_COORD]
    nv = get_pset1_value(graph1.general, PROP_N)

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # Round-trip
        write_tgf_graph(graph1, tgf_file)
        graph2 = read_tgf_graph(tgf_file)
        write_graphml(graph2, graphml_file2)
        graph3 = read_graphml(graphml_file2)

        coords3 = graph3.vertex[PROP_COORD]

        # Check coordinate precision (sample first 10 vertices)
        errors = []
        for i in range(min(nv, 10)):
            x1 = coords1[i]['x'] if 'x' in coords1.dtype.names else coords1[i][0]
            y1 = coords1[i]['y'] if 'y' in coords1.dtype.names else coords1[i][1]
            x3 = coords3[i]['x'] if 'x' in coords3.dtype.names else coords3[i][0]
            y3 = coords3[i]['y'] if 'y' in coords3.dtype.names else coords3[i][1]

            dx = abs(x3 - x1)
            dy = abs(y3 - y1)
            errors.append((dx, dy))

            # Coordinates should match within floating point precision
            assert dx < 1e-6, f"X coordinate mismatch for vertex {i+1}: {dx}"
            assert dy < 1e-6, f"Y coordinate mismatch for vertex {i+1}: {dy}"

        max_error = max(max(e) for e in errors)
        print(f"Maximum coordinate error: {max_error}")


def test_edge_preservation():
    """Test that all edges are preserved correctly."""
    graphml_file = Path(__file__).parent / "fixtures" / "original_MixedModelLayout_nx.graphml"

    # Read original
    graph1 = read_graphml(graphml_file)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)
    vin1 = graph1.brin[PROP_VIN]

    # Build edge set from original
    edges1 = set()
    for e in range(1, ne1 + 1):
        src = vin1[ne1 + e]
        dst = vin1[ne1 - e]
        edges1.add((min(src, dst), max(src, dst)))

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # Round-trip
        write_tgf_graph(graph1, tgf_file)
        graph2 = read_tgf_graph(tgf_file)
        write_graphml(graph2, graphml_file2)
        graph3 = read_graphml(graphml_file2)

        # Build edge set from final
        nv3 = get_pset1_value(graph3.general, PROP_N)
        ne3 = get_pset1_value(graph3.general, PROP_M)
        vin3 = graph3.brin[PROP_VIN]

        edges3 = set()
        for e in range(1, ne3 + 1):
            src = vin3[ne3 + e]
            dst = vin3[ne3 - e]
            edges3.add((min(src, dst), max(src, dst)))

        # Edge sets should be identical
        assert edges3 == edges1, f"Edge sets differ: {len(edges1)} original vs {len(edges3)} final"


if __name__ == "__main__":
    # Run tests with verbose output
    pytest.main([__file__, "-v", "-s"])
