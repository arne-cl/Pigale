"""
Utility functions for TGF format.

Includes alignment, padding, and helper functions.
"""


def num_padding(n: int) -> int:
    """
    Calculate number of padding bytes needed after n bytes.

    TGF uses 4-byte alignment for all data blocks.

    Args:
        n: Number of bytes

    Returns:
        Number of padding bytes needed (0-3)

    Example:
        >>> num_padding(1)
        3
        >>> num_padding(4)
        0
        >>> num_padding(5)
        3
    """
    return (3 - (n + 3) % 4)


def align_offset(offset: int) -> int:
    """
    Round offset up to next 4-byte boundary.

    Args:
        offset: Current offset

    Returns:
        Aligned offset (multiple of 4)

    Example:
        >>> align_offset(1)
        4
        >>> align_offset(4)
        4
        >>> align_offset(5)
        8
    """
    if offset == 0:
        return 0
    # Round up to next multiple of 4
    return ((offset + 3) // 4) * 4
