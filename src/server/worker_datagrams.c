#include "server/worker_datagrams.h"
#include "common/util/alloc.h"
#include "common/io/io.h"

#pragma region ======= HEADER =======
WORKER_DATAGRAM_HEADER create_worker_datagram_header() {
    WORKER_DATAGRAM_HEADER dh = {
        .mode = WORKER_DATAGRAM_MODE_NONE,
        .type = DATAGRAM_TYPE_NONE,
        .task_id = 0
    };

    return dh;
}

WORKER_DATAGRAM_HEADER read_worker_datagram_header(int fd) {
    #define ERR ((WORKER_DATAGRAM_HEADER){ 0 })
    
    WORKER_DATAGRAM_HEADER header = { 0 };
    SAFE_READ(fd, &header, sizeof(WORKER_DATAGRAM_HEADER));

    return header;
    #undef ERR
}
#pragma endregion


#pragma region ======= STATUS =======
WorkerStatusDatagram create_worker_status_datagram() {
    #define ERR NULL

    WorkerStatusDatagram dg = SAFE_ALLOC(WorkerStatusDatagram, sizeof(WORKER_STATUS_DATAGRAM));
    dg->header = create_worker_datagram_header();
    dg->header.mode = WORKER_DATAGRAM_MODE_STATUS;

    return dg;

    #undef ERR
}

WorkerStatusDatagram read_partial_worker_status_datagram(int fd, WORKER_DATAGRAM_HEADER header) {
    #define ERR NULL
    
    WorkerStatusDatagram dg = SAFE_ALLOC(WorkerStatusDatagram, sizeof(WORKER_STATUS_DATAGRAM));
    dg->header = header;

    return dg;
    #undef ERR
}
#pragma endregion


#pragma region ======= EXECUTE =======
WorkerExecuteDatagram create_worker_execute_datagram() {
    #define ERR NULL

    WorkerExecuteDatagram dg = SAFE_ALLOC(WorkerExecuteDatagram, sizeof(WORKER_EXECUTE_DATAGRAM));
    dg->header = create_worker_datagram_header();
    dg->header.mode = WORKER_DATAGRAM_MODE_EXECUTE;

    return dg;

    #undef ERR
}


WorkerExecuteDatagram read_partial_worker_execute_datagram(int fd, WORKER_DATAGRAM_HEADER header) {
    #define ERR NULL
    
    WorkerExecuteDatagram dg = SAFE_ALLOC(WorkerExecuteDatagram, sizeof(WORKER_EXECUTE_DATAGRAM));
    dg->header = header;

    // SAFE_READ(
    //     fd, 
    //     (((void*)dg) + sizeof(WORKER_DATAGRAM_HEADER)), 
    //     sizeof(WORKER_EXECUTE_DATAGRAM) - sizeof(WORKER_DATAGRAM_HEADER)
    // );

    SAFE_READ(fd, dg->data, sizeof(dg->data) - 1);

    return dg;
    #undef ERR
}
#pragma endregion


#pragma region ======= SHUTDOWN =======
WorkerShutdownDatagram create_worker_shutdown_datagram() {
    #define ERR NULL

    WorkerShutdownDatagram dg = SAFE_ALLOC(WorkerShutdownDatagram, sizeof(WORKER_SHUTDOWN_DATAGRAM));
    dg->header = create_worker_datagram_header();
    dg->header.mode = WORKER_DATAGRAM_MODE_SHUTDOWN;

    return dg;

    #undef ERR
}


WorkerShutdownDatagram read_partial_worker_shutdown_datagram(int fd, WORKER_DATAGRAM_HEADER header) {
    #define ERR NULL
    
    WorkerShutdownDatagram dg = SAFE_ALLOC(WorkerShutdownDatagram, sizeof(WORKER_SHUTDOWN_DATAGRAM));
    dg->header = header;

    return dg;
    #undef ERR
}
#pragma endregion


