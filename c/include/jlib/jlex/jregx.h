#pragma once
#ifndef JREGX_H
#define JREGX_H

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../jstd/jerr.h"

#define MAX_INPUT_LEN 256
#define MAX_ERROR_MSG 256

typedef struct Token {
    const char *value;
    const int *code; /* ascii code */
} token_t;

typedef struct RegularExpression {
/* [_r,r]egex.h
---------------
    #include <_types.h>
    #include <Availability.h>
    #include <sys/_types/_size_t.h>
#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
    #include <sys/_types/_wchar_t.h>

    typedef struct {
        int re_magic;
        size_t re_nsub;        number of parenthesized subexpressions
        const char *re_endp;   end pointer for REG_PEND
        struct re_guts *re_g;  none of your business :-)
    } regex_t;

    typedef struct {
        regoff_t rm_so;	 start of match 
        regoff_t rm_eo;	 end of match 
    } regmatch_t;
*/

    int magic; // my magic # -> can be "re-magic"'ed by internal regex_t
    regex_t regex;
    regmatch_t match;
    error_t error;
} regexpr_t;




bool match(const char *pattern, token_t *tokens);
static inline void translate(char *p, int len, const char src, const char dst);

#endif /* JREGX_H */
// #define JREGX_IMPL // #debug-mode
#ifdef JREGX_IMPL

static inline void translate(char *p, int len, const char src, const char dst){
    while (--len >= 0){
        if (*p == src) *p = dst;
        ++p;
    }
}

void regex_test(char *pattern, char *input){
    // "a.b" <> input.txt

    //Ensure "input.txt" contains the text to match against the pattern.
    const char *path = "input.txt";
    FILE *f = fopen(path, "r");

    //char input[MAX_INPUT_LEN];
    //char pattern[MAX_INPUT_LEN];
    regex_t regex;
    int reti;
    char error_message[MAX_ERROR_MSG];

    
    printf("Welcome to the Regular Expression Tester\n");
    printf("Enter a regular expression pattern (e.g., ^[0-9]+): ");

    if (fgets(pattern, sizeof(pattern), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return;
    }
    
    // Remove newline character from pattern
    size_t len = strlen(pattern);
    if (pattern[len - 1] == '\n') {
        pattern[len - 1] = '\0';
    }
    
    // Compile the regular expression
    reti = regcomp(&regex, pattern, 0);
    if (reti) {
        regerror(reti, &regex, error_message, sizeof(error_message));
        fprintf(stderr, "Regex compilation error: %s\n", error_message);
        return;
    }
    
    printf("Enter input text to test the regular expression (or 'quit' to exit):\n");
    
    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            break;
        }
        
        // Remove newline character from input
        len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strcmp(input, "quit") == 0) {
            break;  // Exit the program
        }
        
        // Test the regular expression against the input
        reti = regexec(&regex, input, 0, NULL, 0);
        if (!reti) {
            printf("Match\n");
        } else if (reti == REG_NOMATCH) {
            printf("No match\n");
        } else {
            regerror(reti, &regex, error_message, sizeof(error_message));
            fprintf(stderr, "Regex match error: %s\n", error_message);
        }
    }
    
    // Free the compiled regular expression
    regfree(&regex);

}



bool match(const char *pattern, Token *tokens) {
    // Implement the matching logic using the provided tokens
    // You will need to replace this function with your matching code.
    // It should return true if the pattern matches the tokens, and false otherwise.
    // You can use the tokens array to iterate through the tokenized text.
    // For simplicity, the provided code only checks if the first token matches the pattern.
    return (strcmp(pattern, tokens[0].value) == 0);
}

#endif /* JREGX_IMPL */
