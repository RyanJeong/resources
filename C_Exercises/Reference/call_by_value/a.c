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
