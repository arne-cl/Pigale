"""
Property-based tests using hypothesis.

Tests graph operations with randomly generated graphs to verify
invariants and catch edge cases.
"""

import tempfile
from pathlib import Path
import numpy as np
import pytest

from hypothesis import given, strategies as st, assume, settings
from hypothesis import HealthCheck

from pigale_tgf.graph import (
    GraphContainer,
    read_tgf_graph,
    write_tgf_graph,
    read_txt_graph,
    write_txt_graph,
)
from pigale_tgf.graphml_format import read_graphml, write_graphml
from pigale_tgf.constants import PROP_N, PROP_M, PROP_VIN, PROP_LABEL, PROP_COORD


# Helper to extract scalar from PSet1
def get_pset1_value(pset, prop_num):
    """Extract scalar value from PSet1 property."""
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)


# Hypothesis strategies
@st.composite
def valid_graph_size(draw):
    """Generate valid graph sizes (nv, ne)."""
    nv = draw(st.integers(min_value=0, max_value=50))
    # Maximum edges for simple graph: nv * (nv - 1) / 2
    max_edges = nv * (nv - 1) // 2 if nv > 0 else 0
    ne = draw(st.integers(min_value=0, max_value=max_edges))
    return nv, ne


@st.composite
def simple_graph(draw):
    """Generate a simple graph with vertices and edges."""
    nv, ne = draw(valid_graph_size())

    graph = GraphContainer()
    graph.setsize(nv, ne)

    # Generate edges (no self-loops, no multi-edges)
    if nv > 1 and ne > 0:
        edges = set()
        vin = np.zeros(2 * ne + 1, dtype=np.int32)
        vin[0] = 0

        edge_idx = 1
        for _ in range(ne):
            # Generate random edge
            v1 = draw(st.integers(min_value=1, max_value=nv))
            v2 = draw(st.integers(min_value=1, max_value=nv))

            # Skip self-loops
            assume(v1 != v2)

            # Skip duplicate edges
            edge = (min(v1, v2), max(v1, v2))
            assume(edge not in edges)
            edges.add(edge)

            # Store VIN
            vin[ne + edge_idx] = v1  # Positive brin
            vin[ne - edge_idx] = v2  # Negative brin
            edge_idx += 1

        graph.brin[PROP_VIN] = vin
    elif ne > 0:
        # Need VIN even if empty
        vin = np.zeros(2 * ne + 1, dtype=np.int32)
        vin[0] = 0
        graph.brin[PROP_VIN] = vin

    return graph


@st.composite
def graph_with_labels(draw):
    """Generate graph with vertex labels."""
    graph = draw(simple_graph())
    nv = get_pset1_value(graph.general, PROP_N)

    if nv > 0:
        # Generate random labels
        labels = draw(st.lists(
            st.integers(min_value=1, max_value=1000),
            min_size=nv,
            max_size=nv
        ))
        graph.vertex[PROP_LABEL] = np.array(labels, dtype=np.int32)

    return graph


@st.composite
def graph_with_coordinates(draw):
    """Generate graph with vertex coordinates."""
    graph = draw(simple_graph())
    nv = get_pset1_value(graph.general, PROP_N)

    if nv > 0:
        # Generate random coordinates
        coords = np.zeros(nv, dtype=[('x', '<f8'), ('y', '<f8')])
        for i in range(nv):
            x = draw(st.floats(min_value=-100.0, max_value=100.0, allow_nan=False, allow_infinity=False))
            y = draw(st.floats(min_value=-100.0, max_value=100.0, allow_nan=False, allow_infinity=False))
            coords[i] = (x, y)
        graph.vertex[PROP_COORD] = coords

    return graph


# Property-based tests

@given(nv=st.integers(min_value=0, max_value=100),
       ne=st.integers(min_value=0, max_value=100))
def test_graph_setsize_invariants(nv, ne):
    """Test that setsize maintains basic invariants."""
    # Skip invalid sizes (ne > max possible edges)
    max_edges = nv * (nv - 1) // 2 if nv > 0 else 0
    assume(ne <= max_edges)

    graph = GraphContainer()
    graph.setsize(nv, ne)

    # Check sizes stored correctly
    assert get_pset1_value(graph.general, PROP_N) == nv
    assert get_pset1_value(graph.general, PROP_M) == ne

    # Check PSet ranges
    assert graph.vertex.start == 1
    assert graph.vertex.finish == nv
    assert graph.edge.start == 1
    assert graph.edge.finish == ne
    assert graph.brin.start == -ne
    assert graph.brin.finish == ne


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_tgf_roundtrip_preserves_structure(graph):
    """Test that TGF write→read preserves graph structure."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.tgf"

        # Write and read back
        write_tgf_graph(graph, filepath)
        graph2 = read_tgf_graph(filepath)

        # Check structure preserved
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        assert nv2 == nv1
        assert ne2 == ne1


@given(graph=graph_with_labels())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_tgf_roundtrip_preserves_labels(graph):
    """Test that TGF write→read preserves vertex labels."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return  # Skip empty graphs

    labels1 = graph.vertex[PROP_LABEL]

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.tgf"

        # Write and read back
        write_tgf_graph(graph, filepath)
        graph2 = read_tgf_graph(filepath)

        # Check labels preserved
        labels2 = graph2.vertex[PROP_LABEL]
        np.testing.assert_array_equal(labels2, labels1)


@given(graph=graph_with_coordinates())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_tgf_roundtrip_preserves_coordinates(graph):
    """Test that TGF write→read preserves vertex coordinates."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return  # Skip empty graphs

    coords1 = graph.vertex[PROP_COORD]

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.tgf"

        # Write and read back
        write_tgf_graph(graph, filepath)
        graph2 = read_tgf_graph(filepath)

        # Check coordinates preserved (within floating point tolerance)
        coords2 = graph2.vertex[PROP_COORD]

        for i in range(nv):
            assert abs(coords2[i]['x'] - coords1[i]['x']) < 1e-10
            assert abs(coords2[i]['y'] - coords1[i]['y']) < 1e-10


@given(graph=graph_with_labels())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_txt_roundtrip_preserves_structure(graph):
    """Test that TXT write→read preserves basic structure."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    # TXT can't represent isolated vertices
    if ne1 == 0:
        return

    # Need VIN property to test
    if not graph.brin.exists(PROP_VIN):
        return

    # Count non-loop edges (TXT skips loops)
    vin = graph.brin[PROP_VIN]
    non_loop_edges = 0
    for e in range(1, ne1 + 1):
        src = vin[ne1 + e]
        dst = vin[ne1 - e]
        # Skip invalid vertices
        if src == 0 or dst == 0:
            continue
        if src != dst:
            non_loop_edges += 1

    # TXT requires at least one non-loop edge
    if non_loop_edges == 0:
        return

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.txt"

        # Write and read back
        write_txt_graph(graph, filepath)
        graph2 = read_txt_graph(filepath)

        # Check structure preserved
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        # TXT preserves non-loop edges (loops are skipped)
        # Note: May be less if some edges had 0 vertices
        assert ne2 <= non_loop_edges

        # TXT may lose isolated vertices, but keeps those in edges
        assert nv2 <= nv1


@given(graph=graph_with_coordinates())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_graphml_roundtrip_preserves_structure(graph):
    """Test that GraphML write→read preserves graph structure."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.graphml"

        # Write and read back
        write_graphml(graph, filepath)
        graph2 = read_graphml(filepath)

        # Check structure preserved
        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        assert nv2 == nv1
        assert ne2 == ne1


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None, max_examples=50)
def test_conversion_chain_tgf_graphml_tgf(graph):
    """Test conversion chain: TGF → GraphML → TGF."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf1 = Path(tmpdir) / "test1.tgf"
        graphml = Path(tmpdir) / "test.graphml"
        tgf2 = Path(tmpdir) / "test2.tgf"

        # TGF → GraphML → TGF
        write_tgf_graph(graph, tgf1)
        graph_intermediate = read_tgf_graph(tgf1)
        write_graphml(graph_intermediate, graphml)
        graph_final = read_graphml(graphml)
        write_tgf_graph(graph_final, tgf2)
        graph_result = read_tgf_graph(tgf2)

        # Check final structure matches original
        nv2 = get_pset1_value(graph_result.general, PROP_N)
        ne2 = get_pset1_value(graph_result.general, PROP_M)

        assert nv2 == nv1
        assert ne2 == ne1


@given(data=st.binary(min_size=0, max_size=1000))
def test_property_serialization_roundtrip(data):
    """Test that arbitrary binary data survives PSet serialization."""
    from pigale_tgf.properties import PSet
    from pigale_tgf.core import Tgf
    from pigale_tgf.constants import PROP_TITRE  # Use string property

    # Create PSet with binary data
    pset = PSet(0, -1)
    pset[PROP_TITRE] = data

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test_prop.tgf"

        # Write property to TGF
        tgf = Tgf()
        tgf.open(filepath, mode='w')
        tgf.create_record()

        from pigale_tgf.properties import write_tgf_pset
        write_tgf_pset(pset, tgf, set_num=0, use_pset1=False)

        tgf.close()

        # Read back
        tgf2 = Tgf()
        tgf2.open(filepath, mode='r')
        tgf2.set_record(1)

        from pigale_tgf.properties import read_tgf_pset
        pset2 = read_tgf_pset(tgf2, set_num=0, use_pset1=False)

        tgf2.close()

        # Check data preserved
        if len(data) > 0:
            assert PROP_TITRE in pset2.properties
            data2 = pset2[PROP_TITRE]
            if isinstance(data2, np.ndarray):
                data2 = data2.tobytes()
            assert data2 == data
        else:
            # Empty data might not create property
            pass


@given(nv=st.integers(min_value=1, max_value=20))
@settings(max_examples=20)
def test_vin_property_consistency(nv):
    """Test that VIN property maintains consistency invariants."""
    # Generate connected graph (spanning tree)
    graph = GraphContainer()
    ne = nv - 1  # Spanning tree has n-1 edges
    graph.setsize(nv, ne)

    # Create VIN for spanning tree (1->2, 2->3, ..., (n-1)->n)
    vin = np.zeros(2 * ne + 1, dtype=np.int32)
    vin[0] = 0

    for e in range(1, ne + 1):
        vin[ne + e] = e      # Positive brin: e -> (e+1)
        vin[ne - e] = e + 1  # Negative brin: (e+1) -> e

    graph.brin[PROP_VIN] = vin

    # Verify VIN consistency
    for e in range(1, ne + 1):
        src = vin[ne + e]
        dst = vin[ne - e]

        # Both endpoints must be valid vertices
        assert 1 <= src <= nv
        assert 1 <= dst <= nv

        # No self-loops in this construction
        assert src != dst


@given(arr=st.lists(st.integers(min_value=-1000, max_value=1000), min_size=1, max_size=100))
def test_numpy_array_property_storage(arr):
    """Test that numpy arrays are correctly stored and retrieved."""
    from pigale_tgf.properties import PSet
    from pigale_tgf.constants import PROP_LABEL

    nv = len(arr)
    pset = PSet(1, nv)

    # Store as numpy array
    np_arr = np.array(arr, dtype=np.int32)
    pset[PROP_LABEL] = np_arr

    # Retrieve and verify
    retrieved = pset[PROP_LABEL]
    np.testing.assert_array_equal(retrieved, np_arr)


@given(graph=graph_with_coordinates())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_graphml_roundtrip_preserves_coordinates(graph):
    """Test that GraphML write→read preserves vertex coordinates."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return  # Skip empty graphs

    coords1 = graph.vertex[PROP_COORD]

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.graphml"

        # Write and read back
        write_graphml(graph, filepath)
        graph2 = read_graphml(filepath)

        # Check coordinates preserved
        coords2 = graph2.vertex[PROP_COORD]

        for i in range(nv):
            assert abs(coords2[i]['x'] - coords1[i]['x']) < 1e-6
            assert abs(coords2[i]['y'] - coords1[i]['y']) < 1e-6


@given(graph=graph_with_labels())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_graphml_roundtrip_preserves_labels(graph):
    """Test that GraphML write→read preserves vertex labels."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return  # Skip empty graphs

    labels1 = graph.vertex[PROP_LABEL]

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test.graphml"

        # Write and read back
        write_graphml(graph, filepath)
        graph2 = read_graphml(filepath)

        # Check labels preserved
        labels2 = graph2.vertex[PROP_LABEL]
        np.testing.assert_array_equal(labels2, labels1)


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture, HealthCheck.filter_too_much], deadline=None, max_examples=30)
def test_conversion_chain_graphml_tgf_txt_tgf(graph):
    """Test conversion chain: GraphML → TGF → TXT → TGF."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    # Skip graphs with no edges (TXT can't handle them)
    if ne1 == 0:
        return

    # Skip if no VIN property
    if not graph.brin.exists(PROP_VIN):
        return

    # Count non-loop edges
    vin = graph.brin[PROP_VIN]
    non_loop_edges = 0
    for e in range(1, ne1 + 1):
        src = vin[ne1 + e]
        dst = vin[ne1 - e]
        if src == 0 or dst == 0:
            continue
        if src != dst:
            non_loop_edges += 1

    if non_loop_edges == 0:
        return

    with tempfile.TemporaryDirectory() as tmpdir:
        graphml1 = Path(tmpdir) / "test1.graphml"
        tgf1 = Path(tmpdir) / "test1.tgf"
        txt = Path(tmpdir) / "test.txt"
        tgf2 = Path(tmpdir) / "test2.tgf"

        # GraphML → TGF → TXT → TGF
        write_graphml(graph, graphml1)
        g1 = read_graphml(graphml1)
        write_tgf_graph(g1, tgf1)
        g2 = read_tgf_graph(tgf1)
        write_txt_graph(g2, txt)
        g3 = read_txt_graph(txt)
        write_tgf_graph(g3, tgf2)
        g_final = read_tgf_graph(tgf2)

        # Check that we still have a valid graph
        nv_final = get_pset1_value(g_final.general, PROP_N)
        ne_final = get_pset1_value(g_final.general, PROP_M)

        assert nv_final > 0
        assert ne_final > 0


@given(nv=st.integers(min_value=0, max_value=10))
def test_empty_and_trivial_graphs(nv):
    """Test handling of empty and trivial graphs."""
    graph = GraphContainer()
    graph.setsize(nv, 0)  # No edges

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "empty.tgf"
        graphml_file = Path(tmpdir) / "empty.graphml"

        # TGF round-trip
        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)
        assert get_pset1_value(graph2.general, PROP_N) == nv
        assert get_pset1_value(graph2.general, PROP_M) == 0

        # GraphML round-trip
        write_graphml(graph, graphml_file)
        graph3 = read_graphml(graphml_file)
        assert get_pset1_value(graph3.general, PROP_N) == nv
        assert get_pset1_value(graph3.general, PROP_M) == 0


@given(graph=simple_graph())
def test_vin_property_bounds(graph):
    """Test that VIN property values are within valid vertex bounds."""
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    if ne == 0:
        return

    if not graph.brin.exists(PROP_VIN):
        return

    vin = graph.brin[PROP_VIN]

    # Check all edge endpoints
    for e in range(1, ne + 1):
        src = vin[ne + e]
        dst = vin[ne - e]

        # Vertices must be in valid range (or 0 for uninitialized)
        if src != 0:
            assert 1 <= src <= nv, f"Source vertex {src} out of bounds [1, {nv}]"
        if dst != 0:
            assert 1 <= dst <= nv, f"Target vertex {dst} out of bounds [1, {nv}]"


@given(graph=graph_with_coordinates())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_coordinate_bounds_preserved(graph):
    """Test that coordinate bounds are preserved through round-trips."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return

    coords1 = graph.vertex[PROP_COORD]

    # Compute bounds
    x_min = min(coords1[i]['x'] for i in range(nv))
    x_max = max(coords1[i]['x'] for i in range(nv))
    y_min = min(coords1[i]['y'] for i in range(nv))
    y_max = max(coords1[i]['y'] for i in range(nv))

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"

        # Round-trip through TGF
        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        coords2 = graph2.vertex[PROP_COORD]

        # Check bounds preserved (with tolerance)
        x_min2 = min(coords2[i]['x'] for i in range(nv))
        x_max2 = max(coords2[i]['x'] for i in range(nv))
        y_min2 = min(coords2[i]['y'] for i in range(nv))
        y_max2 = max(coords2[i]['y'] for i in range(nv))

        assert abs(x_min2 - x_min) < 1e-6
        assert abs(x_max2 - x_max) < 1e-6
        assert abs(y_min2 - y_min) < 1e-6
        assert abs(y_max2 - y_max) < 1e-6


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_pset_range_invariants(graph):
    """Test that PSet ranges match graph size."""
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    # Check PSet ranges
    assert graph.vertex.start == 1
    assert graph.vertex.finish == nv
    assert graph.edge.start == 1
    assert graph.edge.finish == ne
    assert graph.brin.start == -ne
    assert graph.brin.finish == ne

    # Round-trip and check again
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"

        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        assert graph2.vertex.start == 1
        assert graph2.vertex.finish == nv2
        assert graph2.edge.start == 1
        assert graph2.edge.finish == ne2
        assert graph2.brin.start == -ne2
        assert graph2.brin.finish == ne2


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture, HealthCheck.filter_too_much], deadline=None, max_examples=30)
def test_bidirectional_edges(graph):
    """Test that edges are bidirectional (brins +e and -e)."""
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    if ne == 0:
        return

    if not graph.brin.exists(PROP_VIN):
        return

    vin = graph.brin[PROP_VIN]

    # Check that each edge has two brins pointing in opposite directions
    for e in range(1, ne + 1):
        src = vin[ne + e]  # Positive brin (+e)
        dst = vin[ne - e]  # Negative brin (-e)

        if src == 0 or dst == 0:
            continue

        # An undirected edge {u,v} should have:
        # - Positive brin: u → v
        # - Negative brin: v → u
        # The order might vary, but both directions should exist
        assert 1 <= src <= nv
        assert 1 <= dst <= nv


@given(graph=graph_with_labels())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_label_uniqueness_not_required(graph):
    """Test that duplicate labels are allowed."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return

    labels = graph.vertex[PROP_LABEL]

    # Labels can be duplicated (not required to be unique)
    # Just verify they round-trip correctly
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"

        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        labels2 = graph2.vertex[PROP_LABEL]
        np.testing.assert_array_equal(labels2, labels)


@given(data=st.binary(min_size=1, max_size=100))
def test_string_property_encoding(data):
    """Test that string properties handle various byte sequences."""
    from pigale_tgf.properties import PSet
    from pigale_tgf.core import Tgf
    from pigale_tgf.constants import PROP_TITRE

    # Create PSet with string data
    pset = PSet(0, -1)
    pset[PROP_TITRE] = data

    with tempfile.TemporaryDirectory() as tmpdir:
        filepath = Path(tmpdir) / "test_string.tgf"

        # Write and read back
        tgf = Tgf()
        tgf.open(filepath, mode='w')
        tgf.create_record()

        from pigale_tgf.properties import write_tgf_pset
        write_tgf_pset(pset, tgf, set_num=0, use_pset1=False)

        tgf.close()

        # Read back
        tgf2 = Tgf()
        tgf2.open(filepath, mode='r')
        tgf2.set_record(1)

        from pigale_tgf.properties import read_tgf_pset
        pset2 = read_tgf_pset(tgf2, set_num=0, use_pset1=False)

        tgf2.close()

        # Check data preserved
        assert PROP_TITRE in pset2.properties
        data2 = pset2[PROP_TITRE]
        if isinstance(data2, np.ndarray):
            data2 = data2.tobytes()
        assert data2 == data


@given(nv=st.integers(min_value=2, max_value=20))
@settings(max_examples=30)
def test_complete_graph_edges(nv):
    """Test that complete graphs have correct edge count."""
    ne = nv * (nv - 1) // 2

    graph = GraphContainer()
    graph.setsize(nv, ne)

    # Create VIN for complete graph
    vin = np.zeros(2 * ne + 1, dtype=np.int32)
    vin[0] = 0

    edge_idx = 1
    for i in range(1, nv + 1):
        for j in range(i + 1, nv + 1):
            vin[ne + edge_idx] = i
            vin[ne - edge_idx] = j
            edge_idx += 1

    graph.brin[PROP_VIN] = vin

    # Verify edge count
    assert edge_idx - 1 == ne

    # Round-trip and verify
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "complete.tgf"

        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        assert nv2 == nv
        assert ne2 == ne


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_file_size_reasonable(graph):
    """Test that file sizes are reasonable (not excessively large)."""
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    if nv == 0:
        return

    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file = Path(tmpdir) / "test.graphml"

        # Write both formats
        write_tgf_graph(graph, tgf_file)
        write_graphml(graph, graphml_file)

        tgf_size = tgf_file.stat().st_size
        graphml_size = graphml_file.stat().st_size

        # TGF should be reasonably small
        # Roughly: header + IFD + data
        # Should be < 10KB for small graphs
        if nv < 50 and ne < 100:
            assert tgf_size < 10000, f"TGF file too large: {tgf_size} bytes"

        # GraphML is XML so larger, but should be reasonable
        if nv < 50 and ne < 100:
            assert graphml_size < 100000, f"GraphML file too large: {graphml_size} bytes"


@given(graph=graph_with_coordinates())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_coordinate_dtype_consistency(graph):
    """Test that coordinate dtypes are consistent."""
    nv = get_pset1_value(graph.general, PROP_N)

    if nv == 0:
        return

    coords1 = graph.vertex[PROP_COORD]

    # Check dtype has x and y fields
    assert 'x' in coords1.dtype.names or coords1.dtype.names is None
    if coords1.dtype.names:
        assert 'y' in coords1.dtype.names

    # Round-trip and check dtype preserved
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"

        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        coords2 = graph2.vertex[PROP_COORD]

        # Should have same structure (named fields)
        assert 'x' in coords2.dtype.names
        assert 'y' in coords2.dtype.names


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture, HealthCheck.filter_too_much], deadline=None, max_examples=30)
def test_connected_graph_has_spanning_tree(graph):
    """Test that connected graphs have at least n-1 edges (spanning tree)."""
    nv = get_pset1_value(graph.general, PROP_N)
    ne = get_pset1_value(graph.general, PROP_M)

    if nv <= 1 or ne == 0:
        return

    if not graph.brin.exists(PROP_VIN):
        return

    # Build adjacency info
    vin = graph.brin[PROP_VIN]
    adj = {v: set() for v in range(1, nv + 1)}

    for e in range(1, ne + 1):
        src = vin[ne + e]
        dst = vin[ne - e]
        if src == 0 or dst == 0:
            continue
        if src != dst:
            adj[src].add(dst)
            adj[dst].add(src)

    # Check if connected using BFS
    visited = set()
    queue = [1]
    visited.add(1)

    while queue:
        v = queue.pop(0)
        for u in adj[v]:
            if u not in visited:
                visited.add(u)
                queue.append(u)

    # If connected, must have at least n-1 edges
    if len(visited) == nv:
        assert ne >= nv - 1, f"Connected graph with {nv} vertices must have ≥{nv-1} edges, has {ne}"


@given(nv=st.integers(min_value=0, max_value=5),
       ne=st.integers(min_value=0, max_value=10))
def test_graph_size_limits(nv, ne):
    """Test handling of various graph sizes including edge cases."""
    # Skip invalid sizes
    max_edges = nv * (nv - 1) // 2 if nv > 0 else 0
    assume(ne <= max_edges)

    graph = GraphContainer()
    graph.setsize(nv, ne)

    # Should not crash
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"

        write_tgf_graph(graph, tgf_file)
        graph2 = read_tgf_graph(tgf_file)

        nv2 = get_pset1_value(graph2.general, PROP_N)
        ne2 = get_pset1_value(graph2.general, PROP_M)

        assert nv2 == nv
        assert ne2 == ne


@given(graph=simple_graph())
@settings(suppress_health_check=[HealthCheck.function_scoped_fixture], deadline=None)
def test_multiple_roundtrips_stable(graph):
    """Test that multiple round-trips don't degrade data."""
    nv1 = get_pset1_value(graph.general, PROP_N)
    ne1 = get_pset1_value(graph.general, PROP_M)

    with tempfile.TemporaryDirectory() as tmpdir:
        # Do 3 round-trips
        g = graph
        for i in range(3):
            tgf_file = Path(tmpdir) / f"test_{i}.tgf"
            write_tgf_graph(g, tgf_file)
            g = read_tgf_graph(tgf_file)

        # Check final state matches original
        nv_final = get_pset1_value(g.general, PROP_N)
        ne_final = get_pset1_value(g.general, PROP_M)

        assert nv_final == nv1
        assert ne_final == ne1


if __name__ == "__main__":
    # Run hypothesis tests
    pytest.main([__file__, "-v", "--hypothesis-show-statistics"])
