/*
 *  ���� �Լ���
 *      getinfo
 *          : was removed in 0.16.0 version of Bitcoin Core. Use the RPCs listed in the See Also subsection below instead.
 *
 *      ��ü�Լ���: 
 *          getblockchaininfo   : provides information about the current state of the block chain.
 *          getMempoolInfo      : returns information about the node��s current transaction memory pool.
 *          getmininginfo       : returns various mining-related information.
 *          getnetworkinfo      : returns information about the node��s connection to the network.
 *      getblockhash(param) -> getblock(getblockhash result) -> getrawtransaction -> decoderawtransaction
 *
 *      GetBestBlockHash: returns the header hash of the most recent block on the best block chain.
 *          result: {"result":"0000000000000000001203178dbf8a6b4f609ae65db4a63fd5023442ef8e38c3","error":null,"id":null}
 */

#include "BlockExplorer.h"
#include "bitcoin.h"

extern HWND hWnd;       //  Popup ���� ���� �����

char    *mem;           //  RPC ���� ����� �����ϴ� �޸�
int     memSize;        //  RPC ���� ����� ������ ������ ũ��(�޸� �Ҵ� ũ��)
char    buf[BUF_SIZE];  //  sprintf�� ����ϱ� ���� ����
char    *tempMem;       //  ������ ���� �� ���� �ּ� ���� ���� 

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

int             BTC_getBlockCount;          //  ���� ���� ü���� ��� ���� 
int             BTC_serializedBlockLen;
char            *BTC_serializedBlock;       //  ����� 16���� ������ �о����
char            BTC_hash[HASH_SIZE];        //  ���� ��ȸ�� ������ ��� �ش� �ؽ� ��
char            BTC_prevHash[HASH_SIZE];    //  ���� ��ȸ�� ������ ��� �ش� �ؽ��� ���� ��
char            BTC_nextHash[HASH_SIZE];    //  ���� ��ȸ�� ������ ��� ��� �ؽ��� ���� ��
char            **BTC_tx;                   //  ���� ��ȸ�� ������ ��Ͽ� ���Ե� �ŷ���
char            BTC_getBlock_versionHex[BITS_SIZE], BTC_getBlock_merkleRoot[HASH_SIZE], BTC_getBlock_bits[BITS_SIZE], BTC_getBlock_chainWork[HASH_SIZE];
int             BTC_getBlock_confirmations, BTC_getBlock_size, BTC_getBlock_strippedSize, BTC_getBlock_weight, BTC_getBlock_height, BTC_getBlock_version, BTC_getBlock_time, BTC_getBlock_medianTime, BTC_getBlock_nTx;
unsigned int    BTC_getBlock_nonce;
double          BTC_getBlock_difficulty;

/*
 *  process :   cURL ���� ���� �� ����� �޴� �Լ� 
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
  *  WritememoryCallback:   process �Լ����� ���Ǵ� callback �Լ�
  *                         process���� server�� cURL ����� ��û�ϸ� ��� ����� mem�� �����ϴ� �Լ�
  *
  *  [�ǹ�]
  *  1. �ݹ��Լ� ���� �����?
  *
  *  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, �ݹ��Լ���);
  *
  *  2. �ݹ��Լ� �۵� �����?
  *
  *  curl_easy_perform �Լ��� ȣ���ϸ� �ݹ��Լ��� ȣ��Ǳ� �����Ѵ�.
  *  �����ͷ��� ���� 1~n�� ȣ��ɼ� ������
  *  �ݹ��Լ� ȣ���� �Ϸ� �Ǿ�� curl_easy_perform �Լ��� �Ϸ�ȴ�.
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
     // mem[memSize] = '\0';    //  �����ͷ��� ���� 1~n�� ȣ��ɼ� ����, ���� process �Լ����� string�� �������� �ǹ��ϴ� null ����

     return realSize;
 }

 /*
  * getBlockCount : server�κ��� getblockcount ����� ����� �޾ƿ��� �Լ�
  *                 ����� BTC_getBlockCount�� ����    
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
  * getBlock :  �Լ��� �Ű������� param ���� ���� server�� getblockhash �Ǵ� getbestblockhash ����� ��û�ϰ�, 
  *             ����� parsing�Ͽ� �� �ǹ̿� �´� �����鿡 ����
  */
 int getBlock(int param)
{
    /*
    *  temp     : �迭 ������ ������ ���� �ϱ� ���� �޸� ���� ���� ����
    *  i        : �ݺ��� ���� ����
    *  c        : getchar()�� ����
    *  index    : parsing ���� �� tx �κ� ���� ��� ����
    *  var      : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, ������
    *  power    : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, �Ǽ���
    *  isFloat : ���ڿ����� �Ǽ����� ��ȯ�� ����ϴ� ����, ���� ���ڿ��� �Ǽ��� �´°�?
    */
    char        *temp;
    int         i, j, c, isFloat;
    char        *index;

    /*  TODO: �� �ʵ� ������ ����� ���� ����� ��Ÿ������ Ȯ���ؾ� ��  */

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
    tempMem = mem;     // �޸� ���� �ּ� ���� ���� ���� ���

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
    tempMem += 15;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = BTC_getBlock_versionHex;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  merkleroot    */
    tempMem += 16;  /* strlen(var) + 6("���� ����, ���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = BTC_getBlock_merkleRoot;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  tx index ����, �Ŀ� nTx ���� �� �����Ҵ� ���� �������� ���    */
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
    tempMem += 9;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = BTC_getBlock_bits;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  difficulty, .13lf    */
    tempMem += 15;  /* strlen(var) + 5("���� ����)  */
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
    tempMem += 14;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
    temp = BTC_getBlock_chainWork;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;
    }
    *temp = '\0';

    /*  nTx    */
    tempMem += 8;  /* strlen(var) + 5("���� ����)  */
    BTC_getBlock_nTx = 0;
    while (*tempMem != ',') {
        c = *tempMem++;
        BTC_getBlock_nTx = (BTC_getBlock_nTx * 10) + (c - '0');
    }
    if (!param) {   /*  genesis block   */
        *BTC_prevHash = '\0';

        /*  nextblockhash    */
        tempMem += 18;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = BTC_nextHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
    } else if (param < 0) { /*  best(tip) block  */
                            /*  previousblockhash    */
        tempMem += 22;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = BTC_prevHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';
        *BTC_nextHash = '\0';
    } else {
        /*  previousblockhash    */
        tempMem += 22;  /* strlen(var) + 5(���ڿ� ���� �տ� "�� �߰��� ����)  */
        temp = BTC_prevHash;
        while (*tempMem != '"') {
            *temp++ = *tempMem++;
        }
        *temp = '\0';

        /*  nextblockhash    */
        tempMem += 19;  /* strlen(var) + 6(���ڿ� ���� �տ� "�� �߰��� ����)  */
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
 *  getSerializedBlock: server�� getblockhash �Ǵ� getbestblockhash�� ��û �Ŀ� �޴� ����� ����ȭ���� �ʰ� raw level�� ������ ����
 */
 int getSerializedBlock(int param)
{
    /*
    *  temp    : �迭 ������ ������ ���� �ϱ� ���� �޸� ���� ���� ����
    *  i       : �ݺ��� ���� ����
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
    tempMem += 11;  /*  offset, Hash ���۹����� �̵�  */
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
    tempMem += 11;  //  ����Ʈ �帧 ���� ����
    BTC_serializedBlockLen = 0;
    while (*tempMem++ != '"') {
        ++BTC_serializedBlockLen;  //  �� ������ ���� Ȯ��
    }
    BTC_serializedBlock = (char *) malloc(BTC_serializedBlockLen + 1);
    tempMem -= (BTC_serializedBlockLen + 1);
    temp = BTC_serializedBlock;
    while (*tempMem != '"') {
        *temp++ = *tempMem++;  //  ������ ����
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
  *  getRawTransaction: server�� �Ķ������ index�� �ŷ� ��ȣ �ε����� ����Ͽ� getblock index's_hash ����� ��û�ϰ�, ����� mem�� �޾ƿ�
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
  *  getBlockchainInfo: server�� getblockchaininfo ����� �����ϰ�, ����� mem�� ����
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
  *  getMempoolInfo: server�� getmempoolinfo ����� �����ϰ�, ����� mem�� ����
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
  *  getMiningInfo: server�� getmininginfo ����� �����ϰ�, ����� mem�� ����
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
  *  getNetworkInfo: server�� getnetworkinfo ����� �����ϰ�, ����� mem�� ����
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
  *  getNetworkInfo: memory ���� ������ ����, �� ����� memory�� ������
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