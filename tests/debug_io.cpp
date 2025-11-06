#include <Pigale.h>
#include <TAXI/Tfile.h>
#include <iostream>
#include "helpers/graph_builders.h"

extern GraphContainer* GenerateGrid(int rows, int cols);

int main() {
    std::cout << "=== Testing TGF I/O ===" << std::endl << std::endl;

    // Test 1: Library-generated Grid
    std::cout << "--- Test 1: Library GenerateGrid(2,2) ---" << std::endl;
    GraphContainer* gc1 = GenerateGrid(2, 2);
    TopologicalGraph G1(*gc1);
    std::cout << "Created: nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;

    SaveGraphTgf(G1, "/tmp/test_grid.tgf", 0);
    std::cout << "Saved to /tmp/test_grid.tgf" << std::endl;

    GraphContainer gc1_loaded;
    int numRecords, graphIndex;
    ReadTgfGraph(gc1_loaded, "/tmp/test_grid.tgf", numRecords, graphIndex);
    TopologicalGraph G1_loaded(gc1_loaded);
    std::cout << "Loaded: nv=" << G1_loaded.nv() << ", ne=" << G1_loaded.ne() << std::endl;

    if (G1.ne() == G1_loaded.ne()) {
        std::cout << "SUCCESS: Edge count preserved!" << std::endl;
    } else {
        std::cout << "BUG: Lost " << (G1.ne() - G1_loaded.ne()) << " edge(s)!" << std::endl;
    }
    delete gc1;
    std::cout << std::endl;

    // Test 2: NewEdge-created cycle (4 vertices, 4 edges)
    std::cout << "--- Test 2: NewEdge cycle (4 vertices, 4 edges) ---" << std::endl;
    GraphContainer* gc2 = new GraphContainer();
    gc2->setsize(4, 0);
    TopologicalGraph G2(*gc2);
    G2.NewEdge(tvertex(1), tvertex(2));
    G2.NewEdge(tvertex(2), tvertex(3));
    G2.NewEdge(tvertex(3), tvertex(4));
    G2.NewEdge(tvertex(4), tvertex(1));
    std::cout << "Created: nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    SaveGraphTgf(G2, "/tmp/test_cycle.tgf", 0);
    std::cout << "Saved to /tmp/test_cycle.tgf" << std::endl;

    GraphContainer gc2_loaded;
    ReadTgfGraph(gc2_loaded, "/tmp/test_cycle.tgf", numRecords, graphIndex);
    TopologicalGraph G2_loaded(gc2_loaded);
    std::cout << "Loaded: nv=" << G2_loaded.nv() << ", ne=" << G2_loaded.ne() << std::endl;

    if (G2.ne() == G2_loaded.ne()) {
        std::cout << "SUCCESS: Edge count preserved!" << std::endl;
    } else {
        std::cout << "BUG: Lost " << (G2.ne() - G2_loaded.ne()) << " edge(s)!" << std::endl;
    }
    delete gc2;
    std::cout << std::endl;

    // Test 3: NewEdge-created triangle (3 vertices, 3 edges)
    std::cout << "--- Test 3: NewEdge triangle (3 vertices, 3 edges) ---" << std::endl;
    GraphContainer* gc3 = new GraphContainer();
    gc3->setsize(3, 0);
    TopologicalGraph G3(*gc3);
    G3.NewEdge(tvertex(1), tvertex(2));
    G3.NewEdge(tvertex(2), tvertex(3));
    G3.NewEdge(tvertex(3), tvertex(1));
    std::cout << "Created: nv=" << G3.nv() << ", ne=" << G3.ne() << std::endl;

    SaveGraphTgf(G3, "/tmp/test_triangle.tgf", 0);
    std::cout << "Saved to /tmp/test_triangle.tgf" << std::endl;

    GraphContainer gc3_loaded;
    ReadTgfGraph(gc3_loaded, "/tmp/test_triangle.tgf", numRecords, graphIndex);
    TopologicalGraph G3_loaded(gc3_loaded);
    std::cout << "Loaded: nv=" << G3_loaded.nv() << ", ne=" << G3_loaded.ne() << std::endl;

    if (G3.ne() == G3_loaded.ne()) {
        std::cout << "SUCCESS: Edge count preserved!" << std::endl;
    } else {
        std::cout << "BUG: Lost " << (G3.ne() - G3_loaded.ne()) << " edge(s)!" << std::endl;
    }
    delete gc3;

    return 0;
}
