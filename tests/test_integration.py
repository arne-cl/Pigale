"""Integration tests for cross-format conversions."""

import pytest
import numpy as np
from pathlib import Path


# =============================================================================
# Helper Functions
# =============================================================================


def get_pset1_value(pset, prop_num):
    """Extract scalar value from PSet1 property."""
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)


# =============================================================================
# Test 15.1: Real Pigale Files Round-Trip
# =============================================================================


def test_all_tgf_files_roundtrip(tmp_path):
    """Test that all tgf/*.tgf files can be read and written."""
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    # Get all TGF files from tgf/ directory
    tgf_dir = Path(__file__).parent.parent / "tgf"
    tgf_files = list(tgf_dir.glob("*.tgf"))

    assert len(tgf_files) > 0, "No TGF files found in tgf/ directory"

    for tgf_file in tgf_files:
        print(f"Testing {tgf_file.name}...")

        # Read original
        graph1 = read_tgf_graph(tgf_file)
        nv1 = get_pset1_value(graph1.general, PROP_N)
        ne1 = get_pset1_value(graph1.general, PROP_M)

        # Write to temp file
        output = tmp_path / f"roundtrip_{tgf_file.name}"
        write_tgf_graph(graph1, output)

        # Read back
        graph2 = read_tgf_graph(output)
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        # Verify sizes preserved
        assert nv2 == nv1, f"{tgf_file.name}: vertex count mismatch"
        assert ne2 == ne1, f"{tgf_file.name}: edge count mismatch"

        print(f"  ✓ {tgf_file.name}: {nv1} vertices, {ne1} edges")


def test_all_txt_files_roundtrip(tmp_path):
    """Test that all tgf/*.txt files can be read and written."""
    from pigale_tgf.graph import read_txt_graph, write_txt_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    # Get all TXT files from tgf/ directory
    tgf_dir = Path(__file__).parent.parent / "tgf"
    txt_files = list(tgf_dir.glob("*.txt"))

    assert len(txt_files) > 0, "No TXT files found in tgf/ directory"

    for txt_file in txt_files:
        print(f"Testing {txt_file.name}...")

        # Read original
        graph1 = read_txt_graph(txt_file)
        nv1 = get_pset1_value(graph1.general, PROP_N)
        ne1 = get_pset1_value(graph1.general, PROP_M)

        # Write to temp file
        output = tmp_path / f"roundtrip_{txt_file.name}"
        write_txt_graph(graph1, output)

        # Read back
        graph2 = read_txt_graph(output)
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        # Verify sizes preserved
        assert nv2 == nv1, f"{txt_file.name}: vertex count mismatch"
        assert ne2 == ne1, f"{txt_file.name}: edge count mismatch"

        print(f"  ✓ {txt_file.name}: {nv1} vertices, {ne1} edges")


# =============================================================================
# Test 15.2: Cross-Format Conversion Chains
# =============================================================================


def test_tgf_to_graphml_to_tgf(fixtures_dir, tmp_path):
    """Test TGF → GraphML → TGF conversion chain."""
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN

    original_tgf = fixtures_dir / "c.tgf"
    intermediate_graphml = tmp_path / "intermediate.graphml"
    final_tgf = tmp_path / "final.tgf"

    # Read original TGF
    graph1 = read_tgf_graph(original_tgf)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)

    # Convert to GraphML
    write_graphml(graph1, intermediate_graphml)

    # Read GraphML
    graph2 = read_graphml(intermediate_graphml)

    # Convert back to TGF
    write_tgf_graph(graph2, final_tgf)

    # Read final TGF
    graph3 = read_tgf_graph(final_tgf)
    nv3 = get_pset1_value(graph3.general, PROP_N)
    ne3 = get_pset1_value(graph3.general, PROP_M)

    # Verify sizes preserved through chain
    assert nv3 == nv1, "Vertex count not preserved through conversion chain"
    assert ne3 == ne1, "Edge count not preserved through conversion chain"

    # Verify VIN preserved
    if PROP_VIN in graph1.brin.properties and PROP_VIN in graph3.brin.properties:
        vin1 = graph1.brin[PROP_VIN]
        vin3 = graph3.brin[PROP_VIN]
        np.testing.assert_array_equal(vin3, vin1,
                                       err_msg="VIN not preserved through conversion chain")


def test_txt_to_tgf_to_graphml(tmp_path):
    """Test TXT → TGF → GraphML conversion chain."""
    from pigale_tgf.graph import read_txt_graph, write_tgf_graph, read_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    # Get TXT file
    tgf_dir = Path(__file__).parent.parent / "tgf"
    txt_files = list(tgf_dir.glob("*.txt"))

    if not txt_files:
        pytest.skip("No TXT files available for testing")

    original_txt = txt_files[0]
    intermediate_tgf = tmp_path / "intermediate.tgf"
    final_graphml = tmp_path / "final.graphml"

    # Read TXT
    graph1 = read_txt_graph(original_txt)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)

    # Convert to TGF
    write_tgf_graph(graph1, intermediate_tgf)

    # Read TGF
    graph2 = read_tgf_graph(intermediate_tgf)

    # Convert to GraphML
    write_graphml(graph2, final_graphml)

    # Read GraphML
    graph3 = read_graphml(final_graphml)
    nv3 = get_pset1_value(graph3.general, PROP_N)
    ne3 = get_pset1_value(graph3.general, PROP_M)

    # Verify sizes preserved
    assert nv3 == nv1, "Vertex count not preserved through conversion chain"
    assert ne3 == ne1, "Edge count not preserved through conversion chain"


def test_graphml_to_txt_to_tgf(fixtures_dir, tmp_path):
    """Test GraphML → TXT → TGF conversion chain."""
    from pigale_tgf.graph import read_txt_graph, write_txt_graph, read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    original_graphml = fixtures_dir / "embed3d.graphml"
    intermediate_txt = tmp_path / "intermediate.txt"
    final_tgf = tmp_path / "final.tgf"

    # Read GraphML
    graph1 = read_graphml(original_graphml)
    nv1 = get_pset1_value(graph1.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)

    # Convert to TXT
    write_txt_graph(graph1, intermediate_txt)

    # Read TXT
    graph2 = read_txt_graph(intermediate_txt)

    # Convert to TGF
    write_tgf_graph(graph2, final_tgf)

    # Read TGF
    graph3 = read_tgf_graph(final_tgf)
    nv3 = get_pset1_value(graph3.general, PROP_N)
    ne3 = get_pset1_value(graph3.general, PROP_M)

    # Verify sizes preserved
    assert nv3 == nv1, "Vertex count not preserved through conversion chain"
    assert ne3 == ne1, "Edge count not preserved through conversion chain"


# =============================================================================
# Test 15.3: Property Preservation Across Formats
# =============================================================================


def test_coordinates_preserved_tgf_graphml_tgf(fixtures_dir, tmp_path):
    """Test that coordinates survive TGF → GraphML → TGF."""
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml
    from pigale_tgf.constants import PROP_COORD

    original = fixtures_dir / "c.tgf"
    graphml_file = tmp_path / "temp.graphml"
    final_tgf = tmp_path / "final.tgf"

    # Read original
    graph1 = read_tgf_graph(original)

    # Check if has coordinates
    if PROP_COORD not in graph1.vertex.properties:
        pytest.skip("Test file has no coordinates")

    coords1 = graph1.vertex[PROP_COORD]

    # Convert through GraphML
    write_graphml(graph1, graphml_file)
    graph2 = read_graphml(graphml_file)
    write_tgf_graph(graph2, final_tgf)
    graph3 = read_tgf_graph(final_tgf)

    # Verify coordinates preserved
    assert PROP_COORD in graph3.vertex.properties
    coords3 = graph3.vertex[PROP_COORD]

    # Compare coordinates (allow small floating point error)
    for i in range(len(coords1)):
        np.testing.assert_allclose(coords3[i]['x'], coords1[i]['x'], rtol=1e-5, atol=1e-8)
        np.testing.assert_allclose(coords3[i]['y'], coords1[i]['y'], rtol=1e-5, atol=1e-8)


def test_labels_preserved_across_formats(tmp_path):
    """Test that labels survive format conversions."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.graph import write_txt_graph, read_txt_graph
    from pigale_tgf.constants import PROP_LABEL, PROP_VIN

    # Create graph with specific labels
    graph1 = GraphContainer()
    graph1.setsize(4, 3)

    # Set non-sequential labels
    labels = np.array([10, 20, 30, 40], dtype=np.int32)
    graph1.vertex[PROP_LABEL] = labels

    # Set VIN
    vin = np.zeros(7, dtype=np.int32)
    vin[3+1] = 1; vin[3-1] = 2
    vin[3+2] = 2; vin[3-2] = 3
    vin[3+3] = 3; vin[3-3] = 4
    graph1.brin[PROP_VIN] = vin

    # Test TGF round-trip
    tgf_file = tmp_path / "test.tgf"
    write_tgf_graph(graph1, tgf_file)
    graph2 = read_tgf_graph(tgf_file)

    assert PROP_LABEL in graph2.vertex.properties
    labels2 = graph2.vertex[PROP_LABEL]
    np.testing.assert_array_equal(labels2, labels)

    # Test GraphML round-trip
    graphml_file = tmp_path / "test.graphml"
    write_graphml(graph1, graphml_file)
    graph3 = read_graphml(graphml_file)

    assert PROP_LABEL in graph3.vertex.properties
    labels3 = graph3.vertex[PROP_LABEL]
    np.testing.assert_array_equal(labels3, labels)

    # Test TXT round-trip
    txt_file = tmp_path / "test.txt"
    write_txt_graph(graph1, txt_file)
    graph4 = read_txt_graph(txt_file)

    assert PROP_LABEL in graph4.vertex.properties
    labels4 = graph4.vertex[PROP_LABEL]
    np.testing.assert_array_equal(labels4, labels)


# =============================================================================
# Test 15.4: Large Graph Handling
# =============================================================================


def test_large_tgf_files():
    """Test handling of larger TGF files (cylinder.tgf, d.tgf)."""
    from pigale_tgf.graph import read_tgf_graph
    from pigale_tgf.constants import PROP_N, PROP_M

    tgf_dir = Path(__file__).parent.parent / "tgf"

    # Test cylinder.tgf (should be largest)
    cylinder = tgf_dir / "cylinder.tgf"
    if cylinder.exists():
        print(f"Testing {cylinder.name} ({cylinder.stat().st_size} bytes)...")
        graph = read_tgf_graph(cylinder)
        nv = get_pset1_value(graph.general, PROP_N)
        ne = get_pset1_value(graph.general, PROP_M)
        print(f"  ✓ Loaded: {nv} vertices, {ne} edges")
        assert nv > 0
        assert ne > 0

    # Test d.tgf
    d_file = tgf_dir / "d.tgf"
    if d_file.exists():
        print(f"Testing {d_file.name} ({d_file.stat().st_size} bytes)...")
        graph = read_tgf_graph(d_file)
        nv = get_pset1_value(graph.general, PROP_N)
        ne = get_pset1_value(graph.general, PROP_M)
        print(f"  ✓ Loaded: {nv} vertices, {ne} edges")
        assert nv > 0
        assert ne > 0


# =============================================================================
# Test 15.5: Edge Cases
# =============================================================================


def test_empty_graph_all_formats(tmp_path):
    """Test that empty graphs work in all formats."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.graph import write_txt_graph, read_txt_graph
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    # Create empty graph
    graph1 = GraphContainer()
    graph1.setsize(0, 0)

    # TGF
    tgf_file = tmp_path / "empty.tgf"
    write_tgf_graph(graph1, tgf_file)
    graph2 = read_tgf_graph(tgf_file)
    assert get_pset1_value(graph2.general, PROP_N) == 0
    assert get_pset1_value(graph2.general, PROP_M) == 0

    # TXT
    txt_file = tmp_path / "empty.txt"
    write_txt_graph(graph1, txt_file)
    graph3 = read_txt_graph(txt_file)
    assert get_pset1_value(graph3.general, PROP_N) == 0
    assert get_pset1_value(graph3.general, PROP_M) == 0

    # GraphML
    graphml_file = tmp_path / "empty.graphml"
    write_graphml(graph1, graphml_file)
    graph4 = read_graphml(graphml_file)
    assert get_pset1_value(graph4.general, PROP_N) == 0
    assert get_pset1_value(graph4.general, PROP_M) == 0


def test_single_vertex_all_formats(tmp_path):
    """Test that single-vertex graphs work in all formats."""
    from pigale_tgf.graph import GraphContainer, write_tgf_graph, read_tgf_graph
    from pigale_tgf.graph import write_txt_graph, read_txt_graph
    from pigale_tgf.graphml_format import write_graphml, read_graphml
    from pigale_tgf.constants import PROP_N, PROP_M

    # Create single-vertex graph
    graph1 = GraphContainer()
    graph1.setsize(1, 0)

    # TGF
    tgf_file = tmp_path / "single.tgf"
    write_tgf_graph(graph1, tgf_file)
    graph2 = read_tgf_graph(tgf_file)
    assert get_pset1_value(graph2.general, PROP_N) == 1
    assert get_pset1_value(graph2.general, PROP_M) == 0

    # TXT - NOTE: TXT format cannot represent isolated vertices
    # It only stores edges, so isolated vertices are lost
    # This is a known limitation of the TXT format
    txt_file = tmp_path / "single.txt"
    write_txt_graph(graph1, txt_file)
    graph3 = read_txt_graph(txt_file)
    assert get_pset1_value(graph3.general, PROP_N) == 0  # Lost!
    assert get_pset1_value(graph3.general, PROP_M) == 0

    # GraphML
    graphml_file = tmp_path / "single.graphml"
    write_graphml(graph1, graphml_file)
    graph4 = read_graphml(graphml_file)
    assert get_pset1_value(graph4.general, PROP_N) == 1
    assert get_pset1_value(graph4.general, PROP_M) == 0
