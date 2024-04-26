/******************************************************************************
 *                             OPERATOR PROCESS                               *
 *                                                                            *
 *   The operator process is responsible for controlling and managing the     *
 * various worker processes used to parallelise the workload of the server.   *
 *   It implements an event system through the usage of anonymous pipes used  *
 * to establish a communication channel between the various worker processes  *
 * and the operator process.                                                  *
 *   Only one operator process is supposed to be used per server instance.    *
 ******************************************************************************/

#include "server/operator.h"

Task_history_entry create_task_history_entry(){
    #define ERR NULL
    Task_history_entry entry = SAFE_ALLOC(Task_history_entry, sizeof(TASK_HISTORY_ENTRY));
    
    return entry;
    #undef ERR
}

Task_history_entry read_task_history_entry(int fd){
    return NULL;
}

char* task_history_entry_to_string(Task_history_entry history_entry){
    return "lul";
}