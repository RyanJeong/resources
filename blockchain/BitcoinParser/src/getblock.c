#include <stdio.h>      /*  sprintf, ...    */
#include <stdlib.h>     /*  malloc, ... */
#include <string.h>     /*  memcpy, ... */
#include <stdint.h>     /*  size_t, ... */

#include "common.h"
#include "getblock.h"
#include "curl.h"       /*  process, ...    */
#include "util.h"       /*  putsForDebug, ...   */

static size_t   flag = DEBUG_OFF;
static size_t   segwit;

static char buffer[SIZE];

size_t requestGetBlockCount(void)
{
    /*
     *  cnt     : 메인 넷에 존재하는 총 블록 갯수
     *  p       : cURL 결과를 받는 포인터
     */
    static char *func = GETBLOCK_REQUESTGETBLOCKCOUNT;
    size_t      cnt;
    char        *p;

    cnt = 0;
    p = process(BTC_GET_BLOCK_COUNT);
    p += BTC_GET_BLOCK_COUNT_OFFSET;
    while (*p != ',') {
        cnt = (cnt * 10) + (*p++ - '0');
    }
    sprintf(buffer, "Block count is %lu", cnt);
    putsForDebug(__GETBLOCK__, func, buffer, flag);

    return cnt;
}

void requestGetBlock(int param)
{
    /*
     *  temp    : getblock 명령을 수행하여 얻은 해시 값을 임시로 보관하는 변수
     *  p       : cURL 결과를 받는 포인터
     *  q       : cURL 결과 주소를 보관하는 포인터
     */
    static char *func = GETBLOCK_REQUESTGETBLOCK;
    char        temp[SIZE_HASH+1];
    char        *p, *q; 

    sprintf(buffer, BTC_GET_BLOCK_HASH, param);
    p = process(buffer);
    if (p == NULL) {
        sprintf(buffer, "%s: %s\n", MSG_FAILURE, BTC_GET_BLOCK_HASH);
        putsForDebug(__GETBLOCK__, func, buffer, flag);

        exit(1);
    }
    q = p;
    p += BTC_GET_BLOCK_OFFSET;
    memcpy(temp, p, SIZE_HASH);
    *(temp + SIZE_HASH) = '\0';
    sprintf(buffer, BTC_GET_BLOCK, temp);
    p = process(buffer);
    if (p == NULL) {
        sprintf(buffer, "%s: %s\n", MSG_FAILURE, BTC_GET_BLOCK);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        
        exit(1);
    }
    q = p;
    p += BTC_GET_BLOCK_OFFSET;
    if (parse(p)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        exit(1);
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return;
}

/*
 *  rpc를 전송하는 측에서 client의 cpu endian type에 맞게 형식을 수정하여 전송함
 *  따라서 rpc 결과를 모두 big endian으로 변환하여 읽어야 함    
 *  구해야 하는 값들: block header, tx count, tx
 */
size_t parse(char *str)
{
    /*
     * header   : 블록 헤더 정보를 담는 구조체
     * tx       : 거래 정보를 담는 구조체
     * nTx      : 거래의 총 갯수를 담는 변수
     * temp     : 해당 거래의 총 bytes 수를 담는 변수
     * i, j, k  : 반복문에 사용하는 제어변수
     */
    static char     *func = GETBLOCK_PARSE;
    BLOCK_HEADER    header;
    size_t          nTx, temp, i, j, k;
    TX              **tx;

    /*  1. get magic byte and size; magic byte and size are omitted when using an RPC protocol  */
    
    /*  2. get block header;    */
    if (getBlockHeader(str, &header)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        str += SIZE_HEADER;
    }

    /*  3. get number of tx(s)  */
    if ((nTx = varInt(str)) < 0) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
        str += (nTx <= VARINT_COND1) ? SIZE_2BYTES : ((nTx <= VARINT_COND2) ? SIZE_6BYTES : ((nTx <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
    }

    /*  4. get tx(s)    */
    /*  nTx cannot be zero; all blocks have a coinbase transaction  */
    tx  = (TX **) malloc(sizeof(TX *) * nTx);
    if (tx == NULL) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    }
    *tx = (TX *) malloc(sizeof(TX) * nTx);
    if (*tx == NULL) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    }
    /*  linking */
    for (i = 1; i < nTx; ++i) {
        *(tx + i) = *(tx + (i - 1)) + 1;    /*  메모리 증감 연산은 해당 변수의 대상의 자료형 크기 배수만큼 수행됨(references 폴더 참고) */
    }
    for (i = 0; i < nTx; ++i) {
        if ((temp = getTx(str, *(tx + i))) <= 0) {
            putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

            return 1;
        } else {
            str += temp;
        }
    }

    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    /*  parsing한 결과를 출력하는 부분  */
    sprintf(buffer, "Block Hash: %s", header.blockHash);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Version: %s", header.version);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Previous Block Hash: %s", header.preBlockHash);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Merkle Root: %s", header.merkleRoot);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Time: %s", header.time);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Bits: %s", header.bits);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "Nonce: %s", header.nonce);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    sprintf(buffer, "nTx: %lu", nTx);
    putsForDebug(__GETBLOCK__, func, buffer, flag);
    for (i = 0; i < nTx; ++i) {
        if (!i) {
            putsForDebug(__GETBLOCK__, func, COINBASE, flag);
        }
        sprintf(buffer, "%lu. TX version: %s", i, (*(tx + i))->version);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        sprintf(buffer, "%lu. TX input count: %s", i, (*(tx + i))->inputCount);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        for (j = 0; j < (*(tx + i))->in; ++j) {
            sprintf(buffer, "%lu-%lu. TX input txid: %s", i, j, (*((*(tx + i))->input + j))->txid);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX input vout: %s", i, j, (*((*(tx + i))->input + j))->vout);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX input scriptSigSize: %s", i, j, (*((*(tx + i))->input + j))->scriptSigSize);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX input scriptSig: %s", i, j, (*((*(tx + i))->input + j))->scriptSig);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX input sequence: %s", i, j, (*((*(tx + i))->input + j))->sequence);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
        }
        sprintf(buffer, "%lu. TX output count: %s", i, (*(tx + i))->outputCount);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        for (j = 0; j < (*(tx + i))->out; ++j) {
            sprintf(buffer, "%lu-%lu. TX output value: %s", i, j, (*((*(tx + i))->output + j))->value);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX output scriptPubKeySize: %s", i, j, (*((*(tx + i))->output + j))->scriptPubKeySize);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            sprintf(buffer, "%lu-%lu. TX output scriptPubKey: %s", i, j, (*((*(tx + i))->output + j))->scriptPubKey);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
        }
        if ((*(tx + i))->isSegwit) {
            for (j = 0; j < (*(tx + i))->in; ++j) {
                for (k = 0; k < (*((*(tx + i))->segwit + j))->count; ++k) {
                    sprintf(buffer, "%lu-%lu-%lu. SegWit Size: %s", i, j, k, (*((*((*(tx + i))->segwit + j))->size + k)));
                    putsForDebug(__GETBLOCK__, func, buffer, flag);
                    sprintf(buffer, "%lu-%lu-%lu. SegWit Data: %s", i, j, k, (*((*((*(tx + i))->segwit + j))->data + k)));
                    putsForDebug(__GETBLOCK__, func, buffer, flag);
                }
            }
        }
        sprintf(buffer, "%lu. TX lockTime: %s", i, (*(tx + i))->lockTime);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
    }

    /*  free 항상 역순!!    */
    for (i = 0; i < nTx; ++i) {
        for (j = 0; j < (*(tx + i))->out; ++j) {
            free((*((*(tx + i))->output + j))->scriptPubKeySize);
            free((*((*(tx + i))->output + j))->scriptPubKey);
        }
        if (j) {    /*  (*(tx + i))->out의 값이 0이 아닐 경우   */
            free(*((*(tx + i))->output));
            free((*(tx + i))->output);
        }   /*  만약 j가 0이라면, (*(tx + i))->out의 값이 0이므로 tx->output, *(tx->output)에 메모리 할당 안 함 */
        free((*(tx + i))->outputCount);
        for (j = 0; j < (*(tx + i))->in; ++j) {
            free((*((*(tx + i))->input + j))->scriptSigSize);
            free((*((*(tx + i))->input + j))->scriptSig);
            if ((*(tx + i))->isSegwit) {
                for (k = 0; k < (*((*(tx + i))->segwit + j))->count; ++k) {
                    free(*((*((*(tx + i))->segwit + j))->size + k));
                    free(*((*((*(tx + i))->segwit + j))->data + k));
                }
                if (k) {    /*  (*((*(tx + i))->segwit + j))->count의 값이 0이 아닐 경우    */
                    free((*((*(tx + i))->segwit + j))->size);
                    free((*((*(tx + i))->segwit + j))->data);
                }
            }
        }
        if (j) {    /*  (*(tx + i))->out의 값이 0이 아닐 경우   */
            if ((*(tx + i))->isSegwit) {
                free(*((*(tx + i))->segwit));
                free((*(tx + i))->segwit);
            }
            free(*((*(tx + i))->input));
            free((*(tx + i))->input);
        }   /*  만약 j가 0이라면, (*(tx + i))->out의 값이 0이므로 tx->output, *(tx->output)에 메모리 할당 안 함 */
        free((*(tx + i))->inputCount);
    }
    free(*tx);
    free(tx);   

    return 0;
}

/*  
 *  구해야 하는 값들: version, previous block header, merkle root, time, bits, nonce
 */
size_t getBlockHeader(char *p, BLOCK_HEADER *header)
{
    static char     *func = GETBLOCK_GETBLOCKHEADER;
    char            blockHeader[SIZE_HEADER];

    memcpy(blockHeader, p, SIZE_HEADER);
    if (getBlockHash(blockHeader, header->blockHash)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    }
    if (getVersion(p, header->version)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        p += SIZE_VERSION;
    }
    if (getPreBlockHash(p, header->preBlockHash)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        p += SIZE_PREBLOCKHASH;
    }
    if (getMerkleRoot(p, header->merkleRoot)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        p += SIZE_MERKLEROOT;
    }
    if (getTime(p, header->time)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        p += SIZE_TIME;
    }
    if (getBits(p, header->bits)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        p += SIZE_BITS;
    }
    if (getNonce(p, header->nonce)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return 1;
    } else {
        /*  p += SIZE_NONCE 불필요 연산 */
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getVersion(char *p, char *q)
{
    static char *func = GETBLOCK_GETVERSION;

    memcpy(q, p, SIZE_VERSION);
    *(q + SIZE_VERSION) = '\0';
    if (toLittleEndian(q, SIZE_VERSION)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return 0;
}

size_t getPreBlockHash(char *p, char *q)
{
    static char *func = GETBLOCK_GETPREBLOCKHASH;

    memcpy(q, p, SIZE_PREBLOCKHASH);
    *(q + SIZE_PREBLOCKHASH) = '\0';
    if (toLittleEndian(q, SIZE_PREBLOCKHASH)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return 0;
}

size_t getMerkleRoot(char *p, char *q)
{
    static char *func = GETBLOCK_GETMERKLEROOT;

    memcpy(q, p, SIZE_MERKLEROOT);
    *(q + SIZE_MERKLEROOT) = '\0';
    if (toLittleEndian(q, SIZE_MERKLEROOT)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return 0;
}

size_t getTime(char *p, char *q)
{
    static char *func = GETBLOCK_GETTIME;

    memcpy(q, p, SIZE_TIME);
    *(q + SIZE_TIME) = '\0';
    if (toLittleEndian(q, SIZE_TIME)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getBits(char *p, char *q)
{
    static char *func = GETBLOCK_GETBITS;

    memcpy(q, p, SIZE_BITS);
    *(q + SIZE_BITS) = '\0';
    if (toLittleEndian(q, SIZE_BITS)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getNonce(char *p, char *q)
{
    static char *func = GETBLOCK_GETNONCE;

    memcpy(q, p, SIZE_NONCE);
    *(q + SIZE_NONCE) = '\0';
    if (toLittleEndian(q, SIZE_NONCE)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return 0;
}

size_t getBlockHash(char *p, char *q)
{
    /*
     * buf  : block hash를 계산하는 중간 값을 담는 변수
     */
    static char *func   = GETBLOCK_GETBLOCKHASH;
    uint8_t     buf[SIZE_HEADER_HEX];
    
    if (hexToBin(p, buf, SIZE_HEADER_HEX)) {
        putsForDebug(__GETBLOCK__, func, MSG_NOT_HEX, flag);

        return 1;
    }
    sha256(sha256(buf, SIZE_HEADER_HEX), SIZE_HASH_HEX);
    if (binToHex(buf, q, SIZE_HASH_HEX)) {
        putsForDebug(__GETBLOCK__, func, MSG_NOT_BIN, flag);

        return 1;
    }
    *(q + SIZE_BLOCKHASH) = '\0';
    if (toLittleEndian(q, SIZE_BLOCKHASH)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

int getTx(char *p, TX *tx)
{
    /*
     *  in          : input의 총 갯수
     *  out         : output의 총 갯수
     *  temp        : getTxInput, getTxOutput 결과 바이트를 저장하는 변수
     *  size        : varInt에 몇 byte가 사용되었는지를 저장하는 변수
     *  total       : 해당 tx의 총 bytes를 계산
     *  i, j        : 반복문 제어 변수
     *  segwit      : 거래가 segwit을 사용하는지 여부를 저장하는 변수
     */
    static char *func = GETBLOCK_GETTX;
    size_t      in, out, size, total, i, j;
    int         temp;   /*  음수 값도 들어갈 수 있으므로, 주의  */
    SEGWIT      *pSegwit;
    
    total           = 0;
    tx->isSegwit    = 0;
    
    /* version  */
    if (getTxVersion(p, tx->version)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "tx version: %s", tx->version);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p += SIZE_VERSION;
        total += SIZE_VERSION;
    }
    
    /*  check segwit    */
    if ((*p == '0') && (*(p + 1) == '0') && (*(p + 2) == '0') && (*(p + 3) == '1')) {
        tx->isSegwit    = 1;   /*  set segwit flag */
        p               += SIZE_SEGWIT_FLAG;             /*  skip '0' '0' '0' '1'    */
        total           += SIZE_SEGWIT_FLAG;
    }

    /*  inputCount  */
    if ((in = varInt(p)) < 0) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
        size            = (in <= VARINT_COND1) ? SIZE_2BYTES : ((in <= VARINT_COND2) ? SIZE_6BYTES : ((in <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
        tx->inputCount  = (char *) malloc(size + 1);
        if (tx->inputCount == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        memcpy(tx->inputCount, p, size);
        *((tx->inputCount) + size) = '\0';
        sprintf(buffer, "input count: %s", tx->inputCount);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p       += size;
        total   += size;
        tx->in  = in;
    }   
    if (!in) {
        putsForDebug(__GETBLOCK__, func, MSG_VARINT_ZERO, flag);
    } else {
        tx->input   = (INPUT **) malloc(sizeof(INPUT *) * in);
        if (tx->input == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        *(tx->input) = (INPUT *) malloc(sizeof(INPUT) * in);
        if (*(tx->input) == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }

        /*  linking, must to set start index to 1   */
        for (i = 1; i < in; ++i) {
            *(tx->input + i) = *(tx->input + (i - 1)) + 1;
        }

        /*  get input(s)    */
        for (i = 0; i < in; ++i) {
            if ((temp = getTxInput(p, *(tx->input + i))) <= 0) {
                putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

                return (-1);
            } else {
                p       += temp;
                total   += temp;
            } 
        }
    }
    
    /*  outputCount */
    if ((out = varInt(p)) < 0) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
        size            = (out <= VARINT_COND1) ? SIZE_2BYTES : ((out <= VARINT_COND2) ? SIZE_6BYTES : ((out <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
        tx->outputCount = (char *) malloc(size + 1);
        if (tx->outputCount == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        memcpy(tx->outputCount, p, size);
        *((tx->outputCount) + size) = '\0';
        sprintf(buffer, "output count: %s", tx->outputCount);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p       += size;
        total   += size;
        tx->out = out;
    }   
    if (!out) {
        putsForDebug(__GETBLOCK__, func, MSG_VARINT_ZERO, flag);
    } else {
        tx->output  = (OUTPUT **) malloc(sizeof(OUTPUT *) * out);
        if (tx->output == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        *(tx->output) = (OUTPUT *) malloc(sizeof(OUTPUT) * out);
        if (*(tx->output) == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }

        /*  linking, must to set start index to 1   */
        for (i = 1; i < out; ++i) {
            *(tx->output + i) = *(tx->output + (i - 1)) + 1;
        }

        /*  get output(s)   */
        for (i = 0; i < out; ++i) {
            if ((temp = getTxOutput(p, *(tx->output + i))) <= 0) {
                putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

                return (-1);
            } else {
                total   += temp;
                p       += temp;
            } 
        }
    }
    
    /*  segwit  */
    if (tx->isSegwit && tx->in) { /*  segwit flag on and inputCount not zero  */
        tx->segwit = (SEGWIT **) malloc(sizeof(SEGWIT *) * tx->in);
        if (tx->segwit == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        *(tx->segwit) = (SEGWIT *) malloc(sizeof(SEGWIT) * tx->in);
        if (*(tx->segwit) == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }

        /*  linking, must to set start index to 1   */
        for (i = 1; i < tx->in; ++i) {
            *(tx->segwit + i) = *(tx->segwit + (i - 1)) + 1;
        }

        /*  get segwit(s)   */
        for (i = 0; i < tx->in; ++i) {
            pSegwit = *(tx->segwit + i);
            if ((temp = varInt(p)) < 0) {
                putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

                return (-1);
            } else {
                /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
                size = (temp <= VARINT_COND1) ? SIZE_2BYTES : ((temp <= VARINT_COND2) ? SIZE_6BYTES : ((temp <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
                pSegwit->segwitCount = (char *) malloc(size + 1);
                if (pSegwit->segwitCount == NULL) {
                    putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

                    return (-1);
                }
                memcpy(pSegwit->segwitCount, p, size);
                *(pSegwit->segwitCount + size) = '\0';
                sprintf(buffer, "segwit count: %s", pSegwit->segwitCount);
                putsForDebug(__GETBLOCK__, func, buffer, flag);
                p               += size;
                total           += size;
                pSegwit->count  = temp;
                if (temp) {
                    pSegwit->size   = (char **) malloc(sizeof(char *) * temp);
                    pSegwit->data   = (char **) malloc(sizeof(char *) * temp);
                }
            }   
            if (!pSegwit->count) {
                sprintf(buffer, "TX [%lu] Segwit is empty", i);
                putsForDebug(__GETBLOCK__, func, buffer, flag);
            } else {
                for (j = 0; j < pSegwit->count; ++j) {
                    if ((temp = varInt(p)) < 0) {
                        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

                        return (-1);
                    } else {
                        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
                        size                    = (temp <= VARINT_COND1) ? SIZE_2BYTES : ((temp <= VARINT_COND2) ? SIZE_6BYTES : ((temp <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
                        *(pSegwit->size + j) = (char *) malloc(size + 1);
                        if (*(pSegwit->size + j) == NULL) {
                            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

                            return (-1);
                        }
                        memcpy(*(pSegwit->size + j), p, size);
                        *(*(pSegwit->size + j) + size)  = '\0';
                        p                               += size;
                        total                           += size;
                    }
                    if (!temp) {    /*  크기가 0일 때, 1 바이트에 널값만 입력하여 나중에 free할 때 오류나지 않도록 구현 */
                        putsForDebug(__GETBLOCK__, func, MSG_VARINT_ZERO, flag);
                        *(pSegwit->data + j)    = (char *) malloc(1);
                        *(*(pSegwit->data + j)) = '\0';
                    } else {
                        temp                    <<= 1;  /*  1 byte in hex -> 2 bytes in string  */
                        *(pSegwit->data + j)    = (char *) malloc(temp + 1);
                        if (*(pSegwit->data + j) == NULL) {
                            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

                            return (-1);
                        }
                        sprintf(buffer, "TX [%lu] index [%lu] size: %s", i, j, *(pSegwit->size + j));
                        putsForDebug(__GETBLOCK__, func, buffer, flag);

                        /*  segwit data */
                        if (getTxInputSegwitData(p, *(pSegwit->data + j), temp)) {
                            putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

                            return (-1);
                        } else {
                            sprintf(buffer, "TX [%lu] index [%lu] data: %s", i, j, *(pSegwit->data + j));
                            putsForDebug(__GETBLOCK__, func, buffer, flag);
                            p       += temp;
                            total   += temp;
                        }
                    }
                }
            }
        }
    }
    /*  lockTime    */
    if (getTxLockTime(p, tx->lockTime)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "lockTime: %s", tx->lockTime);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        /*  p += SIZE_LOCKTIME;     불필요 연산 */
        total   += SIZE_LOCKTIME;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return total;
}

size_t getTxVersion(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXVERSION;

    memcpy(q, p, SIZE_VERSION);
    *(q + SIZE_VERSION) = '\0';
    if (toLittleEndian(q, SIZE_VERSION)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

/*  주의! int이므로, 오류 발생 시 음수 반환 */
/*  구해야 하는 값들: txid, vout, scriptSigSize, scriptSig, sequence    */
int getTxInput(char *p, INPUT *input)
{
    /*
     *  total   : 해당 거래의 총 bytes 수
     *  temp    : varInt 결과를 임시 저장하는 변수
     *  size    : ScriptSig Size를 담는 변수
     */
    static char *func = GETBLOCK_GETTXINPUT;
    size_t      total, temp, size;

    total = 0;
    /*  txid    */
    if (getTxInputTxid(p, input->txid)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "txid: %s", input->txid);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p       += SIZE_TXID;
        total   += SIZE_TXID;
    }

    /*  vout    */
    if (getTxInputVout(p, input->vout)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "vout: %s", input->vout);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p       += SIZE_VOUT;
        total   += SIZE_VOUT;
    }

    /*  scriptSigSize   */
    if ((temp = varInt(p)) < 0) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
        size                    = (temp <= VARINT_COND1) ? SIZE_2BYTES : ((temp <= VARINT_COND2) ? SIZE_6BYTES : ((temp <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
        input->scriptSigSize    = (char *) malloc(size + 1);
        if (input->scriptSigSize == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        memcpy(input->scriptSigSize, p, size);
        *((input->scriptSigSize) + size) = '\0';
        p       += size;
        total   += size;
    }
    
    if (!temp) {    /*  크기가 0일 때, 1 바이트에 널값만 입력하여 나중에 free할 때 오류나지 않도록 구현 */
        putsForDebug(__GETBLOCK__, func, MSG_VARINT_ZERO, flag);
        input->scriptSig        = (char *) malloc(1);
        *(input->scriptSig)     = '\0';
    } else {
        temp                    <<= 1;  /*  1 byte in hex -> 2 bytes in string  */
        input->scriptSig        = (char *) malloc(temp + 1);
        if (input->scriptSig == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        sprintf(buffer, "scriptSigSize: %s", input->scriptSigSize);
        putsForDebug(__GETBLOCK__, func, buffer, flag);

        /*  scriptSig   */
        if (getTxInputScriptSig(p, input->scriptSig, temp)) {
            putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

            return (-1);
        } else {
            sprintf(buffer, "scriptSig: %s", input->scriptSig);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            p       += temp;
            total   += temp;
        }
    }

    /*  sequence    */
    if (getTxInputSequence(p, input->sequence)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "sequence: %s", input->sequence);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        /*  p       += SIZE_SEQUENCE;   불필요 연산 */
        total   += SIZE_SEQUENCE;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return total;
}

size_t getTxInputTxid(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXINPUTTXID;

    memcpy(q, p, SIZE_TXID);
    *(q + SIZE_TXID) = '\0';
    if (toLittleEndian(q, SIZE_TXID)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxInputVout(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXINPUTVOUT;

    memcpy(q, p, SIZE_VOUT);
    *(q + SIZE_VOUT) = '\0';
    if (toLittleEndian(q, SIZE_VOUT)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxInputScriptSig(char *p, char *q, size_t len)
{
    static char *func = GETBLOCK_GETTXINPUTSCRIPTSIG;

    memcpy(q, p, len);
    *(q + len) = '\0';
    if (toLittleEndian(q, len)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxInputSegwitData(char *p, char *q, size_t len)
{
    static char *func = GETBLOCK_GETTXINPUTSEGWITDATA;

    memcpy(q, p, len);
    *(q + len) = '\0';
    if (toLittleEndian(q, len)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxInputSequence(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXINPUTSEQUENCE;

    memcpy(q, p, SIZE_SEQUENCE);
    *(q + SIZE_SEQUENCE) = '\0';
    if (toLittleEndian(q, SIZE_SEQUENCE)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

/*  주의! int이므로, 오류 발생 시 음수 반환 */
/*  구해야 하는 값들: value, scriptPubKeySize, scriptPubKey */
int getTxOutput(char *p, OUTPUT *output)
{
    /*
     *  total   : 해당 거래의 총 bytes 수
     *  temp    : varInt 결과를 임시 저장하는 변수
     *  size    : ScriptSig Size를 담는 변수
     */
    static char *func   = GETBLOCK_GETTXOUTPUT;
    size_t      total, temp, size;

    total   = 0;
    /*  value   */
    if (getTxOutputValue(p, output->value)) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        sprintf(buffer, "value: %s", output->value);
        putsForDebug(__GETBLOCK__, func, buffer, flag);
        p       += SIZE_VALUE;
        total   += SIZE_VALUE;
    }
    /*  scriptPubKeySize    */
    if ((temp = varInt(p)) < 0) {
        putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

        return (-1);
    } else {
        /*  fd -> 2 + 4 bytes, fe -> 2 + 8 bytes, ff -> 2 + 16 bytes    */
        size                        = (temp <= VARINT_COND1) ? SIZE_2BYTES : ((temp <= VARINT_COND2) ? SIZE_6BYTES : ((temp <= VARINT_COND3) ? SIZE_10BYTES : SIZE_18BYTES));
        output->scriptPubKeySize    = (char *) malloc(size + 1);
        if (output->scriptPubKeySize == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        memcpy(output->scriptPubKeySize, p, size);
        *((output->scriptPubKeySize) + size) = '\0';
        p       += size;
        total   += size;
    }

    if (!temp) {    /*  크기가 0일 때, 1 바이트에 널값만 입력하여 나중에 free할 때 오류나지 않도록 구현 */
        putsForDebug(__GETBLOCK__, func, MSG_VARINT_ZERO, flag);
        output->scriptPubKey    = (char *) malloc(1);
        *(output->scriptPubKey) = '\0';
    } else {
        temp                    <<= 1;  /*  1 byte in hex -> 2 bytes in string  */
        output->scriptPubKey    = (char *) malloc(temp + 1);
        if (output->scriptPubKey == NULL) {
            putsForDebug(__GETBLOCK__, func, MSG_ALLOC_ERROR, flag);

            return (-1);
        }
        sprintf(buffer, "scriptPubKeySize: %s", output->scriptPubKeySize);
        putsForDebug(__GETBLOCK__, func, buffer, flag);

        /*  scriptPubKey    */
        if (getTxOutputScriptPubKey(p, output->scriptPubKey, temp)) {
            putsForDebug(__GETBLOCK__, func, MSG_FAILURE, flag);

            return (-1);
        } else {
            sprintf(buffer, "scriptPubKey: %s", output->scriptPubKey);
            putsForDebug(__GETBLOCK__, func, buffer, flag);
            /*  p       += temp;    불필요 연산 */
            total   += temp;
        }
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);
    
    return total;
}

size_t getTxOutputValue(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXOUTPUTVALUE;

    memcpy(q, p, SIZE_VALUE);
    *(q + SIZE_VALUE) = '\0';
    if (toLittleEndian(q, SIZE_VALUE)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxOutputScriptPubKey(char *p, char *q, size_t len)
{
    static char *func = GETBLOCK_GETTXOUTPUTSCRIPTPUBKEY;

    memcpy(q, p, len);
    *(q + len) = '\0';
    if (toLittleEndian(q, len)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t getTxLockTime(char *p, char *q)
{
    static char *func = GETBLOCK_GETTXLOCKTIME;

    memcpy(q, p, SIZE_LOCKTIME);
    *(q + SIZE_LOCKTIME) = '\0';
    if (toLittleEndian(q, SIZE_LOCKTIME)) {
        putsForDebug(__GETBLOCK__, func, MSG_ENDIAN_ERROR, flag);

        return 1;
    }
    putsForDebug(__GETBLOCK__, func, MSG_SUCCESS, flag);

    return 0;
}
