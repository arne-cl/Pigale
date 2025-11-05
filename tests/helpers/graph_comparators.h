/****************************************************************************
**
** Test helpers for validating and comparing graphs
**
****************************************************************************/

#ifndef TEST_GRAPH_COMPARATORS_H
#define TEST_GRAPH_COMPARATORS_H

#include <TAXI/graphs.h>
#include <vector>

namespace TestHelpers {

/**
 * @brief Structural comparisons
 */

// Check if two graphs are isomorphic (same structure)
bool IsIsomorphic(const Graph& G1, const Graph& G2);

// Check if two topological graphs have the same embedding
bool HasSameEmbedding(const TopologicalGraph& G1, const TopologicalGraph& G2);

// Check if vertex and edge counts match
bool HasSameCounts(const Graph& G1, const Graph& G2);

/**
 * @brief Property validators
 */

// Validate circular order properties
bool ValidateCircularOrder(const TopologicalGraph& G);

// Validate planar map properties
bool ValidatePlanarMap(const TopologicalGraph& G);

// Validate geometric coordinates (no NaN, no infinity)
bool ValidateCoordinates(const GeometricGraph& G);

// Check if drawing has edge crossings
bool CheckNoEdgeCrossings(const GeometricGraph& G);

/**
 * @brief Graph invariants
 */

// Verify basic graph invariants (counts, properties)
bool VerifyGraphInvariants(const Graph& G);

// Verify vertex-edge counts match property arrays
bool VerifyVertexEdgeCounts(const Graph& G);

// Verify brin incidence properties (vin[b] correct)
bool VerifyBrinIncidence(const TopologicalGraph& G);

// Verify degree sequence matches expected
bool VerifyDegreeSequence(const Graph& G, const std::vector<int>& expectedDegrees);

// Verify pbrin consistency (pbrin[v] exists and is correct)
bool VerifyPbrinConsistency(const TopologicalGraph& G);

/**
 * @brief Planarity checks
 */

// Verify Euler's formula for planar graphs: v - e + f = 2
bool VerifyEulerFormula(const TopologicalGraph& G);

// Check that edge count satisfies planar bound: m <= 3n - 6
bool CheckPlanarBound(const Graph& G);

/**
 * @brief Embedding validation
 */

// Check that all faces are consistently oriented
bool ValidateFaceOrientation(const TopologicalGraph& G);

// Verify exterior face is properly marked
bool ValidateExteriorFace(const TopologicalGraph& G);

/**
 * @brief Detailed diagnostics
 */

// Print detailed graph information for debugging
void PrintGraphInfo(const Graph& G);

// Print circular order information
void PrintCircularOrder(const TopologicalGraph& G, tvertex v);

// Print all faces
void PrintFaces(const TopologicalGraph& G);

} // namespace TestHelpers

#endif // TEST_GRAPH_COMPARATORS_H
