#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/*
#define CHECK_SIZE  10
#define INNINGS     9
#define NUM_SIZE    3
*/

enum {
    CHECK_SIZE  = 10,   /*  중복 검사에 사용하는 배열의 갯수    */
    INNINGS     = 9,    /*  야구 이닝(inning) 수                */
    NUM_SIZE    = 3     /*  맞추어야 할 숫자의 갯수             */
}; 

/*
 *  void initGame(void);
 *      프로그램 최초 동작 시 초기화 또는 준비 동작을 수행하는 함수
 */
void initGame(void);

/*
 *  void playGame(void);
 *      숫자 야구 게임의 동작이 구현되어 있는 함수
 */
void playGame(void);

/*
 * void genNumbers(int []);
 *      3개의 랜덤 번호를 생성하는 함수
 */
void genNumbers(int []);

/*
 * void getNumbers(int []);
 *      플레이어로부터 3개의 번호를 입력받는 함수
 */
void getNumbers(int []);

int main()
{
    initGame();
    playGame();

    return 0;
}

void initGame(void)
{
    /*  Initialize the seed.    */
    srand(time(NULL));
    printf("*** Number Baseball Game ***\n"
           "You have 9 chances to find out the right answer with 10 numbers(0 - 9).\n");

    return;
}

void playGame(void)
{
    /*
     *  strike      : strike 갯수
     *  ball        : ball 갯수
     *  i, j, k     : 반복문 제어 변수
     *  temp        : 플레이어가 입력한 숫자를 임시로 저장하는 변수
     *  computer[]  : 컴퓨터가 생성한 정수
     *  player[]    : 플레이어가 생성한 정수
     */
    int strike, ball, i, j, k, temp;
    int computer[NUM_SIZE], player[NUM_SIZE];

    genNumbers(computer);
    for (i = 1; i <= INNINGS; ++i) {
        strike = ball = 0;
        printf("Input 3 numbers without duplicates.\n");
        getNumbers(player);
        for (j = 0; j < NUM_SIZE; ++j) {
            /*
             *  Computer    : 1 4 7
             *  Player      : 2 3 4
             */
            if (computer[j] == player[j]) {
                ++strike;
            } else if ((computer[j] == player[(j + 1)%NUM_SIZE]) || (computer[j] == player[(j + 2)%NUM_SIZE])) {
                ++ball;
            }
        }
        printf("[%d%s attempt] Strike%s: %d, Ball%s: %d\n", 
               i, 
               (i == 1) ? "st" : ((i == 2) ? "nd" : ((i == 3) ? "rd" : "th")), 
               (strike == 1) ? "" : "s", 
               strike, 
               (ball == 1) ? "" : "s", 
               ball);
        if (strike == NUM_SIZE) {
            break;
        }
    }
    printf("Computer's random numbers are %d, %d, and %d\n"
           "%s\n",
           computer[0], 
           computer[1], 
           computer[2], 
           (i > INNINGS) ? "You lose!" : "You win!");

    return;
}

void genNumbers(int c[])
{
    /*
     *  i       : 반복문 제어 변수
     *  num     : 난수 발생 후 생성되는 값을 저장하는 변수
     *  check   : computer[] 값 중복을 방지하기 위해 사용하는 배열
     */
    int i, num;
    int check[CHECK_SIZE];

    /*  Init. the array */
    for (i = 0; i < CHECK_SIZE; ++i) {
        check[i] = 0;
    }
    for (i = 0; i < NUM_SIZE; ++i) {
        while (check[num = (rand() % CHECK_SIZE)]) {
            ;
        }
        check[num]  = 1;
        c[i]        = num;
    }

    return;
}

void getNumbers(int p[])
{
    /*
     *  i       : 반복문 제어 변수
     *  c       : getchar() 값을 임시로 저장하는 변수
     *  check   : computer[] 값 중복을 방지하기 위해 사용하는 배열
     */
    int i, c;
    int check[CHECK_SIZE];

    /*  Init. the array */
    for (i = 0; i < CHECK_SIZE; ++i) {
        check[i] = 0;
    }
    /*  i < NUM_SIZE 조건이 우선임    */
    for (i = 0; (i < NUM_SIZE) && ((c = getchar()) != EOF); ) {
        if (!isspace(c)) {
            if (isdigit(c)) {
                c -= '0';
                check[c] ? printf("Number %d has already been entered. Please enter a different number\n", c) : (check[c] = 1, p[i++] = c);
            } else {
                puts("Please enter only numbers(0 - 9).");
            }            
        }
    } 

    return;
}
