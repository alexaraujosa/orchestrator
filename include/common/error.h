/******************************************************************************
 *                               ERROR UTILITY                                *
 *                                                                            *
 *   The Error Utility module contains multiple macros and definitions for    *
 * error-related purposed, such as assertions, error handling and stadardised *
 * error messages.                                                            *
 *                                                                            *
 * Within this module is also defined the Critical Mark macro system, that    *
 * allows the creation of a special variable that marks a critical part of    *
 * the execution flow, which allows for the modification of the behavior of   *
 * a code snippet depending on the current state of the Critical Mark.        *
 *   To enable the Critical Mark system on a given file, define _CRITICAL at  *
 * the top of the file, before any imports.                                   *
 ******************************************************************************/

#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

#pragma region ======= CONFIG =======
/**
 * @brief A header used on all error messages, for consistency.
 */
#define ERROR_STR_HEADER "[" __FILE__ ":" STR(__LINE__) "] "

/**
 * @brief The header used in all functions that may throw errors.
 */
#define ERROR_HEADER char* __error; int __line;

/**
 * @brief The name for the mark used to signal a critical part where errors might be expected.
 */
#define CRITICAL_MARK __critical_mark
#pragma endregion ======= CONFIG =======

/**
 * @brief Initializes a critical mark with a specific type.
 * 
 * @param type The datatype to use for the critical mark.
 */
#define INIT_CRITICAL_MARK_T(type) type CRITICAL_MARK;

/**
 * @brief Initializes a critical mark with the smallest data type.
 */
#define INIT_CRITICAL_MARK INIT_CRITICAL_MARK_T(unsigned char)

/**
 * @brief Sets the value of a critical mark.
 * 
 * @param val A valid value for the datatype used to initialize the critical mark.
 * 
 * @warning This macro requires the critical mark to be initialized beforehand. See INIT_CRITICAL_MARK.
 */
#define SET_CRITICAL_MARK(val) CRITICAL_MARK = val

/**
 * @brief Executes a section if the critical mark is active.
 * 
 * @param expr An expression or code block.
 * 
 * @warning This macro requires the critical mark to be initialized beforehand. See INIT_CRITICAL_MARK.
 */
#define CRITICAL_MARK_SECTION_ACTIVE(expr) if (CRITICAL_MARK) { expr; }

/**
 * @brief Executes a section if the critical mark is inactive.
 * 
 * @param expr An expression or code block.
 * 
 * @warning This macro requires the critical mark to be initialized beforehand. See INIT_CRITICAL_MARK.
 */
#define CRITICAL_MARK_SECTION_INACTIVE(expr) if (CRITICAL_MARK) { expr; }


#ifndef _CRITICAL
/**
 * @brief Prints an error if the critical mark is inactive.
 * 
 * @param msg The message to be printed.
 * 
 * @note By not defining _CRITICAL before the import of this macro, it will ignore the critical mark and error our normally.
 */
#define CRITICAL_MARK_ERROR(msg) perror(msg)
#else
/**
 * @brief Prints an error if the critical mark is inactive.
 * 
 * @param msg The message to be printed.
 * 
 * @note By not defining _CRITICAL before the import of this macro, it will ignore the critical mark and error our normally.
 */
#define CRITICAL_MARK_ERROR(msg) CRITICAL_MARK_SECTION_INACTIVE(perror(msg))
#endif

/**
 * @brief Throws an error using a specific label.
 * 
 * @param msg A string containing the reason for the failure.
 * 
 * @warning In order for this macro to be used, ERROR_HEADER must be defined at the head of the function.
 * And a goto label named "end" must be defined at the end of the function that is responsible for handling the error.
 * A default implementation is available as DEFAULT_ERROR_LABEL.
 */
#define ERRORL(msg, label) {\
    __line = __LINE__;\
    __error = msg;\
    goto label;\
}

/**
 * @brief Throws an error using the default label 'err'.
 * 
 * @param msg A string containing the reason for the failure.
 * 
 * @warning In order for this macro to be used, ERROR_HEADER must be defined at the head of the function.
 * And a goto label named "end" must be defined at the end of the function that is responsible for handling the error.
 * A default implementation is available as DEFAULT_ERROR_LABEL.
 */
#define ERROR(msg) ERRORL(msg, err)

/**
 * @brief Asserts if a condition is true, or fails.
 * 
 * @param cond A condition that results in a boolean value.
 * @param msg The reason for the failure, if it happens.
 */
#define ASSERT(cond, msg) if (!(cond)) ERROR(msg)

#define DEFAULT_ERROR_LABEL err:\
    printf("[%s:%d] %s\n", __FILE__, __line, __error);\

#endif