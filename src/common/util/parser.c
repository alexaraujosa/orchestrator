#define _DEFAULT_SOURCE

#include "common/util/parser.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Tokens tokenize_char_delim(char* line, ssize_t len, char delim[1]) {
    char* ptr = strdup(line);
    char* ptr_root = ptr;

    if (ptr == NULL) exit(1);

    if (ptr[len - 1] == '\n') {
        ptr[len - 1] = '\0';
    }

    int seps = 1;
    for (int i = 0; ptr[i]; i++) seps += (line[i] == delim[0]);

    char** arr = (char**)malloc(seps * sizeof(char*));
    memset(arr, 0, seps * sizeof(char*));

    char* token;
    int i = 0;
    while ((token = strsep(&ptr, delim)) != NULL) {
        char* tokenData = strdup(token);

        arr[i++] = tokenData;
    }

    Tokens ret = (Tokens)malloc(sizeof(TOKENS));
    ret->data = arr;
    ret->len = seps;

    free(ptr_root);
    return ret;
}

void destroy_tokens(Tokens tokens) {
    for (int i = 0; i < tokens->len; i++) free(tokens->data[i]);
    free(tokens->data);
    free(tokens);
}
