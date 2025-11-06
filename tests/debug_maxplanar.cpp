#include <Pigale.h>
#include <iostream>
#include "helpers/graph_builders.h"

extern GraphContainer* GenerateCompleteGraph(int n);

int main() {
    std::cout << "=== Testing MaxPlanar() ===" << std::endl << std::endl;

    // Test 1: Library-generated K5
    std::cout << "--- Test 1: Library GenerateCompleteGraph(5) ---" << std::endl;
    GraphContainer* gc1 = GenerateCompleteGraph(5);
    TopologicalGraph G1(*gc1);
    std::cout << "Created K5: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    std::cout << "Calling MaxPlanar()..." << std::endl;

    int result1 = G1.MaxPlanar();

    std::cout << "MaxPlanar() returned: " << result1 << std::endl;
    std::cout << "After: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    delete gc1;
    std::cout << "SUCCESS: Library-generated graph works!" << std::endl << std::endl;

    // Test 2: NewEdge-created K5
    std::cout << "--- Test 2: NewEdge-created K5 (BuildK5) ---" << std::endl;
    GraphContainer* gc2 = TestHelpers::BuildK5();
    TopologicalGraph G2(*gc2);
    std::cout << "Created K5: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;
    std::cout << "Calling MaxPlanar()..." << std::endl;

    int result2 = G2.MaxPlanar();

    std::cout << "MaxPlanar() returned: " << result2 << std::endl;
    std::cout << "After: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;
    delete gc2;
    std::cout << "SUCCESS: NewEdge-created graph works!" << std::endl;

    return 0;
}
