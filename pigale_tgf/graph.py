"""
Graph container and I/O functions.

Provides GraphContainer class and functions to read/write graphs
in various formats (TGF, TXT, GraphML).
"""

import struct
import numpy as np
from pathlib import Path
from typing import Union
import math

from pigale_tgf.properties import PSet
from pigale_tgf.constants import (
    PROP_TITRE, PROP_N, PROP_M,
    PROP_LABEL, PROP_COORD, PROP_VIN,
)
from pigale_tgf.txt_format import is_file_txt, read_txt_header, parse_txt_edges


class GraphContainer:
    """
    Container for graph data.

    Stores graph as four property sets:
    - general: Graph-level properties (title, n, m, etc.)
    - vertex: Vertex properties (coordinates, labels, etc.)
    - edge: Edge properties (colors, widths, etc.)
    - brin: Brin (half-edge) properties (incidence, rotation)
    """

    def __init__(self):
        """Create empty graph container."""
        self.general = PSet(0, -1)  # General properties (single-value)
        self.vertex = PSet(0, -1)   # Vertex properties
        self.edge = PSet(0, -1)     # Edge properties
        self.brin = PSet(0, -1)     # Brin properties

    def clear(self):
        """Clear all graph data."""
        self.general.clear()
        self.vertex.clear()
        self.edge.clear()
        self.brin.clear()

    def setsize(self, nv: int, ne: int):
        """
        Set graph size and resize property sets.

        Args:
            nv: Number of vertices
            ne: Number of edges
        """
        # Resize vertex set (1-indexed: 1..nv)
        self.vertex.resize(1, nv)

        # Resize edge set (1-indexed: 1..ne)
        self.edge.resize(1, ne)

        # Resize brin set (-ne to ne, excluding 0)
        self.brin.resize(-ne, ne)

        # Store sizes as numpy arrays (for PSet1 serialization)
        self.general[PROP_N] = np.array([nv], dtype=np.int32)
        self.general[PROP_M] = np.array([ne], dtype=np.int32)


def read_txt_graph(filename: Union[str, Path]) -> GraphContainer:
    """
    Read graph from Pigale ASCII (TXT) format.

    Format:
        PIG:0
        <title>
        <v1> <v2>
        ...
        0 0

    Args:
        filename: Path to TXT file

    Returns:
        GraphContainer with graph data

    Note:
        - Loops (v1==v2) are silently skipped
        - Coordinates generated as circle layout
        - Vertex labels may be sparse (arbitrary integers)
    """
    filename = Path(filename)

    if not is_file_txt(filename):
        raise ValueError(f"Not a valid TXT file: {filename}")

    graph = GraphContainer()

    with open(filename, 'r') as f:
        # Read header and title
        header, title = read_txt_header(f)

        if not header.startswith("PIG:"):
            raise ValueError(f"Invalid TXT header: {header}")

        # Read all remaining lines
        lines = f.readlines()

    # Parse edges
    edges = parse_txt_edges(lines)

    # Build vertex label mapping
    # map_label: label -> sequential index (1..n)
    # map_index: sequential index -> label
    map_label = {}  # label -> index
    map_index = {}  # index -> label

    vertex_index = 1
    for v1, v2 in edges:
        if v1 not in map_label:
            map_label[v1] = vertex_index
            map_index[vertex_index] = v1
            vertex_index += 1
        if v2 not in map_label:
            map_label[v2] = vertex_index
            map_index[vertex_index] = v2
            vertex_index += 1

    nv = len(map_label)
    ne = len(edges)

    # Set graph size
    graph.setsize(nv, ne)

    # Store title
    graph.general[PROP_TITRE] = title

    # Create vertex labels (PSet array is 0-indexed with nv elements)
    vlabels = np.zeros(nv, dtype=np.int32)
    for idx, label in map_index.items():
        vlabels[idx - 1] = label  # idx is 1-based, array is 0-based
    graph.vertex[PROP_LABEL] = vlabels

    # Create edge incidence (PROP_VIN)
    # vin[e] = source vertex, vin[-e] = target vertex
    vin = np.zeros(2 * ne + 1, dtype=np.int32)  # -ne..ne
    vin[0] = 0  # Brin 0 is always 0

    for edge_idx, (v1, v2) in enumerate(edges, start=1):
        # Map labels to indices
        src_idx = map_label[v1]
        dst_idx = map_label[v2]

        # Store incidence
        # Note: brin array is indexed -ne..ne, but numpy is 0-based
        # So we need to offset by ne
        # Positive brin (edge_idx): offset = ne + edge_idx
        # Negative brin (-edge_idx): offset = ne - edge_idx
        vin[ne + edge_idx] = src_idx   # Positive brin
        vin[ne - edge_idx] = dst_idx   # Negative brin

    graph.brin[PROP_VIN] = vin

    # Generate circular coordinates
    coords = np.zeros(nv + 1, dtype=[('x', 'f8'), ('y', 'f8')])
    coords[0] = (0.0, 0.0)  # Index 0

    angle = 2.0 * math.pi / nv if nv > 0 else 0
    for i in range(1, nv + 1):
        coords[i] = (math.cos(angle * i), math.sin(angle * i))

    graph.vertex[PROP_COORD] = coords

    return graph


def write_txt_graph(graph: GraphContainer, filename: Union[str, Path]):
    """
    Write graph to Pigale ASCII (TXT) format.

    Format:
        PIG:0
        <title>
        <v1> <v2>
        ...
        0 0

    Args:
        graph: GraphContainer to write
        filename: Path to output file

    Note:
        - Writes vertex labels (not indices) if PROP_LABEL exists
        - Otherwise uses vertex indices as labels
        - Coordinates are NOT saved (TXT format doesn't support them)
    """
    filename = Path(filename)

    # Get title
    title = graph.general[PROP_TITRE] if graph.general.exists(PROP_TITRE) else "No Name"

    # Get sizes (handle both scalar and array formats)
    nv_val = graph.general[PROP_N]
    ne_val = graph.general[PROP_M]
    nv = int(nv_val[0]) if isinstance(nv_val, np.ndarray) else int(nv_val)
    ne = int(ne_val[0]) if isinstance(ne_val, np.ndarray) else int(ne_val)

    # Get vertex labels
    if graph.vertex.exists(PROP_LABEL):
        vlabel = graph.vertex[PROP_LABEL]
        # PROP_LABEL might be 0-indexed (nv elements) or 1-indexed (nv+1 elements)
        # Check which format we have
        if len(vlabel) == nv:
            # 0-indexed format (nv elements) - create 1-indexed array
            vlabel_1indexed = np.zeros(nv + 1, dtype=np.int32)
            vlabel_1indexed[1:] = vlabel
            vlabel = vlabel_1indexed
        elif len(vlabel) == nv + 1:
            # Already 1-indexed format - use as is
            pass
        else:
            # Unknown format - create default
            vlabel = np.arange(nv + 1, dtype=np.int32)
    else:
        # Use indices as labels (creates array [0, 1, 2, ..., nv])
        vlabel = np.arange(nv + 1, dtype=np.int32)

    # Get edge incidence
    vin = graph.brin[PROP_VIN]

    with open(filename, 'w') as f:
        # Write header
        f.write("PIG:0\n")

        # Write title
        f.write(f"{title}\n")

        # Write edges
        # For each edge e (1..ne), write labels of vin[e] and vin[-e]
        # vin array is indexed -ne..ne, stored offset by ne
        for e in range(1, ne + 1):
            # Get vertex indices (1-based)
            v1_idx = vin[ne + e]   # Positive brin
            v2_idx = vin[ne - e]   # Negative brin

            # Get vertex labels (vlabel is now 1-indexed)
            v1_label = vlabel[v1_idx]
            v2_label = vlabel[v2_idx]

            # Write edge
            f.write(f"{v1_label} {v2_label}\n")

        # Write terminator
        f.write("0 0\n")


# =============================================================================
# TGF Graph I/O Functions
# =============================================================================


def detect_tgf_version(filename: Path) -> int:
    """
    Detect TGF file version.

    Args:
        filename: Path to TGF file

    Returns:
        Version number (0, 1, or 2), or -1 if not a valid TGF file
    """
    from .core import Tgf
    from .constants import TAG_VERSION

    filename = Path(filename)

    tgf = Tgf()
    if not tgf.open(filename, mode='r'):
        return -1

    # Read first record
    if tgf.header.RecordNum == 0:
        tgf.close()
        return 0  # Empty file = version 0

    tgf.set_record(1)

    # Try to read TAG_VERSION
    version_data = tgf.field_read(TAG_VERSION, 2)
    tgf.close()

    if not version_data or len(version_data) < 2:
        return 0  # No version tag = version 0

    version = struct.unpack('<h', version_data)[0]
    return version


def get_num_records(filename: Path) -> int:
    """
    Get number of records in TGF file.

    Args:
        filename: Path to TGF file

    Returns:
        Number of records, or -1 on error
    """
    from .core import Tgf

    filename = Path(filename)

    tgf = Tgf()
    if not tgf.open(filename, mode='r'):
        return -1

    num_records = tgf.header.RecordNum
    tgf.close()

    return num_records


def read_tgf_graph(filename: Path, record_num: int = 1) -> GraphContainer:
    """
    Read graph from TGF file (version 1 or 2 only).

    Matches C++ ReadTgfGraph() from File.cpp:123-246.

    Supports:
    - Version 2: Modern format with PSets
    - Version 1: Older format with PSets (PROP_NLOOPS erased)
    - Version 0: NOT SUPPORTED (legacy format)

    Args:
        filename: Path to TGF file
        record_num: Record number to read (1-indexed, default=1)

    Returns:
        GraphContainer with loaded graph

    Raises:
        ValueError: If file is version 0 (not supported)
        FileNotFoundError: If file doesn't exist
    """
    from .core import Tgf
    from .constants import TAG_VERSION, PROP_N, PROP_M, PROP_NLOOPS
    from .properties import read_tgf_pset

    filename = Path(filename)

    if not filename.exists():
        raise FileNotFoundError(f"TGF file not found: {filename}")

    # Open file
    tgf = Tgf()
    if not tgf.open(filename, mode='r'):
        raise ValueError(f"Cannot open TGF file: {filename}")

    # Get and clamp record number (C++ File.cpp:129-131)
    num_records = tgf.header.RecordNum
    if record_num > num_records:
        record_num = num_records
    if record_num <= 0:
        record_num = 1

    # Load record
    tgf.set_record(record_num)

    # Read version
    version_data = tgf.field_read(TAG_VERSION, 2)
    if not version_data or len(version_data) < 2:
        version = 0
    else:
        version = struct.unpack('<h', version_data)[0]

    # Create graph container
    graph = GraphContainer()

    if version >= 1:
        # Modern format (version 1 or 2) - C++ File.cpp:138-148
        # Read property sets
        # Note: General uses PSet1 tags, others use PSet tags
        # But both use SetNum=0 for their respective tag types
        graph.general = read_tgf_pset(tgf, set_num=0, use_pset1=True)
        graph.vertex = read_tgf_pset(tgf, set_num=0, use_pset1=False)
        graph.edge = read_tgf_pset(tgf, set_num=1, use_pset1=False)
        graph.brin = read_tgf_pset(tgf, set_num=2, use_pset1=False)

        # Get graph sizes
        if PROP_N not in graph.general.properties or PROP_M not in graph.general.properties:
            raise ValueError(f"TGF file missing PROP_N or PROP_M in general properties")

        # PSet1 properties are scalars (numpy values), not arrays
        nv_val = graph.general[PROP_N]
        ne_val = graph.general[PROP_M]

        # Convert to int (handle both scalars and arrays)
        nv = int(nv_val) if np.isscalar(nv_val) else int(nv_val[0])
        ne = int(ne_val) if np.isscalar(ne_val) else int(ne_val[0])

        # Set sizes (creates proper ranges)
        graph.setsize(nv, ne)

        # Bug preservation: Version 1 erases PROP_NLOOPS (C++ File.cpp:146-147)
        if version == 1:
            if PROP_NLOOPS in graph.general.properties:
                graph.general.erase(PROP_NLOOPS)

        tgf.close()
        return graph
    else:
        # Version 0 - legacy format not supported
        tgf.close()
        raise ValueError(
            f"Version 0 (legacy) TGF format not supported. "
            f"Please use Pigale to convert to modern format."
        )


def write_tgf_graph(graph: GraphContainer, filename: Path, version: int = 2):
    """
    Write graph to TGF file (version 2 format).

    Args:
        graph: GraphContainer with graph data
        filename: Path to output TGF file
        version: TGF version to write (default=2, only 2 supported)

    Raises:
        ValueError: If version is not 2
    """
    from .core import Tgf
    from .constants import TAG_VERSION, PROP_N, PROP_M
    from .properties import write_tgf_pset

    filename = Path(filename)

    if version != 2:
        raise ValueError(f"Only version 2 writing supported, got version {version}")

    # Open file for writing
    tgf = Tgf()
    if not tgf.open(filename, mode='w'):
        raise ValueError(f"Cannot create TGF file: {filename}")

    # Create record
    tgf.create_record()

    # Write version tag
    tgf.field_write(TAG_VERSION, struct.pack('<h', version))

    # Ensure PROP_N and PROP_M are set
    if PROP_N not in graph.general.properties:
        # Infer from vertex PSet
        nv = graph.vertex.finish if graph.vertex.finish > 0 else 0
        graph.general[PROP_N] = np.array([nv], dtype=np.int32)

    if PROP_M not in graph.general.properties:
        # Infer from edge PSet
        ne = graph.edge.finish if graph.edge.finish > 0 else 0
        graph.general[PROP_M] = np.array([ne], dtype=np.int32)

    # Write property sets
    # General uses PSet1 tags
    write_tgf_pset(graph.general, tgf, set_num=0, use_pset1=True)

    # Vertex/edge/brin use PSet tags
    write_tgf_pset(graph.vertex, tgf, set_num=0, use_pset1=False)
    write_tgf_pset(graph.edge, tgf, set_num=1, use_pset1=False)
    write_tgf_pset(graph.brin, tgf, set_num=2, use_pset1=False)

    # Close file
    tgf.close()
