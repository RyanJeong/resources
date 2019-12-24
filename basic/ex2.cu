#include <stdio.h>

__global__ void KernelFunction(int a, int b, int c)
{
    int sum;

    sum = (a + b + c);

    return;
}

int main()
{
    /*  <<<BLOCK, THREAD>>>, total threads: (BLOCK * THREAD)    */
    KernelFunction<<<6, 6>>>(1, 2, 3);
    puts("Successfully called threads");

    return 0;
}
