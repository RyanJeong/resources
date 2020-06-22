# Variadic Functions<br>
* Variadic functions are functions (e.g. printf) which take a variable number of arguments.<br>

##  Macros<br>
### va_start<br>
* enables access to variadic function arguments **(function macro)**<br>

### va_arg<br>
* accesses the next variadic function argument **(function macro)**<br>

### va_end<br>
* ends traversal of the variadic function arguments **(function macro)**<br>

##  Type<br>
### va_list
* holds the information needed by va_start, va_arg, and va_end **(typedef)**<br>

##  Example:<br>
```C
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
```
```Text
Case #1, result: 3
Case #2, result: 3542
Case #3, result: 32354
Case #4, result: 5
```
