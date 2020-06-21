#include <stdio.h>

void test1(int []);
void test2(int *);

int main()
{
    int a[5];
    int i;

    i = 0;
    while (i < 5) {
        a[i] = (i << 2);
        ++i;
    }
    test1(a);
    test2(a);

    return 0;
}

void test1(int p[])
{
    printf("%d %d %d %d %d\n", *(p + 0), *(p + 1), *(p + 2), *(p + 3), *(p + 4));

    return;
}

void test2(int *p)
{
    printf("%d %d %d %d %d\n", p[0], p[1], p[2], p[3], p[4]);

    return;
}
