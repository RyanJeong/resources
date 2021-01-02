#include <stdio.h>
#include <stdarg.h>

int findMax(int, int, ...);

int main(void)
{
    printf("Case #1, result: %d\n", findMax(2, 3, -1));
    printf("Case #2, result: %d\n", findMax(1532, 0, 254, 3542, 11, -1));
    printf("Case #3, result: %d\n", findMax(64, 345, 1, 3, 6, 23, 3, 1455, 764, 32354, -1));
    printf("Case #4, result: %d\n", findMax(5, 4, 3, 2, 1, -1));

    return 0;
}

int findMax(int a, int b, ...)
{
    int max, temp;
    va_list ap;
    
    max = (a > b) ? a : b;
    for (va_start(ap, b); (temp = va_arg(ap, int)) >= 0; (max = (temp > max) ? temp : max));
    va_end(ap);

    return max;
}
