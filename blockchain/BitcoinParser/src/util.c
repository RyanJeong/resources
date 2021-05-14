#include <stdio.h>  /*  puts, ...   */
#include <stdlib.h> /*  malloc, ... */
#include <string.h> /*  memcpy, ... */
#include <stdint.h> /*  size_t, ... */

#include "common.h"
#include "util.h"

static size_t flag = DEBUG_OFF;

static const size_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

size_t toLittleEndian(uint8_t *str, size_t len)
{
    /*
     * len  : 매개변수로 받는 문자열의 길이
     * i    : loop-statement에서 사용하는 제어 변수
     * temp1: 임시 저장용 변수
     * temp2: 임시 저장용 변수
     */
    static char *func = UTIL_TOLITTLEENDIAN;
    size_t      i;
    uint8_t     temp1, temp2;

    /* 간혹 길이가 0인 경우가 있는데, 이는 아무런 동작을 하지 않은 채로 해당 함수 종료  */
    if (!len) {

        return 0;
    }
    if (len < 0) {
        putsForDebug(__UTIL__, func, MSG_LEN_IS_NEGATIVE, flag);

        return 1;
    }
    if (len & 01) {
        putsForDebug(__UTIL__, func, MSG_LEN_IS_ODD, flag);

        return 1;
    }
    if (len != 2) { /*  len의 값이 2일 경우, len -= 2 구분을 지나면 값 반전되므로 예외처리를 해야 함    */
        --len;  /*  index의 시작 값은 0이므로 문자열의 실제 길이 값과 1 차이가 발생 */
        for (i = 0; i < len; i += 2, len -= 2) {
            temp1               = *(str + i);
            temp2               = *(str + i + 1);
            *(str + i)          = *(str + len - 1);
            *(str + i + 1)      = *(str + len);
            *(str + len - 1)    = temp1;
            *(str + len)        = temp2;
        }
    }
    putsForDebug(__UTIL__, func, MSG_SUCCESS, flag);

    return 0;
}

size_t hexToBin(uint8_t *hex, uint8_t *bin, size_t len)
{
    static char *func = UTIL_HEXTOBIN;
    size_t      i, j, temp;
    
    for (i = j = 0; i < len; ++i, j += 2) {
        temp = *(hex + j);
        if ((temp >= '0') && (temp <= '9')) {
            temp -= '0';
        } else if ((temp >= 'a') && (temp <= 'f')) {
            temp -= ('a' - 10);
        } else if ((temp >= 'A') && (temp <= 'F')) {
            temp -= ('A' - 10);
        } else {
            putsForDebug(__UTIL__, func, MSG_NOT_HEX, flag);

            return 1;
        }
        *(bin + i) = (temp << 4); 
        temp = *(hex + j + 1);
        if ((temp >= '0') && (temp <= '9')) {
            temp -= '0';
        } else if ((temp >= 'a') && (temp <= 'f')) {
            temp -= ('a' - 10);
        } else if ((temp >= 'A') && (temp <= 'F')) {
            temp -= ('A' - 10);
        } else {
            putsForDebug(__UTIL__, func, MSG_NOT_HEX, flag);

            return 1;
        }
        *(bin + i) += temp;
        /*  Test code. DO NOT REMOVE    */
        /*
        printf("%c %c : %d(hex in %x)\n", *(hex + j), *(hex + j + 1), *(bin + i), *(bin + i));
        */
    }
    putsForDebug(__UTIL__, func, MSG_SUCCESS, flag);
    
    return 0;
}

size_t binToHex(uint8_t *bin, uint8_t *hex, size_t len)
{
    static char *func = UTIL_BINTOHEX;
    size_t      i, j, temp;

    for (i = j = 0; i < len; ++i, j += 2) {
        temp = *(bin + i) >> 4;
        if ((temp >= 0) && (temp <= 9)) {
            temp += '0';
        } else if ((temp >= 10) && (temp <= 15)) {
            temp += ('a' - 10);
        } else {
            putsForDebug(__UTIL__, func, MSG_NOT_BIN, flag);

            return 1;
        }
        *(hex + j) = temp;
        temp = *(bin + i) & 0xF;
        if ((temp >= 0) && (temp <= 9)) {
            temp += '0';
        } else if ((temp >= 10) && (temp <= 15)) {
            temp += ('a' - 10);
        } else {
            putsForDebug(__UTIL__, func, MSG_NOT_BIN, flag);

            return 1;
        }
        *(hex + j + 1) = temp;
        /*  Test Code. DO NOT REMOVE    */
        /*
        printf("%02x : %c %c\n", *(bin + i), *(hex + j), *(hex + j + 1));
        */
    }
    putsForDebug(__UTIL__, func, MSG_SUCCESS, flag);

    return 0;
}

uint8_t *sha256(uint8_t *buf, size_t len)
{
    static char *func = UTIL_SHA256;
    SHA256_CTX  ctx;
    size_t      i;

    sha256_init(&ctx);
    sha256_update(&ctx, buf, len);
    sha256_final(&ctx, buf);
    
    return buf;
}

void sha256_transform(SHA256_CTX *ctx, const uint8_t *data)
{
    static char *func = UTIL_SHA256_TRANSFORM;
    uint32_t    a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4) {
        m[i] = (data[j] << 24) | (data[j+1] << 16) | (data[j+2] << 8) | (data[j+3]);
    }
    while (i < 64) {
        m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];
        ++i;
    }
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];
    for (i = 0; i < 64; ++i) {
        t1  = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2  = EP0(a) + MAJ(a, b, c);
        h   = g;
        g   = f;
        f   = e;
        e   = d + t1;
        d   = c;
        c   = b;
        b   = a;
        a   = t1 + t2;
    }
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;

    return;
}

void sha256_init(SHA256_CTX *ctx)
{
    static char *func = UTIL_SHA256_INIT;

    ctx->datalen    = 0;
    ctx->bitlen     = 0;
    ctx->state[0]   = 0x6a09e667;
    ctx->state[1]   = 0xbb67ae85;
    ctx->state[2]   = 0x3c6ef372;
    ctx->state[3]   = 0xa54ff53a;
    ctx->state[4]   = 0x510e527f;
    ctx->state[5]   = 0x9b05688c;
    ctx->state[6]   = 0x1f83d9ab;
    ctx->state[7]   = 0x5be0cd19;
    
    return;
}

void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len)
{
    static char *func = UTIL_SHA256_UPDATE;
    uint32_t    i;

    for (i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen    = 0;
        }
    }

    return;
}

void sha256_final(SHA256_CTX *ctx, uint8_t hash[])
{
    static char *func = UTIL_SHA256_FINAL;
    uint32_t    i;

    i = ctx->datalen;

    /*  Pad whatever data is left in the buffer.    */
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56) {
            ctx->data[i++] = 0x00;
        }
    } else {
        ctx->data[i++] = 0x80;
        while (i < 64) {
            ctx->data[i++] = 0x00;
        }
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

    /*  Append to the padding the total message's length in bits and transform. */
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63]   = ctx->bitlen;
    ctx->data[62]   = ctx->bitlen >> 8;
    ctx->data[61]   = ctx->bitlen >> 16;
    ctx->data[60]   = ctx->bitlen >> 24;
    ctx->data[59]   = ctx->bitlen >> 32;
    ctx->data[58]   = ctx->bitlen >> 40;
    ctx->data[57]   = ctx->bitlen >> 48;
    ctx->data[56]   = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);

    /*
     * Since this implementation uses little endian byte ordering and SHA uses big endian,
     * reverse all the bytes when copying the final state to the output hash.
     */
    for (i = 0; i < 4; ++i) {
        hash[i]     = (ctx->state[0] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+4]   = (ctx->state[1] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+8]   = (ctx->state[2] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+12]  = (ctx->state[3] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+16]  = (ctx->state[4] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+20]  = (ctx->state[5] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+24]  = (ctx->state[6] >> (24 - (i * 8))) & 0x000000ff;
        hash[i+28]  = (ctx->state[7] >> (24 - (i * 8))) & 0x000000ff;
    }

    return;
}

/*
 *  처리 결과가 기대와 다를 경우 음수를 반환하므로, int형이여야 한다.
 */
int64_t varInt(uint8_t *var)
{
    static char *func = UTIL_VARINT;
    int64_t     result;
    size_t      i;
    uint8_t     c;

    result  = 0;
    i       = 0;
    c       = *(var + 1);   /*  만약 *var이 f라면, 그 뒤의 문자가 무엇인가에 따라 처리를 구분해야 함    */
    if ((*var == 'f') && ((c == 'd') || (c == 'e') || (c == 'f'))) {
        switch (c) {
        case 'd':
            i = 4;
            break;
        case 'e':
            i = 8;
            break;
        case 'f':
            i = 16;
            break;
        default:
            putsForDebug(__UTIL__, func, MSG_VARINT_PREFIX, flag);
        }
        if (!i) {
            result = (-1);  /*  f로 시작한다면 fd, fe, ff중 하나여야 함 */
        } else {
            var += 2;       /*  fd, fe, ff skip */
            if (toLittleEndian(var, i)) {
                putsForDebug(__UTIL__, func, MSG_ENDIAN_ERROR, flag);

                return 1;
            }
            while (i--) {
                charToInt(&result, *var++);
                if (result < 0) {
                    putsForDebug(__UTIL__, func, MSG_NOT_HEX, flag);
                    break;
                }   
            }
        }
    } else {
        charToInt(&result, *var++);
        if (result < 0) {
            putsForDebug(__UTIL__, func, MSG_NOT_HEX, flag);
        } else {
            charToInt(&result, *var);
            if (result < 0) {
                putsForDebug(__UTIL__, func, MSG_NOT_HEX, flag);
            }
        }
    }
    
    return result;
}

/*  varInt가 제대로 동작하는지 확인하기 위해 생성한 함수    */
void varIntTest(void)
{
    static char *func = UTIL_VARINTTEST;
    uint8_t     test1[] = VARINT_TEST1;
    uint8_t     test2[] = VARINT_TEST2; 
    uint8_t     test3[] = VARINT_TEST3;
    uint8_t     test4[] = VARINT_TEST4;
    char        buf[SIZE_64BYTES];

    memset(buf, '\0', SIZE_64BYTES);
    sprintf(buf, "0x%s result: %ld", VARINT_TEST1, varInt(test1));
    putsForDebug(__UTIL__, func, buf, flag);
    memset(buf, '\0', SIZE_64BYTES);
    sprintf(buf, "0x%s result: %ld", VARINT_TEST2, varInt(test2));
    putsForDebug(__UTIL__, func, buf, flag);
    memset(buf, '\0', SIZE_64BYTES);
    sprintf(buf, "0x%s result: %ld", VARINT_TEST3, varInt(test3));
    putsForDebug(__UTIL__, func, buf, flag);
    memset(buf, '\0', SIZE_64BYTES);
    sprintf(buf, "0x%s result: %ld", VARINT_TEST4, varInt(test4));
    putsForDebug(__UTIL__, func, buf, flag);

    return;
}

void charToInt(int64_t *res, char c)
{
    static char *func = UTIL_CHARTOINT;

    if ((c >= '0') && (c <= '9')) {
        *res = (*res * 16) + (c - '0');
    } else if ((c >= 'a') && (c <= 'f')) {
        *res = (*res * 16) + (c - 'a' + 10);
    } else {
        *res = (-1);
    }

    return;
}

void calcRunningTime(size_t s)
{
    static char *func = UTIL_CALCRUNNINGTIME;
    char        buf[SIZE_64BYTES];
    size_t      h, m;

    h       = (s / HOUR);
    s       %= HOUR;
    m       = (s / MIN);
    s       %= MIN;
    sprintf(buf, "Running time: %02lu:%02lu:%02lu", h, m, s);
    /*  putsForDebug(__UTIL__, func, buf, flag);    */
    putsForDebug(__UTIL__, func, buf, DEBUG_ON);

    return;
}

/*
    flag 추가, -1: 무조건 미출력, 0: 사용자 정의에 따른 출력, 1: 무조건 출력
    일단 FLAG는 파일 상에서 수정하고, makefile 정리가 되면 make에서 분기할 수 있도록 수정
    
    # undef func
    #endif
    #define func
*/
void putsForDebug(char *file, char *func, char *msg, size_t f)
{
    /*
     * buf          : puts에 사용할 문자열 배열, 화면에 출력할 메세지를 담는 배열
     * p            : buffer를 가리키는 포인터 
     * temp         : buffer에 추가할 문자열의 길이를 잠시 담는 변수
     * len          : buf의 길이
     */

    char    *buf;
    char    *p;
    size_t  temp, len;
    
    if (!f) {   /*  if f is 0, ignore debug message */  /*  if f is 0, ignore debug message */  /*  if f is 0, ignore debug message */  /*  if f is 0, ignore debug message */

        return;
    }
    /*  
     * 출력을 보기 좋게 하기 위하여 file, func 문자열 길이를 고정 길이로 변경
     * 문자열이 너무 긴 경우 적정 길이에서 무시되도록 구현
     * 구분문자(':', ' ') 삽입하여 가독성을 높였음
     */
    len = SIZE_DEBUG_LABEL;     
    len += strlen(msg);
    buf = (char *) malloc(len + 1); /*  NULL 문자 삽입 위한 공간 위해 1 추가 할당   */
    p = buf;
    
    /*  
     * 초기 할당 메모리엔 쓰레기 값이 들어 있으므로 memset으로 고정길이 부분 초기화
     * msg 부분은 msg 길이만큼 추가로 할당 되므로 초기화할 필요 없음
     */
    memset(p, ' ', SIZE_DEBUG_LABEL);
    memcpy(p, file, (temp = strlen(file)));
    memcpy((p + temp), func, strlen(func));
    p += (SIZE_DEBUG_LABEL - 2);    /*  ':', ' ' 추가 위해 2 빼기   */
    *p++ = ':';
    *p++ = ' ';
    memcpy(p, msg, (temp = strlen(msg)));
    p += temp;
    *p = '\0';
    puts(buf);
    free(buf);  /*  메모리 해제 */

    return;
}   
