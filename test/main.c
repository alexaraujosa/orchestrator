#include <stdio.h>
#include "common/util/string.h"
#include "common/io/io.h"
#include "test/common/datagram/datagram.h"

#define TEST_DATA_DIR "test_data"

int IS_DEBUG = 0;

int main(int argc, char const *argv[]) {
    // Setup
    char* cwd = get_cwd();
    char* test_data_dir = join_paths(2, cwd, TEST_DATA_DIR);

    if (argc > 1 && STRING_EQUAL(argv[1], "-d")) IS_DEBUG = 1;

    // Test runners
    test_datagram(test_data_dir);

    // Cleanup
    free(test_data_dir);

    printf("All tests passed!\n");
    
    return 0;
}
