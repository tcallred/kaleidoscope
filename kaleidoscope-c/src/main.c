#include "arena.h"
#define ARENA_IMPLEMENTATION
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

static const char* read_contents_to_string(Arena* arena, const char* filename) {
    // Open the file for reading
    FILE* file = fopen(filename, "r");

    // Check if the file opened successfully
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    // Allocate memory for the code buffer
    char* code = arena_alloc(arena, fileSize + 1);

    // Check if memory allocation was successful
    if (code == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        return NULL;
    }
    // Read the code from the file
    fread(code, 1, fileSize, file);
    code[fileSize] = '\0';

    // Close the file
    fclose(file);
    return code;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Filename required");
        return 1;
    }

    Arena codeArena = { 0 };
    const char* code = read_contents_to_string(&codeArena, argv[1]);
    if (!code) {
        fprintf(stderr, "There was a problem reading the file");
        return 1;
    }

    Arena lexArena = { 0 };
    Token* tokens = lex(&lexArena, code);

    for (int i = 0; tokens[i].kind != TokEof; i++) {
        printf("%s ", token_to_string(&lexArena, tokens[i]));
    }
    printf("\n");

    arena_free(&codeArena);
    arena_free(&lexArena);
    return 0;
}
