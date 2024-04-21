#include "common/io/io.h"

char* get_cwd() {
    static char BIN_PATH[PATH_MAX];

    if(getcwd(BIN_PATH, sizeof(BIN_PATH)) == NULL) {
        perror("ERROR! Exception occurred while getting CWD.\n");
        exit(EXIT_FAILURE);
    }
    
    return BIN_PATH; 
}

char* join_paths(int len, ...) {
    va_list args;
    va_start(args, len);

    char* parts[len];
    int totalLen = len;
    for (int i = 0; i < len; i++) {
        char* part = va_arg(args, char*);
        parts[i] = part;
        totalLen += strlen(part);
    }

    // Some fuckery to shut GCC (https://gcc.gnu.org/bugzilla//show_bug.cgi?id=85783)
    size_t totalLenBits = totalLen * sizeof(char); 
    if (totalLenBits >= PTRDIFF_MAX) return NULL;

    char* fullPath = (char*)malloc(totalLenBits);
    memset(fullPath, 0, totalLenBits);

    int offset = 0;
    for (int i = 0; i < len; i++) {
        int partLen = strlen(parts[i]);

        memcpy(fullPath + offset, parts[i], partLen);
        offset += partLen + 1;
        if (i < len - 1) fullPath[offset - 1] = FS_PATH_SEPARATOR;
    }

    if (fullPath[totalLen - 1] != '\0') fullPath[totalLen - 1] = '\0';

    return fullPath;
}

void drain_fifo(int fd) {
    char buffer[256];
    ssize_t bytes_read;

    do {
        bytes_read = read(fd, buffer, sizeof(buffer));
    } while (bytes_read > 0);
}
