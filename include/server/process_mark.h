#ifndef SERVER_PROCESS_MARK_H
#define SERVER_PROCESS_MARK_H

#include "common/io/io.h"

#define PROCESS_MARK_LEN 2

#define MAIN_SERVER_PROCESS_MARK { 'M', 'S' }
#define WORKER_PROCESS_MARK      { 'W', 'K' }

#define MAIN_SERVER_PROCESS_MARK_DISCRIMINATOR 1 << 0
#define WORKER_PROCESS_MARK_DISCRIMINATOR      1 << 1

/**
 * @brief Checks whether a read mark corresponds to a given control Process Mark.
 * 
 * @param mark The mark read from somewhere.
 * @param ctrl The control mark used for control.
 */
#define PROCESS_MARK_EQUALS(mark, ctrl) (mark[0] == ((int[])ctrl)[0] && mark[1] == ((int[])ctrl)[1])

/**
 * @brief Compares a given mark against the supported marks and returns an int that can be compared againt a discriminator.
 * 
 * @param mark The mark to be discriminated.
 */
#define PROCESS_MARK_SOLVER(mark) (0\
    | PROCESS_MARK_EQUALS(mark, MAIN_SERVER_PROCESS_MARK) << 0\
    | PROCESS_MARK_EQUALS(mark, WORKER_PROCESS_MARK) << 1\
    )

/**
 * @brief Attempts to read a Process Mark from a file descriptior.
 * 
 * @param fd The file descriptor to read from.
 */
static inline char* read_process_mark(int fd) {
    #define ERR ((char[2]){ 0 })

    INIT_CRITICAL_MARK
    SET_CRITICAL_MARK(1);
    
    char* mark_buf = SAFE_ALLOC(char*, 2 * sizeof(char));
    SAFE_READ(fd, mark_buf, 2 * sizeof(char));

    return mark_buf;
    #undef ERR
}
/*
#define READ_PROCESS_MARK(fd, to) {\
    CRITICAL_START\
    SAFE_READ(fd, &to, PROCESS_MARK_LEN * sizeof(char));\
    CRITICAL_END\
}
*/

#define WRITE_PROCESS_MARK(fd, mark) {\
    char mark_buf[] = mark;\
    SAFE_WRITE(fd, &mark_buf, PROCESS_MARK_LEN * sizeof(char));\
}

#endif