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

#define _CRITICAL

#include "server/operator.h"
#include "server/worker.h"
#include "server/worker_datagrams.h"

#define LOG_HEADER "[OPERATOR] "

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

        volatile sig_atomic_t shutdown_requested = 0;

        while(!shutdown_requested) {
            // DEBUG_PRINT("[OPERATOR] New cycle.\n");

            CRITICAL_START
                DATAGRAM_HEADER header = read_datagram_header(pd[0]);
            CRITICAL_END

            if (header.version == 0) {
                // Read 0 bytes, fallback to 0.
                continue;
            } else if (header.version != DATAGRAM_VERSION) {
                // Datagram not supported.
                MAIN_LOG(LOG_HEADER "Recieved unsupported datagram with version %d:\n", header.version);

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
        
        close(read_from_history_fd);
        close(write_to_history_fd);
        close(pd[0]);

        printf(LOG_HEADER "Successfully closed operator.\n");
        _exit(42);
    } else {
        DEBUG_PRINT(LOG_HEADER "Server continue.\n");
        // return pd[1];

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