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
#include "server/process_mark.h"

#define LOG_HEADER "[OPERATOR] "
#define SHUTDOWN_TIMEOUT 1000
#define SHUTDOWN_TIMEOUT_INTERVAL 10

/**
 * @brief Represents the current status of a Worker.
 */
typedef enum {
    WORKER_STATUS_UNKNOWN,
    WORKER_STATUS_IDLE,
    WORKER_STATUS_BUSY
} OperatorStatus;

/**
 * @brief Represents the internal state of a Worker for the Operator.
 */
typedef struct operator_worker_entry {
    Worker worker;
    OperatorStatus status;
} OPERATOR_WORKER_ENTRY, *OperatorWorkerEntry;

#pragma region ============== SIGNAL HANDLING ==============
// Yes, this uses a global variable, but the alternative is having the workers be orphaned after the operator dies.
GArray* _children;
static void operator_signal_sigsegv(int signum) {
    if (signum != SIGSEGV) return;

    MAIN_LOG(LOG_HEADER "Segmentation fault. Terminating workers.\n");

    for (guint i = 0; i < _children->len; ++i) {
        kill(g_array_index(_children, OperatorWorkerEntry, i)->worker->pid, SIGKILL);
    }

    _exit(EXIT_FAILURE);
}
#pragma endregion

#pragma region ============== WORKER ARRAY ==============
typedef GArray* WorkerArray;

OperatorWorkerEntry create_operator_worker_entry(Worker worker) {
    #define ERR NULL

    OperatorWorkerEntry we = SAFE_ALLOC(OperatorWorkerEntry, sizeof(OPERATOR_WORKER_ENTRY));
    we->worker = worker;
    we->status = WORKER_STATUS_IDLE;

    return we;

    #undef ERR
}

WorkerArray create_workers_array() { // len == num_parallel_tasks + 1 (1 for status)
    return g_array_new(FALSE, FALSE, sizeof(OperatorWorkerEntry));
}

OperatorWorkerEntry get_idle_worker(WorkerArray workers) {
    for (guint i = 0; i < workers->len; i++) {
        OperatorWorkerEntry entry = g_array_index(workers, OperatorWorkerEntry, i);
        if (entry->status == WORKER_STATUS_IDLE) return entry;
    }

    return NULL;
}
#pragma endregion

#pragma region ============== WORKER REQUEST QUEUE ==============

typedef struct operator_task {
    int id_task;
    short int speculate_time;
    WorkerDatagram datagram; // Any Worker Datagram
    int datagram_size;
    struct timeval* start;
} OPERATOR_TASK, *OperatorTask;

typedef GQueue* RequestQueue;

#pragma region ======= FUNCTION PREDICATES =======
gint request_queue_compare_fifo(gconstpointer a, gconstpointer b, gpointer user_data) {
    return ((OperatorTask)a)->start - ((OperatorTask)b)->start;
}

gint find_queue_task_by_id(gconstpointer src, gconstpointer ctrl) {
    return ((OperatorTask)src)->id_task == (int)ctrl;
}
#pragma endregion

RequestQueue create_request_queue() {
    return g_queue_new();
}

OperatorTask create_task(int id_task, int speculate_time, WorkerDatagram datagram, int datagram_size) {
    OperatorTask execute_task = malloc(sizeof(OPERATOR_TASK));
    execute_task->id_task = id_task;
    execute_task->speculate_time = speculate_time;
    execute_task->datagram = datagram;
    execute_task->datagram_size = datagram_size;
    execute_task->start = malloc(sizeof(struct timeval));
    gettimeofday(execute_task->start, NULL);    // TODO: Check for error

    return execute_task;
}

OperatorTask get_next_task(RequestQueue request_waiting_queue) {
    return (OperatorTask)g_queue_pop_head(request_waiting_queue);
}

void add_task_to_backlog(RequestQueue request_waiting_queue, GCompareDataFunc comparator, OperatorTask task) {
    g_queue_insert_sorted(request_waiting_queue, task, comparator, NULL);
}

OperatorTask prepare_task_from_queue(RequestQueue request_waiting_queue, RequestQueue active_request_queue) {
    OperatorTask next_task = get_next_task(request_waiting_queue);
    g_queue_push_tail(active_request_queue, next_task);
    
    return next_task;
}

void complete_task_from_queue(RequestQueue active_request_queue, int task_id) {
    int ind = g_queue_find_custom(active_request_queue, &task_id, find_queue_task_by_id);
    g_queue_pop_nth(active_request_queue, ind);
}

void execute_task(OperatorWorkerEntry worker, OperatorTask task) {
    #define ERR
    INIT_CRITICAL_MARK
    SET_CRITICAL_MARK(1);

    MAIN_LOG(LOG_HEADER "WP=%d, TD=%p, TDS=%d\n", worker->worker->pipe_write, task->datagram, task->datagram_size);

    SAFE_WRITE(worker->worker->pipe_write, task->datagram, task->datagram_size);
    #undef ERR
}
#pragma endregion

OPERATOR start_operator(int num_parallel_tasks, char* history_file_path) {
    #define ERR (OPERATOR){ 0 }

    int pd[2];
    pipe(pd); 

    INIT_CRITICAL_MARK
    #define CRITICAL_START SET_CRITICAL_MARK(1);
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
        
        #pragma region ======= WORKER INITIALIZATION =======
        MAIN_LOG(LOG_HEADER "Stating %d Worker Processes.\n", num_parallel_tasks + 1);
        
        WorkerArray worker_array = create_workers_array();
        int workers_busy = 0;

        for(int i = 0 ; i < num_parallel_tasks + 1 ; i++) {
            Worker worker = start_worker(pd[1]);

            OperatorWorkerEntry entry = create_operator_worker_entry(worker);
            g_array_insert_val(worker_array, i, entry);

            MAIN_LOG(LOG_HEADER "Started worker #%d with PID %d.\n", i, worker->pid);
        }

        // Set global variable for SIGSEGV handling.
        _children = worker_array;
        #pragma endregion

        #pragma region ======= WORKER REQUEST QUEUE INITIALIZATION =======
        RequestQueue active_request_queue = create_request_queue();
        RequestQueue request_waiting_queue = create_request_queue();
        #pragma endregion


        // Handle Segmentation Faults "gracefully"
        signal(SIGSEGV, operator_signal_sigsegv);
        volatile sig_atomic_t shutdown_requested = 0;

        // Read data from both the main server and worker
        while(!shutdown_requested) {
            DEBUG_PRINT(LOG_HEADER "New cycle.\n");

            // Read and discriminate process mark.
            char* mark = read_process_mark(pd[0]);
            if (shutdown_requested) break;

            // DEBUG_PRINT(LOG_HEADER "Mark: %d %d\n", mark[0], mark[1]);

            switch (PROCESS_MARK_SOLVER(mark)) {
                case 0: {
                    // TODO: Possibly simply ignore the request?
                    shutdown_requested = 1;
                    break;
                }
                case MAIN_SERVER_PROCESS_MARK_DISCRIMINATOR: {
                    MAIN_LOG(LOG_HEADER "Received message from Main Server Process.\n");
                    CRITICAL_START
                        DATAGRAM_HEADER header = read_datagram_header(pd[0]);
                    CRITICAL_END

                    if (header.version == 0) {
                        // Read fuck all. Ignore it.
                        continue;
                    } else if (header.version != DATAGRAM_VERSION) {
                        // Datagram not supported.
                        MAIN_LOG(LOG_HEADER "Received unsupported datagram with version %d:\n", header.version);

                        // TODO: Read until next Process Mark.
                        drain_fifo(pd[0]);
                        continue;
                    }

                    switch (header.mode) {
                        case DATAGRAM_MODE_EXECUTE_REQUEST: {
                            ExecuteRequestDatagram request = read_partial_execute_request_datagram(pd[0], header);

                            int id = 0;
                            SAFE_READ(pd[0], &id, sizeof(int));

                            char* req_str = execute_request_datagram_to_string(request, 1, 1);
                            MAIN_LOG(LOG_HEADER "Received execute request: %s\n", req_str);
                            MAIN_LOG(LOG_HEADER "Task id: %d\n", id);
                            free(req_str);
                            
                            WorkerExecuteDatagram dg = create_worker_execute_datagram();
                            memcpy(dg->data, request->data, EXECUTE_REQUEST_DATAGRAM_PAYLOAD_LEN);

                            OperatorTask task = create_task(
                                id, 
                                request->time, 
                                (WorkerDatagram)dg, 
                                sizeof(WORKER_EXECUTE_DATAGRAM)
                            );

                            add_task_to_backlog(
                                request_waiting_queue, 
                                request_queue_compare_fifo,
                                task
                            );
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
                    break;
                }
                case WORKER_PROCESS_MARK_DISCRIMINATOR: {
                    MAIN_LOG(LOG_HEADER "Received message from Worker Process.\n");
                    break;
                }
            }

            // Take cycle opportunity to attempt to execute queued requests.
            MAIN_LOG(LOG_HEADER "Attempting to dispatch queued tasks.\n");

            DEBUG_PRINT(
                LOG_HEADER "Tasks in backlog: %d\nTasks in execution: %d\n", 
                request_waiting_queue->length, 
                active_request_queue->length
            );

            if (request_waiting_queue->length > 0) {
                // TODO: Dispatch status tasks

                if (workers_busy == num_parallel_tasks) {
                    MAIN_LOG(LOG_HEADER "Unable to dispatch tasks: All workers are busy.\n");
                    continue;
                }

                OperatorWorkerEntry entry = get_idle_worker(worker_array);
                OperatorTask task = get_next_task(request_waiting_queue);
                execute_task(entry, task);
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