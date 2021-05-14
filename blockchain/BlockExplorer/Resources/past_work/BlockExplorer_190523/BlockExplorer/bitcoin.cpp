#include "BlockExplorer.h"

/*
 *  구현 함수들
 *      getinfo
 *          : was removed in 0.16.0 version of Bitcoin Core. Use the RPCs listed in the See Also subsection below instead.
 *
 *      대체함수들: 
 *          getblockchaininfo   : provides information about the current state of the block chain.
 *          getmempoolinfo      : returns information about the node’s current transaction memory pool.
 *          getmininginfo       : returns various mining-related information.
 *          getnetworkinfo      : returns information about the node’s connection to the network.
 *          getwalletinfo       : provides information about the wallet.
 *      
 *      getblockhash(param) -> getblock(getblockhash result) -> getrawtransaction -> decoderawtransaction
 *
 *      GetBestBlockHash: returns the header hash of the most recent block on the best block chain.
 *          result: {"result":"0000000000000000001203178dbf8a6b4f609ae65db4a63fd5023442ef8e38c3","error":null,"id":null}
 */

/*
 *  [의문]
 *  1. 콜백함수 지정 방법은?
 *
 *  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 콜백함수명);
 *
 *  2. 콜백함수 작동 방식은?
 *
 *  curl_easy_perform 함수를 호출하면 콜백함수가 호출되기 시작한다.
 *  데이터량에 따라 1~n번 호출될수 있으며
 *  콜백함수 호출이 완료 되어야 curl_easy_perform 함수가 완료된다.
 *
 *  3. 콜백함수 인자 설명은?
 *
 *  size_t 콜백함수명(void *contents, size_t size, size_t nmemb, void *userp);
 *      (1) contents    : 데이터 포인터
 *      (2) size        : 데이터 단일 크기
 *      (3) nmemb       : 데이터 개수
 *      (4) userp       : 사용자 임의 포인터
 *
 *  [결론]
 *  콜백함수를 지정하여 원하는 처리를 한다. (단, 콜백함수는 여러번 호출될수 있다.)
 */

extern LPCWSTR      Text;

const char *HEADER  = "content-type: text/plain;";
const char *URL     = "http://106.250.161.211:8332";
const char *USERPWD = "msjeong:msol";

//const char *DATA = "{\"jsonrpc\":\"1.0\", \"id\":\"MSOL\", \"method\":\"getblockhash\", \"params\":[1000]}";

const char *GET_BLOCK_HASH          = "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[}";
const char *GET_BLOCK               = "{\"jsonrpc\":\"1.0\", \"method\":\"getblock\", \"params\":[}";
const char *GET_RAW_TRANSACTION     = "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[}";
const char *DECODE_RAW_TRANSACETION = "{\"jsonrpc\":\"1.0\", \"method\":\"decoderawtransaction\", \"params\":[}";

char    *mem;
char    hash[65];
int     memSize;
wchar_t *buf;

void process(char *data)
{
    CURL                *handle;
    struct curl_slist   *headers;
    CURLcode            result;
    char                *temp;
    int                 i;
    
    handle  = curl_easy_init();
    headers = NULL;
    if (handle) {
        headers = curl_slist_append(headers, HEADER);
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(handle, CURLOPT_USERPWD, USERPWD);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        result = curl_easy_perform(handle);

        if (result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            Text = TEXT("curl_easy_perform() failed");
        } else {
            if (buf) {
                printf("free buf\n");
                free(buf);
            }
            /* 1 byte(ASCII) -> 2 bytes(UNICODE)    */
            buf = (wchar_t *) calloc (2, (memSize + 1));
            if (buf == NULL) {
                fprintf(stderr, "not enough memory (realloc returned NULL)\n");
                Text = TEXT("memory re-allocation failed");
            } else {
                for (i = 0; i < 64; ++i) {
                    hash[i] = mem[11 + i];
                }
                hash[i] = '\0';
                /*
                temp = mem;
                while (*mem) {
                    if (*mem == ',') {
                        *mem = '\n';
                    }
                    ++mem;
                }
                mem = temp;
                */
                mbstowcs(buf, mem, memSize);
                Text = buf;
                printf("%ls\n%s\n", Text, hash);
            }
        }
        printf("CURL end\n");

        curl_easy_cleanup(handle);
        printf("curl_easy_cleanup end\n");
    }
    memSize = 0;
    free(mem);
    mem = 0;

    return;
}

void getBlockHash(int)
{

    return;
}
void getBlock(char *)
{

    return;
}

void getRawTransaction(char *)
{

    return;
}

void decodeRawTransaction(char *)
{

    return;
}

void getBlockchainInfo(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getblockchaininfo\"}";

    process(data);

    return;
}

void getMemPoolInfo(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getmempoolinfo\"}";

    process(data);

    return;
}

void getMiningInfo(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getmininginfo\"}";

    process(data);

    return;
}

void getNetworkInfo(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getnetworkinfo\"}";

    process(data);

    return;
}

void getWalletInfo(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getwalletinfo\"}";

    process(data);

    return;
}

void getBestBlockHash(void)
{
    char *data = "{\"jsonrpc\":\"1.0\", \"method\":\"getbestblockhash\"}";

    process(data);

    return;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmem, void *userp)
{
    int realSize;

    realSize = (size * nmem);
    mem = (char *) realloc(mem, memSize + realSize + 1);

    if (mem == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");
        Text = TEXT("memory re-allocation failed");

        return (-1);
    }
    memcpy(&(mem[memSize]), contents, realSize);
    memSize += realSize;
    mem[memSize] = '\0';

    return realSize;
}