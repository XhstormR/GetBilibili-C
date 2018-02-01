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

#if 1

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

#else

#include "winctrls.h"

enum {
    control_id_start = 100,
    IDC_BOX_INFO,
    IDC_LINKSTATIC, IDC_LINKDISPLAY,
    IDC_BOX_ACTIONS,
    IDC_GENSTATIC, IDC_GENERATE,
    IDC_DOWNLOADSTATIC, IDC_DOWNLOAD,
    IDC_MERGESTATIC, IDC_MERGE,
    IDC_BOX_PARAMS,
    IDC_URLSTATIC, IDC_URLEDIT,
    IDC_DIRSTATIC, IDC_DIREDIT,
    IDC_COOKIESTATIC, IDC_COOKIEEDIT,
    IDC_DELETECHK
};

static int checkURL(HWND hwnd) {
    if (conf.download_url) return 0;

    MessageBox(hwnd, "Please provide a download URL", "GetBilibili-C", MB_ICONINFORMATION | MB_OK);

    return -1;
}

static INT_PTR CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG:;//空语句
            {
                RECT rs, rd;
                HWND hw = GetDesktopWindow();

                if (GetWindowRect(hw, &rs) && GetWindowRect(hwnd, &rd)) {
                    MoveWindow(hwnd,
                               (rs.right + rs.left + rd.left - rd.right) / 2,
                               (rs.bottom + rs.top + rd.top - rd.bottom) / 2,
                               rd.right - rd.left, rd.bottom - rd.top, TRUE);
                }
            }

            {
                struct ctlpos cp;
                ctlposinit(&cp, hwnd, 4, 4, 4);

                beginbox(&cp, "Info", IDC_BOX_INFO);
                bigeditctrl(&cp, "&Segmented video link:", IDC_LINKSTATIC, IDC_LINKDISPLAY, 6);
                endbox(&cp);

                beginbox(&cp, "Actions", IDC_BOX_ACTIONS);
                staticbtn(&cp, "Generate video link", IDC_GENSTATIC, "&Generate", IDC_GENERATE);
                staticbtn(&cp, "Download and merge segmented video", IDC_DOWNLOADSTATIC, "&Download", IDC_DOWNLOAD);
                staticbtn(&cp, "Merge segmented video", IDC_MERGESTATIC, "&Merge", IDC_MERGE);
                endbox(&cp);

                beginbox(&cp, "Parameters", IDC_BOX_PARAMS);
                staticedit(&cp, "Download &url:", IDC_URLSTATIC, IDC_URLEDIT, 75);
                staticedit(&cp, "Download d&ir:", IDC_DIRSTATIC, IDC_DIREDIT, 75);
                staticedit(&cp, "Download &cookie:", IDC_COOKIESTATIC, IDC_COOKIEEDIT, 75);
                cp.ypos += 3;
                checkbox(&cp, "Delete s&egmented video after complete", IDC_DELETECHK);
                endbox(&cp);
            }

            SendMessage(hwnd, WM_SETICON, (WPARAM) ICON_BIG, (LPARAM) LoadIcon(hinst, MAKEINTRESOURCE(200)));
            SendDlgItemMessage(hwnd, IDC_LINKDISPLAY, EM_SETREADONLY, 1, 0);
            EnableWindow(GetDlgItem(hwnd, IDC_GENERATE), 0);

            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_GENERATE:
                    if (HIWORD(wParam) != BN_CLICKED) break;
                    if (checkURL(hwnd) != 0) break;
                    init();
                    generateLink();
                    showLink();
                    break;
                case IDC_DOWNLOAD:
                    if (HIWORD(wParam) != BN_CLICKED) break;
                    if (checkURL(hwnd) != 0) break;
                    init();
                    generateLink();
                    createDirectory();
                    saveLink();
                    download();
                    saveFile();
                    merge();
                    if (conf.flag_delete) rd(conf.download_dir);
                    break;
                case IDC_MERGE:
                    if (HIWORD(wParam) != BN_CLICKED) break;
                    if (checkURL(hwnd) != 0) break;
                    init();
                    createDirectory();
                    saveFile();
                    merge();
                    if (conf.flag_delete) rd(conf.download_dir);
                    break;
                case IDC_DELETECHK:
                    if (HIWORD(wParam) != BN_CLICKED) break;
                    conf.flag_delete = (int) SendDlgItemMessage(hwnd, IDC_DELETECHK, BM_GETCHECK, 0, 0);
                    break;
                case IDC_URLEDIT:
                case IDC_DIREDIT:
                case IDC_COOKIEEDIT:
                    if (HIWORD(wParam) != EN_CHANGE) break;

                    WORD id = LOWORD(wParam);
                    HWND editctl = GetDlgItem(hwnd, id);
                    int len = GetWindowTextLength(editctl) + 1;
                    if (len == 1) break;

                    char **p = NULL;
                    if (id == IDC_URLEDIT) {
                        p = &conf.download_url;
                    } else if (id == IDC_DIREDIT) {
                        p = &conf.download_dir;
                    } else {
                        p = &conf.download_cookie;
                    }

                    *p = realloc(*p, (size_t) len);
                    GetWindowText(editctl, *p, len);
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            free(conf.app_dir);
            free(conf.bin_dir);
            free(conf.download_dir);
            free(conf.download_url);
            free(conf.download_cookie);

            EndDialog(hwnd, IDOK);
            break;
        default:
            break;
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int show) {
    init_common_controls();
    hinst = inst;
    return DialogBox(hinst, MAKEINTRESOURCE(201), NULL, MainDlgProc) != IDOK;
}

/*
 SetDlgItemText(hwnd, IDC_COMMENTEDIT, "123");
 GetDlgItemText(hwnd, IDC_COMMENTEDIT, str, len);

 static int flag;
 ShowWindow(GetDlgItem(hwnd, IDC_BOX_ACTIONS), (flag ? SW_HIDE : SW_SHOW));
 EnableWindow(GetDlgItem(hwnd, IDC_GENERATE), flag);
 EnableWindow(hwnd, flag);
 flag = !flag;

 SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) state); //保存数据
 state = (struct MainDlgState *) GetWindowLongPtr(hwnd, GWLP_USERDATA); //获取数据
 */

#endif
