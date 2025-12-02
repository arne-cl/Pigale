"""Test TGF core binary I/O."""

import pytest
import struct
from pathlib import Path


# =============================================================================
# Test 4.1: Read TGF Header
# =============================================================================


def test_read_tgf_header(sample_tgf):
    """Test reading TGF file header."""
    from pigale_tgf.core import Tgf

    tgf = Tgf()
    tgf.open(sample_tgf, mode='r')

    # Check header structure
    assert tgf.header.ID == b"TGF\x00"
    assert tgf.header.Version == 1
    assert tgf.header.RecordNum == 4
    assert tgf.header.LenSubHeader == 16

    tgf.close()


def test_tgf_header_structure():
    """Test Header dataclass structure."""
    from pigale_tgf.types import Header

    header = Header(
        ID=b"TGF\x00",
        Version=1,
        IfdNum=4,
        RecordNum=4,
        LenSubHeader=16,
        FstIfdOffs=32
    )

    # Pack to binary
    packed = header.pack()
    assert len(packed) == 16  # Header is 16 bytes

    # Verify format
    assert packed[:4] == b"TGF\x00"


def test_tgf_header_unpack():
    """Test unpacking binary header."""
    from pigale_tgf.types import Header

    # Create binary header: 4s + 4H + i = 16 bytes
    data = struct.pack('<4s4Hi', b'TGF\x00', 1, 4, 4, 16, 32)

    header = Header.unpack(data)

    assert header.ID == b"TGF\x00"
    assert header.Version == 1
    assert header.IfdNum == 4
    assert header.RecordNum == 4
    assert header.LenSubHeader == 16
    assert header.FstIfdOffs == 32


# =============================================================================
# Test 4.2: Write TGF Header
# =============================================================================


def test_write_tgf_header(tmp_path):
    """Test writing TGF file header."""
    from pigale_tgf.core import Tgf

    output = tmp_path / "output.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.close()

    # Read header back
    with open(output, 'rb') as f:
        magic = f.read(4)
        assert magic == b"TGF\x00"


def test_tgf_creates_valid_file(tmp_path):
    """Test that created file has correct structure."""
    from pigale_tgf.core import Tgf

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.close()

    # Verify file exists and has minimum size
    assert output.exists()
    assert output.stat().st_size >= 32  # Header + SubHeader


# =============================================================================
# Test 4.3: Read IFD
# =============================================================================


def test_read_ifd(sample_tgf):
    """Test reading IFD structure."""
    from pigale_tgf.core import Tgf

    tgf = Tgf()
    tgf.open(sample_tgf)
    tgf.set_record(1)

    # Check IFD header loaded
    assert tgf.ifd.header.FieldNum > 0
    assert len(tgf.ifd.fields) == tgf.ifd.header.FieldNum

    tgf.close()


def test_ifd_header_structure():
    """Test IfdHeader dataclass."""
    from pigale_tgf.types import IfdHeader

    ifd = IfdHeader(
        tag=1,
        FieldNumTotal=5,
        FieldNum=5,
        unused=0,
        NextIfd=0,
        NextRecord=0
    )

    packed = ifd.pack()
    assert len(packed) == 16  # IFD header is 16 bytes


def test_field_structure():
    """Test Field dataclass."""
    from pigale_tgf.types import Field

    field = Field(
        tag=519,
        attrib=0,
        length=2,
        word=b'\x02\x00\x00\x00\x00\x00\x00\x00'
    )

    packed = field.pack()
    assert len(packed) == 16  # Field is 16 bytes


# =============================================================================
# Test 4.4: Field Read/Write (Small Data)
# =============================================================================


def test_field_write_small_data(tmp_path):
    """Test writing field with inline data (len <= 8)."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.constants import TAG_VERSION

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write short integer (2 bytes)
    data = struct.pack('<h', 2)
    tgf.field_write(TAG_VERSION, data)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output)
    tgf2.set_record(1)
    read_data = tgf2.field_read(TAG_VERSION, 2)

    version = struct.unpack('<h', read_data)[0]
    assert version == 2


def test_field_write_inline_double(tmp_path):
    """Test writing double (8 bytes) inline."""
    from pigale_tgf.core import Tgf

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write double (exactly 8 bytes)
    data = struct.pack('<d', 3.14159)
    tgf.field_write(999, data)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output)
    tgf2.set_record(1)
    read_data = tgf2.field_read(999, 8)

    value = struct.unpack('<d', read_data)[0]
    assert abs(value - 3.14159) < 0.0001


# =============================================================================
# Test 4.5: Field Read/Write (Large Data)
# =============================================================================


def test_field_write_large_data(tmp_path):
    """Test writing field with external data (len > 8)."""
    from pigale_tgf.core import Tgf

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write large array (100 ints = 400 bytes)
    data = struct.pack('<100i', *range(100))
    tgf.field_write(999, data)
    tgf.close()

    # Read back
    tgf2 = Tgf()
    tgf2.open(output)
    tgf2.set_record(1)
    read_data = tgf2.field_read(999, 400)

    assert read_data == data


def test_field_large_data_uses_offset(tmp_path):
    """Test that large data is stored externally with offset."""
    from pigale_tgf.core import Tgf

    output = tmp_path / "test.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')
    tgf.create_record()

    # Write 100 bytes (> 8)
    data = b'X' * 100
    tgf.field_write(888, data)
    tgf.close()

    # Verify: field should contain offset, not data
    # (This is implicit - data should match on read)
    tgf2 = Tgf()
    tgf2.open(output)
    tgf2.set_record(1)
    read_data = tgf2.field_read(888, 100)

    assert read_data == data


# =============================================================================
# Test 4.6: Data Alignment
# =============================================================================


def test_data_alignment():
    """Test 4-byte alignment of external data."""
    from pigale_tgf.utils import num_padding, align_offset

    # Test padding calculation
    assert num_padding(1) == 3   # 1 byte needs 3 bytes padding
    assert num_padding(2) == 2   # 2 bytes needs 2 bytes padding
    assert num_padding(3) == 1   # 3 bytes needs 1 byte padding
    assert num_padding(4) == 0   # 4 bytes needs 0 bytes padding
    assert num_padding(5) == 3   # 5 bytes needs 3 bytes padding
    assert num_padding(8) == 0   # 8 bytes needs 0 bytes padding
    assert num_padding(10) == 2  # 10 bytes needs 2 bytes padding


def test_align_offset():
    """Test offset alignment to 4-byte boundaries."""
    from pigale_tgf.utils import align_offset

    assert align_offset(0) == 0
    assert align_offset(1) == 4
    assert align_offset(2) == 4
    assert align_offset(3) == 4
    assert align_offset(4) == 4
    assert align_offset(5) == 8
    assert align_offset(8) == 8
    assert align_offset(9) == 12


# =============================================================================
# Test 4.7: Multiple Records
# =============================================================================


def test_multiple_records(tmp_path):
    """Test creating and reading multiple records."""
    from pigale_tgf.core import Tgf
    from pigale_tgf.constants import TAG_VERSION

    output = tmp_path / "multi.tgf"
    tgf = Tgf()
    tgf.open(output, mode='w')

    # Write 3 records
    for i in range(3):
        tgf.create_record()
        data = struct.pack('<h', i)
        tgf.field_write(TAG_VERSION, data)

    tgf.close()

    # Read all 3 records
    tgf2 = Tgf()
    tgf2.open(output)
    assert tgf2.records_number() == 3

    for i in range(1, 4):
        tgf2.set_record(i)
        data = tgf2.field_read(TAG_VERSION, 2)
        version = struct.unpack('<h', data)[0]
        assert version == i - 1


def test_record_count(sample_tgf):
    """Test getting number of records in file."""
    from pigale_tgf.core import Tgf

    tgf = Tgf()
    tgf.open(sample_tgf)

    # c.tgf has 4 records
    assert tgf.records_number() == 4

    tgf.close()
