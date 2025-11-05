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
bool IsIsomorphic(Graph& G1, Graph& G2);

// Check if two topological graphs have the same embedding
bool HasSameEmbedding(TopologicalGraph& G1, TopologicalGraph& G2);

// Check if vertex and edge counts match
bool HasSameCounts(Graph& G1, Graph& G2);

/**
 * @brief Property validators
 */

// Validate circular order properties
bool ValidateCircularOrder(TopologicalGraph& G);

// Validate planar map properties
bool ValidatePlanarMap(TopologicalGraph& G);

// Validate geometric coordinates (no NaN, no infinity)
bool ValidateCoordinates(GeometricGraph& G);

// Check if drawing has edge crossings
bool CheckNoEdgeCrossings(GeometricGraph& G);

/**
 * @brief Graph invariants
 */

// Verify basic graph invariants (counts, properties)
bool VerifyGraphInvariants(Graph& G);

// Verify vertex-edge counts match property arrays
bool VerifyVertexEdgeCounts(Graph& G);

// Verify brin incidence properties (vin[b] correct)
bool VerifyBrinIncidence(TopologicalGraph& G);

// Verify degree sequence matches expected
bool VerifyDegreeSequence(Graph& G, const std::vector<int>& expectedDegrees);

// Verify pbrin consistency (pbrin[v] exists and is correct)
bool VerifyPbrinConsistency(TopologicalGraph& G);

/**
 * @brief Planarity checks
 */

// Verify Euler's formula for planar graphs: v - e + f = 2
bool VerifyEulerFormula(TopologicalGraph& G);

// Check that edge count satisfies planar bound: m <= 3n - 6
bool CheckPlanarBound(Graph& G);

/**
 * @brief Embedding validation
 */

// Check that all faces are consistently oriented
bool ValidateFaceOrientation(TopologicalGraph& G);

// Verify exterior face is properly marked
bool ValidateExteriorFace(TopologicalGraph& G);

/**
 * @brief Detailed diagnostics
 */

// Print detailed graph information for debugging
void PrintGraphInfo(Graph& G);

// Print circular order information
void PrintCircularOrder(TopologicalGraph& G, tvertex v);

// Print all faces
void PrintFaces(TopologicalGraph& G);

} // namespace TestHelpers

#endif // TEST_GRAPH_COMPARATORS_H
