"""Test property serialization to TGF format."""

import pytest
import struct
import numpy as np
from pathlib import Path


# =============================================================================
# Test 6.1: PSet Structure (already tested in Phase 1, but verify)
# =============================================================================


def test_pset_creation():
    """Test PSet creation."""
    from pigale_tgf.properties import PSet

    pset = PSet(1, 10)
    assert pset.start == 1
    assert pset.finish == 10
    assert pset.size() == 10


def test_pset_property_storage():
    """Test storing properties in PSet."""
    from pigale_tgf.properties import PSet
    from pigale_tgf.constants import PROP_LABEL

    pset = PSet(1, 5)

    # Store integer array
    labels = np.array([0, 10, 20, 30, 40, 50], dtype=np.int32)
    pset[PROP_LABEL] = labels

    assert PROP_LABEL in pset.properties
    assert len(pset[PROP_LABEL]) == 6


# =============================================================================
# Test 6.2: PSet Header Serialization
# =============================================================================


def test_write_pset_header(tmp_path):
    """Test writing PSet header to TGF file."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset
    from pigale_tgf.constants import PSetHeaderTag

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Create PSet
    pset = PSet(1, 100)

    # Write PSet (SetNum=0 for general/vertex)
    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back and verify header
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    # Read header field
    header_tag = PSetHeaderTag(0)
    header_data = tgf2.field_read(header_tag, 8)

    # Unpack: 2 ints (start, finish)
    start, finish = struct.unpack('<ii', header_data)
    assert start == 1
    assert finish == 100

    tgf2.close()


# =============================================================================
# Test 6.3: Property Data Serialization
# =============================================================================


def test_write_single_property(tmp_path):
    """Test writing a single property to TGF."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset
    from pigale_tgf.constants import PROP_LABEL, PSetTag

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Create PSet with one property
    pset = PSet(1, 5)
    labels = np.array([0, 10, 20, 30, 40, 50], dtype=np.int32)  # 6 elements (0..5)
    pset[PROP_LABEL] = labels

    # Write PSet
    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    # Read property data
    prop_tag = PSetTag(0, PROP_LABEL)
    prop_data = tgf2.field_read(prop_tag, 6 * 4)  # 6 ints = 24 bytes

    # Unpack and verify
    read_labels = np.frombuffer(prop_data, dtype=np.int32)
    assert len(read_labels) == 6
    np.testing.assert_array_equal(read_labels, labels)

    tgf2.close()


def test_write_multiple_properties(tmp_path):
    """Test writing multiple properties to TGF."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset
    from pigale_tgf.constants import PROP_N, PROP_M, PSetTag

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Create general PSet with graph size info
    pset = PSet(0, -1)
    pset[PROP_N] = np.array([5], dtype=np.int32)  # nv=5
    pset[PROP_M] = np.array([7], dtype=np.int32)  # ne=7

    # Write PSet
    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back both properties
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    # Read PROP_N
    n_data = tgf2.field_read(PSetTag(0, PROP_N), 4)
    nv = struct.unpack('<i', n_data)[0]
    assert nv == 5

    # Read PROP_M
    m_data = tgf2.field_read(PSetTag(0, PROP_M), 4)
    ne = struct.unpack('<i', m_data)[0]
    assert ne == 7

    tgf2.close()


# =============================================================================
# Test 6.4: Property Round-Trip
# =============================================================================


def test_pset_round_trip(tmp_path):
    """Test complete PSet write/read round-trip."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset, read_tgf_pset
    from pigale_tgf.constants import PROP_LABEL, PROP_COLOR

    output = tmp_path / "test.tgf"

    # Write
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    pset_write = PSet(1, 10)
    pset_write[PROP_LABEL] = np.arange(11, dtype=np.int32)  # 0..10
    pset_write[PROP_COLOR] = np.ones(11, dtype=np.int16)    # All 1s

    write_tgf_pset(pset_write, tgf, set_num=0)
    tgf.close()

    # Read
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    pset_read = read_tgf_pset(tgf2, set_num=0)

    # Verify structure
    assert pset_read.start == 1
    assert pset_read.finish == 10

    # Verify properties
    assert PROP_LABEL in pset_read.properties
    assert PROP_COLOR in pset_read.properties

    np.testing.assert_array_equal(pset_read[PROP_LABEL], pset_write[PROP_LABEL])
    np.testing.assert_array_equal(pset_read[PROP_COLOR], pset_write[PROP_COLOR])

    tgf2.close()


# =============================================================================
# Test 6.5: 128 Property Limit (Bug Preservation)
# =============================================================================


def test_128_property_limit(tmp_path):
    """Test that property limit of 128 is enforced."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset, read_tgf_pset

    output = tmp_path / "test.tgf"

    # Write: Try to write 130 properties
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    pset = PSet(0, 0)

    # Create 130 properties
    for i in range(130):
        pset[i] = np.array([i], dtype=np.int32)

    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    pset_read = read_tgf_pset(tgf2, set_num=0)

    # Should only have properties 0-127 (128 total)
    assert len(pset_read.properties) == 128

    # Verify properties 0-127 exist
    for i in range(128):
        assert i in pset_read.properties
        assert pset_read[i][0] == i

    # Properties 128-129 should NOT exist
    assert 128 not in pset_read.properties
    assert 129 not in pset_read.properties

    tgf2.close()


# =============================================================================
# Test 6.6: Empty PSet Handling
# =============================================================================


def test_write_empty_pset(tmp_path):
    """Test writing PSet with no properties."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset, read_tgf_pset

    output = tmp_path / "test.tgf"

    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Empty PSet
    pset = PSet(1, 100)

    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    pset_read = read_tgf_pset(tgf2, set_num=0)

    assert pset_read.start == 1
    assert pset_read.finish == 100
    assert len(pset_read.properties) == 0

    tgf2.close()


# =============================================================================
# Test 6.7: Different Data Types
# =============================================================================


def test_pset_with_different_dtypes(tmp_path):
    """Test PSet with different numpy dtypes."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.properties import PSet, write_tgf_pset, read_tgf_pset

    output = tmp_path / "test.tgf"

    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    pset = PSet(0, 5)

    # int32 property
    pset[10] = np.array([0, 1, 2, 3, 4, 5], dtype=np.int32)

    # int16 property
    pset[11] = np.array([0, 10, 20, 30, 40, 50], dtype=np.int16)

    # float64 property (double)
    pset[12] = np.array([0.0, 1.1, 2.2, 3.3, 4.4, 5.5], dtype=np.float64)

    write_tgf_pset(pset, tgf, set_num=0)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output, mode='r')
    tgf2.set_record(1)

    pset_read = read_tgf_pset(tgf2, set_num=0)

    # Verify (dtypes preserved via raw bytes)
    np.testing.assert_array_equal(pset_read[10], pset[10])
    np.testing.assert_array_equal(pset_read[11], pset[11])
    np.testing.assert_array_almost_equal(pset_read[12], pset[12])

    tgf2.close()
