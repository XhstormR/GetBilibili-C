#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <dirent.h>

#include "fio.h"

#define LEN sizeof(DataNode)

inline int isExist(const char *path) {
    return !_access_s(path, F_OK);
}

inline int cd(const char *path) {
    return !_chdir(path);
}

inline int md(const char *path) {
    if (!isExist(path)) {
        return _mkdir(path) ? 0 : 1;
    }
    return 1;
}

static int callback(const char *fpath, const struct stat *sb, int ftype, struct FTW *ftwbuf) {
    if (ftype == FTW_DP) {
        _rmdir(fpath);
    } else {
        remove(fpath);
    }
    return 0;
}

inline int rd(const char *path) {
    return !nftw(path, callback, 20, FTW_DEPTH | FTW_PHYS);
}

inline size_t fsize(const char *path) {
    struct _stat64 st;
    if (_stat64(path, &st) != 0) return 0;

    return (size_t) st.st_size;
}

char *readFile(const char *path) {
    FILE *file;
    if (fopen_s(&file, path, "rb") != 0) return NULL;

    size_t size = fsize(path);

    char *data = malloc(size + 1);

    size = fread(data, 1, size, file);

    data[size] = 0;

    fclose(file);

    return data;
}

DataNode *listFile(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;

    DataNode *head, *last, *p;
    head = last = p = NULL;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        p = malloc(LEN);
        p->data = strdup(entry->d_name);
        if (head == NULL) {
            head = p;
        } else {
            last->next = p;
        }
        last = p;
    }
    if (head != NULL) last->next = NULL; // 防御链表为空的情况下，操作 last

    closedir(dir);

    return head;
}
