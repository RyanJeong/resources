/*
 *  Write a program to print a histogram 
 *  of the lengths of words in its input. 
 *  Draw the histogram with the bars vertical.
 */
#include <stdio.h>
#include <ctype.h>

enum {
    LOWER = 33,
    UPPER = 126
};

int character[256];

int main(void)
{
    int c, max, temp, i, j;

    max = 0;
    while ((c = getchar()) != EOF) {
        temp    = (character[c]++);
        /*  skip ' ', '\t', '\n', ...   */
        if (!isspace(c)) {
            max     = (temp > max) ? temp : max;
        }
    }
    for (i = 0; i < max; ++i) {
        /*  nth low number  */
        printf("%3d |", max-i);
        for (j = LOWER; j <= UPPER; ++j) {
            putchar(character[j] < max ? (character[j]++, ' ') : '*');
        }
        putchar('\n');
    }
    printf("%s", "    +");
    for (j = LOWER; j <= UPPER; ++j) {
        putchar('-');
    }
    putchar('\n');
    printf("%s", "     ");
    for (j = LOWER; j <= UPPER; ++j) {
        putchar(j);
    }
    putchar('\n');

    return 0;
}

