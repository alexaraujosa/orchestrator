#include "common/util/string.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char* isnprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the length of the formatted string
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (length < 0) return NULL;

    va_start(args, format);

    char *result = (char *)malloc(length + 1);
    if (result == NULL) {
        va_end(args);
        return NULL;
    }

    // Format the string into the allocated memory
    vsnprintf(result, length + 1, format, args);
    va_end(args);

    return result;
}

char* bytes_to_hex_string(char* bytes, int len, char separator) {
    int totalLen = separator ? (3 * len) - 1 : (2 * len);
    char* str = (char*)calloc(totalLen + 1, sizeof(char));

    unsigned char* pin = bytes;
    const char* hex = "0123456789ABCDEF";
    char* pout = str;
    
    for(; pin < bytes + len; pout += 2 + !!separator, pin++){
        pout[0] = hex[(*pin >> 4) & 0xF];
        pout[1] = hex[*pin & 0xF];
        if (separator) pout[2] = ':';
    }

    if (separator) pout[-1] = 0;

    return str;
}
