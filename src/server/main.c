#include <stdio.h>
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
        char* server_data_path = join_paths(3, get_cwd(), "build", "server_data");
        CREATE_DIR(server_data_path, 0777); //TODO: Verify mode

        // ID SETUP
        char* id_path = join_paths(2, server_data_path, "id");
        int fd = open(id_path, O_RDWR | O_CREAT, 0777); //TODO: Verify mode
        if(fd == -1) { 
            perror("ERROR! Exception occurred while opening file.\n"); 
            exit(EXIT_FAILURE); 
        }
        
        int id = 1;
        ssize_t r = read(fd, &id, sizeof(int));
        if(r == 0) {
            ssize_t w = write(fd, &id, sizeof(int));
            if(w == 0) {
                perror("ERROR! Exception occurred while writing to file.\n");
                exit(EXIT_FAILURE);
            }
        }
        
        
        close(fd);
        free(server_data_path);
        free(id_path);
    }

    return 0;
}