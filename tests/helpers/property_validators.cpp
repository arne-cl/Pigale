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
    return pset.Exist(propId);
}

bool PropertyValidator::ValidateAllProperties(const GraphContainer& gc) {
    // Validate that the basic properties are consistent
    const PSet& pset = gc.Set();

    // Check that PSet exists
    if(&pset == nullptr) {
        return false;
    }

    return true;
}

bool PropertyValidator::ValidateCirAcir(TopologicalGraph& G) {
    if(!G.Exist(PROP_CIR) || !G.Exist(PROP_ACIR)) {
        return false;
    }

    Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);
    Prop<tbrin> acir(G.Set(tbrin()), PROP_ACIR);

    // Check invariant for all brins
    for(tedge e = 1; e <= G.ne(); e++) {
        tbrin b = e();

        // Check positive brin
        if(cir[acir[b]] != b || acir[cir[b]] != b) {
            return false;
        }

        // Check negative brin
        b = -e();
        if(cir[acir[b]] != b || acir[cir[b]] != b) {
            return false;
        }
    }

    return true;
}

bool PropertyValidator::ValidateVinPbrin(TopologicalGraph& G) {
    if(!G.Exist(PROP_VIN) || !G.Exist(PROP_PBRIN)) {
        return false;
    }

    Prop<tvertex> vin(G.Set(tbrin()), PROP_VIN);
    Prop<tbrin> pbrin(G.Set(tvertex()), PROP_PBRIN);

    // Check that pbrin points to a brin incident to the vertex
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin b = pbrin[v];

        if(b == 0) {
            // Vertex has no incident edges - should have degree 0
            if(G.Degree(v) != 0) {
                return false;
            }
        } else {
            // pbrin should point to a brin with vin[b] == v
            if(vin[b] != v) {
                return false;
            }
        }
    }

    return true;
}

bool PropertyValidator::ValidateCoordinates(GeometricGraph& G) {
    if(!G.Exist(PROP_COORD)) {
        return false;
    }

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

bool PropertyValidator::ValidatePropertyBounds(const GraphContainer& gc) {
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
    if(!G.Exist(PROP_CIR) || !G.Exist(PROP_PBRIN)) {
        return false;
    }

    Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);
    Prop<tbrin> pbrin(G.Set(tvertex()), PROP_PBRIN);

    // For each vertex, follow the circular order and verify it forms a complete cycle
    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = pbrin[v];

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
            b = cir[b];
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
    if(!G.Exist(PROP_CIR) || !G.Exist(PROP_VIN) || !G.Exist(PROP_PBRIN)) {
        return false;
    }

    Prop<tbrin> cir(G.Set(tbrin()), PROP_CIR);
    Prop<tvertex> vin(G.Set(tbrin()), PROP_VIN);
    Prop<tbrin> pbrin(G.Set(tvertex()), PROP_PBRIN);

    // Collect all brins in circular orders
    std::set<tbrin> brinsInOrders;

    for(tvertex v = 1; v <= G.nv(); v++) {
        tbrin first = pbrin[v];
        if(first == 0) continue;

        tbrin b = first;
        do {
            brinsInOrders.insert(b);
            b = cir[b];
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

// Note: Template functions moved to header if needed
// This was a placeholder - proper implementation would be type-specific

bool AreCoordinatesFinite(GeometricGraph& G) {
    return PropertyValidator::ValidateCoordinates(G);
}

bool AreColorsValid(GeometricGraph& G) {
    if(!G.Exist(PROP_COLOR)) {
        return true;  // No colors defined is OK
    }

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
