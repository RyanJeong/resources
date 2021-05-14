#pragma once

#define GET_BLOCK_COUNT_CMD     "{\"jsonrpc\":\"1.0\", \"method\":\"getblockcount\"}"
#define GET_BEST_BLOCK_HASH_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getbestblockhash\"}"
#define GET_BLOCKCHAIN_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getblockchaininfo\"}"
#define GET_mem_POOL_INFO_CMD   "{\"jsonrpc\":\"1.0\", \"method\":\"getMempoolInfo\"}"
#define GET_MINING_INFO_CMD     "{\"jsonrpc\":\"1.0\", \"method\":\"getmininginfo\"}"
#define GET_NETWORK_INFO_CMD    "{\"jsonrpc\":\"1.0\", \"method\":\"getnetworkinfo\"}"
#define HEADER                  "content-type: text/plain;"
#define URL                     "http://106.250.161.211:8332"
#define USERPWD                 "msjeong:msol"
#define NOT_CONNECTED_TO_SERVER TEXT("�����κ��� �����͸� �о�� �� �����ϴ�.")
#define OUT_OF_MEMORY           TEXT("�޸𸮰� �����մϴ�.")
#define BTC_GET_BLOCK_HASH          "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[%d]}"
#define BTC_GET_BLOCK               "{\"jsonrpc\":\"1.0\", \"method\":\"getblock\", \"params\":[\"%s\"]}"
#define BTC_GET_BLOCK_HASH          "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[%d]}"
#define BTC_GET_BLOCK               "{\"jsonrpc\":\"1.0\", \"method\":\"getblock\", \"params\":[\"%s\"]}"
#define BTC_GET_RAW_TRANSACTION         "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[\"%s\", false]}"
#define BTC_GET_RAW_TRANSACTION_JSON    "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[\"%s\", true]}"

 int         process(char *);                                        //  cURL ó����
 size_t      WritememoryCallback(void *, size_t, size_t, void *);    //  ���� ����� ������ �޸� ���� �Ҵ� �Լ�
