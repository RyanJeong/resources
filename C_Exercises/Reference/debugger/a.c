#include <stdio.h>

int main()
{
    int     i;
    double  j;

    /*  
    * 다음은 (i / 2) + i의 값을 출력하는 부분이다.
    * i가 1이면 j는 1.5가 되어야 하지만 실제로는 그렇지 않다.
    */
    for (i = 0; i < 5; ++i) {
        j = (i / 2) + i;
        printf("j is %lf\n", j);
    }

    return 0;
}
