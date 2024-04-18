#ifndef TEST_H
#define TEST_H

/**
 * @brief Fails a test.
 * 
 * @param msg A string containing the reason for the failure.
 * 
 * @warning In order for this macro to be used, the following variables must be defined at the start of the function:
 *     char* error; 
 *     int line;
 * 
 * And a goto label named "end" must be defined at the end of the function that is responsible for handling the error.
 */
#define ERROR(msg) {\
    line = __LINE__;\
    error = msg;\
    goto err;\
}

/**
 * @brief Asserts if a condition is true, or fails.
 * 
 * @param cond A condition that results in a boolean value.
 * @param msg The reason for the failure, if it happens.
 */
#define ASSERT(cond, msg) if (!(cond)) ERROR(msg)

#endif