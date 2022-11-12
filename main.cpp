#include <iostream>
#include "equation_system.hpp"

int main() {
    equation e1;
    e1.variable_terms["x"] = 1;
    e1.variable_terms["y"] = 2;
    e1.variable_terms["z"] = 4;
    e1.constant_term = 3;
    std::cout << e1.to_string() << '\n';

    equation e2;
    e2.variable_terms["x"] = 3;
    e2.variable_terms["y"] = -1;
    e2.variable_terms["z"] = 3;
    e2.constant_term = 4;
    std::cout << e2.to_string() << '\n';

    equation e3;
    e3.variable_terms["x"] = 5;
    e3.variable_terms["y"] = 2;
    e3.variable_terms["z"] = 4;
    e3.constant_term = 18;
    std::cout << e3.to_string() << '\n';

    std::cout << '\n';

    equation_system eqs{e1, e2, e3};
    for (auto [key, value]: eqs.solve())
        std::cout << key << "=" << value.to_string() << '\n';
}
