```C
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

```
```Text
A histogram is an approximate representation of the distribution of numerical or categorical data. It was first introduced by Karl Pearson.[1] To construct a histogram, the first step is to "bin" (or "bucket") the range of values—that is, divide the entire range of values into a series of intervals—and then count how many values fall into each interval. The bins are usually specified as consecutive, non-overlapping intervals of a variable. The bins (intervals) must be adjacent, and are often (but not required to be) of equal size.[2]

If the bins are of equal size, a rectangle is erected over the bin with height proportional to the frequency—the number of cases in each bin. A histogram may also be normalized to display "relative" frequencies. It then shows the proportion of cases that fall into each of several categories, with the sum of the heights equaling 1.
 85 |                                                                    *                         
 84 |                                                                    *                         
 83 |                                                                    *                         
 82 |                                                                    *                         
 81 |                                                                    *                         
 80 |                                                                    *                         
 79 |                                                                    *                         
 78 |                                                                    *                         
 77 |                                                                    *                         
 76 |                                                                    *                         
 75 |                                                                    *                         
 74 |                                                                    *                         
 73 |                                                                    *                         
 72 |                                                                    *                         
 71 |                                                                    *                         
 70 |                                                                    *                         
 69 |                                                                    *                         
 68 |                                                                    *                         
 67 |                                                                    *                         
 66 |                                                                    *                         
 65 |                                                                    *              *          
 64 |                                                                    *              *          
 63 |                                                                    *              *          
 62 |                                                                *   *              *          
 61 |                                                                *   *              *          
 60 |                                                                *   *              *          
 59 |                                                                *   *              *          
 58 |                                                                *   *              *          
 57 |                                                                *   *              *          
 56 |                                                                *   *              *          
 55 |                                                                *   *   *          *          
 54 |                                                                *   *   *          *          
 53 |                                                                *   *   *          *          
 52 |                                                                *   *   *          *          
 51 |                                                                *   *   *          *          
 50 |                                                                *   *   *     *    *          
 49 |                                                                *   *   *     *    *          
 48 |                                                                *   *   *     *    *          
 47 |                                                                *   *   *     *    *          
 46 |                                                                *   *   *    **  ***          
 45 |                                                                *   *   *    **  ***          
 44 |                                                                *   *   *    **  ***          
 43 |                                                                *   *   *    **  ***          
 42 |                                                                *   *   *    **  ***          
 41 |                                                                *   *   *    **  ***          
 40 |                                                                *   *   *    **  ***          
 39 |                                                                *   *   *    **  ***          
 38 |                                                                *   *   *    **  ***          
 37 |                                                                *   *   *    **  ***          
 36 |                                                                *   *   *    **  ***          
 35 |                                                                *   *   *    **  ***          
 34 |                                                                *   *   *    **  ***          
 33 |                                                                *   *   *    **  ***          
 32 |                                                                *   *   *    **  ***          
 31 |                                                                *   *  **    **  ***          
 30 |                                                                *   *  **    **  ***          
 29 |                                                                *   *  **    **  ***          
 28 |                                                                *   *  **  * **  ***          
 27 |                                                                *   *  **  * **  ***          
 26 |                                                                *   *  **  * **  ***          
 25 |                                                                *   *  **  * **  ***          
 24 |                                                                *   *  **  * **  ***          
 23 |                                                                *   *  **  * **  ***          
 22 |                                                                * * *  **  * **  ****         
 21 |                                                                * * ** **  * **  ****         
 20 |                                                                * * ** **  * **  ****         
 19 |                                                                * * ** **  * **  ****         
 18 |                                                                * * ** **  * **  ****         
 17 |                                                                * * ** **  * **  ****         
 16 |                                                                * * ** **  * **  ****         
 15 |                                                                *** ** **  * **  ****         
 14 |                                                                ****** **  * **  *****        
 13 |                                                                ****** **  * **  *****        
 12 |                                                                *********  * *** *****        
 11 |                                                                *********  ***** *****        
 10 |                                                                *********  ***** *****        
  9 |                                                                *********  ***** *****        
  8 |             *                                                  *********  ***** *****        
  7 |             *                                                  *********  ***** *****        
  6 | *         * *                                                  *********  ***********  *     
  5 | *         * *                                                  *********  ************ *     
  4 | *         * *                                                  *********  ************ *     
  3 | *     **  * *                          *          *            *********  ************ **    
  2 | *     **  * *  *               *       *          *      * *   *********  ************ **    
  1 | *     **  ***  **              *       * *    *   *      * *   **************************    
    +----------------------------------------------------------------------------------------------
     !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
```
