#include <ctype.h>  /*  isdigit(), isspace()    */
#include <stdio.h>
#include <stdlib.h> /*  srand(), rand() */
#include <time.h>   /*  time(NULL)  */

#define LOTTO_SIZE  7   /*  6 + '1' */
#define MAX_NUM     45  /*  1 ~ 45  */

int main()
{
    /*
     *  cnt     :   로또 번호를 총 몇 번 생성할 것인지 값을 보관하는 변수
     *  c       :   getchar() 함수의 수행 결과 값을 임시로 보관하는 변수
     *  num     :   rand() 함수의 반환값을 보관하는 변수
     *  i, j, k :   반복문에서 사용하는 제어변수
     *  check[] :   로또 번호 생성 시 값 중복을 검사하기 위해 사용하는 변수
     */
    int cnt, c, num, i, j, k, check[LOTTO_SIZE];

    printf("*** LOTTO NUMBER GENERATOR ***\n"
            "How many lines do you want to generate?\n");
    cnt = 0;
    while ((c = getchar()) != EOF) {
        if (isdigit(c)) {
            cnt = (cnt * 10) + (c - '0');
        } else {
            break;
        }
    }
    if (cnt) {  /*  cnt != 0    */
        for (i = 1; i <= cnt; ++i) {
            /*  Initialize the seed.    */
            /*  Exited before 1 second ends */
            srand(time(NULL) + i);
            printf("Index: [%4d]:", i);
            for (j = 0; j < LOTTO_SIZE; ++j) {
regen:
                /*  0 ~ 44 -> 1 ~ 45    */
                num = (rand() % MAX_NUM) + 1;
                for (k = 0; k < j; ++k) {
                    if (check[k] == num) {
                        goto regen;
                    }
                }
                check[k] = num;
                if (j == (LOTTO_SIZE - 1)) {
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
