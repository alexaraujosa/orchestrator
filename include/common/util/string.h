/******************************************************************************
 *                              STRING UTILITY                                *
 *                                                                            *
 *   The String Uility provides methods for comparison and manipulation of    *
 * strings, used throughout the application.                                  *
 ******************************************************************************/

#ifndef COMMON_UTIL_STRING_H
#define COMMON_UTIL_STRING_H

#define ESCAPE_STR(x) #x
#define STR(x) ESCAPE_STR(x)

/**
 * @brief Checks whether two strings are equal.
 * 
 * @param a A string.
 * @param b Another string.
 * 
 * @returns A boolean value that represents whether the strings are equal.
 */
#define STRING_EQUAL(a, b) (strcmp((a), (b)) == 0)

/**
 * @brief Checks whether the first n characters of two strings are equal.
 * 
 * @param a A string.
 * @param b Another string.
 * @param n A number of characters for the prefix.
 * 
 * @returns A boolean value that represents whether the prefixes are equal.
 */
#define STRING_BEGIN_EQUAL(a, b, n) (strncmp((a), (b), (n)) == 0)

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