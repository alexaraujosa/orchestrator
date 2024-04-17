#ifndef COMMON_DATAGRAM_EXECUTE_H
#define COMMON_DATAGRAM_EXECUTE_H

#include "common/datagram/datagram.h"

#define EXECUTE_DATAGRAM_PAYLOAD_LEN 300

typedef struct execute_datagram {
    DATAGRAM_HEADER header;
    char data[EXECUTE_DATAGRAM_PAYLOAD_LEN];
} EXECUTE_DATAGRAM, *ExecuteDatagram;

/**
 * @brief Creates a new empty Execute Datagram.
 */
ExecuteDatagram create_execute_datagram();

/**
 * @brief Reads an Execute Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
ExecuteDatagram read_execute_datagram(int fd);


/**
 * @brief Reads an Execute Datagram from a file descriptor, or NULL if it fails. THis version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
ExecuteDatagram read_partial_execute_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for a Datagram Header.
 * 
 * @param header A pointer to an EXECUTE_DATAGRAM structure containing an execute datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 * @param stringPayload Whether the payload should be displayed as a string instead of a byte sequence.
 */
char* execute_datagram_to_string(ExecuteDatagram dg, int expandEnums, int stringPayload);

#endif