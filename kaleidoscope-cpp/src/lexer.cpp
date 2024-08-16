#include "token.hpp"
#include "types.h"
#include <string>
#include <vector>

std::vector<Token> lex(const std::string& input) {
    usize idx = 0;
    std::vector<Token> tokens = {};

    while (idx < input.length()) {
        // Skip whitespace
        while (input[idx] && isspace(input[idx])) {
            idx++;
        }
        if (idx >= input.length()) {
            break;
        }
        if (isalpha(input[idx])) {
            // Lex keyword or identifier
            usize start = idx;
            std::string idStr = "";
            while (isalnum(input[idx])) {
                idStr += input[idx];
                idx++;
            }
            if (idStr == "def") {
                tokens.emplace_back(Token::Tag::Def, start);
            } else if (idStr == "extern") {
                tokens.emplace_back(Token::Tag::Extern, start);
            } else {
                tokens.emplace_back(Token::Tag::Id, start);
            }
        } else if (isdigit(input[idx]) || input[idx] == '.') {
            // Lex number
            usize start = idx;
            while (isdigit(input[idx]) || input[idx] == '.') {
                idx++;
            }
            tokens.emplace_back(Token::Tag::Num, start);
        } else if (input[idx] == '#') {
            // Skip comment
            while (input[idx] && input[idx] != '\n' && input[idx] != '\r') {
                idx++;
            }
        } else {
            switch (input[idx]) {
            case '(':
                tokens.emplace_back(Token::Tag::LParen, idx);
                break;
            case ')':
                tokens.emplace_back(Token::Tag::RParen, idx);
                break;
            case ';':
                tokens.emplace_back(Token::Tag::Semicolon, idx);
                break;
            case '+':
                tokens.emplace_back(Token::Tag::Plus, idx);
                break;
            case '-':
                tokens.emplace_back(Token::Tag::Minus, idx);
                break;
            case '*':
                tokens.emplace_back(Token::Tag::Star, idx);
                break;
            default:
                tokens.emplace_back(Token::Tag::Other, idx);
            }
            idx++;
        }
    }

    tokens.emplace_back(Token::Tag::Eof, idx);
    return tokens;
};
