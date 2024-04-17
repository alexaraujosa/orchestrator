#include "common/datagram/datagram.h"
#include "common/datagram/status.h"
#include "common/util/string.h"
#include "common/io/io.h"

// struct status_datagram {
//     struct datagram_header;
//     int data;
// };

StatusDatagram create_status_datagram() {
    STATUS_DATAGRAM dg = {
        .header = create_datagram_header(),
        .data = 0
    };

    dg;
}

StatusDatagram read_status_datagram(int fd) {
    #define ERR NULL

    StatusDatagram status = calloc(1, sizeof(STATUS_DATAGRAM));
    SAFE_READ(fd, status, sizeof(STATUS_DATAGRAM));

    return status;
}

StatusDatagram read_partial_status_datagram(int fd, DATAGRAM_HEADER header) {
    #define ERR NULL
    
    StatusDatagram status = calloc(1, sizeof(STATUS_DATAGRAM));
    status->header = header;

    SAFE_READ(fd, (((void*)status) + sizeof(DATAGRAM_HEADER)), sizeof(STATUS_DATAGRAM) - sizeof(DATAGRAM_HEADER));

    return status;
}

char* status_datagram_to_string(StatusDatagram dg, int expandEnums) {
    char* dh = datagram_header_to_string(&dg->header, expandEnums);
    char* bytes = bytes_to_hex_string(dg->data, STATUS_DATAGRAM_PAYLOAD_LEN, ':');

    char* str = isnprintf(
        "StatusDatagram{ header: %s, data: '%s' }",
        dh,
        bytes
    );

    free(bytes);
    free(dh);

    return str;
}
