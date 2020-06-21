# C_Course
A course on Introduction to Computer Programming
강의를 준비하면서 함께 소개하면 좋을만한 내용들을 정리해놓은 repo.

## Index
* [scanf 함수 원형]()
* [call by reference vs. call by value]()
* [배열 연산과 포인터 연산 중 어느 것이 빠른가]()
* [전위연산과 후위연산 중 어느 것이 빠른가]()
* [integer 변수가 왜 가장 동작 속도가 빠른가]()
* [rand, srand]()
* [buffer, getchar, putchar]()
* [int->scanf("%d", &i), int * -> scanf("%d", i)]()
* [TIME function, UNIX TIME]()
* [디버거 사용법]()
* [쉘 종류]()
* [입력 버퍼 동작 방식]()
* [각 연산자에 따른 속도(클럭 소비)]()
* [전위연산자와 후위연산자 중 빠른 것이 무엇이고, %연산 속도는 어떠한가?]()
* [비트마스킹]()
* [word, int, long 바이트 수 결정되는 방법]()
* [부동소수점, 고정소수점 계산 방법]()
* [재귀함수, 꼬리재귀 설명]()
* [하드웨어 동작 방식 설명(게이트 부터 어떻게 메모리까지 올라가는가)]()
* [각 언어의 패러다임 소개 및 설명(OT)]()
* [바이너리 코드 -> 어셈블리 언어 -> C언어, 컴파일, 빌드, 링킹 등 각 용어에 대한 정확한 설명 필요(2번째 수업)]()
* [시간복잡도, 공간복잡도 간단 설명]()
* [GCC vs. CLANG, 각 차이가 무엇인가?]()
* [do {} while(0), #ifndef, .... 등 여러 테크닉 정리]()
* [object, rvalue, lvalue, UB, side effect, sequence point]()

```C
#include <stdio.h>

int main(void)
{
    float f1, f2, f3, f4;
    double d1, d2, d3, d4;

    f1  = 0.1234567890;
    f2  = 1234567890.0;
    f3  = 0.9999999;
    f4  = 9999999.0;
    d1  = 0.12345678901234567890;
    d2  = 12345678901234567890.0;
    d3  = 0.999999999999999;
    d4  = 999999999999999.0;

    printf("Size of float: %ld bytes\n", sizeof(float));
    printf("Size of double: %ld bytes\n", sizeof(double));
    printf("Size of long double: %ld bytes\n", sizeof(long double));
    printf(
        "f1: %0.10f\n"
        "f2: %10.0f\n"
        "f3: %0.7f\n"
        "f4: %7.0f\n",
        f1, f2, f3, f4);
    printf(
        "d1: %0.20f\n"
        "d2: %20.0f\n"
        "d3: %0.15f\n"
        "d4: %15.0f\n",
        d1, d2, d3, d4);

    return 0;
}
```


