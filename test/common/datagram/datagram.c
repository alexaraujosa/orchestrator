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

#pragma region ======= CONSTANTS AND CONFIG =======
#define STATUS_REQUEST_FILE "test_status_request.dat"
#define EXECUTE_REQUEST_FILE "test_execute_request.dat"
#define STATUS_RESPONSE_FILE "test_status_response.dat"
#define EXECUTE_RESPONSE_FILE "test_execute_response.dat"

#define MOCK_PID 123456

#define CONTROL_DATAGRAM_HEADER_STR_NEE "DatagramHeader{ version: 2, mode: 0, type: 0, pid: " STR(MOCK_PID) " }"
#define CONTROL_DATAGRAM_HEADER_STR_EE "DatagramHeader{ version: 2, mode: DATAGRAM_MODE_NONE, type: DATAGRAM_TYPE_NONE, pid: " STR(MOCK_PID) " }"

#define CONTROL_STATUS_REQUEST_STR_EE "StatusRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_STATUS_REQUEST, type: DATAGRAM_TYPE_NONE, pid: " STR(MOCK_PID) " } }"
#define CONTROL_STATUS_REQUEST_STR_NEE "StatusRequestDatagram{ header: DatagramHeader{ version: 2, mode: 1, type: 0, pid: " STR(MOCK_PID) " } }"

#define CONTROL_EXECUTE_REQUEST_STR_NEE_NSP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: 2, type: 1, pid: " STR(MOCK_PID) " }, time: 69, data: '4C:6F:72:65:6D:20:69:70:73:75:6D:20:64:6F:6C:6F:72:20:73:69:74:20:61:6D:65:74:2C:20:63:6F:6E:73:65:63:74:65:74:75:72:20:61:64:69:70:69:73:63:69:6E:67:20:65:6C:69:74:2E:20:4D:6F:72:62:69:20:6C:6F:62:6F:72:74:69:73:2C:20:65:6E:69:6D:20:65:75:20:66:72:69:6E:67:69:6C:6C:61:20:65:6C:65:6D:65:6E:74:75:6D:2C:20:6C:65:6F:20:65:72:61:74:20:62:69:62:65:6E:64:75:6D:20:6E:75:6C:6C:61:2C:20:61:74:20:65:66:66:69:63:69:74:75:72:20:6C:6F:72:65:6D:20:64:69:61:6D:20:65:67:65:74:20:6E:69:73:69:2E:20:50:72:6F:69:6E:20:65:75:69:73:6D:6F:64:2C:20:75:72:6E:61:20:61:20:63:75:72:73:75:73:20:73:65:6D:70:65:72:2C:20:66:65:6C:69:73:20:65:6C:69:74:20:73:6F:6C:6C:69:63:69:74:75:64:69:6E:20:70:75:72:75:73:2C:20:69:6E:20:6C:6F:62:6F:72:74:69:73:20:64:6F:6C:6F:72:20:6C:65:6F:20:61:20:65:73:74:2E:20:50:72:61:65:73:65:6E:74:20:61:6C:69:71:75:61:6D:20:6C:61:63:75:73:20:6E:65:63:20:6D:61:73:73:61:20:6C:61:6F:72:65:65:74:00' }"
#define CONTROL_EXECUTE_REQUEST_STR_NEE_SP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: 2, type: 1, pid: " STR(MOCK_PID) " }, time: 69, data: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi lobortis, enim eu fringilla elementum, leo erat bibendum nulla, at efficitur lorem diam eget nisi. Proin euismod, urna a cursus semper, felis elit sollicitudin purus, in lobortis dolor leo a est. Praesent aliquam lacus nec massa laoreet' }"
#define CONTROL_EXECUTE_REQUEST_STR_EE_NSP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_EXECUTE_REQUEST, type: DATAGRAM_TYPE_UNIQUE, pid: " STR(MOCK_PID) " }, time: 69, data: '4C:6F:72:65:6D:20:69:70:73:75:6D:20:64:6F:6C:6F:72:20:73:69:74:20:61:6D:65:74:2C:20:63:6F:6E:73:65:63:74:65:74:75:72:20:61:64:69:70:69:73:63:69:6E:67:20:65:6C:69:74:2E:20:4D:6F:72:62:69:20:6C:6F:62:6F:72:74:69:73:2C:20:65:6E:69:6D:20:65:75:20:66:72:69:6E:67:69:6C:6C:61:20:65:6C:65:6D:65:6E:74:75:6D:2C:20:6C:65:6F:20:65:72:61:74:20:62:69:62:65:6E:64:75:6D:20:6E:75:6C:6C:61:2C:20:61:74:20:65:66:66:69:63:69:74:75:72:20:6C:6F:72:65:6D:20:64:69:61:6D:20:65:67:65:74:20:6E:69:73:69:2E:20:50:72:6F:69:6E:20:65:75:69:73:6D:6F:64:2C:20:75:72:6E:61:20:61:20:63:75:72:73:75:73:20:73:65:6D:70:65:72:2C:20:66:65:6C:69:73:20:65:6C:69:74:20:73:6F:6C:6C:69:63:69:74:75:64:69:6E:20:70:75:72:75:73:2C:20:69:6E:20:6C:6F:62:6F:72:74:69:73:20:64:6F:6C:6F:72:20:6C:65:6F:20:61:20:65:73:74:2E:20:50:72:61:65:73:65:6E:74:20:61:6C:69:71:75:61:6D:20:6C:61:63:75:73:20:6E:65:63:20:6D:61:73:73:61:20:6C:61:6F:72:65:65:74:00' }"
#define CONTROL_EXECUTE_REQUEST_STR_EE_SP "ExecuteRequestDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_EXECUTE_REQUEST, type: DATAGRAM_TYPE_UNIQUE, pid: " STR(MOCK_PID) " }, time: 69, data: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi lobortis, enim eu fringilla elementum, leo erat bibendum nulla, at efficitur lorem diam eget nisi. Proin euismod, urna a cursus semper, felis elit sollicitudin purus, in lobortis dolor leo a est. Praesent aliquam lacus nec massa laoreet' }"

#define CONTROL_STATUS_RESPONSE_STR_NEE_NSP "StatusResponseDatagram{ header: DatagramHeader{ version: 2, mode: 3, type: 0, pid: " STR(MOCK_PID) " }, payload_len: 13, data: '48:65:6C:6C:6F:20:77:6F:72:6C:64:21:00' }"
#define CONTROL_STATUS_RESPONSE_STR_NEE_SP "StatusResponseDatagram{ header: DatagramHeader{ version: 2, mode: 3, type: 0, pid: " STR(MOCK_PID) " }, payload_len: 13, data: 'Hello world!' }"
#define CONTROL_STATUS_RESPONSE_STR_EE_NSP "StatusResponseDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_STATUS_RESPONSE, type: DATAGRAM_TYPE_NONE, pid: " STR(MOCK_PID) " }, payload_len: 13, data: '48:65:6C:6C:6F:20:77:6F:72:6C:64:21:00' }"
#define CONTROL_STATUS_RESPONSE_STR_EE_SP "StatusResponseDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_STATUS_RESPONSE, type: DATAGRAM_TYPE_NONE, pid: " STR(MOCK_PID) " }, payload_len: 13, data: 'Hello world!' }"

#define CONTROL_EXECUTE_RESPONSE_STR_NEE "ExecuteResponseDatagram{ header: DatagramHeader{ version: 2, mode: 4, type: 0, pid: " STR(MOCK_PID) " }, taskid: 123 }"
#define CONTROL_EXECUTE_RESPONSE_STR_EE "ExecuteResponseDatagram{ header: DatagramHeader{ version: 2, mode: DATAGRAM_MODE_EXECUTE_RESPONSE, type: DATAGRAM_TYPE_NONE, pid: " STR(MOCK_PID) " }, taskid: 123 }"
#pragma endregion

void test_status_request_datagram(StatusRequestDatagram dg) {
    ERROR_HEADER

    char* status_str_ee = status_request_datagram_to_string(dg, 1);
    char* status_str_nee = status_request_datagram_to_string(dg, 0);

    ASSERT(
        STRING_EQUAL(
            status_str_ee, 
            CONTROL_STATUS_REQUEST_STR_EE
        ),
        "[SRD] [TOSTRING_EE] Status datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            status_str_nee, 
            CONTROL_STATUS_REQUEST_STR_NEE
        ),
        "[SRD] [TOSTRING_NEE] Status datagram read from test data does not match control."
    )

    free(status_str_ee);
    free(status_str_nee);
    return;

    TEST_ERROR_LABEL
}

void test_execute_request_datagram(ExecuteRequestDatagram dg) {
    ERROR_HEADER

    char* execute_str_nee_nsp = execute_request_datagram_to_string(dg, 0, 0);
    char* execute_str_nee_sp =  execute_request_datagram_to_string(dg, 0, 1);
    char* execute_str_ee_nsp =  execute_request_datagram_to_string(dg, 1, 0);
    char* execute_str_ee_sp =   execute_request_datagram_to_string(dg, 1, 1);

    ASSERT(
        STRING_EQUAL(
            execute_str_nee_nsp, 
            CONTROL_EXECUTE_REQUEST_STR_NEE_NSP
        ),
        "[ERD] [TOSTRING_NEE_NSP] Execute datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            execute_str_nee_sp, 
            CONTROL_EXECUTE_REQUEST_STR_NEE_SP
        ),
        "[ERD] [TOSTRING_NEE_SP] Execute datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            execute_str_ee_nsp, 
            CONTROL_EXECUTE_REQUEST_STR_EE_NSP
        ),
        "[ERD] [TOSTRING_EE_NSP] Execute datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            execute_str_ee_sp, 
            CONTROL_EXECUTE_REQUEST_STR_EE_SP
        ),
        "[ERD] [TOSTRING_EE_SP] Execute datagram read from test data does not match control."
    )

    free(execute_str_nee_nsp);
    free(execute_str_nee_sp);
    free(execute_str_ee_nsp);
    free(execute_str_ee_sp);

    return;
    TEST_ERROR_LABEL
}

void test_status_response_datagram(StatusResponseDatagram dg) {
    ERROR_HEADER

    char* status_str_nee_nsp = status_response_datagram_to_string(dg, 0, 0);
    char* status_str_nee_sp =  status_response_datagram_to_string(dg, 0, 1);
    char* status_str_ee_nsp =  status_response_datagram_to_string(dg, 1, 0);
    char* status_str_ee_sp =   status_response_datagram_to_string(dg, 1, 1);

    ASSERT(
        STRING_EQUAL(
            status_str_nee_nsp, 
            CONTROL_STATUS_RESPONSE_STR_NEE_NSP
        ),
        "[TOSTRING_NEE_NSP] Status response datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            status_str_nee_sp, 
            CONTROL_STATUS_RESPONSE_STR_NEE_SP
        ),
        "[TOSTRING_NEE_SP] Status response datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            status_str_ee_nsp, 
            CONTROL_STATUS_RESPONSE_STR_EE_NSP
        ),
        "[TOSTRING_EE_NSP] Status response datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            status_str_ee_sp, 
            CONTROL_STATUS_RESPONSE_STR_EE_SP
        ),
        "[TOSTRING_EE_SP] Status response datagram read from test data does not match control."
    )

    free(status_str_nee_nsp);
    free(status_str_nee_sp);
    free(status_str_ee_nsp);
    free(status_str_ee_sp);

    return;
    TEST_ERROR_LABEL
}

void test_execute_response_datagram(ExecuteResponseDatagram dg) {
    ERROR_HEADER

    char* execute_str_ee = execute_response_datagram_to_string(dg, 1);
    char* execute_str_nee = execute_response_datagram_to_string(dg, 0);

    ASSERT(
        STRING_EQUAL(
            execute_str_ee, 
            CONTROL_EXECUTE_RESPONSE_STR_EE
        ),
        "[TOSTRING_EE] Execute datagram read from test data does not match control."
    )

    ASSERT(
        STRING_EQUAL(
            execute_str_nee, 
            CONTROL_EXECUTE_RESPONSE_STR_NEE
        ),
        "[TOSTRING_NEE] Execute datagram read from test data does not match control."
    )

    free(execute_str_ee);
    free(execute_str_nee);

    return;
    TEST_ERROR_LABEL
}


// void test_template(StatusRequestDatagram dg) {
//     ERROR_HEADER
// 
//     return;
//     TEST_ERROR_LABEL
// }

void test_datagram(char* test_data_dir) {
    ERROR_HEADER

    #pragma region ============== CREATE DATAGRAMS ==============
        #pragma region ======= DATAGRAM HEADER =======
        {
            DATAGRAM_HEADER dh = create_datagram_header();
            dh.pid = MOCK_PID;

            char* dh_str_nee = datagram_header_to_string(&dh, 0);
            char* dh_str_ee = datagram_header_to_string(&dh, 1);

            ASSERT(
                STRING_EQUAL(
                    dh_str_nee, 
                    CONTROL_DATAGRAM_HEADER_STR_NEE
                ),
                "[ALL] [TOSTRING_NEE] Datagram header created does not match control."
            )

            ASSERT(
                STRING_EQUAL(
                    dh_str_ee, 
                    CONTROL_DATAGRAM_HEADER_STR_EE
                ),
                "[ALL] [TOSTRING_EE] Datagram header created does not match control."
            )

            free(dh_str_ee);
            free(dh_str_nee);
        }
        #pragma endregion

        // ======= STATUS REQUEST DATAGRAM =======
        StatusRequestDatagram status_req = create_status_request_datagram();
        status_req->header.pid = MOCK_PID;
        test_status_request_datagram(status_req);

        // ======= STATUS RESPONSE DATAGRAM =======
        uint8_t status_res_payload[] = "Hello world!";
        StatusResponseDatagram status_res = create_status_response_datagram(status_res_payload, 13);
        status_res->header.pid = MOCK_PID;
        test_status_response_datagram(status_res);

        // ======= EXECUTE RESPONSE DATAGRAM =======
        ExecuteRequestDatagram execute_req = create_execute_request_datagram();
        execute_req->header.pid = MOCK_PID;
        execute_req->header.type = DATAGRAM_TYPE_UNIQUE;
        execute_req->time = 0x45;
        memcpy(execute_req->data, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi lobortis, enim eu fringilla elementum, leo erat bibendum nulla, at efficitur lorem diam eget nisi. Proin euismod, urna a cursus semper, felis elit sollicitudin purus, in lobortis dolor leo a est. Praesent aliquam lacus nec massa laoreet", 300);
        test_execute_request_datagram(execute_req);

        // ======= EXECUTE RESPONSE DATAGRAM =======
        ExecuteResponseDatagram execute_res = create_execute_response_datagram();
        execute_res->header.pid = MOCK_PID;
        execute_res->taskid = 123;
        test_execute_response_datagram(execute_res);
    #pragma endregion

    #pragma region ============== REQUESTS ==============
    {
    #pragma region ======= STATUS DATAGRAM =======
    char* test_status_file = join_paths(2, test_data_dir, STATUS_REQUEST_FILE);
    int status_fd = SAFE_OPEN(test_status_file, O_RDONLY, NULL);
    if (status_fd == -1) ERROR("Unable to open status request test data file.")

    // Read status in a single function call
    {
        StatusRequestDatagram status_all = read_status_request_datagram(status_fd);
        status_all->header.pid = MOCK_PID;
        
        test_status_request_datagram(status_all);
        free(status_all);
    }

    lseek(status_fd, 0, SEEK_SET);

    // Read status in a partial call
    {
        DATAGRAM_HEADER header = read_datagram_header(status_fd);
        StatusRequestDatagram status_partial = read_partial_status_request_datagram(status_fd, header);
        status_partial->header.pid = MOCK_PID;

        test_status_request_datagram(status_partial);
        free(status_partial);
    }

    close(status_fd);
    #pragma endregion

    #pragma region ======= EXECUTE DATAGRAM =======
    char* test_execute_file = join_paths(2, test_data_dir, EXECUTE_REQUEST_FILE);
    int execute_fd = SAFE_OPEN(test_execute_file, O_RDONLY, NULL);
    if (execute_fd == -1) ERROR("Unable to open execute request test data file.")

    // Read execute in a single function call
    {
        ExecuteRequestDatagram execute_all = read_execute_request_datagram(execute_fd);
        execute_all->header.pid = MOCK_PID;

        test_execute_request_datagram(execute_all);
        free(execute_all);
    }

    lseek(execute_fd, 0, SEEK_SET);

    // Read execute in a partial call
    {
        DATAGRAM_HEADER header = read_datagram_header(execute_fd);
        ExecuteRequestDatagram execute_partial = read_partial_execute_request_datagram(execute_fd, header);
        execute_partial->header.pid = MOCK_PID;

        test_execute_request_datagram(execute_partial);
        free(execute_partial);
    }

    close(execute_fd);
    #pragma endregion

    free(test_execute_file);
    free(test_status_file);
    }
    #pragma endregion

    #pragma region ============== RESPONSES ==============
    {
        #pragma region ======= STATUS DATAGRAM =======
        char* test_status_file = join_paths(2, test_data_dir, STATUS_RESPONSE_FILE);
        int status_fd = SAFE_OPEN(test_status_file, O_RDONLY, NULL);
        if (status_fd == -1) ERROR("Unable to open status response test data file.")

        // Read execute in a single function call
        {
            StatusResponseDatagram status_all = read_status_response_datagram(status_fd);
            status_all->header.pid = MOCK_PID;

            test_status_response_datagram(status_all);
            free(status_all);
        }

        lseek(status_fd, 0, SEEK_SET);

        // Read status in a partial call
        {
            DATAGRAM_HEADER header = read_datagram_header(status_fd);
            StatusResponseDatagram status_partial = read_partial_status_response_datagram(status_fd, header);
            status_partial->header.pid = MOCK_PID;

            test_status_response_datagram(status_partial);
            free(status_partial);
        }

        close(status_fd);
        #pragma endregion

        #pragma region ======= EXECUTE DATAGRAM =======
        char* test_execute_file = join_paths(2, test_data_dir, EXECUTE_RESPONSE_FILE);
        int execute_fd = SAFE_OPEN(test_execute_file, O_RDONLY, NULL);
        if (execute_fd == -1) ERROR("Unable to open execute response test data file.")

        // Read execute in a single function call
        {
            ExecuteResponseDatagram execute_all = read_execute_response_datagram(execute_fd);
            execute_all->header.pid = MOCK_PID;

            test_execute_response_datagram(execute_all);
            free(execute_all);
        }

        lseek(execute_fd, 0, SEEK_SET);

        // Read execute in a partial call
        {
            DATAGRAM_HEADER header = read_datagram_header(execute_fd);
            ExecuteResponseDatagram execute_partial = read_partial_execute_response_datagram(execute_fd, header);
            execute_partial->header.pid = MOCK_PID;

            test_execute_response_datagram(execute_partial);
            free(execute_partial);
        }

        close(execute_fd);
        #pragma endregion

        free(test_status_file);
        free(test_execute_file);
    }
    #pragma endregion
    return;

    TEST_ERROR_LABEL
}