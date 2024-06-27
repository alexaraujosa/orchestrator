/******************************************************************************
 *                          EXECUTE MODE DATAGRAMS                            *
 *                                                                            *
 *   The Execute Mode Datagrams is the common name given to the Request and   *
 * Response Datagrams for the Execute Mode of the application architecture.   *
 *   The Execute Mode is the mode used to queue a new task on a server        *
 * instance, and get the id of the queued task.                               *
 *                                                                            *
 *   The create_execute_<kind>_datagram functions create a new empty datagram *
 * of the specified kind, initially valid for transmission, that allows, but  *
 * discourages modfication of the data before being sent.                     *
 *   The read_execute_<kind>_datagram functions read a full datagram of the   *
 * specificed kind. They are meant to be used only if it is known beforehand  *
 * that there is a valid datagram present within a file descriptor.           *
 *   The read_partial_execute_<kind>_datagram read the payload of a datagram  *
 * of the specified kind. They are meant to be used in conjunction with the   *
 * read_datagram_header function to process the datagram header separatedly   *
 * from the payload.                                                          *
 *   The execute_<kind>_datagram_to_string converts a datagram of the         *
 * specified kind to a null-terminated, trimmed string.                       *
 ******************************************************************************/

#include "common/datagram/datagram.h"
#include "common/datagram/execute.h"
#include "common/util/string.h"
#include "common/util/alloc.h"
#include "common/io/io.h"

#pragma region ======= REQUEST =======
ExecuteRequestDatagram create_execute_request_datagram() {
    #define ERR NULL
    
    ExecuteRequestDatagram dg = SAFE_ALLOC(ExecuteRequestDatagram, sizeof(EXECUTE_REQUEST_DATAGRAM));
    dg->header = create_datagram_header();
    dg->header.mode = DATAGRAM_MODE_EXECUTE_REQUEST;

    dg->time = 0;
    memset(dg->data, 0, 300);

    return dg;
    #undef ERR
}

ExecuteRequestDatagram read_execute_request_datagram(int fd) {
    #define ERR NULL

    ExecuteRequestDatagram execute = calloc(1, sizeof(EXECUTE_REQUEST_DATAGRAM));
    SAFE_READ(fd, execute, sizeof(EXECUTE_REQUEST_DATAGRAM));

    return execute;
    #undef ERR
}

ExecuteRequestDatagram read_partial_execute_request_datagram(int fd, DATAGRAM_HEADER header) {
    #define ERR NULL
    
    ExecuteRequestDatagram execute = calloc(1, sizeof(EXECUTE_REQUEST_DATAGRAM));
    execute->header = header;

    SAFE_READ(fd, (((void*)execute) + sizeof(DATAGRAM_HEADER)), sizeof(EXECUTE_REQUEST_DATAGRAM) - sizeof(DATAGRAM_HEADER));

    return execute;
    #undef ERR
}

char* execute_request_datagram_to_string(ExecuteRequestDatagram dg, int expandEnums, int stringPayload) {
    char* dh = datagram_header_to_string(&dg->header, expandEnums);
    short int time = dg->time;
    char* bytes = stringPayload ? dg->data : bytes_to_hex_string(dg->data, EXECUTE_REQUEST_DATAGRAM_PAYLOAD_LEN, ':');

    char* str = isnprintf(
        "ExecuteRequestDatagram{ header: %s, time: %d, data: '%s' }",
        dh,
        time,
        bytes
    );

    if (!stringPayload) free(bytes);
    free(dh);

    return str;
}
#pragma endregion

#pragma region ======= RESPONSE =======
ExecuteResponseDatagram create_execute_response_datagram() {
    #define ERR NULL
    
    ExecuteResponseDatagram dg = SAFE_ALLOC(ExecuteResponseDatagram, sizeof(EXECUTE_RESPONSE_DATAGRAM));
    dg->header = create_datagram_header();
    dg->header.mode = DATAGRAM_MODE_EXECUTE_RESPONSE;

    dg->taskid = 0;

    return dg;
    #undef ERR
}

ExecuteResponseDatagram read_execute_response_datagram(int fd) {
    #define ERR NULL

    ExecuteResponseDatagram execute = calloc(1, sizeof(EXECUTE_RESPONSE_DATAGRAM));
    SAFE_READ(fd, execute, sizeof(EXECUTE_RESPONSE_DATAGRAM));

    return execute;
    #undef ERR
}

ExecuteResponseDatagram read_partial_execute_response_datagram(int fd, DATAGRAM_HEADER header) {
    #define ERR NULL
    
    ExecuteResponseDatagram execute = calloc(1, sizeof(EXECUTE_RESPONSE_DATAGRAM));
    execute->header = header;

    SAFE_READ(fd, (((void*)execute) + sizeof(DATAGRAM_HEADER)), sizeof(EXECUTE_RESPONSE_DATAGRAM) - sizeof(DATAGRAM_HEADER));

    return execute;
    #undef ERR
}

char* execute_response_datagram_to_string(ExecuteResponseDatagram dg, int expandEnums) {
    char* dh = datagram_header_to_string(&dg->header, expandEnums);
    
    char* str = isnprintf(
        "ExecuteResponseDatagram{ header: %s, taskid: %d }",
        dh,
        dg->taskid
    );

    free(dh);

    return str;
}
#pragma endregion
