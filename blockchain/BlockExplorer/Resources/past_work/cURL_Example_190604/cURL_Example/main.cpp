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
 *  콜백함수 호출이 완료되야 curl_easy_perform 함수가 완료된다.
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

#define CURL_STATICLIB
#define HASH_SIZE   65
#define BITS_SIZE   9

#include <stdlib.h>
#include <curl/curl.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "libcurl.lib")

void getBlock(int);
void getRawTransaction();
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

char    *mem;
int     memSize;
char    transaction[HASH_SIZE];    //  여러 건의 TX를 처리하기 위해선 동적 할당 필요, [ ~ ] 갯수 세기, 링크드 리스트로 동적 추가 할당
char    *memTemp;           //  포인터 연산 시 복귀 주소 저장 목적 


int     gParam;
char    gHash[HASH_SIZE];
char    gPrevHash[HASH_SIZE];
char    gNextHash[HASH_SIZE];
char    **gTx;

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
            case 1:
                scanf("%d", &j);
                getchar();
                getBlock(j);

                break;
            case 2:
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
    char *data;

    data = "{\"jsonrpc\":\"1.0\", \"method\":\"getblockchaininfo\"}";
    process(data);

    return;
}

void getMemPoolInfo(void)
{
    char *data;

    data = "{\"jsonrpc\":\"1.0\", \"method\":\"getmempoolinfo\"}";
    process(data);

    return;
}

void getMiningInfo(void)
{
    char *data;

    data = "{\"jsonrpc\":\"1.0\", \"method\":\"getmininginfo\"}";
    process(data);

    return;
}

void getNetworkInfo(void)
{
    char *data;

    data = "{\"jsonrpc\":\"1.0\", \"method\":\"getnetworkinfo\"}";
    process(data);

    return;
}

void getWalletInfo(void)
{
    char *data;

    data = "{\"jsonrpc\":\"1.0\", \"method\":\"getwalletinfo\"}";
    process(data);

    return;
}

void getBlock(int param)
{
    /*
     *  data    : process에 넘겨주기 위한 문자열
     *  temp    : 배열 변수를 포인터 연산 하기 위한 메모리 번지 저장 변수
     *  flag    : tx를 모두 확인하기 위한 조건문 제어 변수
     *  buf     : sprintf에 사용하기 위한 배열
     *  i       : 반복문 제어 변수
     *  c       : getchar()용 변수
     *  index   : parsing 과정 중 tx 부분 번지 기억 변수 
     *  var     : 문자열에서 실수로의 변환에 사용하는 변수, 정수부
     *  power   : 문자열에서 실수로의 변환에 사용하는 변수, 실수부
     *  isFloat : 문자열에서 실수로의 변환에 사용하는 변수, 현재 문자열이 실수가 맞는가?
     */
    char    *data, *temp, flag; 
    char    buf[1024];
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
    char        _versionHex[BITS_SIZE], _merkleRoot[HASH_SIZE], _bits[BITS_SIZE], _chainWork[HASH_SIZE], _previousBlockHash[HASH_SIZE], _nextBlockHash[HASH_SIZE];
    int         _confirmations, _size, _strippedSize, _weight, _height, _version, _time, _medianTime, _nTx;
    long long   _nonce;
    double      _difficulty;
    
    flag = 1;
    gParam = param;
    /*
     *  param >= 0  -> getblockhash param
     *  else        -> getbestblockhash
     */
    if (param >= 0) {
        sprintf(buf, "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[%d]}", param);
        process(buf);
    } else {
        data = "{\"jsonrpc\":\"1.0\", \"method\":\"getbestblockhash\"}";
        process(data);
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
    mem += 15;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = _versionHex;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_versionHex: %s\n", _versionHex);

    /*  merkleroot    */
    mem += 16;  /* strlen(var) + 6("부터 시작, 문자열 변수 앞에 "가 추가로 붙음)  */
    temp = _merkleRoot;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_merkleRoot: %s\n", _merkleRoot);

    /*  tx index 저장, 후에 nTx 구한 후 동적할당 위해 번지수를 기록    */
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
    printf("_nonce: %lld\n", _nonce);

    /*  bits    */
    mem += 9;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = _bits;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_bits: %s\n", _bits);

    /*  difficulty, .13lf    */
    mem         += 15;  /* strlen(var) + 5("부터 시작)  */
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
    /*
    while (*mem != '.') {
        c = *mem++;
        var = (var * 10) + (c - '0');
    }
    ++mem;  /*  skip dot    
    while (*mem != ',') {
        c = *mem++;
        var = (var * 10) + (c - '0');
        power *= 10;
    }
    */
    _difficulty = ((double) var / (double) power);

    printf((isFloat ? "_difficulty: %.13lf\n" : "_difficulty: %.0lf\n"), _difficulty);

    /*  chainwork    */
    mem += 14;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = _chainWork;
    while (*mem != '"') {
        *temp++ = *mem++;
    }
    *temp = '\0';
    printf("_chainWork: %s\n", _chainWork);

    /*  nTx    */
    mem += 8;  /* strlen(var) + 5("부터 시작)  */
    _nTx = 0;
    while (*mem != ',') {
        c = *mem++;
        _nTx = (_nTx * 10) + (c - '0');
    }
    printf("_nTx: %d\n", _nTx);
    if (!param) {   /*  genesis block   */
        *_previousBlockHash = '\0';

        /*  nextblockhash    */
        mem += 18;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = _nextBlockHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_nextBlockHash: %s\n", _nextBlockHash);
    } else if (param < 0) { /*  best(tip) block  */
        /*  previousblockhash    */
        mem += 22;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = _previousBlockHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_previousBlockHash: %s\n", _previousBlockHash);

        *_nextBlockHash = '\0';
    } else {
        /*  previousblockhash    */
        mem += 22;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = _previousBlockHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_previousBlockHash: %s\n", _previousBlockHash);

        /*  nextblockhash    */
        mem += 19;  /* strlen(var) + 6(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = _nextBlockHash;
        while (*mem != '"') {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("_nextBlockHash: %s\n", _nextBlockHash);
    }

    /*  tx  */
    mem = index;    // branch to address which start tx datas
    gTx = (char **) malloc(sizeof(char *) * _nTx);
    *gTx = (char *) malloc(sizeof(char) * _nTx * HASH_SIZE);
    for (i = 1; i < _nTx; ++i) {
        *(gTx + i) = *(gTx + i - 1) + HASH_SIZE;
    }
    for (i = 0; i < _nTx; ++i) {
        printf("%d%c%08X\n", i, '\t', *(gTx + i));
    }
    for (i = 0; i < _nTx; ++i) {
        temp = *(gTx + i);
        *(temp + 64) = 'NULL';
    }
    for (i = 0; i < _nTx; ++i) {
        temp = *(gTx + i);
        for (j = 0; j < 64; ++j) {
            *temp++ = *mem++;
        }
        *temp = '\0';
        printf("%d%c%s\n", (i + 1), '\t', *(gTx + i));
        ++mem;  //  skip "
        ++mem;  //  skip ,
        ++mem;  //  skip "
    }
    mem = memTemp;     //recover 
    printf("%s\n", mem);
    free(*gTx);
    free(gTx);
    gTx = NULL;
    free(mem);
    mem = NULL;
    memSize = 0;

    return;
}

void getRawTransaction()
{

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