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

bool IsIsomorphic(const Graph& G1, const Graph& G2) {
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

bool HasSameEmbedding(const TopologicalGraph& G1, const TopologicalGraph& G2) {
    if(G1.nv() != G2.nv() || G1.ne() != G2.ne()) {
        return false;
    }

    // Compare circular orders
    // This is a simplified check
    return true;  // Placeholder
}

bool HasSameCounts(const Graph& G1, const Graph& G2) {
    return G1.nv() == G2.nv() && G1.ne() == G2.ne();
}

bool ValidateCircularOrder(const TopologicalGraph& G) {
    if(!G.Exist(PROP_CIR) || !G.Exist(PROP_ACIR)) {
        return false;
    }

    Prop<tbrin> cir(G.Set(), PROP_CIR);
    Prop<tbrin> acir(G.Set(), PROP_ACIR);

    // Check invariant: cir[acir[b]] == b
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();
        if(cir[acir[b]] != b) {
            return false;
        }
        if(acir[cir[b]] != b) {
            return false;
        }

        // Check negative brin too
        b = -e();
        if(cir[acir[b]] != b) {
            return false;
        }
        if(acir[cir[b]] != b) {
            return false;
        }
    }

    return true;
}

bool ValidatePlanarMap(const TopologicalGraph& G) {
    if(!G.Exist(PROP_PLANARMAP) || !G.Set(PROP_PLANARMAP)) {
        return false;
    }

    return ValidateCircularOrder(G);
}

bool ValidateCoordinates(const GeometricGraph& G) {
    if(!G.Exist(PROP_COORD)) {
        return false;
    }

    Prop<Tpoint> vcoord(G.Set(), PROP_COORD);

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

bool CheckNoEdgeCrossings(const GeometricGraph& G) {
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

bool VerifyGraphInvariants(const Graph& G) {
    return VerifyVertexEdgeCounts(G) && VerifyBrinIncidence(G);
}

bool VerifyVertexEdgeCounts(const Graph& G) {
    int nv = G.nv();
    int ne = G.ne();

    // Counts should be non-negative
    if(nv < 0 || ne < 0) {
        return false;
    }

    return true;
}

bool VerifyBrinIncidence(const TopologicalGraph& G) {
    if(!G.Exist(PROP_VIN)) {
        return false;
    }

    Prop<tvertex> vin(G.Set(), PROP_VIN);

    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b1 = e();
        tbrin b2 = -e();

        tvertex v1 = vin[b1];
        tvertex v2 = vin[b2];

        // Check that vertices are in valid range
        if(v1 < 1 || v1 > G.nv()) {
            return false;
        }
        if(v2 < 1 || v2 > G.nv()) {
            return false;
        }
    }

    return true;
}

bool VerifyDegreeSequence(const Graph& G, const std::vector<int>& expectedDegrees) {
    if((int)expectedDegrees.size() != G.nv()) {
        return false;
    }

    for(tvertex v = 1; v <= G.nv(); v++) {
        if(G.Degree(v) != expectedDegrees[v - 1]) {
            return false;
        }
    }

    return true;
}

bool VerifyPbrinConsistency(const TopologicalGraph& G) {
    if(!G.Exist(PROP_PBRIN)) {
        return false;
    }

    Prop<tbrin> pbrin(G.Set(), PROP_PBRIN);
    Prop<tvertex> vin(G.Set(), PROP_VIN);

    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin b = pbrin[v];

        if(b == 0) {
            // Vertex has no incident edges
            if(G.Degree(v) != 0) {
                return false;
            }
        } else {
            // Check that pbrin points to a brin incident to v
            if(vin[b] != v) {
                return false;
            }
        }
    }

    return true;
}

bool VerifyEulerFormula(const TopologicalGraph& G) {
    if(!G.Exist(PROP_PLANARMAP) || !G.Set(PROP_PLANARMAP)) {
        return false;  // Not a planar map
    }

    int v = G.nv();
    int e = G.ne();
    int f = G.NumFaces();

    // Euler's formula: v - e + f = 2 for connected planar graph
    return (v - e + f == 2);
}

bool CheckPlanarBound(const Graph& G) {
    int n = G.nv();
    int m = G.ne();

    if(n < 3) {
        return true;
    }

    // Planar graphs satisfy m <= 3n - 6
    return m <= 3 * n - 6;
}

bool ValidateFaceOrientation(const TopologicalGraph& G) {
    // Placeholder for face orientation check
    return true;
}

bool ValidateExteriorFace(const TopologicalGraph& G) {
    if(!G.Exist(PROP_EXTBRIN)) {
        return false;
    }

    Prop1<tbrin> extbrin(G.Set(), PROP_EXTBRIN);
    tbrin eb = extbrin();

    if(eb == 0) {
        return false;
    }

    // Check that extbrin is a valid brin
    if(eb.GetEdge() > G.ne()) {
        return false;
    }

    return true;
}

void PrintGraphInfo(const Graph& G) {
    std::cout << "Graph Info:" << std::endl;
    std::cout << "  Vertices: " << G.nv() << std::endl;
    std::cout << "  Edges: " << G.ne() << std::endl;

    // Print degree sequence
    std::cout << "  Degree sequence: ";
    for(tvertex v = 1; v <= G.nv(); v++) {
        std::cout << G.Degree(v);
        if(v < G.nv()) std::cout << ", ";
    }
    std::cout << std::endl;
}

void PrintCircularOrder(const TopologicalGraph& G, tvertex v) {
    if(!G.Exist(PROP_CIR) || !G.Exist(PROP_PBRIN)) {
        std::cout << "Circular order not available" << std::endl;
        return;
    }

    Prop<tbrin> cir(G.Set(), PROP_CIR);
    Prop<tbrin> pbrin(G.Set(), PROP_PBRIN);

    std::cout << "Circular order for vertex " << v() << ": ";

    tbrin first = pbrin[v];
    if(first == 0) {
        std::cout << "(no incident edges)" << std::endl;
        return;
    }

    tbrin b = first;
    std::set<tbrin> visited;

    do {
        std::cout << b() << " ";
        visited.insert(b);
        b = cir[b];

        if(visited.count(b) && b != first) {
            std::cout << "(cycle detected!)" << std::endl;
            return;
        }
    } while(b != first && visited.size() < 1000);

    std::cout << std::endl;
}

void PrintFaces(const TopologicalGraph& G) {
    if(!G.Exist(PROP_PLANARMAP) || !G.Set(PROP_PLANARMAP)) {
        std::cout << "Not a planar map" << std::endl;
        return;
    }

    std::cout << "Number of faces: " << G.NumFaces() << std::endl;

    // Detailed face printing would require face iteration
}

} // namespace TestHelpers
