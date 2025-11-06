#include <Pigale.h>
#include <iostream>

extern GraphContainer* GenerateGrid(int rows, int cols);
extern GraphContainer* GenerateCompleteGraph(int n);

int main() {
    std::cout << "=== Testing MaxPlanar() on different graphs ===" << std::endl << std::endl;

    // Test 1: Grid (already planar)
    std::cout << "--- Test 1: Grid 3x3 (planar) ---" << std::endl;
    GraphContainer* gc1 = GenerateGrid(3, 3);
    TopologicalGraph G1(*gc1);
    std::cout << "Created grid: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    std::cout << "Calling MaxPlanar()..." << std::endl;

    int result1 = G1.MaxPlanar();

    std::cout << "MaxPlanar() returned: " << result1 << std::endl;
    std::cout << "After: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    delete gc1;
    std::cout << "SUCCESS!" << std::endl << std::endl;

    // Test 2: K4 (planar)
    std::cout << "--- Test 2: K4 (planar) ---" << std::endl;
    GraphContainer* gc2 = GenerateCompleteGraph(4);
    TopologicalGraph G2(*gc2);
    std::cout << "Created K4: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;
    std::cout << "Calling MaxPlanar()..." << std::endl;

    int result2 = G2.MaxPlanar();

    std::cout << "MaxPlanar() returned: " << result2 << std::endl;
    std::cout << "After: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;
    delete gc2;
    std::cout << "SUCCESS!" << std::endl << std::endl;

    // Test 3: K5 (non-planar)
    std::cout << "--- Test 3: K5 (non-planar) ---" << std::endl;
    GraphContainer* gc3 = GenerateCompleteGraph(5);
    TopologicalGraph G3(*gc3);
    std::cout << "Created K5: nv=" << G3.nv() << ", ne=" << G3.ne() << std::endl;
    std::cout << "Calling MaxPlanar()..." << std::endl;

    int result3 = G3.MaxPlanar();

    std::cout << "MaxPlanar() returned: " << result3 << std::endl;
    std::cout << "After: nv=" << G3.nv() << ", ne=" << G3.ne() << std::endl;
    delete gc3;
    std::cout << "SUCCESS!" << std::endl;

    return 0;
}
