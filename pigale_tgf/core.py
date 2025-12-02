"""
TGF core binary I/O.

Low-level TGF file operations matching C++ Tgf class behavior.
"""

from pathlib import Path
from typing import Union, Optional
import os

from pigale_tgf.types import Header, IfdHeader, Field, Ifd
from pigale_tgf.constants import TAG_FIRST, TAG_NEXT
from pigale_tgf.utils import num_padding


class Tgf:
    """
    TGF file reader/writer.

    Manages low-level binary I/O for TGF format files.
    Matches behavior of C++ Tgf class in tgraph/Tgf.cpp
    """

    def __init__(self):
        """Create TGF file handler."""
        self.header = Header(
            ID=b"TGF\x00",
            Version=1,
            IfdNum=0,
            RecordNum=0,
            LenSubHeader=16,
            FstIfdOffs=0
        )
        self.ifd = Ifd()
        self.sub_header = b'.' * 16  # Default SubHeader
        self.is_open = False
        self.is_good = True
        self.stream: Optional[object] = None
        self.mode = 'r'

        # Offset tracking
        self.ifd_offsets = {}  # record_num -> offset
        self.current_ifd_offset = 0
        self.offset_new_data = 0

        # State flags
        self.new_ifd = False
        self.new_data = False
        self.seek_flag = False

    def open(self, filename: Union[str, Path], mode: str = 'r'):
        """
        Open TGF file.

        Args:
            filename: Path to TGF file
            mode: 'r' for read (old), 'w' for write (create)

        Returns:
            Number of records (for read mode) or 1 (for write mode)
        """
        filename = Path(filename)
        self.mode = mode

        if mode == 'r':
            # Open for reading
            if not filename.exists():
                return 0

            self.stream = open(filename, 'r+b')  # Read/write binary
            self.is_open = True

            # Read header
            if not self._read_header():
                return 0

            # Read SubHeader
            self._read_sub_header()

            # Read IFD offsets
            if not self._read_offsets():
                return 0

            # Seek to end to track data offset
            self.stream.seek(0, os.SEEK_END)
            self.offset_new_data = self.stream.tell()

            return self.header.RecordNum

        elif mode == 'w':
            # Create new file
            self.stream = open(filename, 'w+b')  # Write binary, truncate
            self.is_open = True

            # Write initial header
            if not self._write_header():
                return 0

            # Write SubHeader
            self._write_sub_header()

            # Position at end
            self.stream.seek(0, os.SEEK_END)
            self.offset_new_data = self.stream.tell()

            return 1

        return 0

    def close(self):
        """Close TGF file."""
        if not self.is_open:
            return

        self._flush()
        self.stream.flush()
        self.stream.close()
        self.is_open = False

    def records_number(self) -> int:
        """Get number of records in file."""
        return self.header.RecordNum

    def create_record(self) -> int:
        """
        Create new record.

        Returns:
            1 on success, 0 on failure
        """
        if self.new_data:
            self._flush()

        # Find end of data (after last IFD)
        if self.header.RecordNum > 0:
            offset_last = self.ifd_offsets.get(self.header.RecordNum, 0)
            if offset_last > 0:
                self.stream.seek(offset_last, os.SEEK_SET)
                ifd_header_data = self.stream.read(IfdHeader.SIZE)
                ifd_header = IfdHeader.unpack(ifd_header_data)

                # Position after this IFD
                self.offset_new_data = offset_last + IfdHeader.SIZE + ifd_header.FieldNum * 16
                self.stream.seek(self.offset_new_data, os.SEEK_SET)
        else:
            self.stream.seek(0, os.SEEK_END)
            self.offset_new_data = self.stream.tell()

        # Increment counts
        self.header.RecordNum += 1
        self.header.IfdNum += 1

        # Prepare empty IFD
        self.ifd.header.tag = TAG_FIRST
        self.ifd.header.FieldNum = 0
        self.ifd.header.FieldNumTotal = 0
        self.ifd.header.unused = 0
        self.ifd.header.NextIfd = 0
        self.ifd.header.NextRecord = 0
        self.ifd.fields = []

        self.new_ifd = True
        self.new_data = True  # Mark as having new data so flush writes

        return 1

    def set_record(self, num: int) -> int:
        """
        Set current record for reading.

        Args:
            num: Record number (1-based)

        Returns:
            Number of fields in record, or 0 on failure
        """
        if self.new_data:
            self._flush()

        if num <= 0 or num > self.header.RecordNum:
            return 0

        offset = self.ifd_offsets.get(num, 0)
        if not offset:
            return 0

        if not self._ifd_read(offset):
            return 0

        # Build tag list
        self.tag_list = {}
        for field in self.ifd.fields:
            if field.tag != 0:
                self.tag_list[field.tag] = field.length

        return self.ifd.header.FieldNum

    def field_write(self, tag: int, data: bytes) -> int:
        """
        Write field data.

        Args:
            tag: Field tag
            data: Binary data to write

        Returns:
            1 on success
        """
        length = len(data)

        # Find field or create new one
        field_idx = None
        for i, field in enumerate(self.ifd.fields):
            if field.tag == 0 or field.tag == tag:
                field_idx = i
                break

        if field_idx is None:
            # Create new field
            field_idx = len(self.ifd.fields)
            self.ifd.fields.append(Field(tag=tag, attrib=0, length=0, word=b'\x00' * 8))
            self.ifd.header.FieldNum += 1
            self.ifd.header.FieldNumTotal += 1

        field = self.ifd.fields[field_idx]
        field.tag = tag
        field.length = length

        if length <= 8:
            # Store inline
            field.word = Field.make_word_from_bytes(data)
        else:
            # Store externally
            self.stream.seek(self.offset_new_data, os.SEEK_SET)

            # Add padding if needed
            if self.seek_flag:
                padding = num_padding(self.offset_new_data - 1)
                if padding > 0:
                    self.stream.write(b'0' * padding)
                    self.offset_new_data += padding
                self.seek_flag = False

            # Store offset in field
            field.word = Field.make_word_from_int(self.offset_new_data)

            # Write data
            self.stream.write(data)

            # Add padding after data
            padding = num_padding(length - 1)
            if padding > 0:
                self.stream.write(b'0' * padding)

            self.offset_new_data = self.stream.tell()

        self.new_data = True
        return 1

    def field_read(self, tag: int, size: int) -> bytes:
        """
        Read field data.

        Args:
            tag: Field tag
            size: Expected size

        Returns:
            Binary data
        """
        # Find field
        field = None
        for f in self.ifd.fields:
            if f.tag == tag:
                field = f
                break

        if field is None:
            return b''

        if field.length > size:
            return b''

        length = min(field.length, size)

        if field.length <= 8:
            # Data stored inline
            return field.word[:length]
        else:
            # Data stored externally
            offset = field.get_word_as_int()
            self.stream.seek(offset, os.SEEK_SET)
            return self.stream.read(length)

    def get_tag_length(self, tag: int) -> int:
        """
        Get length of tag data.

        Matches C++ Tgf::GetTagLength() from Tgf.cpp:154-158.

        Args:
            tag: Field tag to look up

        Returns:
            Length of tag data, or 0 if tag not found
        """
        return self.tag_list.get(tag, 0)

    def _read_header(self) -> bool:
        """Read file header."""
        self.stream.seek(0, os.SEEK_SET)
        data = self.stream.read(Header.SIZE)

        if len(data) < Header.SIZE:
            return False

        self.header = Header.unpack(data)

        if self.header.ID != b"TGF\x00":
            return False

        return True

    def _write_header(self) -> bool:
        """Write file header."""
        self.stream.seek(0, os.SEEK_SET)
        data = self.header.pack()
        self.stream.write(data)
        return True

    def _read_sub_header(self):
        """Read SubHeader (16 bytes)."""
        self.sub_header = self.stream.read(16)

    def _write_sub_header(self):
        """Write SubHeader (16 bytes)."""
        self.stream.write(self.sub_header)

    def _read_offsets(self) -> bool:
        """Read offsets for all records."""
        record_num = 1
        offset = self.header.FstIfdOffs
        self.ifd_offsets[1] = offset

        while offset > 0:
            self.stream.seek(offset, os.SEEK_SET)
            ifd_header_data = self.stream.read(IfdHeader.SIZE)

            if len(ifd_header_data) < IfdHeader.SIZE:
                break

            ifd_header = IfdHeader.unpack(ifd_header_data)
            self.current_ifd_offset = offset

            if ifd_header.tag == TAG_FIRST:
                record_num += 1

            offset = ifd_header.NextRecord
            if offset > 0:
                self.ifd_offsets[record_num] = offset

        record_num -= 1

        if record_num != self.header.RecordNum:
            return False

        return True

    def _ifd_read(self, offset: int) -> bool:
        """Read IFD from file."""
        self.stream.seek(offset, os.SEEK_SET)

        # Read IFD header
        ifd_header_data = self.stream.read(IfdHeader.SIZE)
        if len(ifd_header_data) < IfdHeader.SIZE:
            return False

        self.ifd.header = IfdHeader.unpack(ifd_header_data)

        # Read fields
        self.ifd.fields = []
        for i in range(self.ifd.header.FieldNum):
            field_data = self.stream.read(Field.SIZE)
            if len(field_data) < Field.SIZE:
                return False

            field = Field.unpack(field_data)
            self.ifd.fields.append(field)

        self.current_ifd_offset = offset
        return True

    def _ifd_write(self, offset: int) -> bool:
        """Write IFD to file."""
        self.current_ifd_offset = offset
        self.stream.seek(offset, os.SEEK_SET)

        # Write IFD header
        self.stream.write(self.ifd.header.pack())

        # Write fields
        for i, field in enumerate(self.ifd.fields):
            self.stream.write(field.pack())
            # Clear tag after writing (C++ behavior)
            field.tag = 0

        if offset == self.offset_new_data:
            self.offset_new_data = self.stream.tell()

        return True

    def _flush(self) -> bool:
        """Flush pending writes."""
        if not self.new_data:
            return True

        if not self.new_ifd:
            self.new_data = False
            return self._ifd_write(self.current_ifd_offset)

        # Add padding before IFD if needed
        self.stream.seek(self.offset_new_data, os.SEEK_SET)
        if self.seek_flag:
            padding = num_padding(self.offset_new_data - 1)
            if padding > 0:
                self.stream.write(b'0' * padding)
                self.offset_new_data += padding
            self.seek_flag = False

        # Update header and previous IFD
        if self.header.IfdNum == 1:
            self.header.FstIfdOffs = self.offset_new_data
        else:
            # Update previous record's NextRecord
            prev_offset = self.ifd_offsets.get(self.header.RecordNum - 1, 0)
            if prev_offset > 0:
                self.stream.seek(prev_offset + 12, os.SEEK_SET)
                import struct
                self.stream.write(struct.pack('<i', self.offset_new_data))

        self.ifd_offsets[self.header.RecordNum] = self.offset_new_data
        self._write_header()

        self.new_ifd = False
        self.new_data = False

        # Write the new IFD
        return self._ifd_write(self.offset_new_data)
