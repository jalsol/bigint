#pragma once

#include "bigint.hpp"
#include "token.hpp"

#include <cctype>
#include <cstddef>
#include <vector>
#include <stdexcept>
#include <string_view>

constexpr Bigint evaluate(std::string_view expr);

namespace /* private */ {

constexpr std::vector<Token> parse_into_tokens(std::string_view expr) {
    std::vector<Token> tokens;

    for (std::size_t i = 0; i < expr.size(); ++i) {
        if (std::isblank(expr[i])) {
            continue;
        }

        auto parse_number = [&] {
            const auto head = i;

            while (std::isdigit(expr[i])) {
                ++i;
            }

            tokens.push_back({
                .type = Token::Type::Number,
                .str = expr.substr(head, i - head),
            });
            --i;
        };

        auto parse_others = [&] {
            char c = expr[i];
            Token token { .str = expr.substr(i, 1) };

            switch (c) {
                case '(':
                    token.type = Token::Type::LeftParen;
                    break;
                case ')':
                    token.type = Token::Type::RightParen;
                    break;
                case '^':
                    token.type = Token::Type::Operator;
                    token.precedence = 4;
                    token.right_assoc = true;
                    break;
                case '*':
                case '/':
                    token.type = Token::Type::Operator;
                    token.precedence = 3;
                    break;
                case '+':
                    token.type = Token::Type::Operator;
                    token.precedence = 2;
                    break;
                case '-': {
                    const auto no_after = tokens.empty();
                    const auto after_operator = (tokens.back().type == Token::Type::Operator);
                    const auto after_leftparen = (tokens.back().type == Token::Type::LeftParen);

                    if (no_after || after_operator || after_leftparen) {
                        // is unary (is a sign)
                        token.unary = true;
                        token.type = Token::Type::Operator;
                        token.precedence = 5;
                        token.str = "m";
                    } else {
                        // is binary
                        token.type = Token::Type::Operator;
                        token.precedence = 2;
                    }
                } break;
                default:
                    break;
            }

            tokens.push_back(token);
        };

        if (std::isdigit(expr[i])) {
            parse_number();
        } else {
            parse_others();
        }
    }

    return tokens;
}

constexpr std::vector<Token> infix_to_postfix(const std::vector<Token>& tokens) {
    std::vector<Token> queue;
    std::vector<Token> stack;

    for (const auto& token : tokens) {
        switch (token.type) {
            case Token::Type::Number:
                queue.push_back(token);
                break;

            case Token::Type::Operator: {
                const auto& op1 = token;

                while (!stack.empty()) {
                    const auto& op2 = stack.back();

                    const auto not_leftparen = (op2.type != Token::Type::LeftParen);
                    const auto cond1 = (op2.precedence > op1.precedence);
                    const auto cond2 = (op2.precedence == op1.precedence && !op1.right_assoc);

                    if (not_leftparen && (cond1 || cond2)) {
                        queue.push_back(op2);
                        stack.pop_back();
                        continue;
                    }

                    break;
                }

                stack.push_back(op1);
            } break;

            case Token::Type::LeftParen:
                stack.push_back(token);
                break;

            case Token::Type::RightParen: {
                bool match = false;

                while (!stack.empty() && stack.back().type != Token::Type::LeftParen) {
                    queue.push_back(stack.back());
                    stack.pop_back();
                    match = true;
                }

                if (!match && stack.empty()) {
                    throw std::runtime_error("Mismatched parentheses");
                }
                stack.pop_back();
            } break;

            default:
                throw std::runtime_error("Token unknown: " + std::string(token.str));
        }
    }

    while (!stack.empty()) {
        if (stack.back().type == Token::Type::LeftParen) {
            throw std::runtime_error("Mismatched parentheses");
            return {};
        }

        queue.push_back(stack.back());
        stack.pop_back();
    }

    return queue;
}

} // namespace private

constexpr Bigint evaluate(std::string_view expr) {
    const auto tokens = parse_into_tokens(expr);
    const auto queue = infix_to_postfix(tokens);
    std::vector<Bigint> stack;

    std::size_t i = 0;
    while (i < queue.size()) {
        // at this stage, there are only numbers and operators
        const auto& token = queue[i++];

        if (token.type == Token::Type::Number) {
            stack.emplace_back(token.str);
            continue;
        }

        if (token.type != Token::Type::Operator) {
            throw std::runtime_error("Token unknown: " + std::string(token.str));
        }

        if (token.unary) {
            const auto rhs = stack.back();
            stack.pop_back();

            if (token.str[0] == 'm') {
                stack.push_back(-rhs);
            } else {
                throw std::runtime_error("Not a unary operator: " + std::string(token.str));
            }
            continue;
        }

        // binary operators
        const auto rhs = stack.back();
        stack.pop_back();
        const auto lhs = stack.back();
        stack.pop_back();

        switch (token.str[0]) {
            case '^':
                throw std::runtime_error("Power is currently not supported");
                break;
            case '*':
                stack.push_back(lhs * rhs);
                break;
            case '/':
                throw std::runtime_error("Division is currently not supported");
                break;
            case '+':
                stack.push_back(lhs + rhs);
                break;
            case '-':
                stack.push_back(lhs - rhs);
                break;
            default:
                throw std::runtime_error("Not a binary operator: " + std::string(token.str));
                break;
        }
    }

    return stack.back();
}

