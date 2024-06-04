#include "bigint.hpp"
#include <iostream>

int main() {
    std::cin.tie(nullptr)->sync_with_stdio(false);
    int n; std::cin >> n;
    std::string a, b;

    while (n--) {
        std::cin >> a >> b;
        std::cout << Bigint(a) + Bigint(b) << '\n';
    }
}
