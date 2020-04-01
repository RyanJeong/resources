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
            /*  0 ~ 44 -> 1 ~ 45    */
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
