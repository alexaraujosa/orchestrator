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


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    printf("Hello world from client!\n");

    if(argc < 2) {
        printf("Insufficient arguments. Try again later.\n");
        exit(EXIT_FAILURE);
    } else {
        // ...
    }

    return 0;
}

