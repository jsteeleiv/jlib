#ifndef JLEX_H
#define JLEX_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_WORD,
    TOKEN_SPACE,
    TOKEN_NEWLINE,
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

Token getNextToken(FILE *file);
bool match(const char *pattern, Token *tokens);

#ifdef JLEX_IMPL


#endif /* JLEX_IMPL */



#endif /* JLEX_H */
