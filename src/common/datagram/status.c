#include "common/datagram/datagram.h"
#include "common/datagram/status.h"
#include "common/util/string.h"
#include "common/io/io.h"

// struct status_request_datagram {
//     struct datagram_header;
//     int data;
// };

StatusRequestDatagram create_status_request_datagram() {
    STATUS_REQUEST_DATAGRAM dg = {
        .header = create_datagram_header(),
        .data = 0
    };

    dg;
}

StatusRequestDatagram read_status_request_datagram(int fd) {
    #define ERR NULL

    StatusRequestDatagram status = calloc(1, sizeof(STATUS_REQUEST_DATAGRAM));
    SAFE_READ(fd, status, sizeof(STATUS_REQUEST_DATAGRAM));

    return status;
}

StatusRequestDatagram read_partial_status_request_datagram(int fd, DATAGRAM_HEADER header) {
    #define ERR NULL
    
    StatusRequestDatagram status = calloc(1, sizeof(STATUS_REQUEST_DATAGRAM));
    status->header = header;

    SAFE_READ(fd, (((void*)status) + sizeof(DATAGRAM_HEADER)), sizeof(STATUS_REQUEST_DATAGRAM) - sizeof(DATAGRAM_HEADER));

    return status;
}

char* status_request_datagram_to_string(StatusRequestDatagram dg, int expandEnums) {
    char* dh = datagram_header_to_string(&dg->header, expandEnums);
    char* bytes = bytes_to_hex_string(dg->data, STATUS_REQUEST_DATAGRAM_PAYLOAD_LEN, ':');

    char* str = isnprintf(
        "StatusRequestDatagram{ header: %s, data: '%s' }",
        dh,
        bytes
    );

    free(bytes);
    free(dh);

    return str;
}
