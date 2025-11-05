/****************************************************************************
**
** Test helpers for validating property system
**
****************************************************************************/

#ifndef TEST_PROPERTY_VALIDATORS_H
#define TEST_PROPERTY_VALIDATORS_H

#include <TAXI/graphs.h>
#include <TAXI/Tprop.h>

namespace TestHelpers {

/**
 * @brief Property system validation
 */

class PropertyValidator {
public:
    // Check property consistency (allocated, in range)
    static bool ValidateProperty(const PSet& pset, int propId);

    // Check all properties in a graph container
    static bool ValidateAllProperties(const GraphContainer& gc);

    // Property-specific validators
    static bool ValidateCirAcir(TopologicalGraph& G);
    static bool ValidateVinPbrin(TopologicalGraph& G);
    static bool ValidateCoordinates(GeometricGraph& G);

    // Check property bounds (values within expected ranges)
    static bool ValidatePropertyBounds(const GraphContainer& gc);
};

/**
 * @brief Circular order validation
 */

// Check that cir[acir[b]] == b for all brins
bool CheckCircularInvariant(TopologicalGraph& G);

// Check that each vertex's circular order forms a complete cycle
bool CheckCircularCompleteness(TopologicalGraph& G);

// Check that all incident brins are in the circular order
bool CheckAllBrinsInCircularOrder(TopologicalGraph& G);

/**
 * @brief Property value checks
 */

// Note: IsPropertyInitialized removed - implementation was type-specific
// and better handled by specific validators like AreCoordinatesFinite, etc.

// Check that coordinates are finite (not NaN or infinity)
bool AreCoordinatesFinite(GeometricGraph& G);

// Check that color values are valid (non-negative)
bool AreColorsValid(GeometricGraph& G);

} // namespace TestHelpers

#endif // TEST_PROPERTY_VALIDATORS_H
