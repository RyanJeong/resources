* Number Baseball Game 1st Version
```C
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
```

```Text
*** Number Baseball Game ***
You have 9 chances to find out the right answer with 10 numbers(0 - 9).
Input 3 numbers without duplicates.
1 5 9
[1st attempt] Strike: 1, Ball: 1
Input 3 numbers without duplicates.
2 5 9
[2nd attempt] Strike: 1, Ball: 1
Input 3 numbers without duplicates.
3 9 5
[3rd attempt] Strikes: 0, Balls: 2
Input 3 numbers without duplicates.
3 5 9
[4th attempt] Strike: 1, Ball: 1
Input 3 numbers without duplicates.
4 5 9
[5th attempt] Strike: 1, Ball: 1
Input 3 numbers without duplicates.
5 6 9
[6th attempt] Strikes: 2, Balls: 0
Input 3 numbers without duplicates.
5 7 9
[7th attempt] Strikes: 2, Balls: 0
Input 3 numbers without duplicates.
5 8 9
[8th attempt] Strikes: 3, Balls: 0
Computer's random numbers are 5, 8, and 9
You win!
```

* Number Baseball Game 2nd version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define BUFSIZE 100 

enum {
    CHECK_SIZE  = 10,   /*  중복 검사에 사용하는 배열의 갯수    */
    INNINGS     = 9,    /*  야구 이닝(inning) 수                */
    NUM_SIZE    = 3     /*  맞추어야 할 숫자의 갯수             */
}; 

/* void initGame(void);
    프로그램이 실행되면 최초 한 번 실행되며, 해당 프로그램 실행에 필요한 과정을 처리하는 함수 */
void    initGame(void);

/* void initGame(void);
    숫자 야구 게임의 동작이 구현되어 있는 함수 */
void    playGame(void);

/* void generateNumbers(int []);
    3개의 랜덤 번호를 생성하는 함수 */
void    generateNumbers(int []);

/* int getInt(int *);
    get next integer from input into *pn */
int     getInt(int *);

/* int getch(void);
    get a (possibly pushed-back) character */
int     getch(void);

/* void ungetch(int);
    push character back on input */
void    ungetch(int);

char    buf[BUFSIZE];   /* buffer for ungetch */
int     bufp = 0;       /* next free position in buf */

int main()
{
    initGame();
    playGame();

    return 0;
}

void initGame(void)
{
    srand(time(NULL));  /*  Initialize the seed.    */
    printf("*** Number Baseball Game ***\n"
           "You have 9 chances to find out the right answer with 10 numbers(0 - 9).\n");

    return;
}

void playGame(void)
{
/* Variables
    strike      : strike 갯수
    ball        : ball 갯수
    i, j, k     : 반복문 제어 변수
    temp        : 플레이어가 입력한 숫자를 임시로 저장하는 변수
    computer[]  : 컴퓨터가 생성한 정수
    player[]    : 플레이어가 생성한 정수 */ 
    int strike, ball, i, j, k, temp;
    int computer[NUM_SIZE], player[NUM_SIZE];

    generateNumbers(computer);
    for (i = 1; i <= INNINGS && strike != NUM_SIZE; ++i) {
        strike = ball = 0;
        printf("Input 3 numbers without duplicates.\n");
        for (j = 0; j < NUM_SIZE; ++j) {
            getInt(player + j);
        }
        for (j = 0; j < NUM_SIZE; ++j) {
            if (computer[j] == player[j]) {
                ++strike;
            } else if ((computer[j] == player[(j + 1)%NUM_SIZE]) || (computer[j] == player[(j + 2)%NUM_SIZE])) {
                ++ball;
            }
        }
        printf("[%d%s attempt] %d Strke%s, %d Ball%s\n", 
               i, 
               (i == 1) ? "st" : ((i == 2) ? "nd" : ((i == 3) ? "rd" : "th")), 
               strike, 
               (strike == 1) ? "" : "s", 
               ball,
               (ball == 1) ? "" : "s");
    }
    printf("Computer's random numbers are %d, %d, and %d\n"
           "%s\n",
           computer[0], 
           computer[1], 
           computer[2], 
           (i > INNINGS) ? "You lose!" : "You win!");

    return;
}

void generateNumbers(int c[])
{
/* Variables
    i       : 반복문 제어 변수
    num     : 난수 발생 후 생성되는 값을 저장하는 변수
    check   : computer[] 값 중복을 방지하기 위해 사용하는 배열 */
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

int getch(void) 
{    

    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) 
{    
    if (bufp >= BUFSIZE) {
        printf("ungetch: too many characters \n");
    } else {
        buf[bufp++] = c;
    }

    return;
}
int getInt(int *pn)
{    
    int c, sign;
    
    while (isspace(c = getch())) { /* skip white space */
        ;
    }
    if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
        ungetch(c); /* it is not a number */

        return 0;
    }
    sign = (c == '-') ? (-1) : 1;
    if (c == '+' || c == '-') {
        c = getch();
    }
    for (*pn = 0; isdigit(c); c = getch()) {
        *pn = 10 * *pn + (c - '0');
    }
    *pn *= sign;
    if (c != EOF) {
        ungetch(c);
    }

    return c;
}
```
