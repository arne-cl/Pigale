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
    static bool ValidateCirAcir(const TopologicalGraph& G);
    static bool ValidateVinPbrin(const TopologicalGraph& G);
    static bool ValidateCoordinates(const GeometricGraph& G);

    // Check property bounds (values within expected ranges)
    static bool ValidatePropertyBounds(const GraphContainer& gc);
};

/**
 * @brief Circular order validation
 */

// Check that cir[acir[b]] == b for all brins
bool CheckCircularInvariant(const TopologicalGraph& G);

// Check that each vertex's circular order forms a complete cycle
bool CheckCircularCompleteness(const TopologicalGraph& G);

// Check that all incident brins are in the circular order
bool CheckAllBrinsInCircularOrder(const TopologicalGraph& G);

/**
 * @brief Property value checks
 */

// Check that property values are initialized (not default/garbage)
template<typename T>
bool IsPropertyInitialized(const Prop<T>& prop, int index);

// Check that coordinates are finite (not NaN or infinity)
bool AreCoordinatesFinite(const GeometricGraph& G);

// Check that color values are valid
bool AreColorsValid(const GeometricGraph& G);

} // namespace TestHelpers

#endif // TEST_PROPERTY_VALIDATORS_H
