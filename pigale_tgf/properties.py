"""
Property system for graph elements.

Properties are stored in PSet (property set) objects, which hold
arrays indexed by element numbers (vertices, edges, brins).
"""

import struct
import numpy as np
from typing import Any, Dict, Optional, TYPE_CHECKING

if TYPE_CHECKING:
    from .core import Tgf


class PSet:
    """
    Property set for graph elements.

    Stores properties as numpy arrays indexed from start to finish.
    Matches the C++ PSet class behavior.
    """

    def __init__(self, start: int = 0, finish: int = -1):
        """
        Create property set with index range.

        Args:
            start: First valid index
            finish: Last valid index
        """
        self._start = start
        self._finish = finish
        self.properties: Dict[int, np.ndarray] = {}

    @property
    def start(self) -> int:
        """Get starting index."""
        return self._start

    @property
    def finish(self) -> int:
        """Get ending index."""
        return self._finish

    def size(self) -> int:
        """Get size of property set range."""
        return self._finish - self._start + 1

    def resize(self, start: int, finish: int):
        """
        Resize property set range.

        Args:
            start: New starting index
            finish: New ending index
        """
        self._start = start
        self._finish = finish

        # Resize existing arrays
        new_size = finish - start + 1
        for prop_num, array in self.properties.items():
            # Create new array with new size
            if array.dtype.names:  # Structured array
                new_array = np.zeros(new_size, dtype=array.dtype)
            else:
                new_array = np.zeros(new_size, dtype=array.dtype)

            # Copy over valid data
            old_size = len(array)
            copy_size = min(old_size, new_size)
            if copy_size > 0:
                new_array[:copy_size] = array[:copy_size]

            self.properties[prop_num] = new_array

    def exists(self, prop_num: int) -> bool:
        """
        Check if property exists.

        Args:
            prop_num: Property number

        Returns:
            True if property is stored
        """
        return prop_num in self.properties

    def __getitem__(self, prop_num: int) -> Optional[np.ndarray]:
        """
        Get property array.

        Args:
            prop_num: Property number

        Returns:
            Numpy array or None if not exists
        """
        return self.properties.get(prop_num)

    def __setitem__(self, prop_num: int, value: Any):
        """
        Set property value.

        Args:
            prop_num: Property number
            value: Value to store (will be converted to numpy array if needed)
        """
        if isinstance(value, np.ndarray):
            self.properties[prop_num] = value
        elif isinstance(value, (list, tuple)):
            self.properties[prop_num] = np.array(value)
        else:
            # Single value - store as scalar
            self.properties[prop_num] = value

    def clear(self):
        """Clear all properties."""
        self.properties.clear()

    def erase(self, prop_num: int):
        """
        Erase a property.

        Args:
            prop_num: Property number to erase
        """
        if prop_num in self.properties:
            del self.properties[prop_num]


def write_tgf_pset(pset: PSet, tgf: 'Tgf', set_num: int, use_pset1: bool = False):
    """
    Write PSet to TGF file.

    Matches C++ WriteTGF(PSet &X, Tgf &file, int PNum) from PropTgf.cpp:19-30.

    Bug preservation:
    - Loop breaks at property number 128 (C++ line 25: if (i==128) break;)

    Args:
        pset: Property set to write
        tgf: TGF file object (must be open for writing)
        set_num: Property set number (0 for both general and vertex, 1=edge, 2=brin)
        use_pset1: If True, use PSet1 tags (0x1800+); if False, use PSet tags (0x1400+)
    """
    from .constants import PSetHeaderTag, PSetTag, PSet1Tag

    # Select tag function based on PSet type
    if use_pset1:
        prop_tag_func = PSet1Tag
    else:
        prop_tag_func = PSetTag

    # Write header: struct pset_header { int start; int finish; }
    header_data = struct.pack('<ii', pset.start, pset.finish)
    tgf.field_write(PSetHeaderTag(set_num), header_data)

    # Write properties (up to 128 only - bug preservation)
    for prop_num in range(128):
        if prop_num not in pset.properties:
            continue

        # Get property array
        arr = pset.properties[prop_num]

        # Convert to bytes
        if isinstance(arr, np.ndarray):
            prop_data = arr.tobytes()
        elif isinstance(arr, str):
            # Handle string properties (like PROP_TITRE)
            prop_data = arr.encode('utf-8')
        else:
            # Handle other non-array data
            prop_data = bytes(arr)

        # Write field
        tgf.field_write(prop_tag_func(set_num, prop_num), prop_data)


def read_tgf_pset(tgf: 'Tgf', set_num: int, use_pset1: bool = False) -> PSet:
    """
    Read PSet from TGF file.

    Matches C++ ReadTGF(PSet &X, Tgf &file, int PNum) from PropTgf.cpp:46-63.
    Also supports PSet1 tags for general property set.

    Bug preservation:
    - Loop breaks at property number 128 (C++ line 55: if (i==128) break;)

    Args:
        tgf: TGF file object (must have record set)
        set_num: Property set number (0 for both general and vertex, 1=edge, 2=brin)
        use_pset1: If True, use PSet1 tags (0x1800+); if False, use PSet tags (0x1400+)

    Returns:
        PSet object with properties loaded
    """
    from .constants import PSetHeaderTag, PSetTag, PSet1Tag

    # Select tag function based on PSet type
    if use_pset1:
        prop_tag_func = PSet1Tag
    else:
        prop_tag_func = PSetTag

    # Read header (same for both PSet and PSet1)
    header_tag = PSetHeaderTag(set_num)
    header_data = tgf.field_read(header_tag, 8)

    if not header_data:
        # No header found - return empty PSet
        return PSet(0, -1)

    # Unpack header
    start, finish = struct.unpack('<ii', header_data)

    # Create PSet
    pset = PSet(start, finish)

    # Read properties (up to 128 only - bug preservation)
    for prop_num in range(128):
        prop_tag = prop_tag_func(set_num, prop_num)

        # Get tag length
        prop_size = tgf.get_tag_length(prop_tag)
        if prop_size == 0:
            continue

        # Read property data
        prop_data = tgf.field_read(prop_tag, prop_size)

        if prop_data:
            if use_pset1:
                # PSet1: Each property stored separately (not as array)
                # Just store raw bytes (C++ PropTgf.cpp:64-75)
                # Determine dtype from total size
                if prop_size == 4:
                    dtype = np.int32
                elif prop_size == 8:
                    dtype = np.float64
                elif prop_size == 2:
                    dtype = np.int16
                else:
                    dtype = np.uint8
            else:
                # PSet: Properties stored as arrays indexed by elements
                # Calculate element size from total size / num_elements
                num_elements = finish - start + 1
                element_size = prop_size // num_elements if num_elements > 0 else prop_size

                # Determine dtype from element size
                if element_size == 4:
                    dtype = np.int32
                elif element_size == 8:
                    dtype = np.float64
                elif element_size == 2:
                    dtype = np.int16
                elif element_size == 20:
                    # Coord structure with #pragma pack(4): int + 2 doubles = 20 bytes
                    dtype = np.dtype([('label', '<i4'), ('x', '<f8'), ('y', '<f8')])
                elif element_size == 16:
                    # Coord structure without padding: int + 2 doubles = 16 bytes (on some systems)
                    # Or could be 2 doubles (x, y) without label
                    dtype = np.dtype([('x', '<f8'), ('y', '<f8')])
                else:
                    dtype = np.uint8

            # Convert to numpy array
            arr = np.frombuffer(prop_data, dtype=dtype)
            pset[prop_num] = arr

    return pset
