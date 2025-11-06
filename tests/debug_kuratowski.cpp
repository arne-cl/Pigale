#include <Pigale.h>
#include <iostream>

extern GraphContainer* GenerateCompleteGraph(int n);

int main() {
    std::cout << "Testing Kuratowski() on K5" << std::endl;

    // Create K5
    GraphContainer* gc = GenerateCompleteGraph(5);
    TopologicalGraph G(*gc);

    std::cout << "K5: nv=" << G.nv() << ", ne=" << G.ne() << std::endl;

    // Test planarity first
    int planar = G.TestPlanar();
    std::cout << "TestPlanar result: " << planar << " (0=non-planar, expected)" << std::endl;

    // Create fresh K5 for Kuratowski (TestPlanar modifies graph state)
    delete gc;
    gc = GenerateCompleteGraph(5);
    TopologicalGraph G2(*gc);

    std::cout << "\nCalling Kuratowski() on fresh K5..." << std::endl;
    std::cout << "Before: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    int result = G2.Kuratowski();

    std::cout << "Kuratowski() result: " << result << std::endl;
    std::cout << "After: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    if (result == 0) {
        std::cout << "SUCCESS: Kuratowski extraction worked!" << std::endl;
    } else {
        std::cout << "FAILURE: Kuratowski returned error code " << result << std::endl;
    }

    delete gc;
    return 0;
}
