#include "nio.h"

#define REFERER "https://www.bilibili.com/"
#define USERAGENT "Windows"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *) userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

CURLcode urlGetToMemory(const char *url, const char *cookie, MemoryStruct *chunk) {
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_COOKIE, cookie);
    curl_easy_setopt(curl_handle, CURLOPT_REFERER, REFERER);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USERAGENT);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, chunk);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);

    CURLcode res = curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return res;
}

CURLcode urlGetToFile(const char *url, const char *path) {
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USERAGENT);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);

    CURLcode res = CURLE_FAILED_INIT;
    FILE *file;
    if (!fopen_s(&file, path, "wb")) {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file);
        res = curl_easy_perform(curl_handle);
        fclose(file);
    } else {
        perror(path);
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return res;
}
