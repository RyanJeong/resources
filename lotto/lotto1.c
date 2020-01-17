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

    return 0;
}
