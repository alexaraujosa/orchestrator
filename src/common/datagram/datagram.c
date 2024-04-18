#include "common/datagram/datagram.h"
#include "common/util/string.h"
#include "common/io/io.h"

DATAGRAM_HEADER create_datagram_header() {
   DATAGRAM_HEADER dh = {
        .version = DATAGRAM_VERSION,
        .mode = DATAGRAM_MODE_NONE,
        .type = DATAGRAM_TYPE_NONE,
        .pid = getpid()
    };

    return dh;
}

DATAGRAM_HEADER read_datagram_header(int fd) {
    #define ERR ((DATAGRAM_HEADER){ 0 })
    
    DATAGRAM_HEADER header = { 0 };
    SAFE_READ(fd, &header, sizeof(DATAGRAM_HEADER));

    return header;
}

char* datagram_header_to_string(DatagramHeader header, int expandEnums) {
    static const char* datagram_mode_strings[] = {
        "DATAGRAM_MODE_NONE",
        "DATAGRAM_MODE_STATUS",
        "DATAGRAM_MODE_EXECUTE"
    };
    static const char* datagram_type_strings[] = {
        "DATAGRAM_TYPE_NONE",
        "DATAGRAM_TYPE_UNIQUE",
        "DATAGRAM_TYPE_PIPELINE"
    };

    if (expandEnums) {
        char* template = "DatagramHeader{ version: %d, mode: %s, type: %s, pid: %d }";
        return isnprintf(
            template, 
            header->version, 
            datagram_mode_strings[header->mode], 
            datagram_type_strings[header->type], 
            header->pid
        );
    } else {
        char* template = "DatagramHeader{ version: %d, mode: %d, type: %d, pid: %d }";
        return isnprintf(template, header->version, header->mode, header->type, header->pid);
    }
}