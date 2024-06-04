#include "parser.hpp"

#include <iostream>
#include <string>

int main() {
    std::string expression;

    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, expression);

        try {
            std::cout << evaluate(expression) << std::endl;
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
