#ifndef COMMON_DATAGRAM_EXECUTE_H
#define COMMON_DATAGRAM_EXECUTE_H

#include "common/datagram/datagram.h"

#pragma region ======= REQUEST =======
#define EXECUTE_REQUEST_DATAGRAM_PAYLOAD_LEN 300
typedef struct execute_request_datagram {
    DATAGRAM_HEADER header;
    char data[EXECUTE_REQUEST_DATAGRAM_PAYLOAD_LEN];
} EXECUTE_REQUEST_DATAGRAM, *ExecuteRequestDatagram;

/**
 * @brief Creates a new empty Execute Request Datagram.
 */
ExecuteRequestDatagram create_execute_request_datagram();

/**
 * @brief Reads an Execute Request Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
ExecuteRequestDatagram read_execute_request_datagram(int fd);


/**
 * @brief Reads an Execute Request Datagram from a file descriptor, or NULL if it fails. THis version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
ExecuteRequestDatagram read_partial_execute_request_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for an Execute Request Datagram.
 * 
 * @param header A pointer to an EXECUTE_REQUEST_DATAGRAM structure containing an execute datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 * @param stringPayload Whether the payload should be displayed as a string instead of a byte sequence.
 */
char* execute_request_datagram_to_string(ExecuteRequestDatagram dg, int expandEnums, int stringPayload);
#pragma endregion

#pragma region ======= RESPONSE =======
typedef struct execute_response_datagram {
    DATAGRAM_HEADER header;
    uint32_t taskid;
} EXECUTE_RESPONSE_DATAGRAM, *ExecuteResponseDatagram;

/**
 * @brief Creates a new empty Execute Response Datagram.
 */
ExecuteResponseDatagram create_execute_response_datagram();

/**
 * @brief Reads an Execute Response Datagram from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
ExecuteResponseDatagram read_execute_response_datagram(int fd);


/**
 * @brief Reads an Execute Response Datagram from a file descriptor, or NULL if it fails. This version should be called after
 * reading the header of a datagram, for distinguishing the datagram type.
 * 
 * @param fd     The file descriptor to read.
 * @param header The already read header.
 */
ExecuteResponseDatagram read_partial_execute_response_datagram(int fd, DATAGRAM_HEADER header);

/**
 * @brief Returns a string representation for an Execute Response Datagram.
 * 
 * @param header A pointer to an EXECUTE_RESPONSE_DATAGRAM structure containing an execute datagram.
 * @param expandEnums Whether the enums should be displayed as their numerical value or string value.
 */
char* execute_response_datagram_to_string(ExecuteResponseDatagram dg, int expandEnums);
#pragma endregion

#endif