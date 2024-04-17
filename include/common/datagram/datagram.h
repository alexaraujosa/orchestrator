#ifndef COMMON_DATAGRAM_H
#define COMMON_DATAGRAM_H
#include <stdint.h>
#include <sys/types.h>

#define DATAGRAM_VERSION 2
#define DATAGRAM_HEADER_LENGTH 4

typedef enum datagram_mode {
    DATAGRAM_MODE_NONE,
    DATAGRAM_MODE_STATUS,
    DATAGRAM_MODE_EXECUTE
} DatagramMode;

typedef enum datagram_type {
    DATAGRAM_TYPE_NONE,
    DATAGRAM_TYPE_UNIQUE,
    DATAGRAM_TYPE_PIPELINE
} DatagramType;

typedef struct datagram_header {
    // uint8_t version;
    // uint8_t header_len;
    // uint8_t type;
    // uint8_t _gap_0;
    uint8_t version;
    uint8_t mode;
    uint8_t type;
    pid_t pid;
} DATAGRAM_HEADER, *DatagramHeader;

/**
 * @brief Creates a new empty Status Datagram.
 */
DATAGRAM_HEADER create_datagram_header();

/**
 * @brief Reads a Datagram Header from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
DATAGRAM_HEADER read_datagram_header(int fd);

/**
 * @brief Returns a string representation for a Datagram Header.
 * 
 * @param header A pointer to a DATAGRAM_HEADER structure containing a datagram header.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 */
char* datagram_header_to_string(DatagramHeader header, int expandEnums);

#endif