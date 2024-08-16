#include "lexer.cpp"
#include <fstream>
#include <sstream>
#include <iostream> 
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
        std::cout << tok.str() << ",";
    }
    std::cout << "\n";

    return 0;
}
