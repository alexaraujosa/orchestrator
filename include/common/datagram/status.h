/******************************************************************************
 *                           STATUS MODE DATAGRAMS                            *
 *                                                                            *
 *   The Status Mode Datagrams is the common name given to the Request and    *
 * Response Datagrams for the Status Mode of the application architecture.    *
 *   The Status Mode is the mode used to query the current state of a server  *
 * instance, returning the queues, running and completed tasks.               *
 *                                                                            *
 *   The create_status_<kind>_datagram functions create a new empty datagram  *
 * of the specified kind, initially valid for transmission, that allows, but  *
 * discourages modfication of the data before being sent.                     *
 *   The read_status_<kind>_datagram functions read a full datagram of the    *
 * specificed kind. They are meant to be used only if it is known beforehand  *
 * that there is a valid datagram present within a file descriptor.           *
 *   The read_partial_status_<kind>_datagram read the payload of a datagram   *
 * of the specified kind. They are meant to be used in conjunction with the   *
 * read_datagram_header function to process the datagram header separatedly   *
 * from the payload.                                                          *
 *   The status_<kind>_datagram_to_string converts a datagram of the          *
 * specified kind to a null-terminated, trimmed string.                       *
 ******************************************************************************/

#ifndef COMMON_DATAGRAM_STATUS_H
#define COMMON_DATAGRAM_STATUS_H

#include "common/datagram/datagram.h"

#pragma region ======= REQUEST =======
#define STATUS_REQUEST_DATAGRAM_PAYLOAD_LEN 8

typedef struct status_request_datagram {
    DATAGRAM_HEADER header;
} STATUS_REQUEST_DATAGRAM, *StatusRequestDatagram;

/**
 * @brief Creates a new empty Status Request Datagram.
 */
StatusRequestDatagram create_status_request_datagram();

/**
 * @brief Reads a Status Request Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
StatusRequestDatagram read_status_request_datagram(int fd);


/**
 * @brief Reads a Status Request Datagram from a file descriptor, or NULL if it fails. This version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
StatusRequestDatagram read_partial_status_request_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for a Status Request Datagram.
 * 
 * @param header A pointer to a STATUS_REQUEST_DATAGRAM structure containing a status request datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 */
char* status_request_datagram_to_string(StatusRequestDatagram dg, int expandEnums);
#pragma endregion

#pragma region ======= RESPONSE =======
typedef struct status_response_datagram {
    DATAGRAM_HEADER header;
    uint32_t payload_len;
    uint8_t payload[];
} STATUS_RESPONSE_DATAGRAM, *StatusResponseDatagram;

/**
 * @brief Creates a new empty Status Response Datagram.
 */
StatusResponseDatagram create_status_response_datagram(uint8_t payload[], int payload_len);

/**
 * @brief Reads a Status Response Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
StatusResponseDatagram read_status_response_datagram(int fd);


/**
 * @brief Reads a Status Response Datagram from a file descriptor, or NULL if it fails. THis version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
StatusResponseDatagram read_partial_status_response_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for a Status Response Datagram.
 * 
 * @param header A pointer to a STATUS_REQUEST_DATAGRAM structure containing a status response datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 * @param stringPayload Whether the payload should be displayed as a string instead of a byte sequence.
 */
char* status_response_datagram_to_string(StatusResponseDatagram dg, int expandEnums, int stringPayload);
#pragma endregion

#endif