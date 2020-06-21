## 코어파일 분석하기
* 코어파일은 충돌할 당시 프로세스의 메모리 이미지를 덤프한 것이다.<br>
코어 파일을 gdb와 함께 사용하여 프로그램의 상태를 조사하고 실패 원인을 규명할 수 있다.<br>
어떤 예기치 않은 일이 발생하여 비정상적인 종료가 발생할 때 운영체제는 디스크에 코어 파일을 남긴다.<br>
메모리에 관한 문제는 Checker 패키지를 사용하여 예방할 수 있으나, 메모리 fault를 일으키는 경우에는 충돌하면서 파일을 덤프한다.<br>
코어 파일은 일반적으로 프로세스를 실행시킨 현재 작업 디렉토리에 생성되지만 프로그램 내에서 작업 디렉토리를 바꾸는 경우도 있다.<br>

* 보통 리눅스는 부팅 시 코어 파일을 만들지 않도록 세팅되어 있다. 코어 파일을 만들도록 설정하고자 한다면 셀의 내장 명령을 사용한다.<br>
1. 만약 C쉘 호환 쉘(tcsh)을 쓰고 있는 경우
```Bash
$ limit core unlimited
```
2. 만약 본 쉘류(sh, bash, zsh, pdksh)를 쓰고 있는 경우
```Bash
$ ulimit –c unlimited

# 현재 설정값을 보고자 하는 경우
$ ulimit -a
```
* 코어 파일을 함께 사용하고자 하는 경우, 아래와 같이 한다.
```Bash
$ gdb PROGRAM_NAME core
```

## 실행 중인 프로그램 디버깅하기
* gdb는 이미 실행중인 프로그램 디버깅이 가능하다.<br>
프로세스 실행을 가로채고 조사한 뒤 다시 원래 상태로 실행시킬 수 있다.<br>
attach 명령을 사용하여 실행 중인 프로세서에 gdb를 붙인다.<br>
attach 명령을 사용하기 위해서는 프로세스에 해당하는 실행 프로그램에 허가권을 가지고 있어야 한다.<br>

* 예를 들어 프로세스 ID 254번으로 실행 중인 pgmseq 프로그램이 있다면 다음과 같이 한다.
```Bash
$ gdb pgmseq
$ attach 254

# 다음과 같이 해도 동일하게 동작한다.
$ gdb pgmseq 254
```
* 일단 gdb가 실행 중인 프로세스에 부착되면 프로그램을 일시 중지 시키고 gdb명령을 사용할 수 있도록 제어권을 가져온다.<br>
break를 사용하여 중지점을 사용할 수 있고 중지점에 이를 때까지 실행하도록 continue 명령을 사용할 수 있다.<br>
detach 명령을 사용하면 gdb를 실행 중인 프로세스로부터 떼어낼 수 있다.<br>
필요에 따라 다른 프로세스에 대하여 attach 명령을 사용할 수 있다.<br>

## gdb 실행 방법
```Bash
# run이라는 프로그램을 디버깅하고자 할 때,
# 1. gdb를 먼저 실행한 후에, file 명령을 사용하여 run을 부른다.
$ gdb

# 2. 가장 일반적인 방법
$ gdb run

# 3. 코어 파일을 사용할 때 동시에 인자로 준다.
$ gdb run core

# 4. 실행 중인 프로세스를 디버그하고자 한다면, 프로세스 ID를 두 번째 인자로 준다. 
# 이 명령은 1234란 이름의 파일이 없다면 gdb를 프로세스 1234에 접속시킨다. 
$ gdb run 1234
```
* 실행 절차
```Bash
$ gcc -o -g run main.c
$ gdb run

# gdb를 실행 시키면 아래와 같은 안내 메세지가 출력된다.
GNU gdb (Ubuntu 8.1-0ubuntu3.2) 8.1.0.20180409-git
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from run...(no debugging symbols found)...done.
```

## GDB 명령어
| 명령여 | 설명 |
|-------|-----|
| list | 현재 위치에서 소스 파일의 내용을 10줄 보여준다.<br> **list 2, 15** : 소스 파일의2 ~ 15 까지를 보여준다.|
| run | 프로그램을 시작한다.(break가 있다면 break까지 실행)<br> **run arg** : 새로운 인수를 가지고 프로그램을 시작한다.<br>arg는 “*”나 “[…]”를 포함할 수도 있다.<br>쉘의 사용까지도 확장될 수 있다.<br>또한 “<”, “>”, “>>” 같은 입출력 방향 재지정기호도 허용된다. |
| break | 특정 라인이나 함수에 정지점을 설정한다.<br>**break function** : 현재 파일 안의 함수 function에 정지점을 설정한다.<br>**break file:function** : 파일 file안의 function에 정지점을 설정한다.<br>**watch** : 감시점 설정(감시점은 어떤사건이 일어날 때에만 작동한다)<br>**until** : until 명령을 쓰면, 반복문이 아닌 경우에는 next 명령과 똑같이 동작한다. 반복문일 경우, 현재 스택 프레임(즉, 현재 함수) 안에서, 현재 줄 다음 줄에 올 때까지 프로그램을 실행한다. |
| clear | 특정 라인이나 함수에 있던 정지점을 삭제한다. |
| delete | 몇몇 정지점이나 자동으로 출력되는 표현을 삭제한다. |
| next | 다음 행을 수행한다. 서브루틴을 호출하면서 계속 수행한다.<br>호출이 발생하지 않으면 step와 같다.<br>**next n** : 이를 n번 수행하라는 의미 |
| step | 한 줄씩 실행 시킨다.<br>함수를 포함하고 있으면 함수 내부로 들어가서 한 줄씩 실행시킨다. |
| print | **print expr** : 수식의 값을 보여준다.|
| display | 현재 display된 명령의 목록을 보여준다. |
| bt | 프로그램 스택을 보여준다. (backtrace) |
| kill | 디버깅 중인 프로그램의 실행을 취소한다. |
| file | **file program** : 디버깅할 프로그램으로서 파일을 사용한다. |
| cont | **continue** : 현재 위치에서 프로그램을 계속 실행한다.|
| help | 명령에 관한 정보를 보여주거나 일반적인 정보를 보여준다.|
| quit | gdb를 종료한다.|

## 기타 기능
* Breakpoint
  * 중지점을 조건적으로 설정할 수 있다. 즉 어떤 동작이 참일 때만 작동하도록 할 수 있다.
  ```Bash
  break 184 if (stace == 0)
  ```
  * **info break**를 사용하면 모든 중지점과 감시점 목록을 보여 주고 그 상태도 보여 준다.
  * **disable**을 사용하여 작동불능으로 할 수 있고, **enable**을 사용하여 가능하게 할 수도 있다.

* 인스트럭션 레벨 디버깅
  * gdb를 통해 인스트럭션 레벨의 디버깅을 할 수 있으므로 프로그램의 매우 깊은 곳까지 디버깅할 수 있다.
  ```Bash
  # disass FUNCTION: FUNCTION 함수에 대한 디스어셈블리
  # 위에서 사용했던 sum.c 파일 내에 정의된 sum 함수를 디스어셈블 해보면,
  (gdb) disass sum
  Dump of assembler code for function sum:
   0x00000000000007fc <+0>:	push   %rbp
   0x00000000000007fd <+1>:	mov    %rsp,%rbp
   0x0000000000000800 <+4>:	mov    %edi,-0x4(%rbp)
   0x0000000000000803 <+7>:	mov    %esi,-0x8(%rbp)
   0x0000000000000806 <+10>:	mov    -0x4(%rbp),%edx
   0x0000000000000809 <+13>:	mov    -0x8(%rbp),%eax
   0x000000000000080c <+16>:	add    %edx,%eax
   0x000000000000080e <+18>:	pop    %rbp
   0x000000000000080f <+19>:	retq   
  End of assembler dump.
  
  # display/ i $pc: 현재의 인스트럭션을 보여줌
  # $pc는 gdb내부 변수로서 현재 인스트럭션의 위치를 가리키는 프로그램 카운터(PC)
  # sum.c 파일을 사용하여 예를 들면, 
  (gdb) list
  1	#include <stdio.h>
  2	
  3	int sum(int x, int y);
  4	
  5	int main(void)
  6	{
  7		int a, b, result;
  8		printf("정수 a, b를 차례로 입력하세요.");
  9		scanf("%d %d", &a, &b);
  10		getchar();	//	enter key
  (gdb) 
  11		result = sum(a, b);
  12		printf("\n%d + %d = %d\n", a, b, result);
  13	
  14		return 0;
  15	}
  16	
  17	int sum(int x, int y)
  18	{
  19	
  20	    return (x + y);
  (gdb) 
  21	}
  (gdb) b 11
  Breakpoint 1 at 0x7b3: file sum.c, line 11.
  (gdb) display/i $pc
  1: x/i $pc
  <error: No registers.>
  (gdb) r
  Starting program: /root/run 
  정수 a, b를 차례로 입력하세요.3 5

  Breakpoint 1, main () at sum.c:11
  11		result = sum(a, b);
  1: x/i $pc
  => 0x5555555547b3 <main+73>:	mov    -0x10(%rbp),%edx
  (gdb) record
  (gdb) n
  12		printf("\n%d + %d = %d\n", a, b, result);
  1: x/i $pc
  => 0x5555555547c5 <main+91>:	mov    -0x10(%rbp),%edx
  (gdb) rn

  No more reverse-execution history.
  main () at sum.c:11
  11		result = sum(a, b);
  1: x/i $pc
  => 0x5555555547b3 <main+73>:	mov    -0x10(%rbp),%edx
  (gdb) s
  sum (x=3, y=5) at sum.c:20
  20	    return (x + y);
  1: x/i $pc
  => 0x555555554806 <sum+10>:	mov    -0x4(%rbp),%edx
  (gdb) rs

  No more reverse-execution history.
  main () at sum.c:11
  11		result = sum(a, b);
  1: x/i $pc
  => 0x5555555547b3 <main+73>:	mov    -0x10(%rbp),%edx
  ```

### a. example 1
```C
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
```
```Bash
# -g 옵션을 꼭 넣어 컴파일할 것
$ gcc -g -o run a.c

$ ./run
j is 0.000000
j is 1.000000   # 이 부분이 원래 의도와 다르게 동작함, 디버깅 필요
j is 3.000000
j is 4.000000
j is 6.000000

$ gdb run

# gdb의 list 명령은 10줄씩 출력함
(gdb) list
1	#include <stdio.h>
2	
3	int main()
4	{
5	  int     i;
6	  double  j;
7	  
8	  /*  
9	   * 다음은 (i / 2) + i의 값을 출력하는 부분이다.
10	   * i가 1이면 j는 1.5가 되어야 하지만 실제로는 그렇지 않다.
(gdb) 
11	   */
12	  for (i = 0; i < 5; ++i) {
13	    j = (i / 2) + i;
14	    printf("j is %lf\n", j);
15	  }
16	  
17	  return 0;
18	}
(gdb) 
Line number 19 out of range; test.c has 18 lines.

# break 12 : for 문에 이상이 있다고 판단하여 line 12에 breakpoint를 잡는다.
(gdb) b 12  

# run : breakpoint까지 실행된다.
(gdb) r
Starting program: /root/run 

Breakpoint 1, main () at test.c:12
12	  for (i = 0; i < 5; ++i) {

# step : 한 줄 실행시킨다.
(gdb) s
13	    j = (i / 2) + i;
(gdb) s
14	    printf("j is %lf\n", j);

# print j : j의 값을 본다.
(gdb) p j
$1 = 0

# next: 한 줄 실행시킨다.
# next는 함수 호출시, 해당 함수를 한 줄로 처리하여 함수를 수행하고, 
# step은 함수 호출시, 함수 루틴 안으로 들어간다.
(gdb) n
j is 0.000000
12	  for (i = 0; i < 5; ++i) {
(gdb) display i
1: i = 0
(gdb) display j
2: j = 0

(gdb) n
13	    j = (i / 2) + i;
1: i = 1
2: j = 0
(gdb) n
14	    printf("j is %lf\n", j);
1: i = 1
2: j = 1
(gdb) n
j is 1.000000
12	  for (i = 0; i < 5; ++i) {
1: i = 1
2: j = 1

# 어느 부분에서 초기 의도와 다르게 동작하는지 확인 했다면, quit 명령을 사용해 gdb 종료
(gdb) quit
A debugging session is active.

	Inferior 1 [process 27589] will be killed.

Quit anyway? (y or n) y
```

### b. example 2
```C
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
```
```Bash
# 이 프로그램은 이상은 없고, 다만 스택을 보기 위한 것이다. 
# 여러 곳에서 호출되는 함수 안에서 충돌이 일어날 경우를 생각해 보자. 
# 이때는 함수가 어디로부터 호출되었는지, 그리고 어떤 상황에서 충돌이 일어났는지 파악하고자 할 것이다.
# backtrace(bt) 명령을 이용하면 충돌이 일어난 시점에서 프로그램의 현재 호출 스택(call stack) 상태를 볼 수 있다. 
# 호출 스택은 현재 함수까지 이르는 호출 목록이다. 
# 함수를 호출할 때마다 보관된 레지스터 값, 함수 전달 인수, 지역 변수 등의 자료를 스택에 push한다. 
# 이렇게 해서 각 함수들은 스택상에 일정 공간을 차지한다. 
# 특정함수에 대하여 스택에서 사용되고 있는 메모리 부분을 스택 프레임(frame)이라 부르며, 
# 호출 스택은 이러한 스택 프레임을 순서대로 정렬한 목록이다.
$ gcc -g -o run b.c
$ gdb run
(gdb) list
1	#include <stdio.h>
2	
3	int sum(int x, int y);
4	
5	int main(void)
6	{
7		int a, b, result;
8		printf("정수 a, b를 차례로 입력하세요.");
9		scanf("%d %d", &a, &b);
10		getchar();	//	enter key
(gdb) 
11		result = sum(a, b);
12		printf("\n%d + %d = %d\n", a, b, result);
13	
14		return 0;
15	}
16	
17	int sum(int x, int y)
18	{
19	
20	    return (x + y);
(gdb) 
21	}
(gdb) 
Line number 22 out of range; sum.c has 21 lines.

# breaking point를 소스코드 11번째 줄로 지정함
(gdb) b 11
Breakpoint 1 at 0x7b3: file sum.c, line 11.

# breaking point까지 소스코드 실행
(gdb) r
Starting program: /root/run 
정수 a, b를 차례로 입력하세요.3 5

Breakpoint 1, main () at sum.c:11
11		result = sum(a, b);

# backtrace: 현재 스택에 main이 있음
(gdb) bt
#0  main () at sum.c:11

# 한 단계 실행, step이므로 함수로 분기
(gdb) s
sum (x=3, y=5) at sum.c:20
20	    return (x + y);

# backtrace: 현재 스택에 sum이 있음
(gdb) bt
#0  sum (x=3, y=5) at sum.c:20
#1  0x00005555555547c2 in main () at sum.c:11

# 스택 프레임 0번으로 이동(sum())
(gdb) frame 0
#0  sum (x=3, y=5) at sum.c:20
20	    return (x + y);

# 상위 스택 프레임(즉, 스택 프레임 0번을 호출한 곳)으로 이동 
(gdb) up
#1  0x00005555555547c2 in main () at sum.c:11
11		result = sum(a, b);

# 하위 스택 프레임으로 이동
(gdb) down
#0  sum (x=3, y=5) at sum.c:20
20	    return (x + y);

# 스택 프레임 1번으로 이동(main())
(gdb) frame 1
#1  0x00005555555547c2 in main () at sum.c:11
11		result = sum(a, b);

# 상위 스택 프레임으로 이동
# main() 함수는 초기 스택 프레임이기 때문에, 상위 스택으로 이동할 수가 없음
(gdb) up
Initial frame selected; you cannot go up.

# 현재 프레임 정보를 나타냄
(gdb) info frame
Stack level 1, frame at 0x7fffffffde90:
 rip = 0x5555555547c2 in main (sum.c:11); saved rip = 0x7ffff7a05b97
 caller of frame at 0x7fffffffde60
 source language c.
 Arglist at 0x7fffffffde80, args: 
 Locals at 0x7fffffffde80, Previous frame's sp is 0x7fffffffde90
 Saved registers:
  rbp at 0x7fffffffde80, rip at 0x7fffffffde88
  
# 하위 스택 프레임으로 이동(즉, 호출한 함수로 이동)
(gdb) down
#0  sum (x=3, y=5) at sum.c:20
20	    return (x + y);

(gdb) info frame
Stack level 0, frame at 0x7fffffffde60:
 rip = 0x555555554806 in sum (sum.c:20); saved rip = 0x5555555547c2
 called by frame at 0x7fffffffde90
 source language c.
 Arglist at 0x7fffffffde50, args: x=3, y=5
 Locals at 0x7fffffffde50, Previous frame's sp is 0x7fffffffde60
 Saved registers:
  rbp at 0x7fffffffde50, rip at 0x7fffffffde58

# finish: 해당 함수 종료
(gdb) fin
Run till exit from #0  sum (x=3, y=5) at sum.c:20
0x00005555555547c2 in main () at sum.c:11
11		result = sum(a, b);
Value returned is $1 = 8

# 현재 함수 내에서의 모든 변수의 이름과 값을 출력함
(gdb) info locals
a = 3
b = 5
result = 32767
(gdb) s
12		printf("\n%d + %d = %d\n", a, b, result);
(gdb) info locals
a = 3
b = 5
result = 8

# 해당 프로그램의 정보를 출력함
(gdb) info program
	Using the running image of child process 30598.
Program stopped at 0x5555555547c5.
It stopped after being stepped.
Type "info stack" or "info registers" for more information.

# a 변수의 저장 위치를 나타냄
# a는 스택 프레임 시작지점으로부터 36 바이트 아래에 위치함
(gdb) info address a
Symbol "a" is a complex DWARF expression:
     0: DW_OP_fbreg -36
.
# b는 스택 프레임 시작지점으로부터 32 바이트 아래에 위치함
(gdb) info address b
Symbol "b" is a complex DWARF expression:
     0: DW_OP_fbreg -32
.
# c는 스택 프레임 시작지점으로부터 28 바이트 아래에 위치함
(gdb) info address result
Symbol "result" is a complex DWARF expression:
     0: DW_OP_fbreg -28
.
(gdb) q
A debugging session is active.

	Inferior 1 [process 30994] will be killed.

Quit anyway? (y or n) y
```

### c. example 3
```C
#include <stdio.h>
#include <string.h>

int main()
{
	char *bug = NULL;
	
	strcpy(bug, "DEBUG");
	printf("buf is %s\n", bug);

	return 0;
}
```
```Bash
$ gcc -g -o run c.c
$ ./run
세그멘테이션 오류 (core dumped) # 해당 디렉토리에 core파일 생성됨
$ gdb run
(gdb) list
1	#include <stdio.h>
2	#include <string.h>
3	
4	int main()
5	{
6		char *bug = NULL;
7		
8		strcpy(bug, "DEBUG");
9		printf("buf is %s\n", bug);
10	
(gdb) b 8
Breakpoint 1 at 0x65a: file seg.c, line 8.
(gdb) r
Starting program: /root/run 

Breakpoint 1, main () at seg.c:8
8		strcpy(bug, "DEBUG");

# print bug를 해 보면 bug의 번지가 0x0으로 되어있음을 확인할 수 있음
# bug에 다른 번지수를 지정해야 함
(gdb) p bug
$1 = 0x0
(gdb) s

Program received signal SIGSEGV, Segmentation fault.
0x000055555555465e in main () at seg.c:8
8		strcpy(bug, "DEBUG");
(gdb) q
A debugging session is active.

	Inferior 1 [process 30994] will be killed.

Quit anyway? (y or n) y
```
