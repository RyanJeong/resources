#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>     /*  srand(), rand() */
#include <time.h>       /*  time(NULL)  */

#define LOTTO_SIZE  7

int main()
{
    /*
     *  cnt     : count, 로또 번호를 총 몇 번 생성할 것인지 보관하는 변수
     *  num     : rand() 함수를 거쳐 생성된 임의의 값을 보관하는 변수
     *  c       : getchar() 결과 값을 임시 보관하는 변수
     *  i, j    : 반복문에서 사용하는 제어 변수
     */
    int cnt, num, c, i, j;

    puts("*** LOTTO NUMBER GENERATOR ***");
    puts("How many lines do you want to generate?");
    for (cnt = 0; ((c = getchar()) != EOF); ) {
        if (isdigit(c)) {
            cnt = (cnt * 10) + (c - '0');
        } else if (cnt && isspace(c)) {
            break;
        } else if (!isspace(c)) {
            if (!(cnt && c == '.')) {
                cnt = 0;
            }
            break;
        }
    }
    if (cnt) {
        for (i = 1; i <= cnt; ++i) {
            /*  Initialize the seed.(Exited before 1 second ends)   */
            srand(time(NULL) + i);  
            printf("Index [%4d]:", i);
            for (j = 1; j <= LOTTO_SIZE; ++j) {
                num = (rand() % 45) + 1;
                if (j == LOTTO_SIZE) {
                    putchar('|');
                }
                printf("\t%2d", num);
            }
            putchar('\n');
        }
    } else {
        printf("Please input a positive number.\n");
    }

    return 0;
}
