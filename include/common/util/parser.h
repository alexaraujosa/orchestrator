#ifndef COMMON_UTIL_PARSER_H
#define COMMON_UTIL_PARSER_H

#include <string.h>
#include <unistd.h>

typedef struct tokens {
    char** data;
    int len;
} TOKENS, *Tokens;

Tokens tokenize_char_delim(char* line, ssize_t len, char delim[1]);

void destroy_tokens(Tokens tokens);

#endif