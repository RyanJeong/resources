#ifndef __GETBLOCK_H__
#define __GETBLOCK_H__

#define COINBASE                            "!!COINBASE!!"
#define BTC_GET_BLOCK                       "{\"jsonrpc\":\"1.0\", \"method\":\"getblock\", \"params\":[\"%s\", false]}"
#define BTC_GET_BLOCK_HASH                  "{\"jsonrpc\":\"1.0\", \"method\":\"getblockhash\", \"params\":[%d]}"
#define BTC_GET_BLOCK_COUNT                 "{\"jsonrpc\":\"1.0\", \"method\":\"getblockcount\"}"
#define BTC_GET_BLOCK_OFFSET                11  //  {"result":"
#define BTC_GET_BLOCK_COUNT_OFFSET          10  //  {"result":

#define __GETBLOCK__                        "[getBlock.c]"
#define GETBLOCK_REQUESTGETBLOCKCOUNT       "[requestGetBlockCount(void)] "
#define GETBLOCK_REQUESTGETBLOCK            "[requestGetBlock(int)] "
#define GETBLOCK_PARSE                      "[parse(char *)] "
#define GETBLOCK_GETBLOCKHEADER             "[getBlockHeader(char *, BLOCK_HEADER *)] "
#define GETBLOCK_GETVERSION                 "[getVersion(char *, char *)] "
#define GETBLOCK_GETPREBLOCKHASH            "[getPreBlockHash(char *, char *)] "
#define GETBLOCK_GETMERKLEROOT              "[getMerkleRoot(char *, char *)] "
#define GETBLOCK_GETTIME                    "[getTime(char *, char *)] "
#define GETBLOCK_GETBITS                    "[getBits(char *, char *)] "
#define GETBLOCK_GETNONCE                   "[getNonce(char *, char *)] "
#define GETBLOCK_GETBLOCKHASH               "[getBlockHash(char *, char *)] "
#define GETBLOCK_GETTXNUM                   "[getTxNum(char *, char *)] "
#define GETBLOCK_GETTX                      "[getTx(char *, TX *)] "
#define GETBLOCK_GETTXVERSION               "[getTxVersion(char *, char *)] "
#define GETBLOCK_GETTXINPUT                 "[getTxInput(char *, INPUT *)] "
#define GETBLOCK_GETTXINPUTTXID             "[getTxInputTxid(char *, char *)] "
#define GETBLOCK_GETTXINPUTVOUT             "[getTxInputVout(char *, char *)] "
#define GETBLOCK_GETTXINPUTSCRIPTSIG        "[getTxInputScriptSig(char *, char *, size_t)] "
#define GETBLOCK_GETTXINPUTSEGWITDATA       "[getTxInputSegwitData(char *, char *, size_t)] "
#define GETBLOCK_GETTXINPUTSEQUENCE         "[getTxInputSequence(char *, char *)] "
#define GETBLOCK_GETTXOUTPUT                "[getTxOutput(char *, OUTPUT *)] "
#define GETBLOCK_GETTXLOCKTIME              "[getTxLockTime(char *, char *)] "
#define GETBLOCK_GETTXOUTPUTVALUE           "[getTxOutputValue(char *, char *)] "
#define GETBLOCK_GETTXOUTPUTSCRIPTPUBKEY    "[getTxOutputScriptPubKey(char *, char *, size_t)] "

#define VARINT_COND1                        0xfc
#define VARINT_COND2                        0xffff
#define VARINT_COND3                        0xffffffff
#define VARINT_COND4                        0xffffffffffffffff

typedef struct blockHeader {
    char    version[SIZE_VERSION+1];
    char    preBlockHash[SIZE_PREBLOCKHASH+1];
    char    merkleRoot[SIZE_MERKLEROOT+1];
    char    time[SIZE_TIME+1];
    char    bits[SIZE_BITS+1];
    char    nonce[SIZE_NONCE+1];
    char    blockHash[SIZE_BLOCKHASH+1];    //  원래 존재하지 않는 필드값이지만, 편의상 집어넣은 멤버변수
} BLOCK_HEADER;

typedef struct segwit {
    size_t  count;
    char    *segwitCount;
    char    **size;
    char    **data;
} SEGWIT;

typedef struct input {
    char    txid[SIZE_TXID+1];
    char    vout[SIZE_VOUT+1];
    char    *scriptSigSize;
    char    *scriptSig;
    char    sequence[SIZE_SEQUENCE+1];
} INPUT;

typedef struct output {
    char    value[SIZE_VALUE+1];
    char    *scriptPubKeySize;
    char    *scriptPubKey;  
} OUTPUT;

typedef struct transaction {
    char    version[SIZE_VERSION+1];
    char    *inputCount;
    INPUT   **input;
    size_t  isSegwit;
    SEGWIT  **segwit;
    char    *outputCount;
    OUTPUT  **output;
    char    lockTime[SIZE_LOCKTIME+1];
    size_t  in;     //  편의상 넣은 멤버변수, input 값을 10진으로 변환한 값
    size_t  out;    //  편의상 넣은 멤버변수, output 값을 10진으로 변환한 값
} TX;

/*
 * size_t requestGetBlockCount(void)
 * : 현재 메인 넷에 존재하는 총 블록 갯수를 반환하는 함수
 */
size_t requestGetBlockCount(void);

/*
 * void requestGetBlock(int);
 * : getblock 명령을 수행하는 함수
 */
void requestGetBlock(int);

/*
 * size_t parse(char *)
 * : serialized 결과를 유의미한 정보로 변환하는 변수
 */
size_t parse(char *);

/*
 * size_t getBlockHeader(char *, BLOCK_HEADER *);
 * : Block header를 가져오는 함수
 */
size_t getBlockHeader(char *, BLOCK_HEADER *);

/*
 * size_t getVersion(char *, char *);
 * : 매개변수로부터 version 값을 받아옴
 */
size_t getVersion(char *, char *);

/*
 * size_t getPreBlockHash(char *, char *);
 * : 매개변수로부터 previous block hash 값을 받아옴
 */
size_t getPreBlockHash(char *, char *);

/*
 * size_t getMerkleRoot(char *, char *);
 * : 매개변수로부터 merkle root 값을 받아옴
 */
size_t getMerkleRoot(char *, char *);

/*
 * size_t getTime(char *, char *);
 * : 매개변수로부터 time 값을 받아옴
 */
size_t getTime(char *, char *);

/*
 * size_t getBits(char *, char *);
 * : 매개변수로부터 bits 값을 받아옴
 */
size_t getBits(char *, char *);

/*
 * size_t getNonce(char *, char *);
 * : 매개변수로부터 nonce 값을 받아옴
 */
size_t getNonce(char *, char *);

/*
 * size_t getBlockHash(char *, char *);
 * : 블록 헤더의 값들을 사용하여 현재 블록의 해쉬 값을 생성
 */
size_t getBlockHash(char *, char *);

/*
 * size_t getTxNum(char *, char *);
 * : TX의 총 갯수를 가져옴
 */
size_t getTxNum(char *, char *);

/*
 * int getTx(char *, TX *);
 * : TX 정보를 가져옴
 * : 양수: 트랜잭션 총 바이트 수, 그 외: 오류 
 */
int getTx(char *, TX *);

/*
 * size_t getTxOutput(char *, char *);
 * : 해당 Tx의 version을 가져옴
 */
size_t getTxVersion(char *, char *);

/*
 * int getTxInput(char *, INPUT *);
 * : 해당 tx의 input 정보를 가져옴
 */
int getTxInput(char *, INPUT *);

/*
 * size_t getTxInputTxid(char *, char *);
 * int getTxOutput(char *, OUTPUT *);
 * : 해당 tx의 input txid를 가져옴
 */
size_t getTxInputTxid(char *, char *);

/*
 * size_t getTxInputVout(char *, char *);
 * : 해당 tx의 input vout을 가져옴
 */
size_t getTxInputVout(char *, char *);

/*
 * size_t getTxInputScriptSig(char *, char *, size_t);
 * : 해당 tx의 input script sig를 가져옴
 */
size_t getTxInputScriptSig(char *, char *, size_t);

/*
 * size_t getTxInputSegwitData(char *, char *, size_t);
 * : 해당 tx의 input에 사용된 segwit data를 가져옴
 */
size_t getTxInputSegwitData(char *, char *, size_t);

/*
 * size_t getTxInputSequence(char *, char *);
 * : 해당 tx의 input sequence를 가져옴
 */
size_t getTxInputSequence(char *, char *);

/*
 * int getTxOutput(char *, OUTPUT *);
 * : 해당 tx의 output 정보를 가져옴
 */
int getTxOutput(char *, OUTPUT *);

/*
 * size_t getTxOutputValue(char *, char *);
 * : 해당 tx의 output value를 가져옴
 */
size_t getTxOutputValue(char *, char *);

/*
 * size_t getTxOutputScriptPubKey(char *, char *, size_t);
 * : 해당 tx의 output scriptPubKey를 가져옴
 */
size_t getTxOutputScriptPubKey(char *, char *, size_t);

/*
 * size_t getTxLockTime(char *, char *);
 * : 해당 tx의 lockTime을 가져옴
 */
size_t getTxLockTime(char *, char *);
#endif
