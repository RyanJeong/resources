/*
 *  [�ǹ�]
 *  1. �ݹ��Լ� ���� �����?
 *
 *  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, �ݹ��Լ���);
 *
 *  2. �ݹ��Լ� �۵� �����?
 *
 *  curl_easy_perform �Լ��� ȣ���ϸ� �ݹ��Լ��� ȣ��Ǳ� �����Ѵ�.
 *  �����ͷ��� ���� 1~n�� ȣ��ɼ� ������
 *  �ݹ��Լ� ȣ���� �Ϸ�Ǿ� curl_easy_perform �Լ��� �Ϸ�ȴ�.
 *
 *  3. �ݹ��Լ� ���� ������?
 *
 *  size_t �ݹ��Լ���(void *contents, size_t size, size_t nmemb, void *userp);
 *      (1) contents    : ������ ������
 *      (2) size        : ������ ���� ũ��
 *      (3) nmemb       : ������ ����
 *      (4) userp       : ����� ���� ������
 *
 *  [���]
 *  �ݹ��Լ��� �����Ͽ� ���ϴ� ó���� �Ѵ�. (��, �ݹ��Լ��� ������ ȣ��ɼ� �ִ�.)
 */

#define CURL_STATICLIB
#define HASH_SIZE   65
#define BITS_SIZE   9

#define GET_BEST_BLOCK_HASH_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getbestblockhash\"}"
#define GET_BLOCKCHAIN_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getblockchaininfo\"}"
#define GET_MEM_POOL_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getmempoolinfo\"}"
#define GET_MINING_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getmininginfo\"}"
#define GET_NETWORK_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getnetworkinfo\"}"
#define GET_WALLET_INFO_CMD "{\"jsonrpc\":\"1.0\", \"method\":\"getwalletinfo\"}"

#include <stdlib.h>
#include <curl/curl.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "libcurl.lib")

void getBlock(int);
void getRawTransaction(int);
void getBlockchainInfo();
void getMemPoolInfo();
void getMiningInfo();
void getNetworkInfo();
void getWalletInfo();

size_t WriteMemoryCallback(void *, size_t, size_t, void *);

const char *HEADER  = "content-type: text/plain;";
const char *DATA    = "{\"jsonrpc\":\"1.0\", \"id\":\"MSOL\", \"method\":\"getblockhash\", \"params\":[1000]}";
const char *URL     = "http://106.250.161.211:8332";
const char *USERPWD = "msjeong:msol";

char    *mem;                       //  RPC ���� ����� �����ϴ� �޸�
int     memSize;                    //  RPC ���� ����� ������ ������ ũ��(�޸� �Ҵ� ũ��)
char    buf[1024];                  //  sprintf�� ����ϱ� ���� ����
char    *memTemp;                   //  ������ ���� �� ���� �ּ� ���� ���� 

char    gHash[HASH_SIZE];           //  ���� ��ȸ�� ������ ��� �ش� �ؽ� ��
char    gPrevHash[HASH_SIZE];       //  ���� ��ȸ�� ������ ��� �ش� �ؽ��� ���� ��
char    gNextHash[HASH_SIZE];       //  ���� ��ȸ�� ������ ��� ��� �ؽ��� ���� ��
char    **gTx;                      //  ���� ��ȸ�� ������ ��Ͽ� ���Ե� �ŷ���

int main()
{
    int i, j;
    
    mem = NULL;
    memSize = 0;
    gTx = NULL;
    while (1) {
        scanf("%d", &i);
        getchar();
        switch (i) {
            //getblock -> store txs
            case 1:
                scanf("%d", &j);
                getchar();
                getBlock(j);

                break;
            case 2:
                scanf("%d", &j);
                getchar();
                getRawTransaction(j);
                //
                break;
            case 3:
                getBlockchainInfo();
                break;
            case 4:
                getMemPoolInfo();
                break;
            case 5:
                getMiningInfo();
                break;
            case 6:
                getNetworkInfo();
                break;
            case 7:
                getWalletInfo();
                break;
            default:
                if (gTx != NULL) {
                    if (*gTx != NULL) {
                        free(*gTx);
                    }
                    free(gTx);
                    gTx = NULL;
                }

                return 0;
        }
    }

    return 0;
}

void process(char *data)
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
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(handle, CURLOPT_USERPWD, USERPWD);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        result = curl_easy_perform(handle);

        if (result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        }
        curl_easy_cleanup(handle);
        printf("curl_easy_cleanup end\n");
    }

    return;
}

void getBlock(int param)
{
    /*
     *  temp    : �迭 ������ ������ ���� �ϱ� ���� �޸� ���� ���� ����
     *  i       : �ݺ��� ���� ����
     *  c       : getchar()�� ����
     *  index   : parsing ���� �� tx �κ� ���� ��� ���� 
     *  var     : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, ������
     *  power   : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, �Ǽ���
     *  isFloat : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, ���� ���ڿ��� �Ǽ��� �´°�?
     */
    char    *temp; 
    int     i, j, c, isFloat;
    char    *index;
    long long    var, power;

    /*
     *  {
     *      "hash"              : "hash",       (string) the block hash (same as provided)
     *      "confirmations"     : n,            (numeric) The number of confirmations, or -1 if the block is not on the main chain
     *      "strippedsize"      : n,            (numeric) The block size excluding witness data
     *      "size"              : n,            (numeric) The block size
     *      "weight"            : n             (numeric) The block weight as defined in BIP 141
     *      "height"            : n,            (numeric) The block height or index
     *      "version"           : n,            (numeric) The block version
     *      "versionHex"        : "00000000",   (string) The block version formatted in hexadecimal
     *      "merkleroot"        : "xxxx",       (string) The merkle root
     *      "tx" : [                            (array of string) The transaction ids
     *          "transactionid"                 (string) The transaction id
     *          ,...
     *      ],
     *      "time"              : ttt,          (numeric) The block time in seconds since epoch (Jan 1 1970 GMT)
     *      "mediantime"        : ttt,          (numeric) The median block time in seconds since epoch (Jan 1 1970 GMT)
     *      "nonce"             : n,            (numeric) The nonce
     *      "bits"              : "1d00ffff",   (string) The bits
     *      "difficulty"        : x.xxx,        (numeric) The difficulty
     *      "chainwork"         : "xxxx",       (string) Expected number of hashes required to produce the chain up to this block (in hex)
     *      "nTx"               : "n",          (numeric) The number of transaction
     *      "previousblockhash" : "hash",       (string) The hash of the previous block
     *                                          ******  if 0(genesis block) -> skip this field  ******
     *      "nextblockhash"     : "hash"        (string) The hash of the next block
     *                                          ******  if bestblockhash    -> skip this field  ******
     *  }
     */
    char        _versionHex[BITS_SIZE], _merkleRoot[HASH_SIZE], _bits[BITS_SIZE], _chainWork[HASH_SIZE];
    int         _confirmations, _size, _strippedSize, _weight, _height, _version, _time, _medianTime, _nTx;
    unsigned int    _nonce;
    double      _difficulty;
    
    if (mem != NULL) {
        free(mem);
        memSize = 0;
    }
    if (gTx != NULL) {
        if (*gTx != NULL) {
            free(*gTx);
        }
        free(gTx);
        gTx = NULL;
    }

    /*
     *  param >= 0  -> getblockhash param
     *  else        -> getbestblockhash
     */
    if (param >= 0) {
        sprintf(buf, "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[%d]}", param);
        process(buf);
    } else {
        process(GET_BEST_BLOCK_HASH_CMD);
    }
    memTemp = mem;
    temp = gHash;
    mem += 11;  /*  offset  */
    for (i = 64; i; --i) {
        *temp++ = *mem++;
    }
    mem = memTemp;
    *temp = '\0';
    printf("%s\n", mem);

    free(mem);
    mem = 0;
    memSize = 0;

    /*  getblock header_hash_value  */
    sprintf(buf, "{\"jsonrpc\":\"1.0\", \"method\":\"getblock\", \"params\":[\"%s\"]}", gHash);
    process(buf);
    memTemp = mem;     //store 

    /*  confirmations   */
    mem             += 101; /*  offset  */
    _confirmations  = 0;
    while (*mem != ',') {
        c = *mem++;
        _confirmations = (_confirmations * 10) + (c - '0');
    }
    printf("_confirmations: %d\n", _confirmations);

    /*  strippedsize    */
    mem += 16;  /* strlen(var) + 4  */
    _strippedSize = 0;
    while (*mem != ',') {
        c = *mem++;
        _strippedSize = (_strippedSize * 10) + (c - '0');
    }
    printf("_strippedSize: %d\n", _strippedSize);

    /*  size    */
    mem += 8;  /* strlen(var) + 4  */
    _size = 0;
    while (*mem != ',') {
        c = *mem++;
        _size = (_size * 10) + (c - '0');
    }
    printf("_strippedSize: %d\n", _size);

    /*  weight   */
    mem += 10;  /* strlen(var) + 4  */
    _weight = 0;
    while (*mem != ',') {
        c = *mem++;
        _weight = (_weight * 10) + (c - '0');
    }
    printf("_weight: %d\n", _weight);

    /*  height    */
    mem += 10;  /* strlen(var) + 4  */
    _height = 0;
    while (*mem != ',') {
        c = *mem++;
        _height = (_height * 10) + (c - '0');
    }
    printf("_height: %d\n", _height);

    /*  version    */
    mem += 11;  /* strlen(var) + 4  */
    _version = 0;
    while (*mem != ',') {
        c = *mem++;
        _version = (_version * 10) + (c - '0');
    }
    printf("_version: %d\n", _version);

    /*  versionHex    */
    mem += 15;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = _versionHex;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_versionHex: %s\n", _versionHex);

    /*  merkleroot    */
    mem += 16;  /* strlen(var) + 6("���� ����, ���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = _merkleRoot;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_merkleRoot: %s\n", _merkleRoot);

    /*  tx index ����, �Ŀ� nTx ���� �� �����Ҵ� ���� �������� ���    */
    mem     += 9;
    index   = mem;
    while (*mem++ != ']') {
        ;
    }

    /*  time    */
    mem += 8;  /* strlen(var) + 4  */
    _time = 0;
    while (*mem != ',') {
        c = *mem++;
        _time = (_time * 10) + (c - '0');
    }
    printf("_time: %d\n", _time);

    /*  mediantime    */
    mem += 14;  /* strlen(var) + 4  */
    _medianTime = 0;
    while (*mem != ',') {
        c = *mem++;
        _medianTime = (_medianTime * 10) + (c - '0');
    }
    printf("_medianTime: %d\n", _medianTime);

    /*  nonce   */
    mem += 9;  /* strlen(var) + 4  */
    _nonce = 0;
    while (*mem != ',') {
        c = *mem++;
        _nonce = (_nonce * 10) + (c - '0');
    }
    printf("_nonce: %u\n", _nonce);

    /*  bits    */
    mem += 9;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = _bits;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_bits: %s\n", _bits);

    /*  difficulty, .13lf    */
    mem         += 15;  /* strlen(var) + 5("���� ����)  */
    var         = 0;
    power       = 1;
    _difficulty = 0.0;
    isFloat     = 0;
    while (*mem != ',') {
        c = *mem++;
        if (c == '.') {
            ++isFloat;
        } else {
            var = (var * 10) + (c - '0');
            if (isFloat) {
                power *= 10;
            }
        }
    }
    _difficulty = ((double) var / (double) power);

    printf((isFloat ? "_difficulty: %.13lf\n" : "_difficulty: %.0lf\n"), _difficulty);

    /*  chainwork    */
    mem += 14;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = _chainWork;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_chainWork: %s\n", _chainWork);

    /*  nTx    */
    mem += 8;  /* strlen(var) + 5("���� ����)  */
    _nTx = 0;
    while (*mem != ',') {
        c = *mem++;
        _nTx = (_nTx * 10) + (c - '0');
    }
    printf("_nTx: %d\n", _nTx);
    if (!param) {   /*  genesis block   */
        *gPrevHash = '\0';

        /*  nextblockhash    */
        mem += 18;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = gNextHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_nextBlockHash: %s\n", gNextHash);
    } else if (param < 0) { /*  best(tip) block  */
        /*  previousblockhash    */
        mem += 22;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = gPrevHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_previousBlockHash: %s\n", gPrevHash);

        *gNextHash = '\0';
    } else {
        /*  previousblockhash    */
        mem += 22;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = gPrevHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_previousBlockHash: %s\n", gPrevHash);

        /*  nextblockhash    */
        mem += 19;  /* strlen(var) + 6(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = gNextHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_nextBlockHash: %s\n", gNextHash);
    }

    /*  tx  */
    mem = index;    // branch to address which start tx datas
    gTx = (char **) malloc(sizeof(char *) * _nTx);
    *gTx = (char *) malloc(sizeof(char) * _nTx * HASH_SIZE);
    for (i = 1; i < _nTx; ++i) {
        *(gTx + i) = *(gTx + i - 1) + HASH_SIZE;
    }
    /*  
     *  for debug   
    for (i = 0; i < _nTx; ++i) {
        printf("%d%c%08X\n", i, '\t', *(gTx + i));
    }
    */
    for (i = 0; i < _nTx; ++i) {
        temp = *(gTx + i);
        for (j = 0; j < 64; ++j) {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("%d%c%s\n", (i + 1), '\t', *(gTx + i));  // for debug    
        ++mem;  //  skip "
        ++mem;  //  skip ,
        ++mem;  //  skip "
    }
    mem = memTemp;     //recover 
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getRawTransaction(int index)
{
    sprintf(buf, "{\"jsonrpc\":\"1.0\", \"method\":\"getrawtransaction\", \"params\":[\"%s\", true]}", *(gTx + index));
    process(buf);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getBlockchainInfo(void)
{
    /*
     *  {
     *      "chain":"main",
     *      "blocks":579172,
     *      "headers":579172,
     *      "bestblockhash":"0000000000000000000874611b720d16353a40ff91808388e2af963f4dcd3aba",
     *      "difficulty":7459680720542.296,
     *      "mediantime":1559636994,
     *      "verificationprogress":0.9999970393364542,
     *      "initialblockdownload":false,
     *      "chainwork":"00000000000000000000000000000000000000000684db32cc52a65468b5d969",
     *      "size_on_disk":253109128480,
     *      "pruned":false,
     *      "softforks":[
     *          {"id":"bip34","version":2,"reject":{"status":true}},
     *          {"id":"bip66","version":3,"reject":{"status":true}},
     *          {"id":"bip65","version":4,"reject":{"status":true}}],
     *      "bip9_softforks":{"csv":{"status":"active","startTime":1462060800,"timeout":1493596800,"since":419328},
     *      "segwit":{"status":"active","startTime":1479168000,"timeout":1510704000,"since":481824}},"warnings":""
     *  }
     */
    process(GET_BLOCKCHAIN_INFO_CMD);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getMemPoolInfo(void)
{
    /*
     *  {
     *      "size":31894,
     *      "bytes":17553340,
     *      "usage":60861408,
     *      "maxmempool":300000000,
     *      "mempoolminfee":0.00001000,
     *      "minrelaytxfee":0.00001000
     *  }
     */
    process(GET_MEM_POOL_INFO_CMD);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getMiningInfo(void)
{
    /*
     *  {
     *      "blocks":579172,
     *      "difficulty":7459680720542.296,
     *      "networkhashps":4.702711524794712e+19,
     *      "pooledtx":32144,
     *      "chain":"main",
     *      "warnings":""
     *  }
     */

    process(GET_MINING_INFO_CMD);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getNetworkInfo(void)
{
    /*
    *  {
    *      "version":180000,
    *      "subversion":"/Satoshi:0.18.0/",
    *      "protocolversion":70015,
    *      "localservices":"000000000000040d",
    *      "localrelay":true,
    *      "timeoffset":0,
    *      "networkactive":true,
    *      "connections":62,
    *      "networks":[
    *          {"name":"ipv4","limited":false,"reachable":true,"proxy":"","proxy_randomize_credentials":false},
    *          {"name":"ipv6","limited":false,"reachable":true,"proxy":"","proxy_randomize_credentials":false},
    *          {"name":"onion","limited":true,"reachable":false,"proxy":"","proxy_randomize_credentials":false}],
    *      "relayfee":0.00001000,
    *      "incrementalfee":0.00001000,
    *      "localaddresses":[
    *          {"address":"106.250.161.211","port":8333,"score":40671}],
    *      "warnings":""}
    */

    process(GET_NETWORK_INFO_CMD);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getWalletInfo(void)
{
    /*
     *  {
     *      "walletname":"",
     *      "walletversion":169900,
     *      "balance":0.00000000,
     *      "unconfirmed_balance":0.00000000,
     *      "immature_balance":0.00000000,
     *      "txcount":0,
     *      "keypoololdest":1557994216,
     *      "keypoolsize":1000,
     *      "keypoolsize_hd_internal":1000,
     *      "paytxfee":0.00000000,
     *      "hdseedid":"6cec1d243b6c07a3af929e9bfb43391f5dbe4759",
     *      "private_keys_enabled":true}
     */
    process(GET_WALLET_INFO_CMD);
    printf("%s\n", mem);
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmem, void *userp)
{
    int realSize;

    realSize = (size * nmem);
    mem = (char *) realloc(mem, memSize + realSize + 1);

    if (mem == NULL) {
        fprintf(stderr, "not enough memory (realloc returned NULL)\n");

        return (-1);
    }
    memcpy(&(mem[memSize]), contents, realSize);
    memSize += realSize;
    mem[memSize] = '\0';

    return realSize;
}