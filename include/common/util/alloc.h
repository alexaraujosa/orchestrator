#ifndef COMMON_UTIL_ALLOC_H
#define COMMON_UTIL_ALLOC_H

#include <stdlib.h>

#define STRUCT_MEMBER_SIZE(type, member) (sizeof( ((type *)0)->member ))

#define SAFE_ALLOC(type, size) (type)({\
    type a = (type)malloc(size);\
    memset(a, 0, size);\
    if (a == NULL) return ERR;\
    (type)(a);\
})

#define SAFE_ALLOC_DEFAULT(type, size, fill) (type)({\
    type a = (type)malloc(size);\
    memset(a, fill, size);\
    if (a == NULL) return ERR;\
    (type)(a);\
})

#define SAFE_REALLOC(ptr, size) {\
    void* a = realloc(ptr, size);\
    if (a == NULL) return ERR;\
    ptr = a;\
}

#endif