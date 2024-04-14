#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "common/io.h"
#include "common/test_util.h"

int main(int argc, char const *argv[]) {
    printf("Hello world from server!\n");
    common_test_util();

    if(argc < 4) {
        printf("Insufficient arguments. Try again later.\n");
        exit(EXIT_FAILURE);
    } else {
        // SERVER PERSISTENCE DATA SETUP
        char* sv_data_path = join_paths(3, get_cwd(), "build", "server_data");
        CREATE_DIR(sv_data_path, 0777); //TODO: Verify mode

        // ID SETUP
        char* id_path = join_paths(2, sv_data_path, "id");
        int id_fd = open(id_path, O_RDWR | O_CREAT, 0777); //TODO: Verify mode
        if(id_fd == -1) { 
            perror("ERROR! Exception occurred while opening file.\n"); 
            exit(EXIT_FAILURE); 
        }
        
        int id = 1;
        ssize_t r = read(id_fd, &id, sizeof(int));
        if(r == 0) {
            ssize_t w = write(id_fd, &id, sizeof(int));
            if(w == 0) {
                perror("ERROR! Exception occurred while writing to file.\n");
                exit(EXIT_FAILURE);
            }
        }

        unlink(SERVER_FIFO);
        int server_fifo = mkfifo(SERVER_FIFO, 0600);
        if(server_fifo == -1) {
            perror("ERROR! Exception occurred while creating server fifo.\n");
            exit(EXIT_FAILURE);
        }

        while(1) {
            int server_fd = open(SERVER_FIFO, O_RDONLY, 0600);
            if(server_fd == -1) {
                perror("ERROR! Exception occurred while opening server fifo.\n");
                exit(EXIT_FAILURE);
            }

            REQUEST request;
            while(read(server_fd, &request, sizeof(REQUEST)) != 0) {
                char cl_fifo_path[20];
                snprintf(cl_fifo_path, 20, CLIENT_FIFO "%d", request.id);
                
                id++;
                RESPONSE response;
                response.id = id;
                response.version = 'o';
                printf("[DEBUG] Task with id %d started.\n", id);

                int client_fd = open(cl_fifo_path, O_WRONLY, 0600);
                if(client_fd == -1) {
                    perror("ERROR! server opening client fd\n");
                    exit(EXIT_FAILURE);
                }
                ssize_t w = write(client_fd, &response, sizeof(RESPONSE));
                if(w == 0) {
                    perror("ERROR! Exception occurred while writing response to client fifo.\n");
                    exit(EXIT_FAILURE);
                }
                close(client_fd);

                char cl_task_path[20];  // TODO: Maybe change value?
                snprintf(cl_task_path, 20, TASK "%d", id);
                int client_task_fd = open(cl_task_path, O_WRONLY | O_CREAT, 0600);
                if(client_task_fd == -1) {
                    perror("ERROR! Exception occurred while opening client task file.\n");
                    exit(EXIT_FAILURE);
                }

                // Client task write here.

                close(client_task_fd);
            }

            lseek(id_fd, 0, SEEK_SET);
            ssize_t w = write(id_fd, &id, sizeof(int));
            if(w == 0) {
                perror("ERROR! Exception occurred while writing id to id_file.\n");
                exit(EXIT_FAILURE);
            }

            close(server_fd); 
             
            if(sv_data_path != NULL) {
                free(sv_data_path); 
                sv_data_path = NULL;
            }

            if(id_path != NULL) {
                free(id_path); 
                id_path = NULL;
            }
        }      
        close(id_fd);
    }

    return 0;
}