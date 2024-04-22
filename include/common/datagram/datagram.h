/******************************************************************************
 *                             DATAGRAM HEADERS                               *
 *                                                                            *
 *   The Datagram Headers are the head part of any datagram between a server  *
 * instance and a client. It ensures consistency between sent and recieved    *
 * packets and facilitates the packet processing, in addition to a simple but *
 * useful structural integrety check using a versioning system.               *
 ******************************************************************************/

#ifndef COMMON_DATAGRAM_H
#define COMMON_DATAGRAM_H
#include <stdint.h>
#include <sys/types.h>

#define DATAGRAM_VERSION 2

typedef enum datagram_mode {
    DATAGRAM_MODE_NONE,
    DATAGRAM_MODE_STATUS_REQUEST,
    DATAGRAM_MODE_EXECUTE_REQUEST,
    DATAGRAM_MODE_STATUS_RESPONSE,
    DATAGRAM_MODE_EXECUTE_RESPONSE,
    DATAGRAM_MODE_CLOSE_REQUEST,
    DATAGRAM_MODE_CLOSE_RESPONSE
} DatagramMode;

typedef enum datagram_type {
    DATAGRAM_TYPE_NONE,
    DATAGRAM_TYPE_UNIQUE,
    DATAGRAM_TYPE_PIPELINE
} DatagramType;

typedef struct datagram_header {
    uint8_t version;
    uint8_t mode;
    uint8_t type;
    pid_t pid;
} DATAGRAM_HEADER, *DatagramHeader;

/**
 * @brief Checks whether a datagram header is supported on the current version.
 * @param header A DATAGRAM_HEADER
 */
#define IS_DATAGRAM_SUPPORTED_H(header) (header.version == DATAGRAM_VERSION)

/**
 * @brief Checks whether a datagram is supported on the current version.
 * @param datagram A datagram that implements a DATAGRAM_HEADER.
 */
#define IS_DATAGRAM_SUPPORTED(datagram) (IS_DATAGRAM_SUPPORTED_H(datagram.header))

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