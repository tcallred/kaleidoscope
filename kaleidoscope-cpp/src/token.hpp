#pragma once
#include "types.h"
#include <sstream>
#include <string>

struct Token {
    enum class Tag {
        Eof, Def, Extern, Id, Num, LParen, RParen, Semicolon, Plus, Minus, Star, Other
    };
    Tag tag;
    usize start;

    std::string str() {
        static const std::string tagStrs[] = {"eof", "def", "extern", "id", "num", "lparen", "rparen", ";", "+", "-", "*", "other"};
        std::ostringstream out;
        out << "{" << tagStrs[usize(tag)] << "," << start << "}";
        return out.str();
    }
};
