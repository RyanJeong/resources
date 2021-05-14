#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *c;

    printf("before: %p\n", c);
    c = (char *) malloc(0);
    printf("after: %p\n", c);
    if (c == NULL) {
        puts("FALSE");
    } else {
        puts("SUCCESS");
    }

    return 0;
}
