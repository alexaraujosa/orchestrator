#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "test/test.h"
#include "common/io/io.h"
#include "common/util/string.h"
#include "server/worker_datagrams.h"

#define ERROR_FOOTER TEST_ERROR_LABEL

#define HEADER_FILE "test_worker_datagram_header.dat"
#define STATUS_FILE "test_worker_status_datagram.dat"
#define EXECUTE_FILE "test_worker_execute_datagram.dat"
#define SHUTDOWN_FILE "test_worker_shutdown_datagram.dat"

int _cmp_worker_datagram_header(WORKER_DATAGRAM_HEADER dha, WORKER_DATAGRAM_HEADER dhb) {
    return 1
        && (dha.mode == dhb.mode)
        && (dha.type == dhb.type)
        && (dha.task_id == dhb.task_id);
}

int _cmp_worker_status_datagram(WorkerStatusRequestDatagram dga, WorkerStatusRequestDatagram dgb) {
    return _cmp_worker_datagram_header(dga->header, dgb->header);
}

int _cmp_worker_execute_datagram(WorkerExecuteRequestDatagram dga, WorkerExecuteRequestDatagram dgb) {
    return _cmp_worker_datagram_header(dga->header, dgb->header);
}

int _cmp_worker_shutdown_datagram(WorkerShutdownRequestDatagram dga, WorkerShutdownRequestDatagram dgb) {
    return _cmp_worker_datagram_header(dga->header, dgb->header);
}

void test_direct_create_worker_datagram() {
    ERROR_HEADER

    #pragma region ========== WORKER DATAGRAM HEADER =======
    {
        WORKER_DATAGRAM_HEADER dhc = { 0 };
        WORKER_DATAGRAM_HEADER dh = create_worker_datagram_header();
        ASSERT(
            _cmp_worker_datagram_header(dhc, dh),
            "[WDH] Worker Datagram Header doesn't match control."
        );
    }
    #pragma endregion ======= WORKER DATAGRAM HEADER =======

    #pragma region ========== WORKER STATUS DATAGRAM =======
    {
        // WORKER_STATUS_REQUEST_DATAGRAM dgc = { 0 };
        // dgc.header = create_worker_datagram_header();
        // dgc.header.mode = WORKER_DATAGRAM_MODE_STATUS_REQUEST;

        // WorkerStatusRequestDatagram dg = create_worker_status_request_datagram();
        // ASSERT(
        //     _cmp_worker_status_datagram(&dgc, dg),
        //     "[WDH] Worker Status Datagram doesn't match control."
        // );
    }
    #pragma endregion ======= WORKER DATAGRAM HEADER =======

    #pragma region ========== WORKER EXECUTE DATAGRAM =======
    {
        WORKER_EXECUTE_REQUEST_DATAGRAM dgc = {
            .header = create_worker_datagram_header()
        };
        dgc.header.mode = WORKER_DATAGRAM_MODE_EXECUTE_REQUEST;

        WorkerExecuteRequestDatagram dg = create_worker_execute_request_datagram();
        ASSERT(
            _cmp_worker_execute_datagram(&dgc, dg),
            "[WDH] Worker Execute Datagram doesn't match control."
        );
    }
    #pragma endregion ======= WORKER EXECUTE DATAGRAM =======

    #pragma region ========== WORKER SHUTDOWN DATAGRAM =======
    {
        WORKER_SHUTDOWN_REQUEST_DATAGRAM dgc = {
            .header = create_worker_datagram_header()
        };
        dgc.header.mode = WORKER_DATAGRAM_MODE_SHUTDOWN_REQUEST;

        WorkerShutdownRequestDatagram dg = create_worker_shutdown_request_datagram();
        ASSERT(
            _cmp_worker_shutdown_datagram(&dgc, dg),
            "[WDH] Worker Shutdown Datagram doesn't match control."
        );
    }
    #pragma endregion ======= WORKER SHUTDOWN DATAGRAM =======

    return;
    ERROR_FOOTER
}

void test_read_worker_datagram(char* test_data_dir) {
    ERROR_HEADER

    #pragma region ======= WORKER DATAGRAM HEADER =======
    {
        char* test_header_file = join_paths(2, test_data_dir, HEADER_FILE);
        int header_fd = SAFE_OPEN(test_header_file, O_RDONLY, NULL);
        if (header_fd == -1) ERROR("[READ] Unable to open Worker Datagram Header test data file.")

        WORKER_DATAGRAM_HEADER dhc = {
            .mode = 1,
            .type = 2,
            .task_id = 123
        };
        WORKER_DATAGRAM_HEADER dh = read_worker_datagram_header(header_fd);
        ASSERT(
            _cmp_worker_datagram_header(dhc, dh),
            "[READ] [WDH] Worker Datagram Header doesn't match control."
        );
    }
    #pragma endregion ======= WORKER DATAGRAM HEADER =======

    #pragma region ======= WORKER STATUS DATAGRAM =======
    {
        char* test_status_file = join_paths(2, test_data_dir, STATUS_FILE);
        int status_fd = SAFE_OPEN(test_status_file, O_RDONLY, NULL);
        if (status_fd == -1) ERROR("[READ] Unable to open Worker Status Datagram test data file.")

        WORKER_STATUS_REQUEST_DATAGRAM dgc = {
            .header = {
                .mode = 1,
                .type = 2,
                .task_id = 123
            }
        };

        WORKER_DATAGRAM_HEADER dh = read_worker_datagram_header(status_fd);
        WorkerStatusRequestDatagram dg = read_partial_worker_status_request_datagram(status_fd, dh);
        ASSERT(
            _cmp_worker_status_datagram(&dgc, dg),
            "[READ] [WDH] Worker Status Datagram doesn't match control."
        );
    }
    #pragma endregion ======= WORKER STATUS DATAGRAM =======

    #pragma region ======= WORKER EXECUTE DATAGRAM =======
    {
        char* test_execute_file = join_paths(2, test_data_dir, EXECUTE_FILE);
        int execute_fd = SAFE_OPEN(test_execute_file, O_RDONLY, NULL);
        if (execute_fd == -1) ERROR("[READ] Unable to open Worker Execute Datagram test data file.")

        WORKER_EXECUTE_REQUEST_DATAGRAM dgc = {
            .header = {
                .mode = 1,
                .type = 2,
                .task_id = 123
            },
            .data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam id augue efficitur, varius turpis porttitor, hendrerit arcu. Phasellus elementum, orci quis molestie suscipit, ante velit auctor nisl, id mattis metus lacus semper libero. Sed semper purus at lacus congue, at dictum augue luctus efficitur."
        };

        WORKER_DATAGRAM_HEADER dh = read_worker_datagram_header(execute_fd);
        WorkerExecuteRequestDatagram dg = read_partial_worker_execute_request_datagram(execute_fd, dh);
        ASSERT(
            _cmp_worker_execute_datagram(&dgc, dg),
            "[READ] [WDH] Worker Execute Datagram doesn't match control."
        );
    }
    #pragma endregion ======= WORKER EXECUTE DATAGRAM =======

    #pragma region ======= WORKER SHUTDOWN DATAGRAM =======
    {
        char* test_shutdown_file = join_paths(2, test_data_dir, SHUTDOWN_FILE);
        int shutdown_fd = SAFE_OPEN(test_shutdown_file, O_RDONLY, NULL);
        if (shutdown_fd == -1) ERROR("[READ] Unable to open Worker Shutdown Datagram test data file.")

        WORKER_SHUTDOWN_REQUEST_DATAGRAM dgc = {
            .header = {
                .mode = 1,
                .type = 2,
                .task_id = 123
            }
        };

        WORKER_DATAGRAM_HEADER dh = read_worker_datagram_header(shutdown_fd);
        WorkerShutdownRequestDatagram dg = read_partial_worker_shutdown_request_datagram(shutdown_fd, dh);
        ASSERT(
            _cmp_worker_shutdown_datagram(&dgc, dg),
            "[READ] [WDH] Worker Shutdown Datagram doesn't match control."
        );
    }
    #pragma endregion ======= WORKER SHUTDOWN DATAGRAM =======

    return;
    ERROR_FOOTER
}

void test_worker_datagram(char* test_data_dir) {
    test_direct_create_worker_datagram();
    test_read_worker_datagram(test_data_dir);
}