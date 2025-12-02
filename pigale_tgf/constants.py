"""
Constants for Pigale TGF format.

This module defines all tags and property IDs used in TGF files,
matching the definitions in incl/TAXI/Tgf.h and incl/TAXI/propdef.h
"""

# =============================================================================
# IFD Tags (from Tgf.h)
# =============================================================================

TAG_FIRST = 1  # First IFD in a record
TAG_NEXT = 2  # Continuation IFD


# =============================================================================
# Graph Data Tags (from File.cpp)
# =============================================================================

# Legacy format tags (version 0) - defined for reference but not used
TAG_NAME = 512  # Graph title (string)
TAG_N = 513  # Number of vertices (int)
TAG_M = 514  # Number of edges (int)
TAG_ELIST = 515  # Edge list (e_struct array)
TAG_COORDLAB = 516  # Vertex coords+labels (coord array)
TAG_VCOLOR = 517  # Vertex colors (short array)
TAG_LEDA = 518  # LEDA format flag
TAG_VERSION = 519  # Format version marker

# Modern format tags (version 1+)
TAG_VCOORD = 520  # Vertex coordinates
TAG_VLABEL = 521  # Vertex labels
TAG_VIN = 522  # Vertex incidence (brin->vertex)
TAG_ECOLOR = 523  # Edge colors
TAG_ELABEL = 524  # Edge labels
TAG_EWIDTH = 525  # Edge widths


# =============================================================================
# Property Set Tags (from Tproptgf.h)
# =============================================================================

MAGIC_PROP = 0x1000  # Base for property tags


def PSetHeaderTag(set_num: int) -> int:
    """
    Generate header tag for property set.

    Args:
        set_num: Property set number (0=General/Vertex, 1=Edge, 2=Brin)

    Returns:
        Tag value for property set header
    """
    return MAGIC_PROP | (set_num << 8)


def PSetTag(set_num: int, prop_num: int) -> int:
    """
    Generate tag for a property in PSet.

    Args:
        set_num: Property set number
        prop_num: Property number within set

    Returns:
        Tag value for property
    """
    return MAGIC_PROP | 0x400 | (set_num << 8) | prop_num


def PSet1Tag(set_num: int, prop_num: int) -> int:
    """
    Generate tag for a property in PSet1 (properties with constructors).

    Args:
        set_num: Property set number
        prop_num: Property number within set

    Returns:
        Tag value for PSet1 property
    """
    return MAGIC_PROP | 0x800 | (set_num << 8) | prop_num


# =============================================================================
# Property Numbers (from propdef.h)
# =============================================================================

# --- General Properties (graph-level) ---
PROP_TITRE = 0  # tstring - Graph title
PROP_CONNECTED = 1  # int - Graph is connected
PROP_BICONNECTED = 2  # int - Graph is biconnected
PROP_TRICONNECTED = 3  # int - Graph is triconnected
PROP_FOURCONNECTED = 4  # int - Graph is 4-connected
PROP_PLANAR = 5  # int - Graph is planar
PROP_PLANARMAP = 6  # int - Graph is planar embedded
PROP_N = 7  # int - Number of vertices (backup)
PROP_M = 8  # int - Number of edges (backup)
PROP_EXTBRIN = 9  # tbrin - External brin (defines outer face)
PROP_SIMPLE = 10  # int - Graph is simple
PROP_BIPARTITE = 12  # int - Graph is bipartite
PROP_MAPTYPE = 13  # int - Embedding type
PROP_SYM = 14  # int - Has symmetric labeling
PROP_HYPERGRAPH = 15  # int - Graph is a hypergraph
PROP_NV = 16  # int - #hyper-vertices/red vertices
PROP_NE = 17  # int - #hyper-edges/blue vertices
PROP_NCV = 18  # int - # classes for vertices
PROP_NCE = 19  # int - # classes for edges
PROP_NLOOPS = 30  # int - no loops

# PROP_MAPTYPE values
PROP_MAPTYPE_UNKNOWN = 0  # Unknown origin
PROP_MAPTYPE_ARBITRARY = 1  # Arbitrary map
PROP_MAPTYPE_GEOMETRIC = 2  # Geometric map
PROP_MAPTYPE_LRALGO = 3  # Map computed by LR-Algo

# --- Property Numbers for V, B, E (vertices, brins, edges) ---
PROP_LABEL = 0  # int - Label
PROP_COLOR = 1  # short - Color
PROP_MARK = 2  # bool - Various marks
PROP_TYPE = 7  # int - Associated type
PROP_FLAGS = 8  # long - Various flags
PROP_ORDER = 9  # int - Priority order
PROP_PMETA = 10  # tbrin - First meta brin
PROP_BIRTH = 11  # long - Start of validity
PROP_DEATH = 12  # long - End of validity
PROP_VALUE = 13  # double - A value
PROP_SLABEL = 15  # int - String label index

# --- Vertex-specific Properties ---
PROP_INITIALV = 3  # tvertex - Initial numbering
PROP_NEW = 4  # tvertex - New numbering
PROP_CLASSV = 5  # tvertex - Class of vertex
PROP_REPRESENTATIVEV = 6  # tvertex - Representative of vertex
PROP_COORD = 16  # Tpoint - Coordinates for editor
PROP_PBRIN = 17  # tbrin - First incident brin
PROP_STLINK = 18  # tvertex - Link of st-ordering
PROP_LOW = 19  # tvertex - DFS low
PROP_ELOW = 20  # tedge - DFS edge-low
PROP_TSTATUS = 21  # char - Status of vertex or incoming tree
PROP_DUALORDER = 22  # int - DFS dual order (Kura)
PROP_DEGREE = 25  # int - Vertex degree
PROP_SYMLABEL = 26  # int - Label in the symmetry
PROP_REDUCED = 28  # tvertex - Class in reduced graph
PROP_HYPEREDGE = 29  # bool - Vertex is a hyperedge

# PROP_TSTATUS values
PROP_TSTATUS_ISTHMUS = 0  # Articulation
PROP_TSTATUS_LEAF = 1  # Articulation pair
PROP_TSTATUS_THIN = 2  # Thin incoming tree edge
PROP_TSTATUS_THICK = 3  # Thick incoming tree edge

# --- Edge-specific Properties ---
PROP_INITIALE = 3  # tedge - Initial numbering
PROP_CLASSE = 5  # tedge - Class of edge
PROP_REPRESENTATIVEE = 6  # tedge - Representative of edge
PROP_COLOR2 = 14  # short - Color of the second brin
PROP_WIDTH = 16  # int - Geometric width (editor)
PROP_ORIENTED = 17  # bool - Edge is oriented
PROP_REORIENTED = 18  # bool - Orientation should be reversed
PROP_ISTREE = 19  # bool - Edge belongs to the tree
PROP_MULTIPLICITY = 20  # int - Edge multiplicity

# --- Brin-specific Properties ---
PROP_INITIALB = 3  # tbrin - Initial numbering
PROP_CLASSB = 5  # tbrin - Class of brin
PROP_VIN = 16  # tvertex - Vertex incidence
PROP_CIR = 18  # tbrin - Next brin in circular order
PROP_ACIR = 19  # tbrin - Previous brin in circular order

# --- Drawing Properties ---
PROP_DRAWING = 150  # Start of drawing properties
PROP_DRAW_COORD = PROP_DRAWING  # Tpoint - Main point
PROP_DRAW_POINT_1 = PROP_DRAWING + 1  # Tpoint
PROP_DRAW_POINT_2 = PROP_DRAWING + 2  # Tpoint
PROP_DRAW_POINT_3 = PROP_DRAWING + 3  # Tpoint
PROP_DRAW_POINT_4 = PROP_DRAWING + 4  # Tpoint
PROP_DRAW_POINT_5 = PROP_DRAWING + 5  # Tpoint
PROP_DRAW_POINT_6 = PROP_DRAWING + 6  # Tpoint

# --- Special Property Ranges ---
PROP_NOTSAVED = 128  # Start of not saved properties
PROP_RESERVED = 200  # Reserved for applications
PROP_TMP = 255  # Temporary property
