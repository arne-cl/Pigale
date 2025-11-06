#include <Pigale.h>
#include <TAXI/DFSGraph.h>
#include <iostream>
#include "helpers/graph_builders.h"

extern GraphContainer* GenerateCompleteGraph(int n);

int main() {
    std::cout << "=== Testing Kuratowski() logic ===" << std::endl << std::endl;

    // Test K5
    std::cout << "--- Testing K5 ---" << std::endl;
    GraphContainer* gc_k5 = GenerateCompleteGraph(5);
    TopologicalGraph G_k5(*gc_k5);
    std::cout << "K5: nv=" << G_k5.nv() << ", ne=" << G_k5.ne() << std::endl;

    G_k5.Simplify();
    std::cout << "After Simplify: nv=" << G_k5.nv() << ", ne=" << G_k5.ne() << std::endl;

    G_k5.MakeConnected();
    std::cout << "After MakeConnected: nv=" << G_k5.nv() << ", ne=" << G_k5.ne() << std::endl;

    GraphContainer DFSContainer_k5;
    DFSGraph DG_k5(DFSContainer_k5, G_k5);
    std::cout << "DFSGraph created: nv=" << DG_k5.nv() << ", ne=" << DG_k5.ne() << std::endl;

    int ret_k5 = DG_k5.CotreeCritical();
    std::cout << "CotreeCritical returned: " << ret_k5 << std::endl;
    std::cout << "After CotreeCritical: nv=" << DG_k5.nv() << ", ne=" << DG_k5.ne() << std::endl;

    if (ret_k5 != 0) {
        int kura_ret = DG_k5.Kuratowski();
        std::cout << "DG.Kuratowski() returned: " << kura_ret << std::endl;
    }

    delete gc_k5;

    std::cout << std::endl;

    // Test K33 for comparison
    std::cout << "--- Testing K_{3,3} ---" << std::endl;
    GraphContainer* gc_k33 = TestHelpers::BuildK33();
    TopologicalGraph G_k33(*gc_k33);
    std::cout << "K33: nv=" << G_k33.nv() << ", ne=" << G_k33.ne() << std::endl;

    G_k33.Simplify();
    std::cout << "After Simplify: nv=" << G_k33.nv() << ", ne=" << G_k33.ne() << std::endl;

    G_k33.MakeConnected();
    std::cout << "After MakeConnected: nv=" << G_k33.nv() << ", ne=" << G_k33.ne() << std::endl;

    GraphContainer DFSContainer_k33;
    DFSGraph DG_k33(DFSContainer_k33, G_k33);
    std::cout << "DFSGraph created: nv=" << DG_k33.nv() << ", ne=" << DG_k33.ne() << std::endl;

    int ret_k33 = DG_k33.CotreeCritical();
    std::cout << "CotreeCritical returned: " << ret_k33 << std::endl;
    std::cout << "After CotreeCritical: nv=" << DG_k33.nv() << ", ne=" << DG_k33.ne() << std::endl;

    if (ret_k33 != 0) {
        int kura_ret = DG_k33.Kuratowski();
        std::cout << "DG.Kuratowski() returned: " << kura_ret << std::endl;
    }

    delete gc_k33;

    return 0;
}
