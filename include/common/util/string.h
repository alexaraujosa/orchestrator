#ifndef COMMON_UTIL_STRING_H
#define COMMON_UTIL_STRING_H

/**
 * @brief Format the parameters to a string.
 * 
 * @param format String format.
 * @param ... Values to use.
 * 
 * @return Pointer to the formated string.
*/
char* isnprintf(const char *format, ...);

/**
 * @brief Formats a sequence of raw bytes into a string of octets separated by a separator.
 */
char* bytes_to_hex_string(char* bytes, int len, char separator);

#endif