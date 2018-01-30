#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"

char *vstrcat(const char *first, ...) {
    char *retbuf;
    char *p;
    va_list argp;

    size_t len = strlen(first);

    va_start(argp, first);
    while ((p = va_arg(argp, char *)) != NULL) {
        len += strlen(p);
    }
    va_end(argp); // 扫描结束

    retbuf = malloc(len + 1); // +1 终止符 \0
    strcpy(retbuf, first);

    va_start(argp, first); // 重新扫描
    while ((p = va_arg(argp, char *)) != NULL) {
        strcat(retbuf, p);
    }
    va_end(argp);

    return retbuf;
}

char *trim(const char *head) {
    char *out;
    size_t len;

    while (isspace(*head)) head++;

    if (*head == 0) return NULL; // 空白字符串

    const char *tail = head + strlen(head) - 1;

    while (isspace(*tail)) tail--;

    len = tail - head + 1;
    out = malloc(len);
    memcpy(out, head, len);
    out[len] = 0;

    return out;
}

void removeCharAt(int index, char *str) {
    memmove(&str[index], &str[index + 1], strlen(str) - index);
}

void printFormatSize(size_t size) {
    int divisor;
    char *unit;

    if (size > (divisor = 1024 * 1024 * 1024)) {
        unit = "GB";
    } else if (size > (divisor /= 1024)) {
        unit = "MB";
    } else if (size > (divisor /= 1024)) {
        unit = "KB";
    } else {
        divisor /= 1024;
        unit = "B";
    }

    printf("Size: %.2lf %s (%I64d bytes)\n", size * 1.0 / divisor, unit, size);
}

void printFormatTime(size_t time) {
    size_t s = time / 1000;
    size_t m = s / 60;
    size_t h = m / 60;

    printf("Time: %I64d:%02I64d:%02I64d\n", h, m % 60, s % 60);
}
