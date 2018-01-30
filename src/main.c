#include <direct.h>
#include <locale.h>

#include "main.h"
#include "fio.h"
#include "nio.h"
#include "util.h"
#include "exec.h"
#include "parse.h"

GetBilibiliInfo info = {};
GetBilibiliConf conf = {};

char buf[PATH_MAX];

void init() {
    setlocale(LC_ALL, "");

    if (conf.download_cookie_file != NULL) conf.download_cookie = readFile(conf.download_cookie_file);

    if (conf.app_dir == NULL) conf.app_dir = _getcwd(NULL, 0);

    conf.download_dir = _fullpath(NULL, "GetBilibili", 0);

    cd(getenv("APPDATA"));

    conf.bin_dir = _fullpath(NULL, "GetBilibili", 0);

    if (conf.download_url == NULL) return;

    conf.download_url = trim(conf.download_url);
    if (conf.download_url[4] == 's') removeCharAt(4, conf.download_url);
}

void generateLink() {
    MemoryStruct chunk = {};
    CURLcode res = urlGetToMemory(conf.download_url, conf.download_cookie, &chunk);

    if (res != CURLE_OK) {
        fprintf(stderr, "%s\n", curl_easy_strerror(res));
        exit(1);
    }

    if (parseJSON(chunk.memory, &info) == 0) {
        fprintf(stderr, "%ls\n", L"链接解析失败");
        exit(1);
    }

    free(chunk.memory);
}

void createDirectory() {
    if (!md(conf.app_dir)) {
        perror(conf.app_dir);
    }
    if (!md(conf.bin_dir)) {
        perror(conf.bin_dir);
    }
    if (!md(conf.download_dir)) {
        perror(conf.download_dir);
    }
}

void showLink() {
    putchar('\n');
    printFormatSize(info.size);
    printFormatTime(info.time);
    putchar('\n');
    printList(info.list);
    freeList(info.list);
}

void saveLink() {
    cd(conf.bin_dir);
    writeList(info.list, "1.txt");
    freeList(info.list);
}

void saveFile() {
    cd(conf.bin_dir);
    FILE *file;

    if (fopen_s(&file, "2.txt", "wb")) {
        perror("");
        exit(1);
    }

    cd(conf.download_dir);
    DataNode *p, *files = listFile(".");

    if (countList(files) == 0) {
        fprintf(stderr, "%ls\n", L"没有文件可供合并");
        exit(1);
    }

    sortList(files);

    ListForEach(p, files) {
        fprintf(file, "file '%s'\n", _fullpath(buf, p->data, PATH_MAX));
    }

    freeList(files);
    fclose(file);
}

void help() {
    printf("%ls", L""
            "\n"
            "Usage: GetBilibili-C <command> [<switches>...]\n"
            "\n"
            "<Commands>\n"
            "  -l <url>            解析链接\n"
            "  -d <url>            下载并合并\n"
            "  -m                  只进行合并\n"
            "\n"
            "<Switches>\n"
            "  --dir <path>        设置下载文件夹\n"
            "  --cookie <file>     导入 cookie 文件\n"
            "  --delete            任务完成后自动删除分段视频\n"
    );

    if (!getenv("PROMPT")) system("pause");
}

int main(int argc, char *argv[]) {
    parseArgs(argc, argv, &conf);
    init();

    if (conf.flag_list) {
        generateLink();
        showLink();
        puts("\nDone!");
    } else if (conf.flag_download) {
        generateLink();
        createDirectory();
        saveLink();
        download();
        saveFile();
        merge();
        puts("\nDone!");
    } else if (conf.flag_merge) {
        createDirectory();
        saveFile();
        merge();
        puts("\nDone!");
    } else {
        help();
        exit(0);
    }

    if (conf.flag_delete) rd(conf.download_dir);

    free(conf.app_dir);
    free(conf.bin_dir);
    free(conf.download_dir);
    free(conf.download_url);
    free(conf.download_cookie);

    return 0;
}
