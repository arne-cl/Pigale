"""
Binary structure types for TGF format.

Dataclasses matching C++ structures in incl/TAXI/Tgf.h
"""

import struct
from dataclasses import dataclass
from typing import ClassVar


@dataclass
class Header:
    """
    TGF file header (16 bytes).

    C++ struct:
        char ID[4];         // "TGF\\0"
        short Version;      // version number
        short IfdNum;       // number of IFDs
        short RecordNum;    // number of records
        short LenSubHeader; // size of SubHeader (16)
        LongInt FstIfdOffs; // offset of first IFD
    """

    ID: bytes                # 4 bytes
    Version: int             # 2 bytes (short)
    IfdNum: int              # 2 bytes (short)
    RecordNum: int           # 2 bytes (short)
    LenSubHeader: int        # 2 bytes (short)
    FstIfdOffs: int          # 4 bytes (int/LongInt)

    FORMAT: ClassVar[str] = '<4s4Hi'  # Little-endian: 4 bytes, 4 shorts, 1 int
    SIZE: ClassVar[int] = 16

    def pack(self) -> bytes:
        """Pack header to binary."""
        return struct.pack(
            self.FORMAT,
            self.ID,
            self.Version,
            self.IfdNum,
            self.RecordNum,
            self.LenSubHeader,
            self.FstIfdOffs
        )

    @classmethod
    def unpack(cls, data: bytes) -> 'Header':
        """Unpack header from binary."""
        values = struct.unpack(cls.FORMAT, data[:cls.SIZE])
        return cls(
            ID=values[0],
            Version=values[1],
            IfdNum=values[2],
            RecordNum=values[3],
            LenSubHeader=values[4],
            FstIfdOffs=values[5]
        )


@dataclass
class IfdHeader:
    """
    IFD header (16 bytes).

    C++ struct:
        short tag;          // TAG_FIRST or TAG_NEXT
        short FieldNumTotal;// total fields (for chaining)
        short FieldNum;     // fields in this IFD
        short unused;       // reserved
        LongInt NextIfd;    // offset to next IFD (0=none)
        LongInt NextRecord; // offset to next record (0=last)
    """

    tag: int                 # 2 bytes (short)
    FieldNumTotal: int       # 2 bytes (short)
    FieldNum: int            # 2 bytes (short)
    unused: int              # 2 bytes (short)
    NextIfd: int             # 4 bytes (int)
    NextRecord: int          # 4 bytes (int)

    FORMAT: ClassVar[str] = '<4H2i'  # Little-endian
    SIZE: ClassVar[int] = 16

    def pack(self) -> bytes:
        """Pack IFD header to binary."""
        return struct.pack(
            self.FORMAT,
            self.tag,
            self.FieldNumTotal,
            self.FieldNum,
            self.unused,
            self.NextIfd,
            self.NextRecord
        )

    @classmethod
    def unpack(cls, data: bytes) -> 'IfdHeader':
        """Unpack IFD header from binary."""
        values = struct.unpack(cls.FORMAT, data[:cls.SIZE])
        return cls(
            tag=values[0],
            FieldNumTotal=values[1],
            FieldNum=values[2],
            unused=values[3],
            NextIfd=values[4],
            NextRecord=values[5]
        )


@dataclass
class Field:
    """
    Field entry (16 bytes).

    C++ struct:
        short tag;          // property/tag ID
        short attrib;       // attributes (unused)
        LongInt len;        // data length in bytes
        UnionLongWord word; // data or offset (8 bytes)

    If len <= 8: data stored inline in word
    If len > 8: word contains file offset to data
    """

    tag: int                 # 2 bytes (short)
    attrib: int              # 2 bytes (short)
    length: int              # 4 bytes (int) - named 'length' to avoid shadowing len()
    word: bytes              # 8 bytes (raw data or offset)

    FORMAT: ClassVar[str] = '<2Hi8s'  # Little-endian
    SIZE: ClassVar[int] = 16

    def pack(self) -> bytes:
        """Pack field to binary."""
        return struct.pack(
            self.FORMAT,
            self.tag,
            self.attrib,
            self.length,
            self.word
        )

    @classmethod
    def unpack(cls, data: bytes) -> 'Field':
        """Unpack field from binary."""
        values = struct.unpack(cls.FORMAT, data[:cls.SIZE])
        return cls(
            tag=values[0],
            attrib=values[1],
            length=values[2],
            word=values[3]
        )

    def get_word_as_int(self) -> int:
        """Get word as integer (for offsets)."""
        return struct.unpack('<q', self.word)[0]  # 8-byte int

    def get_word_as_double(self) -> float:
        """Get word as double."""
        return struct.unpack('<d', self.word)[0]

    @staticmethod
    def make_word_from_int(value: int) -> bytes:
        """Create word from integer value."""
        return struct.pack('<q', value)

    @staticmethod
    def make_word_from_bytes(data: bytes) -> bytes:
        """Create word from raw bytes (pad to 8 bytes)."""
        if len(data) >= 8:
            return data[:8]
        else:
            return data + b'\x00' * (8 - len(data))


@dataclass
class Ifd:
    """
    IFD (Image File Directory) structure.

    Contains header and list of fields.
    """

    header: IfdHeader
    fields: list[Field]

    def __init__(self):
        """Create empty IFD."""
        self.header = IfdHeader(
            tag=0,
            FieldNumTotal=0,
            FieldNum=0,
            unused=0,
            NextIfd=0,
            NextRecord=0
        )
        self.fields = []
