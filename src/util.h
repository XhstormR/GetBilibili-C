#ifndef GETBILIBILI_C_UTIL_H
#define GETBILIBILI_C_UTIL_H

#include <stddef.h>

char *vstrcat(const char *first, ...);

char *trim(const char *head);

void removeCharAt(int index, char *str);

void printFormatSize(size_t size);

void printFormatTime(size_t time);

char *EMPTYTONULL(char *str);

#endif //GETBILIBILI_C_UTIL_H
