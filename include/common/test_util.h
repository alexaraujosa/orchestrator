#ifndef COMMON_TEST_UTIL_H
#define COMMON_TEST_UTIL_H

#include <stdbool.h>

void common_test_util();

enum options {
    EXECUTE,
    STATUS
};

enum types {
    UNIQUE,
    PIPELINE
};

// TRIAL VERSION OF REQUEST DATAGRAM
typedef struct request {
    int id;
    int speculate_time;
    char version;
    bool option;
    bool type;
    char args[300];
} *Request, REQUEST;

// ANOTHER TRIAL VERSION BUT NOW FOR RESPONSE DATAGRAM :)
typedef struct response {
    int id;
    char version;
} *Response, RESPONSE;



#endif