/******************************************************************************
 *                               WORKER PROCESS                               *
 *                                                                            *
 *   The worker process is responsible for executing the different tasks that *
 * are assigned to it by the operator process. On creation, it shall connect  *
 * to the event system of the operator process and transmit a ready signal,   *
 * upon which the operator process shall distribute tasks as they arrive.     *
 *   The amount of worker processes to be created depends on the server input *
 * parallel_tasks.                                                            *
 ******************************************************************************/

#include "server/worker.h"
#include "server/worker_datagrams.h"
#include "common/util/alloc.h"
#include "common/util/string.h"
#include "common/error.h"
#include "common/io/io.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define LOG_HEADER "[WORKER] "
#define LOG_HEADER_PID "[WORKER@%d] "
#define READ 0
#define WRITE 1

void worker_signal_sigsegv(int signum) {
    if (signum != SIGSEGV) return;

    int pid = getpid();

    MAIN_LOG(LOG_HEADER_PID "Segmentation fault.\n", pid);
    _exit(1);
}

Worker start_worker(int operator_pd) {
    #define ERR NULL
    ERROR_HEADER
    int _err_pid = 0;

    // Return value
    Worker ret = SAFE_ALLOC(Worker, sizeof(WORKER));

    int pfd[2];
    if (pipe(pfd) != 0) ERROR("Unable to open worker pipe");

    int pid = fork();
    if (pid == 0) {
        pid = getpid();
        _err_pid = pid;

        volatile sig_atomic_t shutdown_requested = 0;

        MAIN_LOG(LOG_HEADER_PID "Ready.\n", pid);

        while (!shutdown_requested) {
            WORKER_DATAGRAM_HEADER dh = read_worker_datagram_header(pfd[READ]);

            switch (dh.mode) {
                case WORKER_DATAGRAM_MODE_STATUS: {
                    WorkerStatusDatagram dg = read_partial_worker_status_datagram(pfd[READ], dh);
                    MAIN_LOG(LOG_HEADER_PID "Received status request.\n", pid);
                    break;
                }
                case WORKER_DATAGRAM_MODE_EXECUTE: {
                    WorkerExecuteDatagram dg = read_partial_worker_execute_datagram(pfd[READ], dh);
                    MAIN_LOG(LOG_HEADER_PID "Received execute request.\n", pid);
                    DEBUG_PRINT(
                        LOG_HEADER_PID "Mode: %d, Type: %d, Id: %d, Data: '%s'\n", 
                        pid,
                        dg->header.mode, 
                        dg->header.type, 
                        dg->header.task_id, 
                        (char*)(dg->data)
                    );
                    break;
                }
                case WORKER_DATAGRAM_MODE_SHUTDOWN: {
                    WorkerShutdownDatagram dg = read_partial_worker_shutdown_datagram(pfd[READ], dh);
                    MAIN_LOG(LOG_HEADER_PID "Received shutdown request.\n", pid);
                    shutdown_requested = 1;
                    break;
                }
            }
        }

        // Shutdown worker
        MAIN_LOG(LOG_HEADER_PID "Shutting down...\n", pid);

        close(pfd[0]);
        close(pfd[1]);

        MAIN_LOG(LOG_HEADER_PID "Successfully shutdown.\n", pid);
        _exit(0);
    }

    close(pfd[0]);
    ret->pid = pid;
    ret->pipe_write = pfd[1];
    
    return ret;

    err: {
        if (_err_pid) MAIN_LOG(LOG_HEADER "[%s:%d] %s", __FILE__, __line, __error);
        else MAIN_LOG(LOG_HEADER_PID "[%s:%d] %s", _err_pid, __FILE__, __line, __error);
        return ERR;
    }
}
