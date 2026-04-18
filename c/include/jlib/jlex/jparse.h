#pragma once
#ifndef JPARSE_H
#define JPARSE_H


#include <stdio.h>
#include <stdbool.h>

typedef enum TokenType {
    TOKEN_UNKNOWN,
    TOKEN_WORD,
    TOKEN_SPACE,
    TOKEN_NEWLINE,
} toktype_t;

typedef struct Token {
    TokenType type;
    char *value;
} token_t;

Token getNextToken(FILE *file);
bool match(const char *pattern, Token *tokens);

#endif /* JPARSE_H */