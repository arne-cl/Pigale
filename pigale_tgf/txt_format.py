"""
TXT format (Pigale ASCII) reader and writer.

Format:
    PIG:0
    <title>
    <v1> <v2>
    <v1> <v2>
    ...
    0 0
"""

from pathlib import Path
from typing import List, Tuple, Union, TextIO


def is_file_txt(filename: Union[str, Path]) -> bool:
    """
    Check if file is a valid Pigale TXT file.

    Args:
        filename: Path to file to check

    Returns:
        True if file starts with "PIG:", False otherwise
    """
    try:
        with open(filename, 'r') as f:
            header = f.read(4)
            return header == "PIG:"
    except (FileNotFoundError, IOError, UnicodeDecodeError):
        return False


def read_txt_header(f: TextIO) -> Tuple[str, str]:
    """
    Read header and title from TXT file.

    Args:
        f: Open file handle positioned at start

    Returns:
        Tuple of (header, title)
        - header: Should be "PIG:0"
        - title: Graph title (up to 80 chars)
    """
    # Read header line (up to 80 chars)
    header_chars = []
    for _ in range(80):
        ch = f.read(1)
        if not ch or ch == '\n' or ch == '\r':
            break
        header_chars.append(ch)

    header = ''.join(header_chars)

    # Skip any remaining newline chars (handle \r\n)
    while True:
        pos = f.tell()
        ch = f.read(1)
        if not ch or (ch != '\n' and ch != '\r'):
            f.seek(pos)  # Put back non-newline
            break

    # Read title line (up to 80 chars)
    title_chars = []
    for _ in range(80):
        ch = f.read(1)
        if not ch or ch == '\n' or ch == '\r':
            break
        title_chars.append(ch)

    title = ''.join(title_chars)

    return header, title


def parse_txt_edges(lines: List[str]) -> List[Tuple[int, int]]:
    """
    Parse edge list from TXT file lines.

    Args:
        lines: List of lines containing "<v1> <v2>" pairs

    Returns:
        List of (v1, v2) tuples, with loops (v1==v2) filtered out

    Note:
        Stops parsing at "0 0" terminator.
        Loops are silently skipped (matches C++ behavior).
    """
    edges = []

    for line in lines:
        parts = line.strip().split()
        if len(parts) < 2:
            continue

        try:
            v1 = int(parts[0])
            v2 = int(parts[1])

            # Terminator
            if v1 == 0 and v2 == 0:
                break

            # Skip loops (v1 == v2)
            if v1 == v2:
                continue

            edges.append((v1, v2))

        except (ValueError, IndexError):
            continue

    return edges
