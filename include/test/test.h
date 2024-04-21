#ifndef TEST_H
#define TEST_H

#include <signal.h>
#include "common/debug.h"

/**
 * @brief Whether the tests should be ran on debug mode. 
 */
extern int IS_DEBUG;

#define TEST_ERROR_LABEL err:\
    printf("Test failed - %s:%d\n - %s\n", __FILE__, __line, __error);\
    if (IS_DEBUG) raise(SIGINT);\
    else exit(EXIT_FAILURE);

#endif