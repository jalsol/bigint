#include "bigint.hpp"

#include <iostream>

int main() {
    std::string a;
    std::string b;

    std::cin >> a >> b;
    std::cout << Bigint(a) + Bigint(b) << '\n';
    std::cout << Bigint(a) - Bigint(b) << '\n';
    std::cout << Bigint(a) * Bigint(b) << '\n';
}
