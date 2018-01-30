#include "exec.h"
#include "main.h"
#include "util.h"
#include "fio.h"
#include "nio.h"

typedef enum {
    ExtractService,
    DownloadService,
    MergeService
} Service;

typedef struct {
    char *link;
    char *name;
    char *args;
} Executor;

static const Executor executors[] = {
        {
                "http://blog.xhstormr.tk/uploads/bin/7zr.exe",
                "7zr.exe",
                "7zr.exe"
                        " x"
                        " -y"
                        " -bso0"
                        " -bse0"
                        " -bsp0"
                        " data.bin"
        },
        {
                "http://ww4.sinaimg.cn/large/a15b4afegw1f7vk9216gvj203k03kb29",
                "aria2c.exe",
                "aria2c.exe"
                        " --input-file=1.txt"
                        " --disk-cache=32M"
                        " --enable-mmap=true"
                        " --max-mmap-limit=2048M"
                        " --continue=true"
                        " --max-concurrent-downloads=1"
                        " --max-connection-per-server=10"
                        " --min-split-size=4M"
                        " --split=10"
                        " --disable-ipv6=true"
                        " --http-no-cache=true"
                        " --check-certificate=false"
                        " --user-agent=Windows"
                        " --referer=https://www.bilibili.com/"
                        " --dir="
        },
        {
                "http://ww4.sinaimg.cn/large/a15b4afegw1f7vhtyv0wbj203k03k4qz",
                "ffmpeg.exe",
                "ffmpeg.exe"
                        " -f"
                        " concat"
                        " -safe"
                        " -1"
                        " -i"
                        " 2.txt"
                        " -c"
                        " copy"
                        " -y "
        }
};

extern GetBilibiliConf conf;

static void checkService(Service service);

void download() {
    cd(conf.bin_dir);
    checkService(DownloadService);
    system(vstrcat(executors[DownloadService].args, conf.download_dir, NULL));
    remove("1.txt");
}

void merge() {
    cd(conf.bin_dir);
    checkService(MergeService);
    system(vstrcat(executors[MergeService].args, conf.app_dir, "\\Video.mp4", NULL));
    remove("2.txt");
}

static void extract() {
    cd(conf.bin_dir);
    checkService(ExtractService);
    system(executors[ExtractService].args);
    remove("data.bin");
}

static void checkService(Service service) {
    Executor executor = executors[service];

    if (isExist(executor.name)) return;

    switch (service) {
        case ExtractService:
            urlGetToFile(executor.link, executor.name);
            return;
        default:
            urlGetToFile(executor.link, "data.bin");
            extract();
            return;
    }
}
