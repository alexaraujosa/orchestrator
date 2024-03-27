#include <stdio.h>
#include <stdlib.h>
#include "client/test.h"
#include "common/test_util.h"

int main(int argc, char const *argv[]) {
    printf("Hello world from client!\n");
    test();
    common_test_util();

    if(argc < 2) {
        printf("Insufficient arguments. Try again later.\n");
        exit(EXIT_FAILURE);
    } else {
        // ...
    }


    return 0;
}
