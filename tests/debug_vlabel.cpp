#include <Pigale.h>
#include <iostream>

extern GraphContainer* GenerateGrid(int rows, int cols);

int main() {
    std::cout << "=== Investigating vlabel property ===" << std::endl << std::endl;

    // Test 1: Library-generated grid
    std::cout << "--- Library-generated Grid 2x2 ---" << std::endl;
    GraphContainer* gc1 = GenerateGrid(2, 2);
    TopologicalGraph G1(*gc1);

    std::cout << "nv=" << G1.nv() << ", ne=" << G1.ne() << std::endl;

    if (G1.Set(tvertex()).exist(PROP_LABEL)) {
        Prop<int> vlabel(G1.Set(tvertex()), PROP_LABEL);
        std::cout << "PROP_LABEL exists for vertices" << std::endl;
        std::cout << "Vertex labels:" << std::endl;
        for (int i = 1; i <= G1.nv(); i++) {
            std::cout << "  vertex " << i << ": label=" << vlabel[i] << std::endl;
        }
    } else {
        std::cout << "PROP_LABEL does NOT exist for vertices" << std::endl;
    }

    if (G1.Set(tedge()).exist(PROP_LABEL)) {
        Prop<int> elabel(G1.Set(tedge()), PROP_LABEL);
        std::cout << "PROP_LABEL exists for edges" << std::endl;
        std::cout << "Edge labels:" << std::endl;
        for (int i = 1; i <= G1.ne(); i++) {
            std::cout << "  edge " << i << ": label=" << elabel[i] << std::endl;
        }
    } else {
        std::cout << "PROP_LABEL does NOT exist for edges" << std::endl;
    }

    delete gc1;
    std::cout << std::endl;

    // Test 2: NewEdge-created cycle
    std::cout << "--- NewEdge-created cycle (4 vertices, 4 edges) ---" << std::endl;
    GraphContainer* gc2 = new GraphContainer();
    gc2->setsize(4, 0);
    TopologicalGraph G2(*gc2);

    G2.NewEdge(tvertex(1), tvertex(2));
    G2.NewEdge(tvertex(2), tvertex(3));
    G2.NewEdge(tvertex(3), tvertex(4));
    G2.NewEdge(tvertex(4), tvertex(1));

    std::cout << "nv=" << G2.nv() << ", ne=" << G2.ne() << std::endl;

    if (G2.Set(tvertex()).exist(PROP_LABEL)) {
        Prop<int> vlabel(G2.Set(tvertex()), PROP_LABEL);
        std::cout << "PROP_LABEL exists for vertices" << std::endl;
        std::cout << "Vertex labels:" << std::endl;
        for (int i = 1; i <= G2.nv(); i++) {
            std::cout << "  vertex " << i << ": label=" << vlabel[i] << std::endl;
        }
    } else {
        std::cout << "PROP_LABEL does NOT exist for vertices" << std::endl;
        // Try to create it and see default values
        Prop<int> vlabel(G2.Set(tvertex()), PROP_LABEL);
        std::cout << "Created PROP_LABEL. Default values:" << std::endl;
        for (int i = 0; i <= G2.nv(); i++) {
            std::cout << "  vertex " << i << ": label=" << vlabel[i] << std::endl;
        }
    }

    if (G2.Set(tedge()).exist(PROP_LABEL)) {
        Prop<int> elabel(G2.Set(tedge()), PROP_LABEL);
        std::cout << "PROP_LABEL exists for edges" << std::endl;
        std::cout << "Edge labels:" << std::endl;
        for (int i = 1; i <= G2.ne(); i++) {
            std::cout << "  edge " << i << ": label=" << elabel[i] << std::endl;
        }
    } else {
        std::cout << "PROP_LABEL does NOT exist for edges" << std::endl;
    }

    delete gc2;

    return 0;
}
