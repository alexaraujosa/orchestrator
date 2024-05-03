/******************************************************************************
 *                             OPERATOR PROCESS                               *
 *                                                                            *
 *   The operator process is responsible for controlling and managing the     *
 * various worker processes used to parallelise the workload of the server.   *
 *   It implements an event system through the usage of anonymous pipes used  *
 * to establish a communication channel between the various worker processes  *
 * and the operator process.                                                  *
 *   Only one operator process is supposed to be used per server instance.    *
 ******************************************************************************/

#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE
#define _CRITICAL

#include <sys/wait.h>
#include "server/operator.h"
#include "server/worker.h"
#include "server/worker_datagrams.h"

#define LOG_HEADER "[OPERATOR] "
#define SHUTDOWN_TIMEOUT 1000
#define SHUTDOWN_TIMEOUT_INTERVAL 10

typedef enum {
    WORKER_UNKNOWN,
    WORKER_IDLE,
    WORKER_EXECUTING
} OperatorStatus;

typedef struct operator_worker_entry {
    Worker worker;
    OperatorStatus status;
} OPERATOR_WORKER_ENTRY, *OperatorWorkerEntry;

OperatorWorkerEntry create_operator_worker_entry(Worker worker) {
    #define ERR NULL

    OperatorWorkerEntry we = SAFE_ALLOC(OperatorWorkerEntry, sizeof(OPERATOR_WORKER_ENTRY));
    we->worker = worker;
    we->status = WORKER_IDLE;

    return we;

    #undef ERR
}

GArray* create_workers_array() { // len == num_parallel_tasks + 1 (1 for status)
    return g_array_new(FALSE, FALSE, sizeof(OperatorWorkerEntry));
}

OPERATOR start_operator(int num_parallel_tasks, char* history_file_path) {
    #define ERR (OPERATOR){ 0 }
    int pd[2];
    pipe(pd); 

    INIT_CRITICAL_MARK
    #define CRITICAL_START SET_CRITICAL_MARK(0);
    #define CRITICAL_END SET_CRITICAL_MARK(0);

    CRITICAL_START
        int write_to_history_fd = SAFE_OPEN(history_file_path, O_APPEND | O_CREAT, 0600);    //TODO: Depois para ler quando se pede um status, o O_APPEND n deixa
        int read_from_history_fd = SAFE_OPEN(history_file_path, O_RDONLY, 0600);  //SEE: Podemos abrir dois descritores e um fica encarregue da escrita e outro da leitura
    CRITICAL_END

    MAIN_LOG(LOG_HEADER "Starting operator.\n");

    pid_t pid = fork();
    if(pid == 0) {
        MAIN_LOG(LOG_HEADER "Operator started.\n");
        close(pd[1]);
        
        MAIN_LOG(LOG_HEADER "Stating %d Worker Processes.\n", num_parallel_tasks + 1);
        GArray* worker_array = create_workers_array();
        int general_tasks_in_execution = 0;

        for(int i = 0 ; i < num_parallel_tasks + 1 ; i++) {
            Worker worker = start_worker();

            OperatorWorkerEntry entry = create_operator_worker_entry(worker);

            g_array_insert_val(worker_array, i, entry);

            MAIN_LOG(LOG_HEADER "Started worker #%d with PID %d.\n", i, worker->pid);
        }

        volatile sig_atomic_t shutdown_requested = 0;

        while(!shutdown_requested) {
            // DEBUG_PRINT("[OPERATOR] New cycle.\n");

            CRITICAL_START
                DATAGRAM_HEADER header = read_datagram_header(pd[0]);
            CRITICAL_END

            if (header.version == 0) {
                // Datagram read fuck all. Ignore it.
                continue;
            } else if (header.version != DATAGRAM_VERSION) {
                // Datagram not supported.
                MAIN_LOG(LOG_HEADER "Received unsupported datagram with version %d:\n", header.version);

                drain_fifo(pd[0]);
                continue;
            }

            switch (header.mode) {
                case DATAGRAM_MODE_EXECUTE_REQUEST: {
                    ExecuteRequestDatagram dg = read_partial_execute_request_datagram(pd[0], header);

                    char* req_str = execute_request_datagram_to_string(dg, 1, 1);
                    MAIN_LOG(LOG_HEADER "Received execute request: %s\n", req_str);
                    free(req_str);
                    break;
                }
                case DATAGRAM_MODE_CLOSE_REQUEST: {
                    MAIN_LOG(LOG_HEADER "Received shutdown request.\n");
                    shutdown_requested = 1;
                    break;
                }
                default: {
                    // Tf is this datagram? Idk and idc.
                    // Do nothing.
                }
            }
        }

        // Handle graceful shutdown.
        MAIN_LOG(LOG_HEADER "Shutting down operator...\n");

        // TODO: Suicide Workers
        for (guint i = 0; i < worker_array->len; i++) {
            OperatorWorkerEntry entry = g_array_index(worker_array, OperatorWorkerEntry, i);
            MAIN_LOG(
                LOG_HEADER "Shutting down worker #%d @ %d\n", 
                i, 
                entry->worker->pid
            );

            WorkerShutdownDatagram request = create_worker_shutdown_datagram();
            SAFE_WRITE(entry->worker->pipe_write, request, sizeof(WORKER_SHUTDOWN_DATAGRAM));

            int status;
            int elapsed = 0;
            int shutdown = 0;
            int wret = 0;
            while ((elapsed += SHUTDOWN_TIMEOUT_INTERVAL) < SHUTDOWN_TIMEOUT / (int)(worker_array->len)) {
                wret = waitpid(entry->worker->pid, &status, WNOHANG);
                if (wret == -1) {
                    _exit(1);
                }

                usleep(SHUTDOWN_TIMEOUT_INTERVAL * 1000);
            
                if (wret && WIFEXITED(status)) {
                    DEBUG_PRINT(
                        LOG_HEADER "Worker #%d @ %d exited with code %d\n", 
                        i, 
                        entry->worker->pid, 
                        WEXITSTATUS(status)
                    );
                    shutdown = 1;
                    break;
                }
            }

            // Worker refuses to shutdown, suicide it.
            if (!shutdown) {
                MAIN_LOG(LOG_HEADER "Worker has not closed within the acceptable timeout. Forcefully killing process.\n");
                kill(entry->worker->pid, SIGKILL);
            }

            MAIN_LOG(
                LOG_HEADER "Successfully closed worker #%d @ %d\n", 
                i, 
                entry->worker->pid
            );
        }
        
        close(read_from_history_fd);
        close(write_to_history_fd);
        close(pd[0]);

        MAIN_LOG(LOG_HEADER "Successfully closed operator.\n");
        _exit(0);
    } else {
        DEBUG_PRINT(LOG_HEADER "Server continue.\n");

        OPERATOR op = (OPERATOR){
            .pid = pid,
            .pd_write = pd[1]
        };

        return op;
    }

    // If flow reaches here, you fucked up.
    DEBUG_PRINT(LOG_HEADER "WTF.\n");
    #undef ERR
}