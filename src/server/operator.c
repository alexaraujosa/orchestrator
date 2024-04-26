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

#include "server/operator.h"

Task_history_entry create_task_history_entry(){
    #define ERR NULL
    Task_history_entry entry = SAFE_ALLOC(Task_history_entry, sizeof(TASK_HISTORY_ENTRY));
    
    return entry;
    #undef ERR
}

Task_history_entry read_task_history_entry(int fd){
    return NULL;
}

char* task_history_entry_to_string(Task_history_entry history_entry){
    return "lul";
}

typedef enum worker_status {
    WORKER_FREE,
    WORKER_BUSY
} WorkerStatus;

typedef enum execute_task_status {
    EXECUTE_TASK_WAITING,
    EXECUTE_TASK_IN_EXECUTION
} ExecyteTaskStatus;

typedef struct operator_execute_task {
    int id_task;
    uint8_t type;
    uint8_t queue_status;
    short int speculate_time;
    char* data;
    struct timeval* start;
} OPERATOR_EXECUTE_TASK, *OperatorExecuteTask;

typedef struct operator_worker_status {
    pid_t pid;
    int pd_write;
    int pd_read;
    uint8_t status;
} OPERATOR_WORKER_STATUS, *OperatorWorkerStatus;

typedef struct worker_execute_request {
    uint8_t type;
    char* data;
} WORKER_EXECUTE_REQUEST, *WorkerExecuteRequest;

GArray* create_execute_tasks_array() {
    return g_array_new(FALSE, FALSE, sizeof(OperatorExecuteTask));
}

GArray* create_status_tasks_array() {
    return g_array_new(FALSE, FALSE, sizeof(StatusRequestDatagram));
}

GArray* create_workers_array() {    // len == num_parallel_tasks + 1 (1 for status)
    return g_array_new(FALSE, FALSE, sizeof(WorkerStatus));
}

OperatorExecuteTask create_execute_task(int id_task, ExecuteRequestDatagram request) {
    OperatorExecuteTask execute_task = malloc(sizeof(OPERATOR_EXECUTE_TASK));
    execute_task->id_task = id_task;
    execute_task->type = request->header.type;
    execute_task->queue_status = EXECUTE_TASK_WAITING;
    execute_task->speculate_time = request->time;
    execute_task->data = request->data;
    execute_task->start = malloc(sizeof(struct timeval));
    gettimeofday(execute_task->start, NULL);    // TODO: Check for error

    return execute_task;
}

OperatorWorkerStatus create_operator_worker(pid_t pid, int pd_write, int pd_read) {
    OperatorWorkerStatus worker = malloc(sizeof(OPERATOR_WORKER_STATUS));
    worker->pid = pid;
    worker->pd_write = pd_write;
    worker->pd_read = pd_read;
    worker->status = WORKER_FREE;

    return worker;
}

WorkerExecuteRequest create_worker_execute_request(uint8_t type, char* data) {
    WorkerExecuteRequest request = malloc(sizeof(WORKER_EXECUTE_REQUEST));
    request->type = type;
    request->data = data;

    return request;
}

WorkerExecuteRequest get_next_execute_task(GArray* array, int execute_tasks_in_execution) {
    if(array->len > (guint) execute_tasks_in_execution) {
        OperatorExecuteTask execute_queued = g_array_index(array, OperatorExecuteTask, execute_tasks_in_execution + 1);
        execute_queued->queue_status = EXECUTE_TASK_IN_EXECUTION;
        WorkerExecuteRequest worker_request = create_worker_execute_request(execute_queued->type, execute_queued->data);
        return worker_request;
    }

    return NULL;
}

void add_execute_task(GArray* execute_tasks_array, OperatorExecuteTask execute_task, GCompareFunc compare_function) {
    g_array_append_val(execute_tasks_array, execute_task);
    g_array_sort(execute_tasks_array, compare_function);
}

void add_status_task(GArray* status_tasks_array, StatusRequestDatagram status_task) {
    g_array_append_val(status_tasks_array, status_task);
}

gint test_compare_func(gconstpointer requests_A, gconstpointer requests_B) {
    const OperatorExecuteTask requests1 = *(const OperatorExecuteTask*)requests_A;
    const OperatorExecuteTask requests2 = *(const OperatorExecuteTask*)requests_B;
// TODO
    return 0;
}

void print_worker(OperatorWorkerStatus worker) {
    printf(
        "PID: %d | PD_READ: %d | PD_WRITE: %d | STATUS: %s", 
            worker->pid, 
            worker->pd_read, 
            worker->pd_write, 
            (worker->status == WORKER_FREE) ? "WorkerFree" : "WorkerBusy"
    );
}

void print_workers_array(GArray* array) {
    for(guint i = 0 ; i < array->len ; i++) 
        print_worker(g_array_index(array, OperatorWorkerStatus, i));
}

int start_operator(int num_parallel_tasks, char* history_file_path) {
    #define ERR 1
    int pd[2];
    pipe(pd); 

    int write_to_history_fd = SAFE_OPEN(history_file_path, O_APPEND | O_CREAT, 0600);    //TODO: Depois para ler quando se pede um status, o O_APPEND n deixa
    int read_from_history_fd = SAFE_OPEN(history_file_path, O_RDONLY, 0600);  //SEE: Podemos abrir dois descritores e um fica encarregue da escrita e outro da leitura

    pid_t pid = fork();
    if(pid == 0) {
        close(pd[1]);
        GArray* execute_tasks_array = create_execute_tasks_array();
        GArray* status_tasks_array = create_status_tasks_array();
        GArray* workers_array = create_workers_array();
        int execute_tasks_in_execution = 0;

        for(int i = 0 ; i < num_parallel_tasks + 1 ; i++) {
            // TODO: Read from start_worker an array. [pid, pd_write, pd_read]

            pid_t pid = 0;int pd_write = 0; int pd_read = 0; // Just for debug purposes.
            
            OperatorWorkerStatus worker = create_operator_worker(pid, pd_write, pd_read);print_worker(worker);  // BUG: NOT PRINTING
            g_array_insert_val(workers_array, i, worker);
        }

        // print_workers_array(workers_array);  //BUG: TRY TO PRINT STATUS AND EXECUTE ARRAYS. THEN GO TO WORKERS

        while(1) {

            if(poll(&(struct pollfd){ .fd = pd[0], .events = POLLIN }, 1, 0) == 1) {  // https://stackoverflow.com/questions/13811614/how-to-see-if-a-pipe-is-empty
                DATAGRAM_HEADER header = read_datagram_header(pd[0]);

                if(header.mode == DATAGRAM_MODE_EXECUTE_REQUEST) {
                    ExecuteRequestDatagram request = read_partial_execute_request_datagram(pd[0], header);
                    int id_task;
                    SAFE_READ(pd[0], &id_task, sizeof(int));

                    OperatorExecuteTask execute_task = create_execute_task(id_task, request);
                    add_execute_task(execute_tasks_array, execute_task, &test_compare_func);
                } else if(header.mode == DATAGRAM_MODE_STATUS_REQUEST) {
                    StatusRequestDatagram status_task = read_partial_status_request_datagram(pd[0], header);
                    add_status_task(status_tasks_array, status_task);
                    // print_status_tasks_array(status_tasks_array);
                } else if(header.mode == DATAGRAM_MODE_CLOSE_REQUEST) {
                    g_array_free(execute_tasks_array, TRUE);
                    g_array_free(status_tasks_array, TRUE);
                    // kill all workers pid
                    // free array of slaves
                    close(read_from_history_fd);
                    close(write_to_history_fd);
                    close(pd[0]);
                    _exit(0);
                }
            }
                    
            for(guint i = 0 ; i < workers_array->len ; i++) {
                OperatorWorkerStatus worker = g_array_index(workers_array, OperatorWorkerStatus, i);
                if(poll(&(struct pollfd){ .fd = worker->pd_read, .events = POLLIN }, 1, 0) == 1) {
                    // read
                    // subtract gettimeofday() with g_array_index(execute_tasks_array, worker->task_id)->start
                    // escrever no history
                    // remover a task do execute_tasks_array
                    execute_tasks_in_execution--;
                } else if(worker->status == WORKER_FREE) {
                    WorkerExecuteRequest worker_request = get_next_execute_task(execute_tasks_array, execute_tasks_in_execution);
                    SAFE_WRITE(worker->pd_write, worker_request, sizeof(WorkerExecuteRequest));
                    execute_tasks_in_execution++;
                }

                // TODO: Status request
            }

        }

    } else {
        return pd[1];
    }
    #undef ERR
}