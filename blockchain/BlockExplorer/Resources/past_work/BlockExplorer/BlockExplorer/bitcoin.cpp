#include "common.h"

const char *HEADER = "content-type: text/plain;";
const char *DATA = "{\"jsonrpc\":\"1.0\", \"id\":\"MSOL\", \"method\":\"getblockhash\", \"params\":[1000]}";
const char *URL = "http://106.250.161.211:8332";
const char *USERPWD = "msjeong:msol";

char     *mem;
int      memSize;
extern LPCWSTR  Text;

int getBlockHash()
{
    CURL                *handle;
    struct curl_slist   *headers;
    CURLcode            result;

    handle = curl_easy_init();
    headers = NULL;
    if (handle) {
        headers = curl_slist_append(headers, HEADER);
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)strlen(DATA));
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, DATA);
        curl_easy_setopt(handle, CURLOPT_USERPWD, USERPWD);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        result = curl_easy_perform(handle);
        if (result != CURLE_OK) {
            /*  fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));    */

            return 1;
        } else {
            Text = (LPCWSTR) mem;
            free(mem);
        }
        curl_easy_cleanup(handle);
    }

    return 0;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmem, void *userp)
{
    int realSize;

    realSize = size * nmem;
    mem = (char *)realloc(mem, memSize + realSize + 1);
    if (mem == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");

        return 0;
    }
    memcpy(&(mem[memSize]), contents, realSize);
    memSize += realSize;
    mem[memSize] = '\0';

    return realSize;
}

