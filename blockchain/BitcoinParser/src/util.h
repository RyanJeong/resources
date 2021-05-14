#ifndef __UTIL_H__
#define __UTIL_H__

typedef struct {
    uint8_t     data[64];
    uint32_t    datalen;
    uint64_t    bitlen;
    uint32_t    state[8];
} SHA256_CTX;

#define __UTIL__                "[util.c]"
#define UTIL_TOLITTLEENDIAN     "[toLittleEndian(uint8_t *, size_t)] "
#define UTIL_HEXTOBIN           "[hexToBin(uint8_t *, uint8_t *, size_t] "
#define UTIL_BINTOHEX           "[binToHex(uint8_t *, uint8_t *, size_t] "
#define UTIL_SHA256             "[*sha256(uint8_t *, size_t] "
#define UTIL_SHA256_INIT        "[sha256_init(SHA256_CTX *)] "
#define UTIL_SHA256_UPDATE      "[sha256_update(SHA256_CTX *, const uint8_t *, size_t] "
#define UTIL_SHA256_FINAL       "[sha256_final(SHA256_CTX *, uint8_t *)] "
#define UTIL_SHA256_TRANSFORM   "[sha256_transform(SHA256_CTX *, const uint8_t *] "
#define UTIL_VARINT             "[varInt(uint8_t *)] "
#define UTIL_VARINTTEST         "[varIntTest(void)] "
#define UTIL_CHARTOINT          "[charToInt(int *, char)] "
#define UTIL_CALCRUNNINGTIME    "[calcRunningTime(size_t)] "

#define VARINT_TEST1    "12"
#define VARINT_TEST2    "fd1234"
#define VARINT_TEST3    "fe12345678"
#define VARINT_TEST4    "ff1234567890af0000"

#define HOUR    3600
#define MIN     60

#define DEBUG_ON        1
#define DEBUG_OFF       0


//  SHA256 Macros
#define SHA256_BLOCK_SIZE   32            // SHA256 outputs a 32 byte digest
#define ROTLEFT(a, b)       (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b)      (((a) >> (b)) | ((a) << (32 - (b))))
#define CH(x, y, z)         (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z)        (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)              (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x)              (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x)             (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x)             (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))
/*
 * size_t toLittleEndian(uint8_t *, size_t);
 * : store string from big endian to little endian;
*/
size_t toLittleEndian(uint8_t *, size_t);

/*
 * size_t hexToBin(uint8_t *, uint8_t *, size_t);
 * : convert from hex string to bin array
 */
size_t hexToBin(uint8_t *, uint8_t *, size_t);

/*
 * size_t binToHex(uint8_t *, uint8_t *, size_t);
 * : convert from bin array to hex string
 */
size_t binToHex(uint8_t *, uint8_t *, size_t);

/*
 * uint8_t *sha256(uint8_t *, size_t);
 * : secure hash algorithm(SHA256) original code by Angel Marin, Paul Johnston.
 */
uint8_t *sha256(uint8_t *, size_t);

/*
 * void sha256_init(SHA256_CTX *);
 * : 암호화 관련 정보를 설정한다. (암호화 방식, 키 길이, 비밀 번호 등)
 */
void sha256_init(SHA256_CTX *);

/*
 * void sha256_update(SHA256_CTX *, const uint8_t *, size_t);
 * : 설정한 암호화 방식으로 블럭을 분할해 암호화를 수행한다. 
 */
void sha256_update(SHA256_CTX *, const uint8_t *, size_t);

/*
 * void sha256_final(SHA256_CTX *, uint8_t *);
 * : 입력한 plain text의 크기가 블럭의 배수가 아닐 경우 데이터 끝에 여분의 데이터 바이트가 남게 되는 해당 바이트를 패딩하여 처리 가능한 크기의 블럭으로 만든 다음 암호화를 수행한다.
 */
void sha256_final(SHA256_CTX *, uint8_t *);

/*
 * void sha256_transform(SHA256_CTX *, const uint8_t *)
 * : secure hash algorithm(SHA256) original code by Angel Marin, Paul Johnston.
 */
void sha256_transform(SHA256_CTX *, const uint8_t *);

/*
 * void putsForDebug(char *, char *, char *, size_t);
 * : Debug message를 출력하는 함수
 */
void putsForDebug(char *, char *, char *, size_t);

/*
 * int64_t varInt(uint8_t *);
 * : calcurate varInt, if occurred error, then return negative value
 */
int64_t varInt(uint8_t *);

/*
 * void varIntTest(void);
 * : run varInt test
 */
void varIntTest(void);

/*
 * void charToInt(int *, char);
 * : convert a character into integer
 */
void charToInt(int64_t *, char);

/*
 * void calcRunningTime(size_t)
 * : measure a runtime
 */
void calcRunningTime(size_t);
#endif
