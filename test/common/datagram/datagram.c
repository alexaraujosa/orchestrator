#define GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "test/test.h"
#include "test/common/datagram/datagram.h"
#include "common/datagram/datagram.h"
#include "common/datagram/status.h"
#include "common/datagram/execute.h"
#include "common/util/string.h"
#include "common/io/io.h"

#define CONTROL_STATUS_STR_EE "StatusRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_NONE, type: DATAGRAM_TYPE_NONE, pid: 3423508 }, data: '7B:00:00:00:00:00:00:07' }"

#define CONTROL_STATUS_STR_NEE "StatusRequestDatagram{ header: DatagramHeader{ version: 2, mode: 0, type: 0, pid: 3423508 }, data: '7B:00:00:00:00:00:00:07' }"

#define CONTROL_EXECUTE_STR_NEE_NSP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: 2, type: 1, pid: 305419896 }, data: '4C:6F:72:65:6D:20:69:70:73:75:6D:20:64:6F:6C:6F:72:20:73:69:74:20:61:6D:65:74:2C:20:63:6F:6E:73:65:63:74:65:74:75:72:20:61:64:69:70:69:73:63:69:6E:67:20:65:6C:69:74:2E:20:4D:6F:72:62:69:20:6C:6F:62:6F:72:74:69:73:2C:20:65:6E:69:6D:20:65:75:20:66:72:69:6E:67:69:6C:6C:61:20:65:6C:65:6D:65:6E:74:75:6D:2C:20:6C:65:6F:20:65:72:61:74:20:62:69:62:65:6E:64:75:6D:20:6E:75:6C:6C:61:2C:20:61:74:20:65:66:66:69:63:69:74:75:72:20:6C:6F:72:65:6D:20:64:69:61:6D:20:65:67:65:74:20:6E:69:73:69:2E:20:50:72:6F:69:6E:20:65:75:69:73:6D:6F:64:2C:20:75:72:6E:61:20:61:20:63:75:72:73:75:73:20:73:65:6D:70:65:72:2C:20:66:65:6C:69:73:20:65:6C:69:74:20:73:6F:6C:6C:69:63:69:74:75:64:69:6E:20:70:75:72:75:73:2C:20:69:6E:20:6C:6F:62:6F:72:74:69:73:20:64:6F:6C:6F:72:20:6C:65:6F:20:61:20:65:73:74:2E:20:50:72:61:65:73:65:6E:74:20:61:6C:69:71:75:61:6D:20:6C:61:63:75:73:20:6E:65:63:20:6D:61:73:73:61:20:6C:61:6F:72:65:65:74:00' }"
#define CONTROL_EXECUTE_STR_NEE_SP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: 2, type: 1, pid: 305419896 }, data: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi lobortis, enim eu fringilla elementum, leo erat bibendum nulla, at efficitur lorem diam eget nisi. Proin euismod, urna a cursus semper, felis elit sollicitudin purus, in lobortis dolor leo a est. Praesent aliquam lacus nec massa laoreet' }"
#define CONTROL_EXECUTE_STR_EE_NSP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_EXECUTE, type: DATAGRAM_TYPE_UNIQUE, pid: 305419896 }, data: '4C:6F:72:65:6D:20:69:70:73:75:6D:20:64:6F:6C:6F:72:20:73:69:74:20:61:6D:65:74:2C:20:63:6F:6E:73:65:63:74:65:74:75:72:20:61:64:69:70:69:73:63:69:6E:67:20:65:6C:69:74:2E:20:4D:6F:72:62:69:20:6C:6F:62:6F:72:74:69:73:2C:20:65:6E:69:6D:20:65:75:20:66:72:69:6E:67:69:6C:6C:61:20:65:6C:65:6D:65:6E:74:75:6D:2C:20:6C:65:6F:20:65:72:61:74:20:62:69:62:65:6E:64:75:6D:20:6E:75:6C:6C:61:2C:20:61:74:20:65:66:66:69:63:69:74:75:72:20:6C:6F:72:65:6D:20:64:69:61:6D:20:65:67:65:74:20:6E:69:73:69:2E:20:50:72:6F:69:6E:20:65:75:69:73:6D:6F:64:2C:20:75:72:6E:61:20:61:20:63:75:72:73:75:73:20:73:65:6D:70:65:72:2C:20:66:65:6C:69:73:20:65:6C:69:74:20:73:6F:6C:6C:69:63:69:74:75:64:69:6E:20:70:75:72:75:73:2C:20:69:6E:20:6C:6F:62:6F:72:74:69:73:20:64:6F:6C:6F:72:20:6C:65:6F:20:61:20:65:73:74:2E:20:50:72:61:65:73:65:6E:74:20:61:6C:69:71:75:61:6D:20:6C:61:63:75:73:20:6E:65:63:20:6D:61:73:73:61:20:6C:61:6F:72:65:65:74:00' }"
#define CONTROL_EXECUTE_STR_EE_SP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_EXECUTE, type: DATAGRAM_TYPE_UNIQUE, pid: 305419896 }, data: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi lobortis, enim eu fringilla elementum, leo erat bibendum nulla, at efficitur lorem diam eget nisi. Proin euismod, urna a cursus semper, felis elit sollicitudin purus, in lobortis dolor leo a est. Praesent aliquam lacus nec massa laoreet' }"

void test_datagram(char* test_data_dir) {
    char* error;
    int line;

    #pragma region ======= STATUS DATAGRAM =======
    char* test_status_file = join_paths(2, test_data_dir, "test_status.dat");
    int status_fd = SAFE_OPEN(test_status_file, O_RDONLY, NULL);
    if (status_fd == -1) ERROR("Unable to open status test data file.")

    // Read status in a single function call
    {
        StatusRequestDatagram status_all = read_status_request_datagram(status_fd);
        char* status_all_str_ee = status_request_datagram_to_string(status_all, 1);
        char* status_all_str_nee = status_request_datagram_to_string(status_all, 0);

        ASSERT(
            STRING_EQUAL(
                status_all_str_ee, 
                CONTROL_STATUS_STR_EE
            ),
            "[TOSTRING_EE] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                status_all_str_nee, 
                CONTROL_STATUS_STR_NEE
            ),
            "[TOSTRING_NEE] Status datagram read from test data does not match control."
        )

        free(status_all_str_ee);
        free(status_all_str_nee);
    }

    lseek(status_fd, 0, SEEK_SET);

    // Read status in a partial call
    {
        DATAGRAM_HEADER header = read_datagram_header(status_fd);
        StatusRequestDatagram status_partial = read_partial_status_request_datagram(status_fd, header);
        char* status_partial_str_ee = status_request_datagram_to_string(status_partial, 1);
        char* status_partial_str_nee = status_request_datagram_to_string(status_partial, 0);
        ASSERT(
            STRING_EQUAL(
                status_partial_str_ee, 
                CONTROL_STATUS_STR_EE
            ),
            "[TOSTRING_EE] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                status_partial_str_nee, 
                CONTROL_STATUS_STR_NEE
            ),
            "[TOSTRING_NEE] Status datagram read from test data does not match control."
        )
    }

    close(status_fd);
    #pragma endregion

    #pragma region ======= EXECUTE DATAGRAM =======
    char* test_execute_file = join_paths(2, test_data_dir, "test_execute.dat");
    int execute_fd = SAFE_OPEN(test_execute_file, O_RDONLY, NULL);
    if (execute_fd == -1) ERROR("Unable to open execute test data file.")

    // Read execute in a single function call
    {
        ExecuteRequestDatagram execute_all = read_execute_request_datagram(execute_fd);
        char* execute_all_str_nee_nsp = execute_request_datagram_to_string(execute_all, 0, 0);
        char* execute_all_str_nee_sp =  execute_request_datagram_to_string(execute_all, 0, 1);
        char* execute_all_str_ee_nsp =  execute_request_datagram_to_string(execute_all, 1, 0);
        char* execute_all_str_ee_sp =   execute_request_datagram_to_string(execute_all, 1, 1);

        ASSERT(
            STRING_EQUAL(
                execute_all_str_nee_nsp, 
                CONTROL_EXECUTE_STR_NEE_NSP
            ),
            "[TOSTRING_NEE_NSP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_all_str_nee_sp, 
                CONTROL_EXECUTE_STR_NEE_SP
            ),
            "[TOSTRING_NEE_SP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_all_str_ee_nsp, 
                CONTROL_EXECUTE_STR_EE_NSP
            ),
            "[TOSTRING_EE_NSP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_all_str_ee_sp, 
                CONTROL_EXECUTE_STR_EE_SP
            ),
            "[TOSTRING_EE_SP] Status datagram read from test data does not match control."
        )

        free(execute_all_str_nee_nsp);
        free(execute_all_str_nee_sp);
        free(execute_all_str_ee_nsp);
        free(execute_all_str_ee_sp);
    }

    lseek(execute_fd, 0, SEEK_SET);

    // Read execute in a partial call
    {
        DATAGRAM_HEADER header = read_datagram_header(execute_fd);
        ExecuteRequestDatagram execute_partial = read_partial_execute_request_datagram(execute_fd, header);
        char* execute_partial_str_nee_nsp = execute_request_datagram_to_string(execute_partial, 0, 0);
        char* execute_partial_str_nee_sp =  execute_request_datagram_to_string(execute_partial, 0, 1);
        char* execute_partial_str_ee_nsp =  execute_request_datagram_to_string(execute_partial, 1, 0);
        char* execute_partial_str_ee_sp =   execute_request_datagram_to_string(execute_partial, 1, 1);

        ASSERT(
            STRING_EQUAL(
                execute_partial_str_nee_nsp, 
                CONTROL_EXECUTE_STR_NEE_NSP
            ),
            "[TOSTRING_NEE_NSP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_partial_str_nee_sp, 
                CONTROL_EXECUTE_STR_NEE_SP
            ),
            "[TOSTRING_NEE_SP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_partial_str_ee_nsp, 
                CONTROL_EXECUTE_STR_EE_NSP
            ),
            "[TOSTRING_EE_NSP] Status datagram read from test data does not match control."
        )

        ASSERT(
            STRING_EQUAL(
                execute_partial_str_ee_sp, 
                CONTROL_EXECUTE_STR_EE_SP
            ),
            "[TOSTRING_EE_SP] Status datagram read from test data does not match control."
        )

        free(execute_partial_str_nee_nsp);
        free(execute_partial_str_nee_sp);
        free(execute_partial_str_ee_nsp);
        free(execute_partial_str_ee_sp);
    }

    close(execute_fd);
    #pragma endregion

    free(test_execute_file);
    free(test_status_file);
    return;

    err:
    printf("Test failed - %s:%d\n - %s\n", __FILE__, line, error);
    exit(EXIT_FAILURE);
}