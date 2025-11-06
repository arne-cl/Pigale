#include <Pigale.h>
#include <iostream>

extern GraphContainer* GenerateGrid(int rows, int cols);
extern GraphContainer* GenerateCompleteGraph(int n);

int main() {
    std::cout << "=== Testing what Kuratowski() returns on different graphs ===" << std::endl << std::endl;

    // Test 1: Grid (planar)
    std::cout << "--- Test 1: Grid 3x3 (planar) ---" << std::endl;
    GraphContainer* gc1 = GenerateGrid(3, 3);
    TopologicalGraph G1(*gc1);
    std::cout << "Created: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    std::cout << "TestPlanar: " << G1.TestPlanar() << " (should be > 0 for planar)" << std::endl;

    // Test Kuratowski on fresh graph
    delete gc1;
    gc1 = GenerateGrid(3, 3);
    TopologicalGraph G1b(*gc1);
    std::cout << "Calling Kuratowski()..." << std::endl;
    int result1 = G1b.Kuratowski();
    std::cout << "Kuratowski() returned: " << result1 << std::endl;
    delete gc1;
    std::cout << std::endl;

    // Test 2: K4 (planar)
    std::cout << "--- Test 2: K4 (planar) ---" << std::endl;
    GraphContainer* gc2 = GenerateCompleteGraph(4);
    TopologicalGraph G2(*gc2);
    std::cout << "Created: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;
    std::cout << "TestPlanar: " << G2.TestPlanar() << " (should be > 0 for planar)" << std::endl;

    delete gc2;
    gc2 = GenerateCompleteGraph(4);
    TopologicalGraph G2b(*gc2);
    std::cout << "Calling Kuratowski()..." << std::endl;
    int result2 = G2b.Kuratowski();
    std::cout << "Kuratowski() returned: " << result2 << std::endl;
    delete gc2;
    std::cout << std::endl;

    // Test 3: K5 (non-planar)
    std::cout << "--- Test 3: K5 (non-planar) ---" << std::endl;
    GraphContainer* gc3 = GenerateCompleteGraph(5);
    TopologicalGraph G3(*gc3);
    std::cout << "Created: nv=" << G3.nv() << ", ne=" << G3.ne() << std::endl;
    std::cout << "TestPlanar: " << G3.TestPlanar() << " (should be 0 for non-planar)" << std::endl;

    delete gc3;
    gc3 = GenerateCompleteGraph(5);
    TopologicalGraph G3b(*gc3);
    std::cout << "Calling Kuratowski()..." << std::endl;
    int result3 = G3b.Kuratowski();
    std::cout << "Kuratowski() returned: " << result3 << std::endl;
    delete gc3;
    std::cout << std::endl;

    std::cout << "=== Analysis ===" << std::endl;
    std::cout << "Kuratowski() should only work on non-planar graphs." << std::endl;
    std::cout << "MaxPlanar() calls Kuratowski() without checking return value," << std::endl;
    std::cout << "causing crashes when it fails." << std::endl;

    return 0;
}
