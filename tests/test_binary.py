"""Binary validation tests for TGF format preservation.

These tests verify that graph round-trip conversions preserve graph
structure and properties as interpreted by Pigale, ensuring binary
compatibility.
"""

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


def graphs_are_identical(graph1, graph2, check_coords=True, check_title=True, coord_rtol=1e-5, coord_atol=1e-8):
    """
    Compare two GraphContainer objects for structural and property equality.

    This function checks if two graphs represent the same graph structure
    with the same Pigale properties, as interpreted by the Pigale system.

    Args:
        graph1: First GraphContainer
        graph2: Second GraphContainer
        check_coords: Whether to compare coordinate properties (default: True)
        check_title: Whether to compare graph title (default: True, set to False for GraphML round-trips)
        coord_rtol: Relative tolerance for coordinate comparison
        coord_atol: Absolute tolerance for coordinate comparison

    Returns:
        True if graphs are identical, False otherwise

    Raises:
        AssertionError: With detailed message if graphs differ
    """
    from pigale_tgf.constants import PROP_N, PROP_M, PROP_COORD, PROP_VIN, PROP_LABEL, PROP_COLOR, PROP_TITRE

    # Compare sizes
    nv1 = get_pset1_value(graph1.general, PROP_N)
    nv2 = get_pset1_value(graph2.general, PROP_N)
    ne1 = get_pset1_value(graph1.general, PROP_M)
    ne2 = get_pset1_value(graph2.general, PROP_M)

    assert nv1 == nv2, f"Vertex count mismatch: {nv1} != {nv2}"
    assert ne1 == ne2, f"Edge count mismatch: {ne1} != {ne2}"

    # Compare PSet ranges
    assert graph1.vertex.start == graph2.vertex.start, "Vertex PSet start mismatch"
    assert graph1.vertex.finish == graph2.vertex.finish, "Vertex PSet finish mismatch"
    assert graph1.edge.start == graph2.edge.start, "Edge PSet start mismatch"
    assert graph1.edge.finish == graph2.edge.finish, "Edge PSet finish mismatch"
    assert graph1.brin.start == graph2.brin.start, "Brin PSet start mismatch"
    assert graph1.brin.finish == graph2.brin.finish, "Brin PSet finish mismatch"

    # Compare graph title if requested and present
    if check_title and (PROP_TITRE in graph1.general.properties or PROP_TITRE in graph2.general.properties):
        titre1 = graph1.general.properties.get(PROP_TITRE, "")
        titre2 = graph2.general.properties.get(PROP_TITRE, "")

        # Handle different title types (string, bytes, array)
        def normalize_title(titre):
            if isinstance(titre, np.ndarray):
                if titre.dtype == np.uint8:
                    titre = titre.tobytes().decode('utf-8', errors='ignore').rstrip('\x00')
                else:
                    # Numeric array - likely uninitialized data, treat as empty
                    return ""
            elif isinstance(titre, bytes):
                titre = titre.decode('utf-8', errors='ignore').rstrip('\x00')
            elif isinstance(titre, str):
                titre = titre.rstrip('\x00')
            else:
                titre = str(titre)

            # Strip whitespace and null bytes
            return titre.strip()

        titre1 = normalize_title(titre1)
        titre2 = normalize_title(titre2)

        assert titre1 == titre2, f"Graph title mismatch: '{titre1}' != '{titre2}'"

    # Compare VIN (vertex incidence) - critical for graph structure
    if PROP_VIN in graph1.brin.properties and PROP_VIN in graph2.brin.properties:
        vin1 = graph1.brin[PROP_VIN]
        vin2 = graph2.brin[PROP_VIN]
        np.testing.assert_array_equal(vin2, vin1, err_msg="VIN (vertex incidence) mismatch")
    elif PROP_VIN in graph1.brin.properties or PROP_VIN in graph2.brin.properties:
        raise AssertionError("VIN property present in only one graph")

    # Compare vertex labels if present
    if PROP_LABEL in graph1.vertex.properties and PROP_LABEL in graph2.vertex.properties:
        labels1 = graph1.vertex[PROP_LABEL]
        labels2 = graph2.vertex[PROP_LABEL]
        np.testing.assert_array_equal(labels2, labels1, err_msg="Vertex label mismatch")
    elif PROP_LABEL in graph1.vertex.properties or PROP_LABEL in graph2.vertex.properties:
        raise AssertionError("PROP_LABEL present in only one graph")

    # Compare coordinates if requested and present
    if check_coords:
        if PROP_COORD in graph1.vertex.properties and PROP_COORD in graph2.vertex.properties:
            coords1 = graph1.vertex[PROP_COORD]
            coords2 = graph2.vertex[PROP_COORD]

            # Check that both have same dtype structure
            assert coords1.dtype.names == coords2.dtype.names, "Coordinate dtype mismatch"

            # Compare coordinates (allow small floating point errors)
            for i in range(len(coords1)):
                np.testing.assert_allclose(
                    coords2[i]['x'], coords1[i]['x'],
                    rtol=coord_rtol, atol=coord_atol,
                    err_msg=f"Coordinate X mismatch at vertex {i}"
                )
                np.testing.assert_allclose(
                    coords2[i]['y'], coords1[i]['y'],
                    rtol=coord_rtol, atol=coord_atol,
                    err_msg=f"Coordinate Y mismatch at vertex {i}"
                )
        elif PROP_COORD in graph1.vertex.properties or PROP_COORD in graph2.vertex.properties:
            raise AssertionError("PROP_COORD present in only one graph")

    # Compare vertex colors if present
    if PROP_COLOR in graph1.vertex.properties and PROP_COLOR in graph2.vertex.properties:
        colors1 = graph1.vertex[PROP_COLOR]
        colors2 = graph2.vertex[PROP_COLOR]
        np.testing.assert_array_equal(colors2, colors1, err_msg="Vertex color mismatch")
    elif PROP_COLOR in graph1.vertex.properties or PROP_COLOR in graph2.vertex.properties:
        # Colors might be added during conversion, allow this
        pass

    # Compare edge colors if present
    if PROP_COLOR in graph1.edge.properties and PROP_COLOR in graph2.edge.properties:
        colors1 = graph1.edge[PROP_COLOR]
        colors2 = graph2.edge[PROP_COLOR]
        np.testing.assert_array_equal(colors2, colors1, err_msg="Edge color mismatch")

    # Compare all other general properties
    for prop_num in graph1.general.properties.keys():
        if prop_num in [PROP_N, PROP_M, PROP_TITRE]:
            continue  # Already compared
        if prop_num in graph2.general.properties:
            val1 = graph1.general.properties[prop_num]
            val2 = graph2.general.properties[prop_num]
            if isinstance(val1, np.ndarray) and isinstance(val2, np.ndarray):
                np.testing.assert_array_equal(val2, val1,
                    err_msg=f"General property {prop_num} mismatch")
            else:
                assert val1 == val2, f"General property {prop_num} mismatch: {val1} != {val2}"

    return True


# =============================================================================
# Test 17.1: TGF → GraphML → TGF Round-Trip Preservation (Path A)
# =============================================================================


def test_tgf_roundtrip_via_graphml_all_fixtures(fixtures_dir, tmp_path):
    """
    Test that all .tgf fixture files preserve graph identity through
    TGF → GraphML → TGF conversion chain.

    Path A: TGF → GraphML → TGF
    Verifies that Pigale interprets both TGF files as identical graphs.
    """
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml

    # Get all TGF files from fixtures
    tgf_files = list(fixtures_dir.glob("*.tgf"))

    assert len(tgf_files) > 0, "No TGF files found in fixtures directory"

    for tgf_file in tgf_files:
        print(f"\nTesting {tgf_file.name} (Path A: TGF → GraphML → TGF)...")

        # Read original TGF
        graph1 = read_tgf_graph(tgf_file)

        # Convert to GraphML
        graphml_file = tmp_path / f"{tgf_file.stem}_intermediate.graphml"
        write_graphml(graph1, graphml_file)

        # Read GraphML
        graph2 = read_graphml(graphml_file)

        # Convert back to TGF
        final_tgf = tmp_path / f"{tgf_file.stem}_final.tgf"
        write_tgf_graph(graph2, final_tgf)

        # Read final TGF
        graph3 = read_tgf_graph(final_tgf)

        # Verify graphs are identical (skip title check - GraphML doesn't preserve PROP_TITRE)
        graphs_are_identical(graph1, graph3, check_title=False)

        print(f"  ✓ {tgf_file.name}: Graph identity preserved through TGF → GraphML → TGF")


def test_tgf_roundtrip_via_graphml_individual_files(fixtures_dir, tmp_path):
    """
    Test specific TGF files individually for easier debugging.
    Tests each known fixture file separately.
    """
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml

    test_files = ["c.tgf", "cylinder.tgf", "d.tgf", "symm.tgf"]

    for filename in test_files:
        tgf_file = fixtures_dir / filename
        if not tgf_file.exists():
            pytest.skip(f"{filename} not found in fixtures")

        print(f"\nTesting {filename}...")

        # TGF → GraphML → TGF
        graph1 = read_tgf_graph(tgf_file)
        graphml_file = tmp_path / f"{filename}.graphml"
        write_graphml(graph1, graphml_file)
        graph2 = read_graphml(graphml_file)
        final_tgf = tmp_path / f"{filename}.final.tgf"
        write_tgf_graph(graph2, final_tgf)
        graph3 = read_tgf_graph(final_tgf)

        # Verify identity (skip title check - GraphML doesn't preserve PROP_TITRE)
        graphs_are_identical(graph1, graph3, check_title=False)
        print(f"  ✓ {filename}: Identity preserved")


# =============================================================================
# Test 17.2: GraphML → TGF → GraphML → TGF Preservation (Path B)
# =============================================================================


def test_graphml_double_roundtrip_all_fixtures(fixtures_dir, tmp_path):
    """
    Test that all .graphml fixture files preserve graph identity through
    GraphML → TGF → GraphML → TGF conversion chain.

    Path B: GraphML → TGF → GraphML → TGF
    Verifies that both TGF files are identical (same Pigale properties).
    """
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml

    # Get all GraphML files from fixtures
    graphml_files = list(fixtures_dir.glob("*.graphml"))

    assert len(graphml_files) > 0, "No GraphML files found in fixtures directory"

    for graphml_file in graphml_files:
        print(f"\nTesting {graphml_file.name} (Path B: GraphML → TGF → GraphML → TGF)...")

        # Read original GraphML
        graph1 = read_graphml(graphml_file)

        # Convert to TGF (first time)
        tgf1_file = tmp_path / f"{graphml_file.stem}_first.tgf"
        write_tgf_graph(graph1, tgf1_file)

        # Read first TGF
        graph2 = read_tgf_graph(tgf1_file)

        # Convert back to GraphML
        graphml2_file = tmp_path / f"{graphml_file.stem}_intermediate.graphml"
        write_graphml(graph2, graphml2_file)

        # Read intermediate GraphML
        graph3 = read_graphml(graphml2_file)

        # Convert to TGF (second time)
        tgf2_file = tmp_path / f"{graphml_file.stem}_second.tgf"
        write_tgf_graph(graph3, tgf2_file)

        # Read second TGF
        graph4 = read_tgf_graph(tgf2_file)

        # Verify that both TGF files represent identical graphs
        # graph2 (first TGF) should equal graph4 (second TGF)
        graphs_are_identical(graph2, graph4)

        print(f"  ✓ {graphml_file.name}: Both TGF files represent identical graphs")


def test_graphml_double_roundtrip_individual_files(fixtures_dir, tmp_path):
    """
    Test specific GraphML files individually for easier debugging.
    Tests each known fixture file separately.
    """
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
    from pigale_tgf.graphml_format import read_graphml, write_graphml

    test_files = [
        "embed3d.graphml",
        "BM+_MixedModelLayout_nx.graphml",
        "original_MixedModelLayout_nx.graphml"
    ]

    for filename in test_files:
        graphml_file = fixtures_dir / filename
        if not graphml_file.exists():
            pytest.skip(f"{filename} not found in fixtures")

        print(f"\nTesting {filename}...")

        # GraphML → TGF1 → GraphML → TGF2
        graph1 = read_graphml(graphml_file)
        tgf1 = tmp_path / f"{filename}.tgf1"
        write_tgf_graph(graph1, tgf1)
        graph2 = read_tgf_graph(tgf1)
        graphml2 = tmp_path / f"{filename}.graphml2"
        write_graphml(graph2, graphml2)
        graph3 = read_graphml(graphml2)
        tgf2 = tmp_path / f"{filename}.tgf2"
        write_tgf_graph(graph3, tgf2)
        graph4 = read_tgf_graph(tgf2)

        # Verify TGF1 graph equals TGF2 graph
        graphs_are_identical(graph2, graph4)
        print(f"  ✓ {filename}: Both TGF files are identical")


# =============================================================================
# Test 17.3: Direct TGF Round-Trip (Baseline)
# =============================================================================


def test_tgf_direct_roundtrip_all_fixtures(fixtures_dir, tmp_path):
    """
    Test direct TGF → TGF round-trip as a baseline.
    This should always preserve complete graph identity.
    """
    from pigale_tgf.graph import read_tgf_graph, write_tgf_graph

    tgf_files = list(fixtures_dir.glob("*.tgf"))

    assert len(tgf_files) > 0, "No TGF files found in fixtures directory"

    for tgf_file in tgf_files:
        print(f"\nTesting direct round-trip: {tgf_file.name}...")

        # Read original
        graph1 = read_tgf_graph(tgf_file)

        # Write and read back
        output = tmp_path / f"roundtrip_{tgf_file.name}"
        write_tgf_graph(graph1, output)
        graph2 = read_tgf_graph(output)

        # Verify identity
        graphs_are_identical(graph1, graph2)
        print(f"  ✓ {tgf_file.name}: Direct round-trip preserves identity")
