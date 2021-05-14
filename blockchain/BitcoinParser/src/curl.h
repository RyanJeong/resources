#ifndef __CURL_H__
#define __CURL_H__

#define HEADER                          "content-type: text/plain;"
#define URL                             "http://106.250.161.211:8332"
#define USERPWD                         "msjeong:msol"
#define GET_BLOCK_COUNT_CMD             "{\"jsonrpc\":\"1.0\", \"method\":\"getblockcount\"}"
#define GET_BEST_BLOCK_HASH_CMD         "{\"jsonrpc\":\"1.0\", \"method\":\"getbestblockhash\"}"
#define GET_BLOCKCHAIN_INFO_CMD         "{\"jsonrpc\":\"1.0\", \"method\":\"getblockchaininfo\"}"
#define GET_mem_POOL_INFO_CMD           "{\"jsonrpc\":\"1.0\", \"method\":\"getMempoolInfo\"}"
#define GET_MINING_INFO_CMD             "{\"jsonrpc\":\"1.0\", \"method\":\"getmininginfo\"}"
#define GET_NETWORK_INFO_CMD            "{\"jsonrpc\":\"1.0\", \"method\":\"getnetworkinfo\"}"
#define BTC_GET_RAW_TRANSACTION         "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[\"%s\", false]}"
#define BTC_GET_RAW_TRANSACTION_JSON    "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[\"%s\", true]}"

#define __CURL__                        "[curl.c]"
#define CURL_INITCURLRESPONSEDATA       "[initCurlResponseData(CURL_RED_DATA *)] "
#define CURL_WRITEMEMORYCALLBACK        "[writeMemoryCallback(void *, size_t, size_t, CURL_RES_DATA *)] "
#define CURL_PROCESS                    "[*process(char *)] "
#define CURL_INITCURL                   "[initCurl(void)] "
#define CURL_CLEANUPCURL                "[cleanUpCurl(void)] "

#define TRIAL                           10

typedef struct curlResponseData {
    char    *response;
    size_t  size;
} CURL_RES_DATA;

typedef struct curl_slist CONTENT_HEADER;

/*
 * void initCurlResponseData (CURL_RES_DATA *)
 * : a function that initialize the structure CURL_RES_DATA
 */
void initCurlResponseData (CURL_RES_DATA *);

/*
 * size_t writeMemoryCallback(void *, size_t, size_t, CURL_RES_DATA *)
 * : a call-back function that using on cURL process routine
 * : 해당 call-back 함수는 필요에 의해 몇 번이고 호출될 수 있음
 */
size_t writeMemoryCallback(void *, size_t, size_t, CURL_RES_DATA *);

/*
 * char *process(char *);
 * : process curl 
 */
char *process(char *);

/*
 * size_t initCurl(void);
 * : init. curl before use it
 */
size_t initCurl(void);

/*
 * void cleanUpCurl(void);
 * : clean up curl after all process is done
 */
void cleanUpCurl(void);

#endif
