```C
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
```
```Text
$./number
Usage   : .number NUMBER LIMIT

$./number 100
Usage   : .number NUMBER LIMIT

$./number 100 2000

$./number 100 2000 30000
Too many arguments supplied.
Usage   : .number NUMBER LIMIT

$cat numbers.txt
1728 167 732 105 1344 607 506 420 1380 270 917 1536 1020 397 399 1754 1738 531 1997 170 1326 1152 1484 1528 1655 1471 1012 1252 803 1778 1005 883 298 90 1340 1642 1049 1847 63 781 469 1332 317 1489 1730 1068 1596 1468 1952 1945 1990 1278 1097 1826 806 1104 1298 170 357 453 1948 1714 1336 246 156 676 241 1205 875 656 339 1344 1988 656 1186 1718 77 782 1539 29 1079 1529 1659 176 1356 817 1281 1006 987 1990 1459 1287 1704 1147 1886 213 175 127 1418 1403
```
