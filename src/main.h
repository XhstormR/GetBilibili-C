#ifndef GETBILIBILI_C_MAIN_H
#define GETBILIBILI_C_MAIN_H

#include "list.h"

typedef struct {
    size_t time;
    size_t size;
    DataNode *list;
} GetBilibiliInfo;

typedef struct {
    int flag_list;
    int flag_download;
    int flag_merge;

    int flag_delete;

    char *download_url;
    char *download_dir;
    char *download_cookie;
    char *download_cookie_file;

    char *app_dir;
    char *bin_dir;
} GetBilibiliConf;

#endif //GETBILIBILI_C_MAIN_H
