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
#include <fcntl.h>
#include "common/debug.h"

/**
 * @brief Attempts to open a file, and prints a message if an error occured.
 * @param fn The file name to open.
 * @param flags The flags to be passed to the open call.
 * @param mode The permission for the file. Only necessary for O_CREATE.
 */
#define SAFE_OPEN(fn, flags, mode) ({\
    int fd = open(fn, flags, mode);\
    if (fd == -1) {\
        /*perror(ERROR_STR_HEADER "Unable to open file");*/\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Unable to open file");\
    }\
    (fd);\
})

/**
 * @brief Attempts to seek a position in a file descriptor, and prints an error if an error occured.
 * @param fd     The file descriptor.
 * @param off    The offset to seek.
 * @param whence The whence to be used (SEEK_SET, SEEK_CUR, SEEK_END).
 * 
 * @warning The return code in case of an error requires the define `ERR` to be defined.
 */
#define SAFE_SEEK(fd, off, whence) {\
    if (lseek(fd, off, whence) == -1) {\
        /* perror(ERROR_STR_HEADER "Unable to seek file position"); */\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Unable to seek file position");\
        return ERR;\
    }\
}

/**
 * @brief Attempts to read a position in a file descriptor, and prints an error if an error occured.
 * @param fd     The file descriptor.
 * @param buf    The buffer to read the bytes into.
 * @param nbytes The number of bytes to read.
 * 
 * @warning The return code in case of an error requires the define `ERR` to be defined.
 */
#define SAFE_READ(fd, buf, nbytes) ({\
    int rd = read(fd, buf, nbytes);\
    if (rd == -1) {\
        /* perror(ERROR_STR_HEADER "Unable to read file"); */\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Unable to read file");\
        return ERR;\
    }\
    (rd);\
})


/**
 * @brief Attempts to write to a position in a file descriptor, and prints an error if an error occured.
 * @param fd  The file descriptor.
 * @param buf The buffer to read the bytes from.
 * @param n   The number of bytes to write.
 * 
 * @warning The return code in case of an error requires the define `ERR` to be defined.
 */
#define SAFE_WRITE(fd, buf, n) {\
    if (write(fd, buf, n) == -1) {\
        /* perror(ERROR_STR_HEADER "Unable to write file"); */\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Unable to write file");\
        return ERR;\
    }\
}

/**
 * @brief Attempts to write to a position and offset in a file descriptor, and prints an error if an error occured.
 * @param fd  The file descriptor.
 * @param buf The buffer to read the bytes from.
 * @param n   The number of bytes to write.
 * @param off The offset to write at.
 * 
 * @warning The return code in case of an error requires the define `ERR` to be defined.
 */
#define SAFE_PWRITE(fd, buf, n, off) {\
    if (pwrite(fd, buf, n, off) == -1) {\
        /* perror(ERROR_STR_HEADER "Unable to write file"); */\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Unable to write file");\
        return ERR;\
    }\
}

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
        /* perror(ERROR_STR_HEADER "Exception occurred while creating file.\n"); */\
        CRITICAL_MARK_ERROR(ERROR_STR_HEADER "Exception occurred while creating file.\n");\
        exit(EXIT_FAILURE);\
    }\
    status;\
})

#define SETUP_ID(fd) ({\
    int id = 0;\
    ssize_t r = read(fd, &id, sizeof(int));\
    if(r == 0) {\
        ssize_t w = write(fd, &id, sizeof(int));\
        if(w == 0) {\
            perror(ERROR_STR_HEADER "Exception occurred while writing to id file.\n");\
            exit(EXIT_FAILURE);\
        }\
        /* SAFE_WRITE(fd, &id, sizeof(int));*/\
    }\
    id;\
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

/**
 * @brief Drains an entire FIFO.
 */
void drain_fifo(int fd);


#endif