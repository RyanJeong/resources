* Rock Paper Scissors, 1st Version
```C
#include <ctype.h>  /*  isspace()   */
#include <stdio.h>
#include <stdlib.h> /*  srand(), rand() */
#include <time.h>   /*  time(NULL)  */

/*  #define */
enum {
    ROCK,       /*  0   */
    PAPER,      /*  1   */
    SCISSORS,   /*  2   */
    SHAPES      /*  3   */
};

int main()
{
    /*    
     *  c       : getchar() 결과 값을 보관하는 변수
     *  shape   : 컴퓨터의 손 모양(가위 바위 보 중에 하나)
     *  cnt     : 가위 바위 보 총 시행 횟수
     *  win     : 가위 바위 보 시행 중 이긴 횟수
     */
    int     c, shape;
    double  cnt, win;

    c = 'h';
    cnt = win = 0.0;
    /*  Initialize the seed.    */
    srand(time(NULL));
    printf("*** ROCK PAPER SCISSORS ***\n");
    do {    /*  'r' 'p' 's' 'h' 'e' */
        if (!isspace(c)) {
            shape = (rand() % SHAPES);
            switch (c) {
            case 'r':
                ++cnt;
                printf("Player: [ROCK], Computer: ");
                if (shape == ROCK) {
                    printf("[ROCK], Draw!\n");
                } else if (shape == PAPER) {
                    printf("[PAPER], Computer wins!\n");
                } else {
                    printf("[SCISSORS], Player wins!\n");
                    ++win;
                }
                break;
            case 'p':
                ++cnt;
                printf("Player: [PAPER], Computer: ");
                if (shape == PAPER) {
                    printf("[PAPER], Draw!\n");
                } else if (shape == SCISSORS) {
                    printf("[SCISSORS], Computer wins!\n");
                } else {
                    printf("[ROCK], Player wins!\n");
                    ++win;
                }
                break;
            case 's':
                ++cnt;
                printf("Player: [SCISSORS], Computer: ");
                if (shape == SCISSORS) {
                    printf("[SCISSORS], Draw!\n");
                } else if (shape == ROCK) {
                    printf("[ROCK], Computer wins!\n");
                } else {
                    printf("[PAPER], Player wins!\n");
                    ++win;
                }
                break;
            case 'h':
                printf("r: Rock, p: Paper, s: Scissors, h: Help, e: Exit\n");
                break;
            case 'e':
                printf("Exit the game\n");
                if (cnt) {
                    printf("Played: %3d, Win rate: %.2f%%\n", (int) cnt, (win / cnt) * 100.0);
                }
                break;
            default:
                printf("Not a choice\n");
                break;
            }
            if (c == 'e') {
                break;
            }
        }
    } while ((c = getchar()) != EOF);

    return 0;
}
```

* Rock Paper Scissors, 2nd Version
```C
#include <ctype.h>  /*  isspace()   */
#include <stdio.h>
#include <stdlib.h> /*  srand(), rand() */
#include <time.h>   /*  time(NULL)  */

/*  #define */
enum {
    ROCK,       /*  0   */
    PAPER,      /*  1   */
    SCISSORS,   /*  2   */
    SHAPES      /*  3   */
};

int main()
{
    /*
     *  c       : getchar() 결과 값을 보관하는 변수
     *  shape   : 컴퓨터의 손 모양(가위 바위 보 중에 하나)
     *  cnt     : 가위 바위 보 총 시행 횟수
     *  win     : 가위 바위 보 시행 중 이긴 횟수
     */
    int     c, shape;
    double  cnt, win;

    c = 'h';
    cnt = win = 0.0;
    /*  Initialize the seed.    */
    srand(time(NULL));
    printf("*** ROCK PAPER SCISSORS ***\n");
    do {
        if (!isspace(c)) {
            switch (c) {
            case 'r':
            case 'p':
            case 's':
                shape = (rand() % SHAPES);
                ++cnt;
                c = ((c == 'r') ? ROCK : ((c == 'p') ? PAPER : SCISSORS));
                printf("Player: [%s], Computer: [%s], %s\n",
                    (c == ROCK) ? "ROCK" :
                        ((c == PAPER) ? "PAPER" : "SCISSORS"),
                    (shape == ROCK) ? "ROCK" :
                        ((shape == PAPER) ? "PAPER" : "SCISSORS"),
                    (c == shape) ? "Draw!" :
                        ((((c + 1) % SHAPES) == shape) ? "Computer wins!" : (++win, "Player wins!"))
                );
                break;
            default:
                printf("%s\n",
                    (c == 'h') ? "r: Rock, p: Paper, s: Scissors, h: Help, e: Exit" :
                        ((c == 'e') ? "Exit the game" : "Not a choice"));
                if (c == 'e') {
                    printf("Played: %3d, Win rate: %.2f%%\n", (int) cnt, (win / cnt) * 100.0);
                }
                break;
            }
            if (c == 'e') {
                break;
            }
        }
    } while ((c = getchar()) != EOF);

    return 0;
}
```
