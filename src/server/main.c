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

        SAFE_FIFO_SETUP(SERVER_FIFO, 0600); // TODO: QUANDO TERMINAR O SERVIDOR, DAR UNLINK AO SERVER_FIFO
        //TODO: COLOCAR TUDO NA BUILD QUE SEJA TEMPORARIO
        int id = SETUP_ID(id_fd);

        while(1) {
            int server_fifo_fd = SAFE_OPEN(SERVER_FIFO, O_RDONLY, 0600);
            DATAGRAM_HEADER header = read_datagram_header(server_fifo_fd);
            if(header.version != DATAGRAM_VERSION) {
                close(server_fifo_fd);
                continue;
            }

            char* client_fifo_name = isnprintf(CLIENT_FIFO "%d", header.pid);
            int client_fifo_fd = SAFE_OPEN(client_fifo_name, O_WRONLY, 0600);

            if(header.mode == DATAGRAM_MODE_EXECUTE_REQUEST) {
                printf("[DEBUG] Execute started.\n");

                ExecuteResponseDatagram response = create_execute_response_datagram();
                response->taskid = ++id;
                write(client_fifo_fd, response, sizeof(EXECUTE_RESPONSE_DATAGRAM));    //TODO: PASS TO SAFE_WRITE

                char* task_name = isnprintf(TASK "%d", id);
                char* task_path = join_paths(2, output_folder, task_name);
                int task_fd = SAFE_OPEN(task_path, O_WRONLY | O_CREAT, 0600);

                // ..

                close(task_fd);
                free(task_path);
                free(task_name);

                printf("[DEBUG] Execute ended.\n");
            } else if(header.mode == DATAGRAM_MODE_STATUS_REQUEST) {
                printf("[DEBUG] Status started.\n");

                // ..

                uint8_t status_res_payload[] = "Hello world!";
                StatusResponseDatagram response = create_status_response_datagram(status_res_payload, 13);
                write(client_fifo_fd, response, sizeof(STATUS_RESPONSE_DATAGRAM));

                printf("[DEBUG] Status ended.\n");
            }

            close(client_fifo_fd);
            free(client_fifo_name);
            close(server_fifo_fd);
        }


        close(id_fd);
        close(history_fd);

        free(id_file_path);
        free(history_file_path);
    }

    #undef ERR
}

// int main(int argc, char const *argv[]) {
//     printf("Hello world from server!\n");

//     if(argc < 4) {
//         printf("Insufficient arguments. Try again later.\n");
//         exit(EXIT_FAILURE);
//     } else {
//         // SERVER PERSISTENCE DATA SETUP
//         char* sv_data_path = join_paths(3, get_cwd(), "build", "server_data");
//         CREATE_DIR(sv_data_path, 0777); //TODO: Verify mode

//         // ID SETUP
//         char* id_path = join_paths(2, sv_data_path, "id");
//         int id_fd = open(id_path, O_RDWR | O_CREAT, 0777); //TODO: Verify mode
//         if(id_fd == -1) { 
//             perror("ERROR! Exception occurred while opening file.\n"); 
//             exit(EXIT_FAILURE); 
//         }
        
//         int id = 1;
//         ssize_t r = read(id_fd, &id, sizeof(int));
//         if(r == 0) {
//             ssize_t w = write(id_fd, &id, sizeof(int));
//             if(w == 0) {
//                 perror("ERROR! Exception occurred while writing to file.\n");
//                 exit(EXIT_FAILURE);
//             }
//         }

//         unlink(SERVER_FIFO);
//         int server_fifo = mkfifo(SERVER_FIFO, 0600);
//         if(server_fifo == -1) {
//             perror("ERROR! Exception occurred while creating server fifo.\n");
//             exit(EXIT_FAILURE);
//         }

//         while(1) {
//             int server_fd = open(SERVER_FIFO, O_RDONLY, 0600);
//             if(server_fd == -1) {
//                 perror("ERROR! Exception occurred while opening server fifo.\n");
//                 exit(EXIT_FAILURE);
//             }

//             REQUEST request;
//             while(read(server_fd, &request, sizeof(REQUEST)) != 0) {
//                 char cl_fifo_path[20];
//                 snprintf(cl_fifo_path, 20, CLIENT_FIFO "%d", request.id);
                
//                 id++;
//                 RESPONSE response;
//                 response.id = id;
//                 response.version = 'o';
//                 printf("[DEBUG] Task with id %d started.\n", id);

//                 int client_fd = open(cl_fifo_path, O_WRONLY, 0600);
//                 if(client_fd == -1) {
//                     perror("ERROR! server opening client fd\n");
//                     exit(EXIT_FAILURE);
//                 }
//                 ssize_t w = write(client_fd, &response, sizeof(RESPONSE));
//                 if(w == 0) {
//                     perror("ERROR! Exception occurred while writing response to client fifo.\n");
//                     exit(EXIT_FAILURE);
//                 }
//                 close(client_fd);

//                 char cl_task_path[20];  // TODO: Maybe change value?
//                 snprintf(cl_task_path, 20, TASK "%d", id);
//                 int client_task_fd = open(cl_task_path, O_WRONLY | O_CREAT, 0600);
//                 if(client_task_fd == -1) {
//                     perror("ERROR! Exception occurred while opening client task file.\n");
//                     exit(EXIT_FAILURE);
//                 }

//                 // Client task write here.

//                 close(client_task_fd);
//             }

//             lseek(id_fd, 0, SEEK_SET);
//             ssize_t w = write(id_fd, &id, sizeof(int));
//             if(w == 0) {
//                 perror("ERROR! Exception occurred while writing id to id_file.\n");
//                 exit(EXIT_FAILURE);
//             }

//             close(server_fd); 
             
//             if(sv_data_path != NULL) {
//                 free(sv_data_path); 
//                 sv_data_path = NULL;
//             }

//             if(id_path != NULL) {
//                 free(id_path); 
//                 id_path = NULL;
//             }
//         }      
//         close(id_fd);
//     }

//     return 0;
// }