#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "common/io/io.h"
#include "common/io/fifo.h"
#include "common/datagram/datagram.h"
#include "common/datagram/execute.h"
#include "common/datagram/status.h"
#include "common/util/string.h"

int main(int argc, char const *argv[]) {
    #define ERR 1
    printf("Hello world from server!\n");

    if(argc < 3) {
        printf("Insufficient arguments. Try again later.\n");
        exit(EXIT_FAILURE);
    } else if(argc == 3) {
        // ...
    } else if(argc == 4) {
        char* output_folder = (char*) argv[1];
        CREATE_DIR(output_folder, 0700);

        char* id_file_path = join_paths(2, output_folder, "id");
        int id_fd = SAFE_OPEN(id_file_path, O_RDWR | O_CREAT, 0600);

        char* history_file_path = join_paths(2, output_folder, "history.log");
        int history_fd = SAFE_OPEN(history_file_path, O_APPEND | O_CREAT, 0600);    //TODO: Depois para ler quando se pede um status, o O_APPEND n deixa

        char* server_fifo_path = join_paths(2, "build/", SERVER_FIFO);
        SAFE_FIFO_SETUP(server_fifo_path, 0600);

        int id = SETUP_ID(id_fd);

        while(1) {
            DEBUG_PRINT("[DEBUG] New cycle.\n");
            int server_fifo_fd = SAFE_OPEN(server_fifo_path, O_RDONLY, 0600);
            
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
                printf("[DEBUG] Execute started.\n");

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

                printf("[DEBUG] Execute ended.\n");
            } else if(header.mode == DATAGRAM_MODE_STATUS_REQUEST) {
                // printf("[DEBUG] Status started.\n");

                // TODO: Create status response payload

                // uint8_t status_res_payload[] = "Hello world!";
                // StatusResponseDatagram response = create_status_response_datagram(status_res_payload, 13);
                // write(client_fifo_fd, response, sizeof(STATUS_RESPONSE_DATAGRAM));

                printf("[DEBUG] Status ended.\n");
            } else if(header.mode == DATAGRAM_MODE_CLOSE_REQUEST) {
                printf("[DEBUG] Close started.\n");

                DATAGRAM_HEADER response = create_datagram_header();
                response.mode = DATAGRAM_MODE_CLOSE_RESPONSE;
                SAFE_WRITE(client_fifo_fd, &response, sizeof(DATAGRAM_HEADER));

                printf("Server shutting down...\n");

                lseek(id_fd, 0, SEEK_SET);
                SAFE_WRITE(id_fd, &id, sizeof(int));
                close(id_fd);
                close(history_fd);
                close(client_fifo_fd);
                close(server_fifo_fd);
                free(id_file_path);
                free(history_file_path);
                free(client_fifo_path);
                free(client_fifo_name);
                unlink(server_fifo_path);
                free(server_fifo_path);

                printf("[DEBUG] Close ended.\n");
                exit(EXIT_SUCCESS);
            }

            close(client_fifo_fd);
            free(client_fifo_path);
            free(client_fifo_name);
            close(server_fifo_fd);
        }

    }

    #undef ERR
}
