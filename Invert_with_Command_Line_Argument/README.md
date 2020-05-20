```C
/*
 *  Write a program invert(x,p,n) that returns x with the n bits that begin at position p inverted
 *  (i.e., 1 changed into 0 and vice versa), leaving the others unchanged.
 *  Set the values of the variables x, p, and n through the command line arguments.
 *  Example: ./invert 1011 2 3
 *  > 1100
 */

#include <stdio.h>
#include <stdlib.h>

unsigned    invert(unsigned, int, int);
void        printResult(unsigned, int);

enum {
    ARGC = 4
};

int main(int argc, char *argv[])
{
    unsigned int    x, result;
    int             p, n, len;
    char            c;
    
    if (argc != ARGC) {
        printf("%s"
                "Usage   : ./invert x p n\n"
                "Example : ./invert 1011 2 3\n"
                "> 1100\n",
                (argc > ARGC) ? "Too many arguments supplied.\n" : "");
    } else {
        for (len = x = 0, ++argv; (c = *argv[0]); ++argv[0]) {
            x = (x << 1) + (c - '0');
            ++len;
        }
        p       = atoi(*++argv);
        n       = atoi(*++argv);
        result  = invert(x, p, n);
        printResult(result, len);
        putchar('\n');
    }

    return 0;
}

unsigned invert(unsigned x, int p, int n)
{
    
    return (x ^ (~(~0 << n) << (p + 1 - n)));
}

void printResult(unsigned u, int len)
{
    if (len > 0 || u) {
        printResult((u >> 1), --len);   
        putchar((u & 01) + '0');
    }

    return;
}
```
```Text
$./invert
Usage   : ./invert x p n
Example : ./invert 1011 2 3
> 1100

$./invert 1011
Usage   : ./invert x p n
Example : ./invert 1011 2 3
> 1100
$./invert 1011 2
Usage   : ./invert x p n
Example : ./invert 1011 2 3
> 1100
$./invert 1011 2 3
1100
$./invert 1011 2 3 4
Too many arguments supplied.
Usage   : ./invert x p n
Example : ./invert 1011 2 3
> 1100
$./invert 00001111 7 2
11001111
```
