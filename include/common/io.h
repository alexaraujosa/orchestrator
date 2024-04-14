#ifndef COMMON_IO_H
#define COMMON_IO_H

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <linux/limits.h>
#include <unistd.h>

/**
 * @brief The name for the server fifo.
 */
#define SERVER_FIFO "sv_fifo"

/**
 * @brief The name for the client fifo.
 */
#define CLIENT_FIFO "cl_fifo"

/**
 * @brief The name of tasks files.
*/
#define TASK "task_"

/**
 * @brief The filesystem path separator for Unix systems. 
 */
#define FS_PATH_SEPARATOR '/'

/**
 * @brief Creates, if doesn't exist, a directory on the given path/name and mode.
 * 
 * @param dirname The path/name of the directory to create.
 * @param mode The mode to create the directory.
 * 
 * @return The creation status of the directory.
 */
#define CREATE_DIR(dirname, mode) ({\
    int status = mkdir(dirname, mode);\
    status;\
})

/**
 * @brief Creates a directory on the given path/name and mode.
 * 
 * @param dirname The path/name of the directory to create.
 * @param mode The mode to create the directory.
 * 
 * @return The creation status of the directory.
 */
#define CREATE_SECURE_DIR(dirname, mode) ({\
    int status = mkdir(dirname, mode);\
    if (status) {\
        perror("ERROR! Exception occured while creating file.\n");\
        exit(EXIT_FAILURE);\
    }\
    status;\
})

/**
 * @brief Concatenates an array of strings into a single filesystem path.
 * 
 * @param len The number of path parts to join.
 * @param ... The path parts to be joined.
 * 
 * @return The joint path (not resolved).
 */
char* join_paths(int len, ...);

/**
 * @brief Returns the Current Working Directory of this program (CWD).
 * 
 * @note This function is memoized.
 * 
 * @return The CWD of the program.
 */
char* get_cwd();


#endif