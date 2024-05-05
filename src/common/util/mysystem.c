#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "common/util/parser.h"
#include "common/util/string.h"

typedef struct ll {
    void* data;
    struct ll* next;
    struct ll* end;
} LINKED_LIST, *LinkedList;

typedef struct exec_args {
    char* command;
    LinkedList args;
} EXEC_ARGS, *ExecArgs;

LinkedList make_ll_node(void* data) {
    LinkedList ll = (LinkedList)malloc(sizeof(LINKED_LIST));
    if (ll == NULL) {
        perror("Unable to allocate memory for LinkedList node");
        exit(1);
    }

    ll->data = data;
    ll->next = NULL;
    ll->end = NULL;

    return ll;
}

LinkedList append_end_ll_node(LinkedList pll, LinkedList ll) {
    if (ll == NULL) return NULL;
    if (pll == NULL) return ll;
    
    if (pll->end == NULL) {
        pll->next = ll;
    } else {
        pll->end->next = ll;
    }
    
    pll->end = ll;

    return pll;
}

char** ll_to_char_array(LinkedList ll) {
    if (ll == NULL) return NULL;

    LinkedList node = ll;

    int len = 1;
    while ((node = node->next) != NULL) len++;

    char** arr = (char**)calloc(len + 1, sizeof(char*));
    if (arr == NULL) {
        perror("Unable to allocate array:");
        return NULL;
    }

    int i = 0;
    node = ll;
    while (node != NULL) {
        arr[i++] = strdup(node->data);
        node = node->next;
    }

    return arr;
}

char** ea_to_args(EXEC_ARGS ea) {
    // if (ea.args == NULL) return NULL;

    LinkedList node = ea.args;

    int len = 1;
    if (ea.args != NULL) while ((node = node->next) != NULL) len++;

    char** arr = (char**)calloc(len + 2, sizeof(char*));
    if (arr == NULL) {
        perror("Unable to allocate array:");
        return NULL;
    }
    arr[0] = strdup(ea.command);

    if (ea.args == NULL) return arr;

    int i = 1;
    node = ea.args;
    while (node != NULL) {
        arr[i++] = strdup(node->data);
        node = node->next;
    }

    return arr;
}

void destroy_ll(LinkedList ll) {
    if (ll == NULL) return;

    LinkedList root = ll;
    LinkedList node = ll;
    while (root != NULL) {
        root = node->next;
        free(node);
        node = root;
    }
}

int mysystem(const char* command) {
    Tokens tokens = tokenize_char_delim((char*)command, strlen(command), " ");

    EXEC_ARGS ea = {
        .command = tokens->data[0],
        .args = NULL
    };

    for (int i = 1; i < tokens->len; i++) {
        char stringHead;
        if (tokens->data[i][0] == '"' || tokens->data[i][0] == '\'') {
            stringHead = tokens->data[i][0];

            int start = i;
            int end = i;

            char* data = NULL;
            while (end < tokens->len && (data = tokens->data[end]) != NULL && data[strlen(data) - 1] != stringHead) end++;

            int totalLen = -1 + (end - start);
            for (int i = start; i <= end; i++) totalLen += strlen(tokens->data[i]);

            char* arg = (char*)calloc((totalLen + 1), sizeof(char));
            if (arg == NULL) {
                perror("Unable to allocate memory for argument");
                return -1;
            }

            memcpy(arg, tokens->data[start] + 1, strlen(tokens->data[start]) - 1);
            strncat(arg, " ", 2);

            for (int i = start + 1; i < end; i++) {
                strncat(arg, tokens->data[i], strlen(tokens->data[i]));
                strncat(arg, " ", 2);
            }
            strncat(arg, tokens->data[end], strlen(tokens->data[end]) - 1);

            ea.args = append_end_ll_node(ea.args, make_ll_node(arg));
            i = end;
        } else {
            ea.args = append_end_ll_node(ea.args, make_ll_node(tokens->data[i]));
        }
    }

    int pid = fork();
    if (pid == -1) {
        perror("Unable to create fork");
        return -1;
    } else if (pid == 0) {
        char** args = ea_to_args(ea);

        int len = 1;
        LinkedList node = ea.args;
        if (ea.args != NULL) while ((node = node->next) != NULL) len++;

        if (execvp(ea.command, args) == -1) {
            perror("Unable to execute command");
        }

        free(args);

        _exit(0);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    destroy_ll(ea.args);
    destroy_tokens(tokens);

    return WEXITSTATUS(status);
}
