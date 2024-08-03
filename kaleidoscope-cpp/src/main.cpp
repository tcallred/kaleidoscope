#include "lexer.cpp"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Filename required");
        return 1;
    }

    std::ifstream t(argv[1]);
    std::stringstream buffer;
    buffer << t.rdbuf();

    std::vector<Token> tokens = lex(buffer.str());

    for (Token tok : tokens) {
        std::printf("%s,", tok.str().c_str());
    }
    printf("\n");

    return 0;
}
