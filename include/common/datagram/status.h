#ifndef COMMON_DATAGRAM_STATUS_H
#define COMMON_DATAGRAM_STATUS_H

#include "common/datagram/datagram.h"

#define STATUS_REQUEST_DATAGRAM_PAYLOAD_LEN 8

typedef struct status_request_datagram {
    DATAGRAM_HEADER header;
    char data[STATUS_REQUEST_DATAGRAM_PAYLOAD_LEN];
} STATUS_REQUEST_DATAGRAM, *StatusRequestDatagram;

/**
 * @brief Creates a new empty Status Datagram.
 */
StatusRequestDatagram create_status_request_datagram();

/**
 * @brief Reads a Status Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
StatusRequestDatagram read_status_request_datagram(int fd);


/**
 * @brief Reads a Status Datagram from a file descriptor, or NULL if it fails. THis version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
StatusRequestDatagram read_partial_status_request_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for a Datagram Header.
 * 
 * @param header A pointer to a STATUS_REQUEST_DATAGRAM structure containing a status datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 */
char* status_request_datagram_to_string(StatusRequestDatagram dg, int expandEnums);

#endif