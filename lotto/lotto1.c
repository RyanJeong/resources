#include <ctype.h>
#include <stdio.h>
#include <time.h>

/*  123 -1 1a2s a1s2 0.123 1.23 ' '' '123 */
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
        /*  Do something    */
        printf("%d\n", cnt);
    } else {
        printf("Please input a positive number.\n");
    }

    return 0;
}
