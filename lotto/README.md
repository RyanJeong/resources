* Lotto Number Generator, 1st Version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOTTO_SIZE  7

int main()
{
    /*
     *  cnt     : count, 로또 번호를 총 몇 번 생성할 것인지 보관하는 변수
     *  num     : rand() 함수를 거쳐 생성된 임의의 값
     *  i, j    : 반복문에서 사용하는 제어 변수
     */
    int cnt, num, i, j;

    puts("*** LOTTO NUMBER GENERATOR ***");
    puts("How many lines do you want to generate?");
    scanf("%d", &cnt);
    /*  flush buffer    */
    getchar();
    for (i = 0; i < cnt; ++i) {
        /*  Initialize the seed.(Exited before 1 second ends)   */
        srand(time(NULL) + i);  
        printf("Index [%3d]:", i);
        for (j = 0; j < LOTTO_SIZE; ++j) {
            num = (rand() % 45) + 1;
            if (j == (LOTTO_SIZE - 1)) {
                putchar('|');
            }
            printf("\t%2d", num);
        }
        putchar('\n');
    }

    return 0;
}
```

* Lotto Number Generator, 2nd Version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHECK_SIZE  45
#define LOTTO_SIZE  7

int main()
{
    /*
     *  check[] : 로또 번호 생성 시 값 중복을 검사하기 위해 사용하는 배열
     *  cnt     : count, 로또 번호를 총 몇 번 생성할 것인지 보관하는 변수
     *  num     : rand() 함수를 거쳐 생성된 임의의 값
     *  i, j    : 반복문에서 사용하는 제어 변수
     */
    int check[CHECK_SIZE];
    int cnt, num, i, j;

    puts("*** LOTTO NUMBER GENERATOR ***");
    puts("How many lines do you want to generate?");
    scanf("%d", &cnt);
    /*  flush buffer    */
    getchar();
    for (i = 1; i <= cnt; ++i) {
        /*  Initialize the array.  */
        for (j = 0; j < CHECK_SIZE; ++j) {
            check[j] = 0;
        }
        /*  Initialize the seed.(Exited before 1 second ends)   */
        srand(time(NULL) + i);  
        printf("Index [%4d]:", i);
        for (j = 1; j <= LOTTO_SIZE; ++j) {
            /*  Escape duplicate number    */
            while (check[(num = (rand() % CHECK_SIZE))]) {
                ;
            }
            check[num++]++;
            if (j == LOTTO_SIZE) {
                putchar('|');
            }
            printf("\t%2d", num);
        }
        putchar('\n');
    }

    return 0;
}
```

* Lotto Number Generator, 3rd Version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHECK_SIZE  45
#define LOTTO_SIZE  7

void getCount(int *);
void genLotto(int);

int main()
{
    /*
     *  cnt     : count, 로또 번호를 총 몇 번 생성할 것인지 보관하는 변수
     */
    int cnt;

    getCount(&cnt);
    genLotto(cnt);

    return 0;
}

void getCount(int *cnt)
{
    puts("*** LOTTO NUMBER GENERATOR ***");
    puts("How many lines do you want to generate?");
    scanf("%d", cnt);
    /*  flush buffer    */
    getchar();

    return;
}

void genLotto(int cnt)
{
    /*
     *  check[] : 로또 번호 생성 시 값 중복을 검사하기 위해 사용하는 배열
     *  num     : rand() 함수를 거쳐 생성된 임의의 값
     *  i, j    : 반복문에서 사용하는 제어 변수
     */
    int check[CHECK_SIZE];
    int num, i, j;

    for (i = 1; i <= cnt; ++i) {
        /*  Initialize the array.  */
        for (j = 0; j < CHECK_SIZE; ++j) {
            check[j] = 0;
        }
        /*  Initialize the seed.(Exited before 1 second ends)   */
        srand(time(NULL) + i);  
        printf("Index [%4d]:", i);
        for (j = 1; j <= LOTTO_SIZE; ++j) {
            /*  Escape duplicate number    */
            while (check[(num = (rand() % CHECK_SIZE))]) {
                ;
            }
            check[num++]++;
            if (j == LOTTO_SIZE) {
                putchar('|');
            }
            printf("\t%2d", num);
        }
        putchar('\n');
    }

    return;
}
```

```Text
*** LOTTO NUMBER GENERATOR ***
How many lines do you want to generate?
10
Index [   1]:   43   4   3  36  25  11| 39
Index [   2]:   29   5  26  34  24   6| 10
Index [   3]:   41  24   4  26  36  28|  1
Index [   4]:   38  23   9  30  36  33| 21
Index [   5]:   38  25   2  45  11   7| 16
Index [   6]:   43  42   1  33  41  26|  9
Index [   7]:    1  21  26  32   2  45| 14
Index [   8]:   22  14   6   7  34  28| 21
Index [   9]:    3  42  25  26  19   5| 36
Index [  10]:   15  26   9  25   5  17| 41
```
