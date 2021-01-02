#include <stdio.h>

int sum(int, int);

int main(void)
{
	int a, b, result;

	printf("정수 a, b를 차례로 입력하세요.");
	scanf("%d %d", &a, &b);
    /*  flush buffer    */
	getchar();  
	result = sum(a, b);
	printf("\n%d + %d = %d\n", a, b, result);

	return 0;
}

int sum(int x, int y)
{

    return (x + y);
}
