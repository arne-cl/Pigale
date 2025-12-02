"""Test constants definitions."""

import pytest


def test_tag_constants_defined():
    """Test that TGF tag constants are defined."""
    from pigale_tgf.constants import (
        TAG_NAME,
        TAG_VERSION,
        TAG_N,
        TAG_M,
        TAG_VCOORD,
        TAG_VIN,
    )

    # Legacy tags (we won't use but should define for reference)
    assert TAG_NAME == 512
    assert TAG_N == 513
    assert TAG_M == 514

    # Modern tags (version 2)
    assert TAG_VERSION == 519
    assert TAG_VCOORD == 520
    assert TAG_VIN == 522


def test_property_constants_defined():
    """Test that property ID constants are defined."""
    from pigale_tgf.constants import (
        PROP_TITRE,
        PROP_N,
        PROP_M,
        PROP_COORD,
        PROP_VIN,
        PROP_CIR,
        PROP_ACIR,
        PROP_LABEL,
        PROP_COLOR,
    )

    # General properties
    assert PROP_TITRE == 0
    assert PROP_N == 7
    assert PROP_M == 8

    # Vertex properties
    assert PROP_COORD == 16
    assert PROP_LABEL == 0  # Same number, different set

    # Brin properties
    assert PROP_VIN == 16  # Same number as PROP_COORD, different set
    assert PROP_CIR == 18
    assert PROP_ACIR == 19

    # Color (can be on vertices or edges)
    assert PROP_COLOR == 1


def test_magic_prop_constant():
    """Test MAGIC_PROP constant."""
    from pigale_tgf.constants import MAGIC_PROP

    assert MAGIC_PROP == 0x1000


def test_pset_tag_functions():
    """Test property set tag encoding functions."""
    from pigale_tgf.constants import PSetHeaderTag, PSetTag, PSet1Tag

    # Header tags
    assert PSetHeaderTag(0) == 0x1000  # General set
    assert PSetHeaderTag(1) == 0x1100  # Set 1
    assert PSetHeaderTag(2) == 0x1200  # Set 2

    # Property tags
    assert PSetTag(0, 5) == 0x1405  # Set 0, property 5
    assert PSetTag(1, 10) == 0x150A  # Set 1, property 10

    # PSet1 tags
    assert PSet1Tag(0, 3) == 0x1803  # Set 0, property 3


def test_ifd_tag_constants():
    """Test IFD tag constants."""
    from pigale_tgf.constants import TAG_FIRST, TAG_NEXT

    assert TAG_FIRST == 1
    assert TAG_NEXT == 2
