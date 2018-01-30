#ifndef GETBILIBILI_C_NIO_H
#define GETBILIBILI_C_NIO_H

#include <curl/curl.h>

typedef struct {
    char *memory;
    size_t size;
} MemoryStruct;

CURLcode urlGetToMemory(const char *url, const char *cookie, MemoryStruct *chunk);

CURLcode urlGetToFile(const char *url, const char *path);

#endif //GETBILIBILI_C_NIO_H
