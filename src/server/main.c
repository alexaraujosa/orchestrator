/******************************************************************************
 *                              SERVER PROCESS                                *
 *                                                                            *
 *   The server process is the main entry point for any connecting client.    *
 *   It is responsible for listening for incomming requests, process them,    *
 * pass the to the operator process, and respond to the connecting clients.   *
 *   At startup, it should create all required files for execution and spawn  *
 * all processes that are required.                                           *
 *   Only one server process is supposed to exist per instance.               *
 ******************************************************************************/

#define _POSIX_C_SOURCE 199309L
#define _CRITICAL

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "common/io/io.h"
#include "common/io/fifo.h"
#include "common/datagram/datagram.h"
#include "common/datagram/execute.h"
#include "common/datagram/status.h"
#include "common/util/string.h"

volatile sig_atomic_t shutdown_requested = 0;

void signal_handler_shutdown(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        shutdown_requested = 1;
    }
}

int main(int argc, char const *argv[]) {
    #define ERR 1
    printf("Hello world from server!\n\n");

    // // Attach signal handlers
    // signal(SIGINT, signal_handler_shutdown);
    // signal(SIGTERM, signal_handler_shutdown);

    // // Prepare signal mask
    // sigset_t signal_set;
    // sigemptyset(&signal_set);
    // sigaddset(&signal_set, SIGINT);
    // sigaddset(&signal_set, SIGTERM);

    struct sigaction critical_sa_apply;
    struct sigaction critical_sa_restore;

    memset(&critical_sa_apply, 0, sizeof(critical_sa_apply));
    sigemptyset(&critical_sa_apply.sa_mask);
    critical_sa_apply.sa_handler = signal_handler_shutdown;
    critical_sa_apply.sa_flags = 0;

    // #define CRITICAL_START sigprocmask(SIG_BLOCK, &signal_set, NULL);
    // #define CRITICAL_END sigprocmask(SIG_UNBLOCK, &signal_set, NULL);

    INIT_CRITICAL_MARK

    #define CRITICAL_START SET_CRITICAL_MARK(0);\
        sigaction(SIGINT, &critical_sa_apply, &critical_sa_restore);\
        sigaction(SIGTERM, &critical_sa_apply, &critical_sa_restore);
    #define CRITICAL_END SET_CRITICAL_MARK(0);\
        sigaction(SIGINT, &critical_sa_restore, NULL);\
        sigaction(SIGTERM, &critical_sa_restore, NULL);

    if(argc < 3) {
        printf("Insufficient arguments.\n"
            "Please provide the following parameters:\n"
            "(output_folder) (number_of_parallel_tasks) (escalation_policy)\n");
        exit(EXIT_FAILURE);
    } else if(argc == 3) {
        // ...
    } else if(argc == 4) {
        char* output_folder = (char*) argv[1];
        CREATE_DIR(output_folder, 0700);

        CRITICAL_START
            char* id_file_path = join_paths(2, output_folder, "id");
            int id_fd = SAFE_OPEN(id_file_path, O_RDWR | O_CREAT, 0600);

            char* history_file_path = join_paths(2, output_folder, "history.log");
            int write_to_history_fd = SAFE_OPEN(history_file_path, O_APPEND | O_CREAT, 0600);    //TODO: Depois para ler quando se pede um status, o O_APPEND n deixa
            int read_from_history_fd = SAFE_OPEN(history_file_path, O_RDONLY, 0600);  //SEE: Podemos abrir dois descritores e um fica encarregue da escrita e outro da leitura

            char* server_fifo_path = join_paths(2, "build/", SERVER_FIFO);
            SAFE_FIFO_SETUP(server_fifo_path, 0600);

            int id = SETUP_ID(id_fd);
        CRITICAL_END

        while(!shutdown_requested) {
            DEBUG_PRINT("[DEBUG] New cycle.\n");

            // Read incoming requests.
            CRITICAL_START
                int server_fifo_fd = SAFE_OPEN(server_fifo_path, O_RDONLY, 0600);
            CRITICAL_END

            // If kill signal recieved, do not read or process requests
            if (shutdown_requested) break;

            DATAGRAM_HEADER header = read_datagram_header(server_fifo_fd);
            if (header.version == 0) {
                // Read 0 bytes, fallback to 0.
                close(server_fifo_fd);
                continue;
            } else if (header.version != DATAGRAM_VERSION) {
                // Datagram not supported.
                printf("Recieved unsupported datagram with version %d:\n", header.version);

                drain_fifo(server_fifo_fd);
                close(server_fifo_fd);
                continue;
            }

            char* dh_str = datagram_header_to_string(&header, 1);
            printf("%s\n", dh_str);
            free(dh_str);

            char* client_fifo_name = isnprintf(CLIENT_FIFO "%d", header.pid);
            char* client_fifo_path = join_paths(2, "build/", client_fifo_name);
            int client_fifo_fd = SAFE_OPEN(client_fifo_path, O_WRONLY, 0600);

            if(header.mode == DATAGRAM_MODE_EXECUTE_REQUEST) {
                printf("Recieved Execute Request.\n");

                ExecuteRequestDatagram erd = read_partial_execute_request_datagram(server_fifo_fd, header);

                ExecuteResponseDatagram response = create_execute_response_datagram();
                response->taskid = ++id;
                SAFE_WRITE(client_fifo_fd, response, sizeof(EXECUTE_RESPONSE_DATAGRAM));

                // TODO: Queue task using escalation politicy and speculate_time
                printf("Task with identifier %d queued.\n", id);

                char* task_name = isnprintf(TASK "%d", id);
                char* task_path = join_paths(2, output_folder, task_name);
                int task_fd = SAFE_OPEN(task_path, O_WRONLY | O_CREAT, 0600);

                // TODO: Execute tasks
                // TODO: Write to task file the stdout and stderr
                // TODO: Write to history file

                close(task_fd);
                free(task_path);
                free(task_name);

                printf("Execute Request finalized.\n");
            } else if(header.mode == DATAGRAM_MODE_STATUS_REQUEST) {
                printf("Recieved Status Request.\n");

                // TODO: Create status response payload

                // uint8_t status_res_payload[] = "Hello world!";
                // StatusResponseDatagram response = create_status_response_datagram(status_res_payload, 13);
                // write(client_fifo_fd, response, sizeof(STATUS_RESPONSE_DATAGRAM));

                printf("Status Request finalized.\n");
            } else if(header.mode == DATAGRAM_MODE_CLOSE_REQUEST) {
                printf("Recieved Close request.\n");

                CRITICAL_START
                    DATAGRAM_HEADER response = create_datagram_header();
                    response.mode = DATAGRAM_MODE_CLOSE_RESPONSE;
                    SAFE_WRITE(client_fifo_fd, &response, sizeof(DATAGRAM_HEADER));
                CRITICAL_END

                // Request shutdown and fallthrough.
                shutdown_requested = 1;

                // printf("Server shutting down...\n");

                // // Save current ID
                // lseek(id_fd, 0, SEEK_SET);
                // SAFE_WRITE(id_fd, &id, sizeof(int));

                // // Close file descriptors
                // close(id_fd);
                // close(history_fd);
                // close(client_fifo_fd);
                // close(server_fifo_fd);

                // // Delete server fifo
                // unlink(server_fifo_path);

                // // Free allocated strings
                // free(id_file_path);
                // free(history_file_path);
                // free(client_fifo_path);
                // free(client_fifo_name);
                // free(server_fifo_path);

                // printf("Successfully closed server.\n");
                // exit(EXIT_SUCCESS);
            }

            close(client_fifo_fd);
            close(server_fifo_fd);
            free(client_fifo_path);
            free(client_fifo_name);
        }

        // Handle server shutdown
        {
            printf("\nServer shutting down...\n");

            // Save current ID
            lseek(id_fd, 0, SEEK_SET);
            SAFE_WRITE(id_fd, &id, sizeof(int));

            // Close file descriptors
            close(id_fd);
            close(write_to_history_fd);
            close(read_from_history_fd);

            // Delete server fifo
            unlink(server_fifo_path);

            // Free allocated strings
            free(id_file_path);
            free(history_file_path);
            free(server_fifo_path);

            printf("Successfully closed server.\n");
            exit(EXIT_SUCCESS);
        }
    }

    #undef ERR
}
