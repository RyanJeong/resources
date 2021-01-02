### a. Call by value vs. Call by reference
```C
#include <stdio.h>

void incValue(int *, int *);
void incAddr(int *, int *);

int main()
{
    int a, b;

    a = 3;
    b = 5;
    printf("[main()]                    a = %d, &a = %p\n", a, &a);
    printf("[main()]                    b = %d, &b = %p\n", b, &b);
    incValue(&a, &b);
    printf("[main()] after incValue(),  a = %d, &a = %p\n", a, &a);
    printf("[main()] after incValue(),  b = %d, &b = %p\n", b, &b);
    incAddr(&a, &b);
    printf("[main()] after incAddr(),   a = %d, &a = %p\n", a, &a);
    printf("[main()] after incAddr(),   b = %d, &b = %p\n", b, &b);

    return 0;
}

void incValue(int *a, int *b)
{
    printf("[incValue()] before,        *a = %d, a = &a = %p\n", *a, a);
    printf("[incValue()] before,        *b = %d, b = &b = %p\n", *b, b);
    ++(*a);
    ++(*b);
    printf("[incValue()] after,         *a = %d, a = &a = %p\n", *a, a);
    printf("[incValue()] after,         *b = %d, b = &b = %p\n", *b, b);

    return;
}
    

void incAddr(int *a, int *b)
{
    printf("[incAddr()] before,         *a = %d, a = &a = %p\n", *a, a);
    printf("[incAddr()] before,         *b = %d, b = &b = %p\n", *b, b);
    ++a;
    ++b;
    printf("[incAddr()] after,          *a = %d, a = &a = %p\n", *a, a);
    printf("[incAddr()] after,          *b = %d, b = &b = %p\n", *b, b);

    return;
}
```
```
[main()]                    a = 3, &a = 0x7ffe50e8ceb0
[main()]                    b = 5, &b = 0x7ffe50e8ceb4
[incValue()] before,        *a = 3, a = &a = 0x7ffe50e8ceb0
[incValue()] before,        *b = 5, b = &b = 0x7ffe50e8ceb4
[incValue()] after,         *a = 4, a = &a = 0x7ffe50e8ceb0
[incValue()] after,         *b = 6, b = &b = 0x7ffe50e8ceb4
[main()] after incValue(),  a = 4, &a = 0x7ffe50e8ceb0
[main()] after incValue(),  b = 6, &b = 0x7ffe50e8ceb4
[incAddr()] before,         *a = 4, a = &a = 0x7ffe50e8ceb0
[incAddr()] before,         *b = 6, b = &b = 0x7ffe50e8ceb4
[incAddr()] after,          *a = 6, a = &a = 0x7ffe50e8ceb4
[incAddr()] after,          *b = -1807422976, b = &b = 0x7ffe50e8ceb8
[main()] after incAddr(),   a = 4, &a = 0x7ffe50e8ceb0
[main()] after incAddr(),   b = 6, &b = 0x7ffe50e8ceb4Text
```
