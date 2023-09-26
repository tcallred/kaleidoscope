#include "arena.h"
#define ARENA_IMPLEMENTATION
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

const char* read_contents_to_string(Arena* arena, const char* filename)
{
    // Open the file for reading
    FILE* file = fopen(filename, "r");

    // Check if the file opened successfully
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // Allocate memory for the code buffer
    char* code = arena_alloc(arena, file_size + 1);

    // Check if memory allocation was successful
    if (code == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        return NULL;
    }
    // Read the code from the file
    fread(code, 1, file_size, file);
    code[file_size] = '\0';

    // Close the file
    fclose(file);
    return code;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Filename required");
        return 1;
    }

    Arena code_arena = { 0 };
    const char* code = read_contents_to_string(&code_arena, argv[1]);
    if (!code) {
        fprintf(stderr, "There was a problem reading the file");
        return 1;
    }

    Arena lex_arena = { 0 };
    Token* tokens = lex(&lex_arena, code);

    for (int i = 0; tokens[i].kind != Eof; i++) {
        printf("%s ", token_to_string(&lex_arena, tokens[i]));
    }
    printf("\n");

    arena_free(&code_arena);
    arena_free(&lex_arena);
    return 0;
}
