#include <Pigale.h>
#include <iostream>

extern GraphContainer* GenerateGrid(int rows, int cols);

int main() {
    std::cout << "=== Testing PROP_LABEL Patterns ===" << std::endl << std::endl;

    // Test 1: GeometricGraph (uses default value 0)
    std::cout << "--- GeometricGraph initialization ---" << std::endl;
    GraphContainer* gc1 = new GraphContainer();
    gc1->setsize(4, 0);

    GeometricGraph G1(*gc1);
    G1.NewEdge(tvertex(1), tvertex(2));
    G1.NewEdge(tvertex(2), tvertex(3));
    G1.NewEdge(tvertex(3), tvertex(4));
    G1.NewEdge(tvertex(4), tvertex(1));

    std::cout << "nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;
    std::cout << "GeometricGraph vlabel values:" << std::endl;
    for (int i = 0; i <= G1.nv(); i++) {
        std::cout << "  vlabel[" << i << "]=" << G1.vlabel[i] << std::endl;
    }

    std::cout << "\nGeometricGraph elabel values:" << std::endl;
    for (int i = 0; i <= G1.ne(); i++) {
        std::cout << "  elabel[" << i << "]=" << G1.elabel[i] << std::endl;
    }

    delete gc1;
    std::cout << std::endl;

    // Test 2: TopologicalGraph then convert to GeometricGraph
    std::cout << "--- TopologicalGraph -> GeometricGraph conversion ---" << std::endl;
    GraphContainer* gc2 = new GraphContainer();
    gc2->setsize(3, 0);

    TopologicalGraph G2(*gc2);
    G2.NewEdge(tvertex(1), tvertex(2));
    G2.NewEdge(tvertex(2), tvertex(3));
    G2.NewEdge(tvertex(3), tvertex(1));

    std::cout << "As TopologicalGraph: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    if (G2.Set(tvertex()).exist(PROP_LABEL)) {
        std::cout << "PROP_LABEL exists" << std::endl;
    } else {
        std::cout << "PROP_LABEL does NOT exist" << std::endl;
    }

    // Now create GeometricGraph from same container
    GeometricGraph G2geo(*gc2);
    std::cout << "\nAs GeometricGraph: nv=" << G2geo.nv() << ", ne=" << G2geo.ne() << std::endl;
    std::cout << "vlabel values after GeometricGraph creation:" << std::endl;
    for (int i = 0; i <= G2geo.nv(); i++) {
        std::cout << "  vlabel[" << i << "]=" << G2geo.vlabel[i] << std::endl;
    }

    delete gc2;

    return 0;
}
