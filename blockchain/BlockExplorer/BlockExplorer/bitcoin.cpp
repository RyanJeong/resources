/*
 *  구현 함수들
 *      getinfo
 *          : was removed in 0.16.0 version of Bitcoin Core. Use the RPCs listed in the See Also subsection below instead.
 *
 *      대체함수들: 
 *          getblockchaininfo   : provides information about the current state of the block chain.
 *          getMempoolInfo      : returns information about the node’s current transaction memory pool.
 *          getmininginfo       : returns various mining-related information.
 *          getnetworkinfo      : returns information about the node’s connection to the network.
 *      getblockhash(param) -> getblock(getblockhash result) -> getrawtransaction -> decoderawtransaction
 *
 *      GetBestBlockHash: returns the header hash of the most recent block on the best block chain.
 *          result: {"result":"0000000000000000001203178dbf8a6b4f609ae65db4a63fd5023442ef8e38c3","error":null,"id":null}
 */

#include "BlockExplorer.h"
#include "bitcoin.h"

extern HWND hWnd;       //  Popup 생성 위해 사용함

char    *mem;           //  RPC 수행 결과를 저장하는 메모리
int     memSize;        //  RPC 수행 결과를 저장한 내용의 크기(메모리 할당 크기)
char    buf[BUF_SIZE];  //  sprintf에 사용하기 위한 버퍼
char    *tempMem;       //  포인터 연산 시 복귀 주소 저장 목적 

   /*   getblock return value structure
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

int             BTC_getBlockCount;          //  현재 메인 체인의 블록 갯수 
int             BTC_serializedBlockLen;
char            *BTC_serializedBlock;       //  블록을 16진수 값으로 읽어들임
char            BTC_hash[HASH_SIZE];        //  현재 조회한 높이의 블록 해더 해쉬 값
char            BTC_prevHash[HASH_SIZE];    //  현재 조회한 높이의 블록 해더 해쉬의 이전 값
char            BTC_nextHash[HASH_SIZE];    //  현재 조회한 높이의 블록 헤더 해쉬의 다음 값
char            **BTC_tx;                   //  현재 조회한 높이의 블록에 포함된 거래들
char            BTC_getBlock_versionHex[BITS_SIZE], BTC_getBlock_merkleRoot[HASH_SIZE], BTC_getBlock_bits[BITS_SIZE], BTC_getBlock_chainWork[HASH_SIZE];
int             BTC_getBlock_confirmations, BTC_getBlock_size, BTC_getBlock_strippedSize, BTC_getBlock_weight, BTC_getBlock_height, BTC_getBlock_version, BTC_getBlock_time, BTC_getBlock_medianTime, BTC_getBlock_nTx;
unsigned int    BTC_getBlock_nonce;
double          BTC_getBlock_difficulty;

/*
 *  process :   cURL 동작 수행 및 결과를 받는 함수 
 */
 int process(char *data)
{
    CURL                *handle;
    struct curl_slist   *headers;
    CURLcode            result;

    handle  = curl_easy_init();
    headers = NULL;
    if (handle) {
        /*  curl_slist_append appends a string to a linked list of strings. */
        headers = curl_slist_append(headers, HEADER);
        /*  set our custom set of headers   */
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_URL, URL);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(handle, CURLOPT_USERPWD, USERPWD);
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WritememoryCallback);
        result = curl_easy_perform(handle);
        /*  Check for errors    */
        if (result != CURLE_OK) {
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
#endif
            MessageBox(hWnd, NOT_CONNECTED_TO_SERVER, NULL, MB_ICONSTOP | MB_OK | MB_SYSTEMMODAL);

            return OCCURRED_ERROR;
        }
        mem[memSize] = '\0';
        /*  always cleanup  */
        curl_easy_cleanup(handle);
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
        printf("curl_easy_cleanup end\n");
#endif
    }

    return 0;
}

 /*
  *  WritememoryCallback:   process 함수에서 사용되는 callback 함수
  *                         process에서 server로 cURL 명령을 요청하면 명령 결과를 mem에 저장하는 함수
  *
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
 size_t WritememoryCallback(void *contents, size_t size, size_t nmem, void *userp)
 {
     int realSize;

     realSize   = (size * nmem);
     mem        = (char *) realloc(mem, memSize + realSize + 1);

     if (mem == NULL) {
         MessageBox(hWnd, OUT_OF_MEMORY, NULL, MB_ICONSTOP | MB_OK | MB_SYSTEMMODAL);
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
         fprintf(stderr, "not enough memory (realloc returned NULL)\n");
#endif

         return OCCURRED_ERROR;
     }
     memcpy(&(mem[memSize]), contents, realSize);
     memSize += realSize;
     // mem[memSize] = '\0';    //  데이터량에 따라 1~n번 호출될수 있음, 따라서 process 함수에서 string의 마지막을 의미하는 null 삽입

     return realSize;
 }

 /*
  * getBlockCount : server로부터 getblockcount 명령의 결과를 받아오는 함수
  *                 결과는 BTC_getBlockCount에 저장    
  */
 int getBlockCount(void)
{
    int c;

    if (process(GET_BLOCK_COUNT_CMD) < 0) {

        return OCCURRED_ERROR;
    }
    tempMem             = mem;  //  store address
    mem                 += 10;
    BTC_getBlockCount   = 0;
    while (*mem != ',') {
        c = *mem++;
        BTC_getBlockCount = (BTC_getBlockCount * 10) + (c - '0');
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("BTC_getBlockCount: %d\n", BTC_getBlockCount);
#endif
    mem     = tempMem;  //  recover address
    free(mem);
    mem     = NULL;
    memSize = 0;
    
    return 0;
}

 /*
  * getBlock :  함수의 매개변수인 param 값에 따라 server로 getblockhash 또는 getbestblockhash 명령을 요청하고, 
  *             결과는 parsing하여 각 의미에 맞는 변수들에 저장
  */
 int getBlock(int param)
{
    /*
    *  temp     : 배열 변수를 포인터 연산 하기 위한 메모리 번지 저장 변수
    *  i        : 반복문 제어 변수
    *  c        : getchar()용 변수
    *  index    : parsing 과정 중 tx 부분 번지 기억 변수
    *  var      : 문자열에서 실수로의 변환에 사용하는 변수, 정수부
    *  power    : 문자열에서 실수로의 변환에 사용하는 변수, 실수부
    *  isFloat : 문자열에서 실수로의 변환에 사용하는 변수, 현재 문자열이 실수가 맞는가?
    */
    char        *temp;
    int         i, j, c, isFloat;
    char        *index;

    /*  TODO: 각 필드 값들이 제대로 연산 결과를 나타내는지 확인해야 함  */

    long long   var, power;

    if (mem != NULL) {
        free(mem);
        memSize = 0;
        mem = NULL;
    }
    if (BTC_tx != NULL) {
        if (*BTC_tx != NULL) {
            free(*BTC_tx);
        }
        free(BTC_tx);
        BTC_tx = NULL;
    }

    /*
    *  param >= 0  -> getblockhash param
    *  else        -> getbestblockhash
    */
    if (param >= 0) {
        sprintf(buf, BTC_GET_BLOCK_HASH, param);
        if (process(buf) < 0) {

            return OCCURRED_ERROR;
        }
    } else {
        if (process(GET_BEST_BLOCK_HASH_CMD) < 0) {

            return OCCURRED_ERROR;
        }
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("getBlock buf: %s\n", buf);
#endif
    tempMem = mem;
    temp    = BTC_hash;
    tempMem += 11;  /*  offset  */
    for (i = HASH_OFFSET; i; --i) {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    free(mem);
    mem     = 0;
    memSize = 0;

    /*  getblock header_hash_value  */
    sprintf(buf, BTC_GET_BLOCK, BTC_hash);
    process(buf);
    tempMem = mem;     // 메모리 해제 주소 보존 위해 변수 사용

    /*  confirmations   */
    tempMem += 101; /*  offset  */
    BTC_getBlock_confirmations = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_confirmations = (BTC_getBlock_confirmations * 10) + (c - '0');
    }

    /*  strippedsize    */
    tempMem += 16;  /* strlen(var) + 4  */
    BTC_getBlock_strippedSize = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_strippedSize = (BTC_getBlock_strippedSize * 10) + (c - '0');
    }

    /*  size    */
    tempMem += 8;  /* strlen(var) + 4  */
    BTC_getBlock_size = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_size = (BTC_getBlock_size * 10) + (c - '0');
    }

    /*  weight   */
    tempMem += 10;  /* strlen(var) + 4  */
    BTC_getBlock_weight = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_weight = (BTC_getBlock_weight * 10) + (c - '0');
    }

    /*  height    */
    tempMem += 10;  /* strlen(var) + 4  */
    BTC_getBlock_height = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_height = (BTC_getBlock_height * 10) + (c - '0');
    }

    /*  version    */
    tempMem += 11;  /* strlen(var) + 4  */
    BTC_getBlock_version = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_version = (BTC_getBlock_version * 10) + (c - '0');
    }

    /*  versionHex    */
    tempMem += 15;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = BTC_getBlock_versionHex;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  merkleroot    */
    tempMem += 16;  /* strlen(var) + 6("부터 시작, 문자열 변수 앞에 "가 추가로 붙음)  */
    temp = BTC_getBlock_merkleRoot;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  tx index 저장, 후에 nTx 구한 후 동적할당 위해 번지수를 기록    */
    tempMem += 9;
    index = tempMem;
    while (*tempMem++ != ']') {
        ;
    }

    /*  time    */
    tempMem += 8;  /* strlen(var) + 4  */
    BTC_getBlock_time = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_time = (BTC_getBlock_time * 10) + (c - '0');
    }

    /*  mediantime    */
    tempMem += 14;  /* strlen(var) + 4  */
    BTC_getBlock_medianTime = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_medianTime = (BTC_getBlock_medianTime * 10) + (c - '0');
    }

    /*  nonce   */
    tempMem += 9;  /* strlen(var) + 4  */
    BTC_getBlock_nonce = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_nonce = (BTC_getBlock_nonce * (unsigned) 10) + (unsigned) (c - '0');
    }

    /*  bits    */
    tempMem += 9;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = BTC_getBlock_bits;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  difficulty, .13lf    */
    tempMem += 15;  /* strlen(var) + 5("부터 시작)  */
    var = 0;
    power = 1;
    BTC_getBlock_difficulty = 0.0;
    isFloat = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        if (c == '.') {
            ++isFloat;
        } else {
            var = (var * 10) + (c - '0');
            if (isFloat) {
                power *= 10;
            }
        }
    }
    BTC_getBlock_difficulty = ((double) var / (double) power);

    /*  chainwork    */
    tempMem += 14;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
    temp = BTC_getBlock_chainWork;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  nTx    */
    tempMem += 8;  /* strlen(var) + 5("부터 시작)  */
    BTC_getBlock_nTx = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_nTx = (BTC_getBlock_nTx * 10) + (c - '0');
    }
    if (!param) {   /*  genesis block   */
        *BTC_prevHash = '\0';

        /*  nextblockhash    */
        tempMem += 18;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = BTC_nextHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
    } else if (param < 0) { /*  best(tip) block  */
                            /*  previousblockhash    */
        tempMem += 22;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = BTC_prevHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
        *BTC_nextHash = '\0';
    } else {
        /*  previousblockhash    */
        tempMem += 22;  /* strlen(var) + 5(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = BTC_prevHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';

        /*  nextblockhash    */
        tempMem += 19;  /* strlen(var) + 6(문자열 변수 앞에 "가 추가로 붙음)  */
        temp = BTC_nextHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
    }

    /*  tx  */
    tempMem = index;    // branch to address which start tx datas
    BTC_tx = (char **) malloc(sizeof(char *) * BTC_getBlock_nTx);
    *BTC_tx = (char *) malloc(sizeof(char) * BTC_getBlock_nTx * HASH_SIZE);
    for (i = 1; i < BTC_getBlock_nTx; ++i) {
        *(BTC_tx + i) = *(BTC_tx + i - 1) + HASH_SIZE;
    }
    for (i = 0; i < BTC_getBlock_nTx; ++i) {
        temp = *(BTC_tx + i);
        for (j = 0; j < 64; ++j) {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
        printf("%d%c%s\n", (i + 1), '\t', *(BTC_tx + i));  // for debug    
#endif
        ++tempMem;  //  skip "
        ++tempMem;  //  skip ,
        ++tempMem;  //  skip "
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("mem: %s\n", mem);
    printf("BTC_getBlock_confirmations: %d\n", BTC_getBlock_confirmations);
    printf("BTC_getBlock_strippedSize: %d\n", BTC_getBlock_strippedSize);
    printf("BTC_getBlock_strippedSize: %d\n", BTC_getBlock_size);
    printf("BTC_getBlock_weight: %d\n", BTC_getBlock_weight);
    printf("BTC_getBlock_height: %d\n", BTC_getBlock_height);
    printf("BTC_getBlock_version: %d\n", BTC_getBlock_version);
    printf("BTC_getBlock_versionHex: %s\n", BTC_getBlock_versionHex);
    printf("BTC_getBlock_merkleRoot: %s\n", BTC_getBlock_merkleRoot);
    printf("BTC_getBlock_time: %d\n", BTC_getBlock_time);
    printf("BTC_getBlock_medianTime: %d\n", BTC_getBlock_medianTime);
    printf("BTC_getBlock_nonce: %u\n", BTC_getBlock_nonce);
    printf("BTC_getBlock_bits: %s\n", BTC_getBlock_bits);
    printf((isFloat ? "BTC_getBlock_difficulty: %.13lf\n" : "BTC_getBlock_difficulty: %.0lf\n"), BTC_getBlock_difficulty);
    printf("BTC_getBlock_chainWork: %s\n", BTC_getBlock_chainWork);
    printf("BTC_getBlock_nTx: %d\n", BTC_getBlock_nTx);
    if (!param) {   /*  genesis block   */
        printf("BTC_getBlock_nextHash: %s\n", BTC_nextHash);
    } else if (param < 0) { /*  best(tip) block  */
        printf("BTC_getBlock_prevHash: %s\n", BTC_prevHash);
    } else {
        printf("BTC_getBlock_prevHash: %s\n", BTC_prevHash);
        printf("BTC_getBlock_nextHash: %s\n", BTC_nextHash);
    }
#endif
    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

/*
 *  getSerializedBlock: server에 getblockhash 또는 getbestblockhash를 요청 후에 받는 결과를 구조화하지 않고 raw level의 값으로 받음
 */
 int getSerializedBlock(int param)
{
    /*
    *  temp    : 배열 변수를 포인터 연산 하기 위한 메모리 번지 저장 변수
    *  i       : 반복문 제어 변수
    */
    char    *temp;
    int     i;

    if (mem != NULL) {
        free(mem);
        memSize = 0;
        mem = NULL;
    }
    if (BTC_serializedBlock != NULL) {
        free(BTC_serializedBlock);
        BTC_serializedBlock = NULL;
    }
    /*
    *  param >= 0  -> getblockhash param
    *  else        -> getbestblockhash
    */
    if (param >= 0) {
        sprintf(buf, BTC_GET_BLOCK_HASH, param);
        if (process(buf) < 0) {

            return OCCURRED_ERROR;
        }
    } else {
        if (process(GET_BEST_BLOCK_HASH_CMD) < 0) {

            return OCCURRED_ERROR;
        }
    }
    tempMem = mem;
    temp = BTC_hash;
    tempMem += 11;  /*  offset, Hash 시작번지로 이동  */
    for (i = 64; i; --i) {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    free(mem);
    mem = 0;
    memSize = 0;

    sprintf(buf, BTC_GET_BLOCK, BTC_hash);
    process(buf);

    tempMem = mem;     //store 
    tempMem += 11;  //  바이트 흐름 시작 번지
    BTC_serializedBlockLen = 0;
    while (*tempMem++ != '"') {
        ++BTC_serializedBlockLen;  //  총 데이터 길이 확인
    }
    BTC_serializedBlock = (char *) malloc(BTC_serializedBlockLen + 1);
    tempMem -= (BTC_serializedBlockLen + 1);
    temp = BTC_serializedBlock;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;  //  데이터 복사
    }
    *temp = '\0';
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("BTC_serializedBlock: %s\n", BTC_serializedBlock);
    printf("BTC_serializedBlockLen: %d\n", BTC_serializedBlockLen);
#endif

    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

 /*
  *  getRawTransaction: server에 파라미터인 index를 거래 번호 인덱스로 사용하여 getblock index's_hash 명령을 요청하고, 결과를 mem에 받아옴
  */
 int getRawTransaction(void)
{
    int i;
    
    for (i = 0; i < BTC_getBlock_nTx; ++i) {
        sprintf(buf, BTC_GET_RAW_TRANSACTION_JSON, *(BTC_tx + i));
        if (process(buf) < 0) {

            return OCCURRED_ERROR;
        }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
        printf("mem: %s\n", mem);
#endif
        free(mem);
        mem = NULL;
        memSize = 0;
    }

    return 0;
}

 /*
  *  getBlockchainInfo: server에 getblockchaininfo 명령을 수행하고, 결과를 mem에 저장
  */
 int getBlockchainInfo(void)
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
    if (process(GET_BLOCKCHAIN_INFO_CMD) < 0) {

        return OCCURRED_ERROR;
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("mem: %s\n", mem);
#endif
    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

 /*
  *  getMempoolInfo: server에 getmempoolinfo 명령을 수행하고, 결과를 mem에 저장
  */
 int getMempoolInfo(void)
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
    if (process(GET_mem_POOL_INFO_CMD) < 0) {

        return OCCURRED_ERROR;
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("mem: %s\n", mem);
#endif
    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

 /*
  *  getMiningInfo: server에 getmininginfo 명령을 수행하고, 결과를 mem에 저장
  */
 int getMiningInfo(void)
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
    if (process(GET_MINING_INFO_CMD) < 0) {

        return OCCURRED_ERROR;
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("mem: %s\n", mem);
#endif
    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

 /*
  *  getNetworkInfo: server에 getnetworkinfo 명령을 수행하고, 결과를 mem에 저장
  */
 int getNetworkInfo(void)
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
    if (process(GET_NETWORK_INFO_CMD) < 0) {

        return OCCURRED_ERROR;
    }
#if (DEBUG_MODE && DEBUG_BITCOIN_CPP)
    printf("mem: %s\n", mem);
#endif
    free(mem);
    mem = NULL;
    memSize = 0;

    return 0;
}

 /*
  *  getNetworkInfo: memory 누수 방지를 위해, 다 사용한 memory를 해제함
  */
void freeMemory(void)
{
    if (mem != NULL) {
        free(mem);
        memSize = 0;
        mem = NULL;
    }
    if (BTC_tx != NULL) {
        if (*BTC_tx != NULL) {
            free(*BTC_tx);
        }
        free(BTC_tx);
        BTC_tx = NULL;
    }

    return;
}