#ifndef GETBILIBILI_C_FIO_H
#define GETBILIBILI_C_FIO_H

#include "list.h"

int isExist(const char *path);

int cd(const char *path);

int md(const char *path);

int rd(const char *path);

size_t fsize(const char *path);

char *readFile(const char *path);

DataNode *listFile(const char *path);

#endif //GETBILIBILI_C_FIO_H
