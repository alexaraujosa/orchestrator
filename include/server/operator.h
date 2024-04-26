/******************************************************************************
 *                          HISTORY FILE STRUCTS                              *
 *                                                                            *
 ******************************************************************************/

#ifndef SERVER_OPERATOR_H
#define SERVER_OPERATOR_H

#include <stdint.h>
#include <sys/types.h>
#include <common/util/alloc.h>

#define HISTORY_VERSION 1


#pragma region ======= HISTORY =======
//NOTE: NÃO SEI SE PERCEBI BEM, MAS A IDEIA ENTÃO SERIA TER FUNÇÕES RELACIONADAS AO HISTORY (FILE INTEIRO) NESTA REGION
// e ter a zona de baixo para coisas relacionadas com a entry em especifico


/**
 * @brief Checks whether the history has entrys which are supported on the current version.
 * @param history_version A uint16_t, which represents the history file version
 */
#define IS_HISTORY_SUPPORTED(history_version) (history_version == HISTORY_VERSION)

#pragma endregion

#pragma region ======= HISTORY_ENTRY =======
typedef struct task_history_entry{
    uint32_t taskid;
    short int tempo;
    char data[300];
} TASK_HISTORY_ENTRY, *Task_history_entry;

/**
 * @brief Creates a new empty entry for the history file.
 * 
 */
Task_history_entry create_task_history_entry();

/**
 * @brief Reads a history entry from a file descriptor, or NULL if it fails.
 * @param fd The file descriptor to read.
 */
Task_history_entry read_task_history_entry(int fd);

/**
 * @brief Returns a string representation for a history entry.
 * 
 * @param header A pointer to a TASK_HISTORY_ENTRY structure containing a history entry.
 */
char* task_history_entry_to_string(Task_history_entry history_entry);

#pragma endregion

#endif