#pragma once

#include <string_view>

struct Token {
    enum class Type {
        Unknown,
        Number,
        Operator,
        LeftParen,
        RightParen,
    };

    Type type = Type::Unknown;
    std::string_view str;
    int precedence = -1;
    bool right_assoc = false;
    bool unary = false;
};
