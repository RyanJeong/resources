/*  https://curl.haxx.se/libcurl/c/ */

#include <stdio.h>  //  frpintf, printf
#include <stdlib.h> //  realloc
#include <string.h> //  memcpy
#include <stdint.h> //  size_t
#include <curl/curl.h>

#include "common.h"
#include "curl.h"
#include "util.h"

static size_t flag = DEBUG_OFF;

static CURL             *curl;
static CONTENT_HEADER   *headers;
static CURL_RES_DATA    resData;

size_t initCurl(void)
{
    static char *func   = CURL_INITCURL;

    headers = NULL;
    initCurlResponseData(&resData);
    if (curl = curl_easy_init()) {
        /*  curl_slist_append appends a string to a linked list of strings. */
        headers = curl_slist_append(headers, HEADER);
        if (headers == NULL) {
            putsForDebug(__CURL__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        /*  set our custom set of headers   */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_USERPWD, USERPWD);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData);
    } else {
        putsForDebug(__CURL__, func, MSG_CURL_INIT_ERROR, flag);

        return (-1);
    }

    return 0;
}

void initCurlResponseData(CURL_RES_DATA *res) 
{
    static char *func   = CURL_INITCURLRESPONSEDATA;

    res->size       = 0;
    res->response   = malloc(res->size + 1);
    if (res->response == NULL) {
        putsForDebug(__CURL__, func, MSG_ALLOC_ERROR, flag);

        exit(EXIT_FAILURE);
    } else {
        res->response[0] = '\0';
        putsForDebug(__CURL__, func, MSG_SUCCESS, flag);
    }

    return;
}

char *process(char *cmd)
{
    static char     *func   = CURL_PROCESS;
    size_t          counter;
    int             code;
    char            buf[SIZE_32BYTES];

    resData.size    = 0;    /*  매번 새로운 요청 할 때마다 사이즈를 초기화 해야 함  */
    counter         = 0;
    /*  cURL Processing routine */
    while (curl) {
        /*  set our custom set of headers   */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(cmd));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cmd);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);

        if (!(code = curl_easy_perform(curl))) {
            putsForDebug(__CURL__, func, MSG_SUCCESS, flag);
            break;
        } else {
            putsForDebug(__CURL__, func, MSG_FAILURE, flag);
            ++counter;
            if (counter == TRIAL) {
                sprintf(buf, "Error codes: %d", code);  
                putsForDebug(__CURL__, func, buf, flag);

                exit(EXIT_FAILURE);
            }
        }
    }
    *(resData.response + resData.size) = '\0';  /* data 마지막에 null   */
    /*  end of the routine  */

    return resData.response;
}

size_t writeMemoryCallback(void *ptr, size_t size, size_t nmem, CURL_RES_DATA *res)
{
    static char *func   = CURL_WRITEMEMORYCALLBACK;
    size_t      realSize;

    realSize        = (size * nmem);
    res->response   = realloc(res->response, (res->size + realSize + 1));   // 마지막에 null 입력하기 위한 1 추가
    if (res->response == NULL) {
        putsForDebug(__CURL__, func, MSG_ALLOC_ERROR, flag);

        exit(EXIT_FAILURE);
    } else {
        memcpy(res->response + res->size, ptr, realSize);
        res->size                   += realSize;
        /*
         *  res->response[res->size]    = '\0';
         *  Data 양에 따라 여러 번 호출될 수 있으므로, 마지막에 한 번만 선언해야 함 
         */
        putsForDebug(__CURL__, func, MSG_SUCCESS, flag);
    }

    return realSize;    
}

void cleanUpCurl(void)
{
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(resData.response);

    return;
}
