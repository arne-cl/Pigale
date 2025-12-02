"""
GraphML format reading and writing.

Provides functions to read/write graphs in GraphML XML format,
supporting both Pigale property mappings and generic GraphML properties.
"""

import xml.etree.ElementTree as ET
import numpy as np
from pathlib import Path
from typing import Union, Dict, Any

from pigale_tgf.graph import GraphContainer
from pigale_tgf.constants import (
    PROP_N, PROP_M, PROP_LABEL, PROP_COLOR, PROP_COORD, PROP_VIN, PROP_WIDTH
)

# Special attribute name for storing generic GraphML data
GRAPHML_DATA_ATTR = '_graphml_data'


def read_graphml(filename: Union[str, Path]):
    """
    Read graph from GraphML file.

    Args:
        filename: Path to GraphML file

    Returns:
        GraphContainer with graph data

    Note:
        - Supports both Pigale property keys (Pigale/V/*, Pigale/E/*) and generic GraphML keys
        - All GraphML properties are preserved in GraphContainer._graphml_data
        - Coordinates: special handling for 'x' and 'y' attributes
        - Node IDs mapped to sequential vertex indices
    """
    filename = Path(filename)

    # Parse XML
    tree = ET.parse(filename)
    root = tree.getroot()

    # GraphML namespace
    ns = {'graphml': 'http://graphml.graphdrawing.org/xmlns'}

    # Find graph element
    graph_elem = root.find('graphml:graph', ns)
    if graph_elem is None:
        raise ValueError(f"No graph element found in {filename}")

    # Parse ALL keys (not just Pigale ones)
    key_defs = {}  # key_id -> {for, attr.name, attr.type}
    pigale_key_map = {}  # Pigale key_id -> (for, attr.name, prop_num)

    for key_elem in root.findall('graphml:key', ns):
        key_id = key_elem.get('id')
        key_for = key_elem.get('for')
        attr_name = key_elem.get('attr.name')
        attr_type = key_elem.get('attr.type', 'string')

        key_defs[key_id] = {
            'for': key_for,
            'attr.name': attr_name,
            'attr.type': attr_type
        }

        # Parse Pigale property keys: "Pigale/V/16" -> (node, 16)
        if key_id and key_id.startswith('Pigale/'):
            parts = key_id.split('/')
            if len(parts) == 3:
                prop_type = parts[1]  # V or E
                prop_num = int(parts[2])
                pigale_key_map[key_id] = (key_for, attr_name, prop_num)

    # Build node mapping: node_id -> vertex_index
    nodes = graph_elem.findall('graphml:node', ns)
    node_to_idx = {}
    idx_to_node = {}
    for idx, node_elem in enumerate(nodes, start=1):
        node_id = node_elem.get('id')
        node_to_idx[node_id] = idx
        idx_to_node[idx] = node_id

    nv = len(nodes)

    # Storage for ALL node data (generic GraphML properties)
    node_data_all = {}  # vertex_idx -> {key_id: value}
    x_coords = {}  # vertex_idx -> x value
    y_coords = {}  # vertex_idx -> y value

    # Parse ALL node properties (not just Pigale ones)
    node_properties = {}  # prop_num -> {idx: value} (for Pigale properties)

    for idx, node_elem in enumerate(nodes, start=1):
        node_data_all[idx] = {}

        for data_elem in node_elem.findall('graphml:data', ns):
            key_id = data_elem.get('key')
            value = data_elem.text

            # Store ALL data
            node_data_all[idx][key_id] = value

            # Special handling for x,y coordinates
            if key_id in key_defs:
                attr_name = key_defs[key_id].get('attr.name', '')
                if attr_name == 'x':
                    try:
                        x_coords[idx] = float(value)
                    except (ValueError, TypeError):
                        pass
                elif attr_name == 'y':
                    try:
                        y_coords[idx] = float(value)
                    except (ValueError, TypeError):
                        pass

            # Also parse Pigale properties
            if key_id in pigale_key_map:
                key_for, attr_name, prop_num = pigale_key_map[key_id]
                if key_for == 'node':
                    if prop_num not in node_properties:
                        node_properties[prop_num] = {}
                    node_properties[prop_num][idx] = value

    # Parse edges
    edges = []
    edge_data_all = {}  # edge_idx -> {key_id: value}
    edge_properties = {}  # prop_num -> {edge_idx: value} (for Pigale properties)

    for edge_idx, edge_elem in enumerate(graph_elem.findall('graphml:edge', ns), start=1):
        source = edge_elem.get('source')
        target = edge_elem.get('target')

        src_idx = node_to_idx[source]
        tgt_idx = node_to_idx[target]
        edges.append((src_idx, tgt_idx))

        edge_data_all[edge_idx] = {}

        # Parse ALL edge properties
        for data_elem in edge_elem.findall('graphml:data', ns):
            key_id = data_elem.get('key')
            value = data_elem.text

            # Store ALL data
            edge_data_all[edge_idx][key_id] = value

            # Also parse Pigale properties
            if key_id in pigale_key_map:
                key_for, attr_name, prop_num = pigale_key_map[key_id]
                if key_for == 'edge':
                    if prop_num not in edge_properties:
                        edge_properties[prop_num] = {}
                    edge_properties[prop_num][edge_idx] = value

    ne = len(edges)

    # Create graph container
    graph = GraphContainer()
    graph.setsize(nv, ne)

    # Store generic GraphML data in graph container
    setattr(graph, GRAPHML_DATA_ATTR, {
        'keys': key_defs,
        'node_data': node_data_all,
        'edge_data': edge_data_all,
        'node_id_map': idx_to_node
    })

    # Create PROP_COORD from x,y if available
    if x_coords and y_coords:
        coords = np.zeros(nv, dtype=np.dtype([('x', '<f8'), ('y', '<f8')]))
        for idx in range(1, nv + 1):
            if idx in x_coords and idx in y_coords:
                coords[idx - 1] = (x_coords[idx], y_coords[idx])
        graph.vertex[PROP_COORD] = coords

    # Store Pigale vertex properties
    for prop_num, values in node_properties.items():
        if prop_num == PROP_COORD:
            # Parse coordinates "x,y" format
            coords = np.zeros(nv, dtype=np.dtype([('x', '<f8'), ('y', '<f8')]))
            for idx in range(1, nv + 1):
                if idx in values and values[idx]:
                    try:
                        x_str, y_str = values[idx].split(',')
                        coords[idx - 1] = (float(x_str), float(y_str))
                    except (ValueError, AttributeError):
                        pass
            graph.vertex[PROP_COORD] = coords
        elif prop_num == PROP_LABEL:
            # Parse labels as integers
            labels = np.zeros(nv, dtype=np.int32)
            for idx in range(1, nv + 1):
                if idx in values:
                    labels[idx - 1] = int(values[idx])
            graph.vertex[PROP_LABEL] = labels
        elif prop_num == PROP_COLOR:
            # Parse colors as integers
            colors = np.zeros(nv, dtype=np.int16)
            for idx in range(1, nv + 1):
                if idx in values:
                    colors[idx - 1] = int(values[idx])
            graph.vertex[PROP_COLOR] = colors

    # Create VIN property (edge incidence)
    vin = np.zeros(2 * ne + 1, dtype=np.int32)
    vin[0] = 0  # Brin 0 is always 0

    for edge_idx, (src_idx, tgt_idx) in enumerate(edges, start=1):
        # Positive brin (edge_idx): source vertex
        # Negative brin (-edge_idx): target vertex
        vin[ne + edge_idx] = src_idx   # Positive brin
        vin[ne - edge_idx] = tgt_idx   # Negative brin

    graph.brin[PROP_VIN] = vin

    return graph


def write_graphml(graph, filename: Union[str, Path]):
    """
    Write graph to GraphML file.

    Args:
        graph: GraphContainer to write
        filename: Path to output GraphML file

    Note:
        - Writes ALL GraphML properties (both Pigale and generic)
        - Preserves original key definitions and data from read_graphml
        - Falls back to Pigale-style output if no GraphML data stored
    """
    filename = Path(filename)

    # Get sizes
    nv_val = graph.general[PROP_N]
    ne_val = graph.general[PROP_M]
    nv = int(nv_val[0]) if isinstance(nv_val, np.ndarray) else int(nv_val)
    ne = int(ne_val[0]) if isinstance(ne_val, np.ndarray) else int(ne_val)

    # Create XML structure
    root = ET.Element('graphml')
    root.set('xmlns', 'http://graphml.graphdrawing.org/xmlns')
    root.set('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
    root.set('xsi:schemaLocation',
             'http://graphml.graphdrawing.org/xmlns '
             'http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd')

    # Check if we have stored GraphML data
    graphml_data = getattr(graph, GRAPHML_DATA_ATTR, None)

    if graphml_data:
        # Write ALL keys from original file
        key_defs = graphml_data['keys']
        for key_id, key_info in key_defs.items():
            key = ET.SubElement(root, 'key')
            key.set('id', key_id)
            key.set('for', key_info['for'])
            key.set('attr.name', key_info['attr.name'])
            key.set('attr.type', key_info['attr.type'])

        # Create graph element
        graph_elem = ET.SubElement(root, 'graph')
        graph_elem.set('id', 'G')
        graph_elem.set('edgedefault', 'undirected')

        # Write nodes with ALL their data
        node_data_all = graphml_data['node_data']
        node_id_map = graphml_data.get('node_id_map', {})

        for v in range(1, nv + 1):
            node = ET.SubElement(graph_elem, 'node')
            # Use original node ID if available
            node_id = node_id_map.get(v, f'n{v}')
            node.set('id', node_id)

            # Write ALL data for this node
            if v in node_data_all:
                for key_id, value in node_data_all[v].items():
                    data = ET.SubElement(node, 'data')
                    data.set('key', key_id)
                    data.text = str(value) if value is not None else ''

        # Write edges with ALL their data
        edge_data_all = graphml_data['edge_data']

        if PROP_VIN in graph.brin.properties:
            vin = graph.brin[PROP_VIN]

            for e in range(1, ne + 1):
                src_idx = vin[ne + e]
                tgt_idx = vin[ne - e]

                edge = ET.SubElement(graph_elem, 'edge')
                src_node_id = node_id_map.get(src_idx, f'n{src_idx}')
                tgt_node_id = node_id_map.get(tgt_idx, f'n{tgt_idx}')
                edge.set('source', src_node_id)
                edge.set('target', tgt_node_id)

                # Write ALL data for this edge
                if e in edge_data_all:
                    for key_id, value in edge_data_all[e].items():
                        data = ET.SubElement(edge, 'data')
                        data.set('key', key_id)
                        data.text = str(value) if value is not None else ''

    else:
        # Fallback: Write Pigale-style properties only
        # Define keys for properties that exist
        if PROP_LABEL in graph.vertex.properties:
            key = ET.SubElement(root, 'key')
            key.set('id', f'Pigale/V/{PROP_LABEL}')
            key.set('for', 'node')
            key.set('attr.name', 'Label')
            key.set('attr.type', 'string')

        if PROP_COLOR in graph.vertex.properties:
            key = ET.SubElement(root, 'key')
            key.set('id', f'Pigale/V/{PROP_COLOR}')
            key.set('for', 'node')
            key.set('attr.name', 'Color')
            key.set('attr.type', 'string')

        if PROP_COORD in graph.vertex.properties:
            key = ET.SubElement(root, 'key')
            key.set('id', f'Pigale/V/{PROP_COORD}')
            key.set('for', 'node')
            key.set('attr.name', 'Coordinates')
            key.set('attr.type', 'string')

        # Create graph element
        graph_elem = ET.SubElement(root, 'graph')
        graph_elem.set('id', 'G')
        graph_elem.set('edgedefault', 'undirected')

        # Write nodes
        for v in range(1, nv + 1):
            node = ET.SubElement(graph_elem, 'node')
            node.set('id', f'n{v}')

            # Write vertex properties
            if PROP_LABEL in graph.vertex.properties:
                labels = graph.vertex[PROP_LABEL]
                data = ET.SubElement(node, 'data')
                data.set('key', f'Pigale/V/{PROP_LABEL}')
                data.text = str(labels[v - 1])

            if PROP_COLOR in graph.vertex.properties:
                colors = graph.vertex[PROP_COLOR]
                data = ET.SubElement(node, 'data')
                data.set('key', f'Pigale/V/{PROP_COLOR}')
                data.text = str(colors[v - 1])

            if PROP_COORD in graph.vertex.properties:
                coords = graph.vertex[PROP_COORD]
                data = ET.SubElement(node, 'data')
                data.set('key', f'Pigale/V/{PROP_COORD}')
                # Check if coords has structured dtype
                if hasattr(coords.dtype, 'names') and coords.dtype.names:
                    x = coords[v - 1]['x']
                    y = coords[v - 1]['y']
                else:
                    try:
                        coord = coords[v - 1]
                        if hasattr(coord, 'dtype') and coord.dtype.names:
                            x = coord['x']
                            y = coord['y']
                        else:
                            x, y = float(coord[0]), float(coord[1])
                    except (KeyError, TypeError, IndexError):
                        continue
                data.text = f'{x},{y}'

        # Write edges
        if PROP_VIN in graph.brin.properties:
            vin = graph.brin[PROP_VIN]

            for e in range(1, ne + 1):
                src_idx = vin[ne + e]
                tgt_idx = vin[ne - e]

                edge = ET.SubElement(graph_elem, 'edge')
                edge.set('source', f'n{src_idx}')
                edge.set('target', f'n{tgt_idx}')

    # Write to file
    tree = ET.ElementTree(root)
    ET.indent(tree, space='  ')
    tree.write(filename, encoding='UTF-8', xml_declaration=True)

