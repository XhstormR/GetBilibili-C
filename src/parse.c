#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "parse.h"
#include "cjson/cJSON.h"

#define LEN sizeof(DataNode)

int parseJSON(const char *const value, GetBilibiliInfo *const info) {
    cJSON *dom = cJSON_Parse(value);

    cJSON *time = cJSON_GetObjectItemCaseSensitive(dom, "timelength");

    if (time == NULL) return 0;

    info->time = time->valueint;

    DataNode *head, *last, *p;
    head = last = p = NULL;

    const cJSON *durl = NULL;
    const cJSON *durls = cJSON_GetObjectItemCaseSensitive(dom, "durl");

    cJSON_ArrayForEach(durl, durls) {
        info->size += cJSON_GetObjectItemCaseSensitive(durl, "size")->valueint;

        p = malloc(LEN);
        p->data = strdup(cJSON_GetObjectItemCaseSensitive(durl, "url")->valuestring);
        if (head == NULL) {
            head = p;
        } else {
            last->next = p;
        }
        last = p;
    }
    if (head != NULL) last->next = NULL; // 防御链表为空的情况下，操作 last

    info->list = head;

    cJSON_Delete(dom);

    return 1;
}

void parseArgs(int argc, char *argv[], GetBilibiliConf *const conf) {
    struct option options[] = {
            {"l",      1, 0,                  'a'}, //-,--
            {"d",      1, 0,                  'b'},
            {"dir",    1, 0,                  'c'},
            {"cookie", 1, 0,                  'd'},
            {"m",      0, &conf->flag_merge,  1},
            {"delete", 0, &conf->flag_delete, 1},
            {0,        0, 0,                  0}
    };

    int c;
    while ((c = getopt_long_only(argc, argv, "", options, NULL)) != -1) {
        switch (c) {
            case 'a':
                conf->flag_list = 1;
                conf->download_url = optarg;
                break;
            case 'b':
                conf->flag_download = 1;
                conf->download_url = optarg;
                break;
            case 'c':
                conf->app_dir = optarg;
                break;
            case 'd':
                conf->download_cookie_file = optarg;
                break;
            default:
                break;
        }
    }
}
