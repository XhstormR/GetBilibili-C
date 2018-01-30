#ifndef GETBILIBILI_C_LIST_H
#define GETBILIBILI_C_LIST_H

#include <stddef.h>

typedef struct DataNode {
    void *data;
    struct DataNode *next;
} DataNode;

size_t countList(DataNode *head);

void sortList(DataNode *start);

void freeList(DataNode *head);

void printList(DataNode *head);

void writeList(DataNode *head, const char *path);

#define ListForEach(p, list) for ((p) = (list); (p) != NULL; (p) = (p)->next)

#endif //GETBILIBILI_C_LIST_H
