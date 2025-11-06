#include <Pigale.h>
#include <TAXI/DFSGraph.h>
#include <iostream>

extern GraphContainer* GenerateCompleteGraph(int n);

// Recreate the buggy logic to demonstrate it
bool buggy_condition(int nv, int ne) {
    // This is the BUGGY condition from line 458 of DFSGraph.cpp
    return (nv == 6) && ((ne == 9) || ((nv == 5) && (ne == 10)));
}

bool fixed_condition(int nv, int ne) {
    // This is what it SHOULD be
    return (nv == 6 && ne == 9) || (nv == 5 && ne == 10);
}

int main() {
    std::cout << "=== Demonstrating the Bug in MarkKuratowski line 458 ===" << std::endl << std::endl;

    // Test K5
    std::cout << "K5 after CotreeCritical: nv=5, ne=10" << std::endl;
    std::cout << "  Buggy condition evaluates to: " << buggy_condition(5, 10) << " (should be 1/true)" << std::endl;
    std::cout << "  Fixed condition evaluates to: " << fixed_condition(5, 10) << std::endl;
    std::cout << std::endl;

    // Test K33
    std::cout << "K33 after CotreeCritical: nv=6, ne=9" << std::endl;
    std::cout << "  Buggy condition evaluates to: " << buggy_condition(6, 9) << " (correct)" << std::endl;
    std::cout << "  Fixed condition evaluates to: " << fixed_condition(6, 9) << std::endl;
    std::cout << std::endl;

    std::cout << "=== Analysis ===" << std::endl;
    std::cout << "The buggy condition is:" << std::endl;
    std::cout << "  if ((nv() == 6) && ((ne() == 9) || ((nv() == 5) && (ne() == 10))))" << std::endl;
    std::cout << std::endl;
    std::cout << "For K5 (nv=5, ne=10):" << std::endl;
    std::cout << "  nv() == 6 evaluates to FALSE" << std::endl;
    std::cout << "  Therefore the entire condition is FALSE (short-circuit AND)" << std::endl;
    std::cout << "  So K5 doesn't take the fast path at line 458-461" << std::endl;
    std::cout << "  It falls through to the general algorithm which fails" << std::endl;
    std::cout << std::endl;
    std::cout << "The condition SHOULD be:" << std::endl;
    std::cout << "  if ((nv() == 6 && ne() == 9) || (nv() == 5 && ne() == 10))" << std::endl;
    std::cout << std::endl;

    std::cout << "This is a REAL BUG in the library code!" << std::endl;

    return 0;
}
