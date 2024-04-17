#ifndef COMMON_DATAGRAM_STATUS_H
#define COMMON_DATAGRAM_STATUS_H

#include "common/datagram/datagram.h"

#define STATUS_DATAGRAM_PAYLOAD_LEN 8

typedef struct status_datagram {
    DATAGRAM_HEADER header;
    char data[STATUS_DATAGRAM_PAYLOAD_LEN];
} STATUS_DATAGRAM, *StatusDatagram;

/**
 * @brief Creates a new empty Status Datagram.
 */
StatusDatagram create_status_datagram();

/**
 * @brief Reads a Status Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
StatusDatagram read_status_datagram(int fd);


/**
 * @brief Reads a Status Datagram from a file descriptor, or NULL if it fails. THis version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
StatusDatagram read_partial_status_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for a Datagram Header.
 * 
 * @param header A pointer to a STATUS_DATAGRAM structure containing a status datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 */
char* status_datagram_to_string(StatusDatagram dg, int expandEnums);

#endif