/****************************************************************************
**
** Test helpers for validating and comparing graphs - Implementation
**
****************************************************************************/

#include "graph_comparators.h"
#include <TAXI/Tprop.h>
#include <iostream>
#include <set>
#include <cmath>

namespace TestHelpers {

bool IsIsomorphic(Graph& G1, Graph& G2) {
    // TODO: Full isomorphism testing is NP-complete
    // This is a basic check only - just checks counts match
    // For proper isomorphism testing, would need to implement
    // graph isomorphism algorithm (e.g., VF2, Ullmann, etc.)

    if(G1.nv() != G2.nv() || G1.ne() != G2.ne()) {
        return false;
    }

    // Could add degree sequence comparison here for better filtering

    // WARNING: This returns true if counts match, but graphs may not be isomorphic
    return true;  // Placeholder - needs full implementation
}

bool HasSameEmbedding(TopologicalGraph& G1, TopologicalGraph& G2) {
    if(G1.nv() != G2.nv() || G1.ne() != G2.ne()) {
        return false;
    }

    // Compare circular orders
    // This is a simplified check
    return true;  // Placeholder
}

bool HasSameCounts(Graph& G1, Graph& G2) {
    return G1.nv() == G2.nv() && G1.ne() == G2.ne();
}

bool ValidateCircularOrder(TopologicalGraph& G) {
    // Check if circular order properties exist
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tbrin()).exist(PROP_ACIR)) {
        return false;
    }

    // Use existing G.cir and G.acir members (don't create new ones!)

    // Check invariant: cir[acir[b]] == b for all brins
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();
        if(G.cir[G.acir[b]] != b) {
            return false;
        }
        if(G.acir[G.cir[b]] != b) {
            return false;
        }

        // Check negative brin too
        b = -e();
        if(G.cir[G.acir[b]] != b) {
            return false;
        }
        if(G.acir[G.cir[b]] != b) {
            return false;
        }
    }

    return true;
}

bool ValidatePlanarMap(TopologicalGraph& G) {
    // Check if planar map property exists
    if(!G.Set().exist(PROP_PLANARMAP) || !G.planarMap()) {
        return false;
    }

    return ValidateCircularOrder(G);
}

bool ValidateCoordinates(GeometricGraph& G) {
    // Check if coordinates exist
    if(!G.Set(tvertex()).exist(PROP_COORD)) {
        return false;
    }

    // Create Prop to access coordinates
    Prop<Tpoint> vcoord(G.Set(tvertex()), PROP_COORD);

    for(tvertex v = 1; v <= G.nv(); v++) {
        Tpoint p = vcoord[v];
        if(std::isnan(p.x()) || std::isnan(p.y())) {
            return false;
        }
        if(std::isinf(p.x()) || std::isinf(p.y())) {
            return false;
        }
    }

    return true;
}

bool CheckNoEdgeCrossings(GeometricGraph& G) {
    // TODO: Implement edge crossing detection
    // This requires checking all pairs of non-adjacent edges for intersection
    // Algorithm:
    // 1. For each pair of edges (e1, e2) where e1 and e2 don't share a vertex
    // 2. Get coordinates of endpoints: (p1,p2) for e1, (p3,p4) for e2
    // 3. Check if line segments intersect using geometric test
    // 4. If any pair intersects, return false

    // WARNING: This is a placeholder that always returns true
    // Actual crossing detection not yet implemented
    return true;
}

bool VerifyGraphInvariants(Graph& G) {
    // VerifyBrinIncidence needs TopologicalGraph
    // For now, just check vertex edge counts
    return VerifyVertexEdgeCounts(G);
}

bool VerifyVertexEdgeCounts(Graph& G) {
    int nv = G.nv();
    int ne = G.ne();

    // Counts should be non-negative
    if(nv < 0 || ne < 0) {
        return false;
    }

    return true;
}

bool VerifyBrinIncidence(TopologicalGraph& G) {
    // Use G.vin (inherited from Graph)

    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b1 = e();
        tbrin b2 = -e();

        tvertex v1 = G.vin[b1];
        tvertex v2 = G.vin[b2];

        // Check that vertices are in valid range
        if(v1() < 1 || v1() > G.nv()) {
            return false;
        }
        if(v2() < 1 || v2() > G.nv()) {
            return false;
        }
    }

    return true;
}

bool VerifyDegreeSequence(TopologicalGraph& G, const std::vector<int>& expectedDegrees) {
    if((int)expectedDegrees.size() != G.nv()) {
        return false;
    }

    for(tvertex v = 1; v <= G.nv(); v++) {
        if(G.Degree(v) != expectedDegrees[v() - 1]) {  // v() gets int, -1 for 0-based vector
            return false;
        }
    }

    return true;
}

bool VerifyPbrinConsistency(TopologicalGraph& G) {
    // Check if pbrin property exists
    if(!G.Set(tvertex()).exist(PROP_PBRIN)) {
        return false;
    }

    // Use G.pbrin and G.vin (existing members)

    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin b = G.pbrin[v];

        if(b == 0) {
            // Vertex has no incident edges
            if(G.Degree(v) != 0) {
                return false;
            }
        } else {
            // Check that pbrin points to a brin incident to v
            if(G.vin[b] != v) {
                return false;
            }
        }
    }

    return true;
}

bool VerifyEulerFormula(TopologicalGraph& G) {
    // Check if it's a planar map
    if(!G.Set().exist(PROP_PLANARMAP) || !G.planarMap()) {
        return false;  // Not a planar map
    }

    int v = G.nv();
    int e = G.ne();
    int f = G.NumFaces();

    // Euler's formula: v - e + f = 2 for connected planar graph
    return (v - e + f == 2);
}

bool CheckPlanarBound(Graph& G) {
    int n = G.nv();
    int m = G.ne();

    if(n < 3) {
        return true;
    }

    // Planar graphs satisfy m <= 3n - 6
    return m <= 3 * n - 6;
}

bool ValidateFaceOrientation(TopologicalGraph& G) {
    // Placeholder for face orientation check
    return true;
}

bool ValidateExteriorFace(TopologicalGraph& G) {
    // Check if exterior brin property exists
    if(!G.Set().exist(PROP_EXTBRIN)) {
        return false;
    }

    // Use G.extbrin (existing member)
    tbrin eb = G.extbrin();

    if(eb == 0) {
        return false;
    }

    // Check that extbrin is a valid brin
    if(eb.GetEdge() > G.ne()) {
        return false;
    }

    return true;
}

void PrintGraphInfo(Graph& G) {
    std::cout << "Graph Info:" << std::endl;
    std::cout << "  Vertices: " << G.nv() << std::endl;
    std::cout << "  Edges: " << G.ne() << std::endl;

    // To print degrees, need TopologicalGraph
    // Cast if possible, or just skip for basic Graph
    std::cout << "  (Use TopologicalGraph for degree info)" << std::endl;
}

void PrintCircularOrder(TopologicalGraph& G, tvertex v) {
    // Check if circular order exists
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tvertex()).exist(PROP_PBRIN)) {
        std::cout << "Circular order not available" << std::endl;
        return;
    }

    std::cout << "Circular order for vertex " << v() << ": ";

    // Use G.pbrin and G.cir (existing members)
    tbrin first = G.pbrin[v];
    if(first == 0) {
        std::cout << "(no incident edges)" << std::endl;
        return;
    }

    tbrin b = first;
    std::set<tbrin> visited;

    do {
        std::cout << b() << " ";
        visited.insert(b);
        b = G.cir[b];

        if(visited.count(b) && b != first) {
            std::cout << "(cycle detected!)" << std::endl;
            return;
        }
    } while(b != first && visited.size() < 1000);

    std::cout << std::endl;
}

void PrintFaces(TopologicalGraph& G) {
    // Check if it's a planar map
    if(!G.Set().exist(PROP_PLANARMAP) || !G.planarMap()) {
        std::cout << "Not a planar map" << std::endl;
        return;
    }

    std::cout << "Number of faces: " << G.NumFaces() << std::endl;

    // Detailed face printing would require face iteration
}

} // namespace TestHelpers
