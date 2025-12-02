"""
Tests for GraphML property preservation validation.

These tests verify whether properties (especially non-Pigale properties)
are correctly preserved through GraphML round-trips.
"""

import tempfile
from pathlib import Path
import xml.etree.ElementTree as ET
import pytest

from pigale_tgf.graphml_format import read_graphml, write_graphml
from pigale_tgf.graph import read_tgf_graph, write_tgf_graph
from pigale_tgf.constants import PROP_N, PROP_M
import numpy as np


def get_pset1_value(pset, prop_num):
    """Extract scalar value from PSet1 property."""
    val = pset[prop_num]
    return int(val[0]) if isinstance(val, np.ndarray) else int(val)


def test_large_graphml_original_keys():
    """Test what keys are defined in the original large GraphML file."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    # Parse original
    tree = ET.parse(graphml_file)
    root = tree.getroot()

    # Count keys
    keys = root.findall(".//{http://graphml.graphdrawing.org/xmlns}key")
    assert len(keys) > 0, "No keys found in GraphML"

    # Categorize keys
    pigale_keys = []
    other_keys = []

    for key in keys:
        key_id = key.get('id')
        if key_id.startswith('Pigale/'):
            pigale_keys.append(key_id)
        else:
            other_keys.append(key_id)

    print(f"\nKey analysis:")
    print(f"  Total keys: {len(keys)}")
    print(f"  Pigale keys: {len(pigale_keys)}")
    print(f"  Non-Pigale keys: {len(other_keys)}")

    if other_keys:
        print(f"  Non-Pigale key IDs: {other_keys}")


def test_large_graphml_data_element_count_via_tgf():
    """Test data loss when round-tripping through TGF format.

    EXPECTED BEHAVIOR: Data loss is expected because TGF can only store
    Pigale properties, not arbitrary GraphML properties.
    """
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    # Count data elements in original
    tree1 = ET.parse(graphml_file)
    root1 = tree1.getroot()
    data_elements_original = root1.findall(".//{http://graphml.graphdrawing.org/xmlns}data")
    count_original = len(data_elements_original)

    # Round-trip through TGF (THIS LOSES DATA - EXPECTED!)
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # Read, write to TGF, read back, write to GraphML
        graph1 = read_graphml(graphml_file)
        write_tgf_graph(graph1, tgf_file)
        graph2 = read_tgf_graph(tgf_file)
        write_graphml(graph2, graphml_file2)

        # Count data elements in output
        tree2 = ET.parse(graphml_file2)
        root2 = tree2.getroot()
        data_elements_output = root2.findall(".//{http://graphml.graphdrawing.org/xmlns}data")
        count_output = len(data_elements_output)

    print(f"\nData elements through TGF round-trip:")
    print(f"  Original: {count_original:,}")
    print(f"  After GraphML→TGF→GraphML: {count_output:,}")
    print(f"  Loss: {count_original - count_output:,} ({100 * (1 - count_output/count_original):.1f}%)")
    print(f"\nThis is EXPECTED - TGF format cannot store arbitrary GraphML properties.")

    # This documents that TGF cannot preserve non-Pigale properties
    # We expect massive data loss here
    assert count_output < count_original, "Should lose non-Pigale data through TGF"


def test_large_graphml_file_size_comparison_via_tgf():
    """Test file size changes through TGF round-trip.

    EXPECTED BEHAVIOR: File size reduction is expected because TGF
    cannot store arbitrary GraphML properties.
    """
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    size_original = graphml_file.stat().st_size

    # Round-trip through TGF
    with tempfile.TemporaryDirectory() as tmpdir:
        tgf_file = Path(tmpdir) / "test.tgf"
        graphml_file2 = Path(tmpdir) / "test.graphml"

        graph1 = read_graphml(graphml_file)
        write_tgf_graph(graph1, tgf_file)
        graph2 = read_tgf_graph(tgf_file)
        write_graphml(graph2, graphml_file2)

        size_output = graphml_file2.stat().st_size

    print(f"\nFile size through TGF round-trip:")
    print(f"  Original: {size_original:,} bytes ({size_original/1024/1024:.1f} MB)")
    print(f"  After GraphML→TGF→GraphML: {size_output:,} bytes ({size_output/1024/1024:.1f} MB)")
    print(f"  Ratio: {size_original/size_output:.1f}x smaller")
    print(f"\nThis is EXPECTED - TGF format cannot store arbitrary GraphML properties.")

    # Document that TGF round-trip reduces file size
    ratio = size_original / size_output
    assert ratio > 2.0, f"Expected significant size reduction through TGF, got {ratio:.1f}x"


def test_graphml_to_graphml_preserves_all_properties():
    """Test that GraphML → GraphML (without TGF) preserves ALL properties."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    # Count data elements in original
    tree1 = ET.parse(graphml_file)
    root1 = tree1.getroot()
    data_elements_original = root1.findall(".//{http://graphml.graphdrawing.org/xmlns}data")
    count_original = len(data_elements_original)

    # Direct GraphML → GraphML (no TGF intermediate)
    with tempfile.TemporaryDirectory() as tmpdir:
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # Read and write directly
        graph1 = read_graphml(graphml_file)
        write_graphml(graph1, graphml_file2)

        # Count data elements in output
        tree2 = ET.parse(graphml_file2)
        root2 = tree2.getroot()
        data_elements_output = root2.findall(".//{http://graphml.graphdrawing.org/xmlns}data")
        count_output = len(data_elements_output)

    print(f"\nDirect GraphML→GraphML (no TGF):")
    print(f"  Original: {count_original:,}")
    print(f"  Output: {count_output:,}")

    if count_output == count_original:
        print(f"  ✓ All properties preserved!")
    else:
        print(f"  Loss: {count_original - count_output:,} ({100 * (1 - count_output/count_original):.1f}%)")

    # GraphML → GraphML should preserve everything
    assert count_output == count_original, \
        f"Direct GraphML round-trip lost data: {count_original:,} → {count_output:,}"


def test_graphml_reader_parses_all_keys():
    """Test that GraphML reader now parses ALL keys (not just Pigale ones)."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    # Parse with our reader
    graph = read_graphml(graphml_file)

    # Check that _graphml_data attribute exists
    assert hasattr(graph, '_graphml_data'), "Graph should have _graphml_data attribute"

    graphml_data = getattr(graph, '_graphml_data')
    assert 'keys' in graphml_data, "Should store key definitions"
    assert 'node_data' in graphml_data, "Should store node data"
    assert 'edge_data' in graphml_data, "Should store edge data"

    # Parse manually to see what keys exist
    tree = ET.parse(graphml_file)
    root = tree.getroot()

    keys = root.findall(".//{http://graphml.graphdrawing.org/xmlns}key")
    all_key_ids = [k.get('id') for k in keys]

    stored_key_ids = list(graphml_data['keys'].keys())

    print(f"\nKey preservation:")
    print(f"  Keys in original: {len(all_key_ids)}")
    print(f"  Keys stored: {len(stored_key_ids)}")

    # All keys should be stored
    assert set(stored_key_ids) == set(all_key_ids), \
        f"Should store all keys. Missing: {set(all_key_ids) - set(stored_key_ids)}"


def test_graphml_to_graphml_file_size_preserved():
    """Test that direct GraphML → GraphML preserves file size."""
    graphml_file = Path(__file__).parent / "fixtures" / "BM+_MixedModelLayout_nx.graphml"

    if not graphml_file.exists():
        pytest.skip("Large GraphML file not found")

    with tempfile.TemporaryDirectory() as tmpdir:
        graphml_file2 = Path(tmpdir) / "test.graphml"

        # Direct GraphML → GraphML (no TGF)
        graph1 = read_graphml(graphml_file)
        write_graphml(graph1, graphml_file2)

        # Compare file sizes
        size1 = graphml_file.stat().st_size
        size2 = graphml_file2.stat().st_size

        size_ratio = max(size1, size2) / min(size1, size2)

        print(f"\nDirect GraphML→GraphML file size:")
        print(f"  Original: {size1:,} bytes ({size1/1024/1024:.1f} MB)")
        print(f"  Output: {size2:,} bytes ({size2/1024/1024:.1f} MB)")
        print(f"  Ratio: {size_ratio:.2f}x")

        # File sizes should be very similar (within 10% for formatting differences)
        assert size_ratio < 1.1, \
            f"File sizes differ significantly: {size1:,} vs {size2:,} ({size_ratio:.2f}x)"


if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])
