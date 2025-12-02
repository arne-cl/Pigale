#!/usr/bin/env python3
"""
pigale-convert: Command-line tool for converting Pigale graph files.

Supports conversion between TGF (binary), TXT (ASCII), and GraphML (XML) formats.
"""

import sys
import argparse
from pathlib import Path
from typing import Optional

from pigale_tgf.graph import (
    GraphContainer,
    read_txt_graph,
    write_txt_graph,
    read_tgf_graph,
    write_tgf_graph,
)
from pigale_tgf.graphml_format import read_graphml, write_graphml
from pigale_tgf.txt_format import is_file_txt
from pigale_tgf.constants import PROP_N, PROP_M


def detect_format(filename: Path) -> str:
    """
    Detect file format from extension or content.

    Args:
        filename: Path to file

    Returns:
        Format string: 'tgf', 'txt', or 'graphml'

    Raises:
        ValueError: If format cannot be determined
    """
    # Try extension first
    ext = filename.suffix.lower()
    if ext == '.tgf':
        return 'tgf'
    elif ext == '.txt':
        return 'txt'
    elif ext in ['.graphml', '.xml']:
        return 'graphml'

    # Try content detection
    if filename.exists():
        # Check for TXT format
        if is_file_txt(filename):
            return 'txt'

        # Check for TGF format (starts with "TGF" magic bytes)
        with open(filename, 'rb') as f:
            magic = f.read(3)
            if magic == b'TGF':
                return 'tgf'

        # Check for GraphML format (XML starting with <?xml or <graphml)
        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            first_line = f.readline().strip()
            if '<?xml' in first_line or '<graphml' in first_line:
                return 'graphml'

    raise ValueError(f"Cannot determine format for: {filename}")


def read_graph(filename: Path, record_num: int = 1, verbose: bool = False) -> GraphContainer:
    """
    Read graph from file, auto-detecting format.

    Args:
        filename: Path to input file
        record_num: Record number for TGF files (1-indexed)
        verbose: Print progress messages

    Returns:
        GraphContainer with loaded graph
    """
    fmt = detect_format(filename)

    if verbose:
        print(f"Reading {fmt.upper()} file: {filename}", file=sys.stderr)

    if fmt == 'tgf':
        graph = read_tgf_graph(filename, record_num=record_num)
    elif fmt == 'txt':
        graph = read_txt_graph(filename)
    elif fmt == 'graphml':
        graph = read_graphml(filename)
    else:
        raise ValueError(f"Unsupported input format: {fmt}")

    if verbose:
        # Get graph stats
        nv_val = graph.general[PROP_N]
        ne_val = graph.general[PROP_M]
        nv = int(nv_val[0]) if hasattr(nv_val, '__getitem__') else int(nv_val)
        ne = int(ne_val[0]) if hasattr(ne_val, '__getitem__') else int(ne_val)
        print(f"  Loaded graph: {nv} vertices, {ne} edges", file=sys.stderr)

    return graph


def write_graph(graph: GraphContainer, filename: Path, verbose: bool = False):
    """
    Write graph to file, auto-detecting format from extension.

    Args:
        graph: GraphContainer to write
        filename: Path to output file
        verbose: Print progress messages
    """
    fmt = detect_format(filename)

    if verbose:
        print(f"Writing {fmt.upper()} file: {filename}", file=sys.stderr)

    if fmt == 'tgf':
        write_tgf_graph(graph, filename)
    elif fmt == 'txt':
        write_txt_graph(graph, filename)
    elif fmt == 'graphml':
        write_graphml(graph, filename)
    else:
        raise ValueError(f"Unsupported output format: {fmt}")

    if verbose:
        size = filename.stat().st_size
        print(f"  Written: {size} bytes", file=sys.stderr)


def main():
    """Main entry point for pigale-convert CLI."""
    parser = argparse.ArgumentParser(
        prog='pigale-convert',
        description='Convert between Pigale graph file formats (TGF, TXT, GraphML)',
        epilog='Examples:\n'
               '  %(prog)s input.tgf output.graphml\n'
               '  %(prog)s --record 2 input.tgf output.txt\n'
               '  %(prog)s -v input.graphml output.tgf',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument(
        'input',
        type=Path,
        help='Input file (TGF, TXT, or GraphML)'
    )

    parser.add_argument(
        'output',
        type=Path,
        help='Output file (format detected from extension)'
    )

    parser.add_argument(
        '-r', '--record',
        type=int,
        default=1,
        metavar='N',
        help='Record number to read from TGF file (default: 1)'
    )

    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Print progress messages'
    )

    parser.add_argument(
        '--version',
        action='version',
        version='pigale-convert 1.0.0'
    )

    args = parser.parse_args()

    # Validate input file exists
    if not args.input.exists():
        print(f"Error: Input file not found: {args.input}", file=sys.stderr)
        return 1

    try:
        # Read graph
        graph = read_graph(args.input, record_num=args.record, verbose=args.verbose)

        # Write graph
        write_graph(graph, args.output, verbose=args.verbose)

        if args.verbose:
            print("Conversion complete!", file=sys.stderr)

        return 0

    except ValueError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Error: {type(e).__name__}: {e}", file=sys.stderr)
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
