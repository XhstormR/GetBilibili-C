#ifndef GETBILIBILI_C_PARSE_H
#define GETBILIBILI_C_PARSE_H

#include "main.h"

int parseJSON(const char *value, GetBilibiliInfo *info);

void parseArgs(int argc, char *argv[], GetBilibiliConf *conf);

#endif //GETBILIBILI_C_PARSE_H
