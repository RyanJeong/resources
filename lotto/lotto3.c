#include <ctype.h>  /*  isdigit(), isspace()    */
#include <stdio.h>
#include <stdlib.h> /*  srand(), rand() */
#include <time.h>   /*  time(NULL)  */

#define LOTTO_SIZE  7   /*  6 + '1' */
#define MAX_NUM     45  /*  1 ~ 45  */

int main()
{
    /*
     *  cnt     :   �ζ� ��ȣ�� �� �� �� ������ ������ ���� �����ϴ� ����
     *  c       :   getchar() �Լ��� ���� ��� ���� �ӽ÷� �����ϴ� ����
     *  num     :   rand() �Լ��� ��ȯ���� �����ϴ� ����
     *  i, j, k :   �ݺ������� ����ϴ� �����
     *  check[] :   �ζ� ��ȣ ���� �� �� �ߺ��� �˻��ϱ� ���� ����ϴ� ����
     */
    int cnt, c, num, i, j, k, check[MAX_NUM];

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
            for (j = 0; j < MAX_NUM; ++j) {
                check[j] = 0;
            }
            srand(time(NULL) + i);
            printf("Index: [%4d]:", i);
            for (j = 0; j < LOTTO_SIZE; ++j) {
                while (check[num = (rand() % MAX_NUM)]) {
                    ;
                }
                /*  0 ~ 44 -> 1 ~ 45    */
                check[num++] = 1;
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
