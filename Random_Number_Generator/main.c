/*  Random Number Generator: 
 *  Generates and exports a random list of numbers to a file.
 *  Usage: ./number NUMBER LIMIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILENAME "numbers.txt"

enum {
    NUM = 1,
    LIM = 2,
    ARGC = 3
};

int main(int argc, char *argv[])
{
    FILE    *fp;
    int      num, lim, i;

    if (argc != ARGC) {
        printf("%s"
                "Usage   : .number NUMBER LIMIT\n",
                (argc > ARGC) ? "Too many arguments supplied.\n" : "");
    } else {
        num = atoi(argv[NUM]);
        lim = atoi(argv[LIM]);
        srand(time(NULL));
        fp = fopen(FILENAME, "w");
        for (i = 0; i < num; ++i) {
            fprintf(fp, "%d ", rand() % lim);
        }
        fclose(fp);
    }

    return 0;
}
