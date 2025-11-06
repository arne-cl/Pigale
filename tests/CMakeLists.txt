cmake_minimum_required(VERSION 3.14)
project(PigaleTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../incl)

# Find or build tgraph library
set(TGRAPH_LIB_DIR ${CMAKE_SOURCE_DIR}/../lib)
if(EXISTS ${TGRAPH_LIB_DIR}/libtgraph.a)
    add_library(tgraph STATIC IMPORTED)
    set_target_properties(tgraph PROPERTIES
        IMPORTED_LOCATION ${TGRAPH_LIB_DIR}/libtgraph.a
    )
elseif(EXISTS ${TGRAPH_LIB_DIR}/libtgraph_debug.a)
    add_library(tgraph STATIC IMPORTED)
    set_target_properties(tgraph PROPERTIES
        IMPORTED_LOCATION ${TGRAPH_LIB_DIR}/libtgraph_debug.a
    )
else()
    message(FATAL_ERROR "tgraph library not found. Please build it first using: cd ../tgraph && qmake && make")
endif()

# Test helpers library
add_library(test_helpers STATIC
    helpers/graph_builders.cpp
    helpers/graph_comparators.cpp
    helpers/property_validators.cpp
)
target_link_libraries(test_helpers tgraph)
target_include_directories(test_helpers PUBLIC helpers)

# Unit tests
set(UNIT_TEST_SOURCES
    unit/core/test_graph_container.cpp
    unit/core/test_graph.cpp
    unit/core/test_topological_graph.cpp
    unit/core/test_geometric_graph.cpp
    unit/data_structures/test_circular_order.cpp
    unit/data_structures/test_pset.cpp
    unit/algorithms/traversal/test_dfs_bfs.cpp
    unit/algorithms/planarity/test_planarity.cpp
    unit/algorithms/embedding/test_embedding.cpp
    # More test files will be added here
)

add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests
    GTest::gtest_main
    tgraph
    test_helpers
)

# Integration tests
set(INTEGRATION_TEST_SOURCES
    integration/test_planar_pipeline.cpp
    integration/test_property_persistence.cpp
    integration/test_graph_invariants.cpp
)

add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests
    GTest::gtest_main
    tgraph
    test_helpers
)

# Enable testing
enable_testing()
include(GoogleTest)

gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)

# Optional: Add custom test target for convenience
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS unit_tests integration_tests
)

# Optional: Code coverage support
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(unit_tests PRIVATE --coverage)
        target_link_options(unit_tests PRIVATE --coverage)
        target_compile_options(integration_tests PRIVATE --coverage)
        target_link_options(integration_tests PRIVATE --coverage)
    endif()
endif()
