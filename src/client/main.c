// #include <fcntl.h>
// #include <unistd.h>
// #include "client/test.h"
// #include "common/test_util.h"
// #include "common/io.h"

// int main(int argc, char const *argv[]) {
//     printf("Hello world from client!\n");
//     test();
//     common_test_util();

//     if(argc < 2) {
//         printf("Insufficient arguments. Try again later.\n");
//         exit(EXIT_FAILURE);
//     } else {
//         char* option = (char*)argv[1];

//         if(!strcmp(option, "execute")) {
//             printf("[DEBUG] In execute mode.\n");
            
//             char cl_fifo_path[20];  //TODO: Maybe change value?
//             snprintf(cl_fifo_path, 20, CLIENT_FIFO "%d", getpid());
//             unlink(cl_fifo_path);
//             int cl_fifo = mkfifo(cl_fifo_path, 0600);
//             if(cl_fifo == -1) {
//                 perror("ERROR! Exception occurred while creating client fifo.\n");
//                 exit(EXIT_FAILURE);
//             }

//             int server_fd = open(SERVER_FIFO, O_WRONLY, 0600);
//             if(server_fd == -1) {
//                 perror("ERROR! Exception occurred while opening server fifo.\n");
//                 exit(EXIT_FAILURE);
//             }

//             REQUEST request;
//             request.id = getpid();
//             request.option = EXECUTE;
//             request.speculate_time = atoi(argv[2]);
//             request.type = (strcmp(argv[3], "-p") == 0) ? PIPELINE : UNIQUE;
//             request.version = 'o';
//             strcpy(request.args, (char*)argv[4]);

//             ssize_t w = write(server_fd, &request, sizeof(REQUEST));
//             if(w == 0) {
//                 perror("ERROR! Exception occured while writting request to server fifo.\n");
//                 exit(EXIT_FAILURE);
//             }

//             int client_fd = open(cl_fifo_path, O_RDONLY, 0600);
//             if(client_fd == -1) {
//                 perror("ERROR! Exception occurred while opening client fifo.\n");
//                 exit(EXIT_FAILURE);
//             }

//             RESPONSE response;
//             int r = read(client_fd, &response, sizeof(RESPONSE));
//             if(r == 0) {
//                 perror("ERROR! Exception occurred while reading response datagram from client fifo.\n");
//                 exit(EXIT_FAILURE);
//             }
//             printf("Task started with id: %d.\n", response.id);

//             close(client_fd);
//             close(server_fd);
//         } else {
//             printf("[DEBUG] No mode find.\n");
//         }
//     }

//     return 0;
// }

#include <common/io/io.h>
#include <common/io/fifo.h>
#include <common/datagram/datagram.h>
#include <common/datagram/execute.h>
#include <common/datagram/status.h>
#include "common/util/string.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    #define ERR 1
    printf("Hello world from client!\n");

    if(argc == 2) {
        char* mode = (char*) argv[1];

        if(!strcmp("status", mode)) {

            char* client_fifo_name = isnprintf(CLIENT_FIFO "%d", getpid());
            char* client_fifo_path = join_paths(2, "build/", client_fifo_name);
            SAFE_FIFO_SETUP(client_fifo_path, 0600);

            char* server_fifo_path = join_paths(2, "build/", SERVER_FIFO);
            int server_fifo_fd = SAFE_OPEN(server_fifo_path, O_WRONLY, 0600);

            StatusRequestDatagram request = create_status_request_datagram();
            
            #ifdef DEBUG
            char* req_str = status_request_datagram_to_string(request, 1);
            DEBUG_PRINT("[DEBUG] Sending request with %d bytes:\n%s\n", sizeof(STATUS_REQUEST_DATAGRAM), req_str);
            free(req_str);
            #endif
            
            SAFE_WRITE(server_fifo_fd, request, sizeof(STATUS_REQUEST_DATAGRAM));

            int client_fifo_fd = SAFE_OPEN(client_fifo_path, O_RDONLY, 0600);
            StatusResponseDatagram response = read_status_response_datagram(client_fifo_fd);

            // TODO: Print response datagram payload.

            close(client_fifo_fd);
            unlink(client_fifo_path);
            free(client_fifo_path);
            free(client_fifo_name);
            close(server_fifo_fd);
            free(server_fifo_path);
            
        } else if(!strcmp("close", mode)) {
            
            char* client_fifo_name = isnprintf(CLIENT_FIFO "%d", getpid());
            char* client_fifo_path = join_paths(2, "build/", client_fifo_name);
            SAFE_FIFO_SETUP(client_fifo_path, 0600);

            char* server_fifo_path = join_paths(2, "build/", SERVER_FIFO);
            int server_fifo_fd = SAFE_OPEN(server_fifo_path, O_WRONLY, 0600);

            DATAGRAM_HEADER request = create_datagram_header();
            request.mode = DATAGRAM_MODE_CLOSE_REQUEST;
            SAFE_WRITE(server_fifo_fd, &request, sizeof(DATAGRAM_HEADER));

            int client_fifo_fd = SAFE_OPEN(client_fifo_path, O_RDONLY, 0600);
            DATAGRAM_HEADER response = read_datagram_header(client_fifo_fd);

            if(response.mode == DATAGRAM_MODE_CLOSE_RESPONSE) {
                printf("Server shutdown request sucessfully sent.\n");
            } else {
                printf("Server shutdown request was not received.\n");
            }

            close(client_fifo_fd);
            unlink(client_fifo_path);
            free(client_fifo_path);
            free(client_fifo_name);
            close(server_fifo_fd);
            free(server_fifo_path);

        } else {
            printf("Invalid mode. Try again later.\n");
            exit(EXIT_FAILURE);
        }
    } else if(argc == 5) {
        char* mode = (char*) argv[1];

        if(!strcmp("execute", mode)) {

            char* type = (char*) argv[3];
            char* data = (char*) argv[4];
            if(strlen(data) > 300) {
                perror("ERROR! Arguments passed to execute mode exceeds 300 bytes.\n");
                exit(EXIT_FAILURE);
            }

            char* client_fifo_name = isnprintf(CLIENT_FIFO "%d", getpid());
            char* client_fifo_path = join_paths(2, "build/", client_fifo_name);
            SAFE_FIFO_SETUP(client_fifo_path, 0600);

            char* server_fifo_path = join_paths(2, "build/", SERVER_FIFO);
            int server_fifo_fd = SAFE_OPEN(server_fifo_path, O_WRONLY, 0600);

            ExecuteRequestDatagram request = create_execute_request_datagram();
            request->header.type = (!strcmp("-u", type)) ? DATAGRAM_TYPE_UNIQUE : DATAGRAM_TYPE_PIPELINE;
            memcpy(request->data, data, strlen(data));
            SAFE_WRITE(server_fifo_fd, request, sizeof(EXECUTE_REQUEST_DATAGRAM));

            int client_fifo_fd = SAFE_OPEN(client_fifo_path, O_RDONLY, 0600);
            ExecuteResponseDatagram response = read_execute_response_datagram(client_fifo_fd);

            printf("Task queued with identifier %d.\n", response->taskid);

            close(client_fifo_fd);
            unlink(client_fifo_path);
            free(client_fifo_path);
            free(client_fifo_name);
            close(server_fifo_fd);
            free(server_fifo_path);

        } else {
            printf("Invalid mode. Try again later.\n");
        }
    } else {
        printf("Invalid number of arguments. Try again later.\n");
        exit(EXIT_FAILURE);
    }

    #undef ERR
    return 0;
}

