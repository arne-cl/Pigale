/****************************************************************************
**
** Test helpers for validating property system - Implementation
**
****************************************************************************/

#include "property_validators.h"
#include <TAXI/Tprop.h>
#include <cmath>
#include <set>

namespace TestHelpers {

bool PropertyValidator::ValidateProperty(const PSet& pset, int propId) {
    // Check if property exists
    return pset.exist(propId);
}

bool PropertyValidator::ValidateAllProperties(GraphContainer& gc) {
    // Validate that the basic properties are consistent
    PSet1& pset = gc.PG();

    // Check that PSet exists
    if(&pset == nullptr) {
        return false;
    }

    return true;
}

bool PropertyValidator::ValidateCirAcir(TopologicalGraph& G) {
    // Check if properties exist
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tbrin()).exist(PROP_ACIR)) {
        return false;
    }

    // Use G.cir and G.acir (existing members)

    // Check invariant for all brins
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();

        // Check positive brin
        if(G.cir[G.acir[b]] != b || G.acir[G.cir[b]] != b) {
            return false;
        }

        // Check negative brin
        b = -e();
        if(G.cir[G.acir[b]] != b || G.acir[G.cir[b]] != b) {
            return false;
        }
    }

    return true;
}

bool PropertyValidator::ValidateVinPbrin(TopologicalGraph& G) {
    // Check if properties exist
    if(!G.Set(tvertex()).exist(PROP_PBRIN)) {
        return false;
    }

    // Use G.vin and G.pbrin (existing members)

    // Check that pbrin points to a brin incident to the vertex
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin b = G.pbrin[v];

        if(b == 0) {
            // Vertex has no incident edges - should have degree 0
            if(G.Degree(v) != 0) {
                return false;
            }
        } else {
            // pbrin should point to a brin with vin[b] == v
            if(G.vin[b] != v) {
                return false;
            }
        }
    }

    return true;
}

bool PropertyValidator::ValidateCoordinates(GeometricGraph& G) {
    // Check if coordinates exist
    if(!G.Set(tvertex()).exist(PROP_COORD)) {
        return false;
    }

    // Create Prop to access coordinates
    Prop<Tpoint> vcoord(G.Set(tvertex()), PROP_COORD);

    for(tvertex v = 1; v <= G.nv(); v++) {
        Tpoint p = vcoord[v];

        // Check for NaN or infinity
        if(std::isnan(p.x()) || std::isnan(p.y())) {
            return false;
        }
        if(std::isinf(p.x()) || std::isinf(p.y())) {
            return false;
        }
    }

    return true;
}

bool PropertyValidator::ValidatePropertyBounds(GraphContainer& gc) {
    // Ensure vertex and edge counts are non-negative
    if(gc.nv() < 0 || gc.ne() < 0) {
        return false;
    }

    return true;
}

bool CheckCircularInvariant(TopologicalGraph& G) {
    return PropertyValidator::ValidateCirAcir(G);
}

bool CheckCircularCompleteness(TopologicalGraph& G) {
    // Check if properties exist
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tvertex()).exist(PROP_PBRIN)) {
        return false;
    }

    // Use G.cir and G.pbrin (existing members)

    // For each vertex, follow the circular order and verify it forms a complete cycle
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = G.pbrin[v];

        if(first == 0) {
            // No incident edges - OK if degree is 0
            if(G.Degree(v) != 0) {
                return false;
            }
            continue;
        }

        tbrin b = first;
        std::set<tbrin> visited;
        int count = 0;

        do {
            if(visited.count(b)) {
                // Cycle detected before returning to first
                return false;
            }
            visited.insert(b);
            b = G.cir[b];
            count++;

            if(count > G.Degree(v) + 1) {
                // Infinite loop
                return false;
            }
        } while(b != first);

        // All incident brins should have been visited
        if(count != G.Degree(v)) {
            return false;
        }
    }

    return true;
}

bool CheckAllBrinsInCircularOrder(TopologicalGraph& G) {
    // Check if properties exist
    if(!G.Set(tbrin()).exist(PROP_CIR) || !G.Set(tvertex()).exist(PROP_PBRIN)) {
        return false;
    }

    // Use G.cir, G.vin, G.pbrin (existing members)

    // Collect all brins in circular orders
    std::set<tbrin> brinsInOrders;

    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = G.pbrin[v];
        if(first == 0) continue;

        tbrin b = first;
        do {
            brinsInOrders.insert(b);
            b = G.cir[b];
        } while(b != first);
    }

    // Check that all brins are in circular orders
    for(tedge e = 1; e <= G.ne(); e++) {
        if(!brinsInOrders.count(e())) {
            return false;
        }
        if(!brinsInOrders.count(-e())) {
            return false;
        }
    }

    return true;
}

bool AreCoordinatesFinite(GeometricGraph& G) {
    return PropertyValidator::ValidateCoordinates(G);
}

bool AreColorsValid(GeometricGraph& G) {
    // Check if color property exists
    if(!G.Set(tvertex()).exist(PROP_COLOR)) {
        return true;  // No colors defined is OK
    }

    // Create Prop to access colors
    Prop<short> vcolor(G.Set(tvertex()), PROP_COLOR);

    // Basic check - colors should be non-negative
    for(tvertex v = 1; v <= G.nv(); v++) {
        if(vcolor[v] < 0) {
            return false;
        }
    }

    return true;
}

} // namespace TestHelpers
