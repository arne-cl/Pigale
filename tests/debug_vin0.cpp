#include <Pigale.h>
#include <TAXI/Tfile.h>
#include <iostream>

extern GraphContainer* GenerateGrid(int rows, int cols);

int main() {
    std::cout << "=== Investigating vin[0] and edge indexing ===" << std::endl << std::endl;

    // Test 1: Library-generated grid
    std::cout << "--- Library-generated Grid 2x2 ---" << std::endl;
    GraphContainer* gc1 = GenerateGrid(2, 2);
    TopologicalGraph G1(*gc1);

    std::cout << "nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;

    // Check vin array
    Prop<tvertex> vin1(G1.Set(tbrin()), PROP_VIN);
    std::cout << "vin[0] = " << vin1[0]() << std::endl;
    std::cout << "Edges (brin -> vertex):" << std::endl;
    for (int i = 1; i <= G1.ne(); i++) {
        std::cout << "  Edge " << i << ": vin[" << i << "]=" << vin1[i]()
                  << ", vin[" << -i << "]=" << vin1[-i]() << std::endl;
    }

    SaveGraphTgf(G1, "/tmp/test_grid_check.tgf", 0);
    std::cout << "Saved to /tmp/test_grid_check.tgf" << std::endl;

    GraphContainer gc1_loaded;
    int numRecords, graphIndex;
    ReadTgfGraph(gc1_loaded, "/tmp/test_grid_check.tgf", numRecords, graphIndex);
    TopologicalGraph G1_loaded(gc1_loaded);
    std::cout << "Loaded: nv=" << G1_loaded.nv() << ", ne=" << G1_loaded.ne() << std::endl;

    Prop<tvertex> vin1_loaded(G1_loaded.Set(tbrin()), PROP_VIN);
    std::cout << "After load, vin[0] = " << vin1_loaded[0]() << std::endl;

    delete gc1;
    std::cout << std::endl;

    // Test 2: NewEdge-created cycle
    std::cout << "--- NewEdge-created cycle (4 vertices, 4 edges) ---" << std::endl;
    GraphContainer* gc2 = new GraphContainer();
    gc2->setsize(4, 0);
    TopologicalGraph G2(*gc2);

    std::cout << "After setsize(4, 0):" << std::endl;
    std::cout << "nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    // Check if vin exists and what vin[0] is
    if (G2.Set(tbrin()).exist(PROP_VIN)) {
        Prop<tvertex> vin2_initial(G2.Set(tbrin()), PROP_VIN);
        std::cout << "vin[0] = " << vin2_initial[0]() << std::endl;
    } else {
        std::cout << "PROP_VIN doesn't exist yet" << std::endl;
    }

    G2.NewEdge(tvertex(1), tvertex(2));
    G2.NewEdge(tvertex(2), tvertex(3));
    G2.NewEdge(tvertex(3), tvertex(4));
    G2.NewEdge(tvertex(4), tvertex(1));

    std::cout << "\nAfter NewEdge() calls:" << std::endl;
    std::cout << "nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    Prop<tvertex> vin2(G2.Set(tbrin()), PROP_VIN);
    std::cout << "vin[0] = " << vin2[0]() << std::endl;
    std::cout << "Edges (brin -> vertex):" << std::endl;
    for (int i = 1; i <= G2.ne(); i++) {
        std::cout << "  Edge " << i << ": vin[" << i << "]=" << vin2[i]()
                  << ", vin[" << -i << "]=" << vin2[-i]() << std::endl;
    }

    SaveGraphTgf(G2, "/tmp/test_cycle_check.tgf", 0);
    std::cout << "\nSaved to /tmp/test_cycle_check.tgf" << std::endl;

    GraphContainer gc2_loaded;
    ReadTgfGraph(gc2_loaded, "/tmp/test_cycle_check.tgf", numRecords, graphIndex);
    TopologicalGraph G2_loaded(gc2_loaded);
    std::cout << "Loaded: nv=" << G2_loaded.nv() << ", ne=" << G2_loaded.ne() << std::endl;

    Prop<tvertex> vin2_loaded(G2_loaded.Set(tbrin()), PROP_VIN);
    std::cout << "After load, vin[0] = " << vin2_loaded[0]() << std::endl;
    std::cout << "Loaded edges (brin -> vertex):" << std::endl;
    for (int i = 1; i <= G2_loaded.ne(); i++) {
        std::cout << "  Edge " << i << ": vin[" << i << "]=" << vin2_loaded[i]()
                  << ", vin[" << -i << "]=" << vin2_loaded[-i]() << std::endl;
    }

    delete gc2;

    return 0;
}
