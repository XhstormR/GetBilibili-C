#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

extern char buf[PATH_MAX];

void freeList(DataNode *head) {
    DataNode *p;
    while ((p = head) != NULL) {
        head = head->next;
        free(p->data);
        free(p);
    }
}

void printList(DataNode *head) {
    DataNode *p = head;
    while (p != NULL) {
        puts(p->data);
        p = p->next;
    }
}

void writeList(DataNode *head, const char *path) {
    DataNode *p = head;
    FILE *file;
    if (!fopen_s(&file, path, "wb")) {
        while (p != NULL) {
            fprintf(file, "%s\n", (char *) p->data);
            p = p->next;
        }
        fclose(file);
    } else {
        perror(path);
    }
}

size_t countList(DataNode *head) {
    DataNode *p = head;
    size_t num = 0;
    for (; p != NULL; p = p->next, num++) {}
    return num;
}

static void swap(DataNode *a, DataNode *b) {
    void *temp = a->data;
    a->data = b->data;
    b->data = temp;
}

static long getNum(const char *str) { // 源格式 28906362-1-80.flv
    strcpy(buf, str);
    strtok(buf, "-");
    char *token = strtok(NULL, "-");
    return strtol(token, NULL, 10);
}

void sortList(DataNode *start) {
    if (start == NULL) return;

    DataNode *head, *tail;
    head = tail = NULL;

    int changed;
    do {
        changed = 0;
        head = start;

        while (head->next != tail) {
            if (getNum(head->data) > getNum(head->next->data)) {
                swap(head, head->next);
                changed = 1;
            }
            head = head->next;
        }
        tail = head;
    } while (changed);
}
