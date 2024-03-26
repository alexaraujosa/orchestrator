#include <stdio.h>
#include "client/test.h"
#include "common/test_util.h"

int main(int argc, char const *argv[]) {
    printf("Hello world from client!\n");
    test();

    common_test_util();

    return 0;
}
