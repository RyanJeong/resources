* Rock Paper Scissors, 1st Version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*  isspace()   */
#include <ctype.h>

#define ROCK        0
#define PAPER       1
#define SCISSORS    2
#define SHAPES      3
#define HELP_MSG    "r: Rock, p: Paper, s: Scissors, h: Help, e: Exit"

int main()
{
    /*
     *  c       : character buffer
     *  shape   : 상대방의 손 모양
     *  cnt     : 가위바위보 총 시행 횟수
     *  win     : 가위바위보 시행 중 이긴 횟수
     */
    int     c, shape;
    double  cnt, win;

    cnt = win = 0;
    /*  Initialize the seed.    */
    srand(time(NULL));
    puts("*** ROCK PAPER SCISSORS ***");
    puts("r: Rock, p: Paper, s: Scissors, h: Help, e: Exit");
    while ((c = getchar()) != EOF) {
        if (!isspace(c)) {
            shape = (rand() % SHAPES);
            switch (c) {
            case 'r':
                ++cnt;
                printf("Player: [ROCK], Computer: ");
                if (shape == ROCK) {
                    puts("[ROCK], Draw!");
                } else if (shape == PAPER) {
                    puts("[PAPER], Computer wins!");
                } else {
                    puts("[SCISSORS], Player wins!");
                    ++win;
                }
                break;
            case 'p':
                ++cnt;
                printf("Player: [PAPER], Computer: ");
                if (shape == PAPER) {
                    puts("[PAPER], Draw!");
                } else if (shape == SCISSORS) {
                    puts("[SCISSORS], Computer wins!");
                } else {
                    puts("[ROCK], Player wins!");
                    ++win;
                }
                break;
            case 's':
                ++cnt;
                printf("Player: [SCISSORS], Computer: ");
                if (shape == SCISSORS) {
                    puts("[SCISSORS], Draw!");
                } else if (shape == ROCK) {
                    puts("[ROCK], Computer wins!");
                } else {
                    puts("[PAPER], Player wins!");
                    ++win;
                }
                break;
            case 'h':
                puts("r: Rock, p: Paper, s: Scissors, h: Help, e: Exit");
                break;
            case 'e':
                puts("Exit the game");
                if (cnt) {
                    printf("Played: %3d, Win rate: %.2lf%% \n", (int) cnt, (win / cnt) * 100.0);
                }
                break;
            default:
                puts("Not a choise");
                break;
            }
            if ((c == 'e')) {
                break;
            }
        }
    }

    return 0;
}
```

* Rock Paper Scissors, 2nd Version
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*  isspace()   */
#include <ctype.h>

#define ROCK        0
#define PAPER       1
#define SCISSORS    2
#define SHAPES      3
#define HELP_MSG    "r: Rock, p: Paper, s: Scissors, h: Help, e: Exit"

int main()
{
    /*
     *  c       : character buffer
     *  shape   : 상대방의 손 모양
     *  cnt     : 가위바위보 총 시행 횟수
     *  win     : 가위바위보 시행 중 이긴 횟수
     */
    int     c, shape;
    double  cnt, win;

    cnt = win = 0;
    /*  Initialize the seed.    */
    srand(time(NULL));
    puts("*** ROCK PAPER SCISSORS ***");
    puts(HELP_MSG);
    while ((c = getchar()) != EOF) {
        if (!isspace(c)) {
            shape = (rand() % SHAPES);
            switch (c) {
            case 'r':
            case 'p':
            case 's':
                ++cnt;
                c = ((c == 'r') ? ROCK : ((c == 'p') ? PAPER : SCISSORS));
                printf("Player: [%s], Computer: [%s], %s\n",
                    (c == ROCK) ? "ROCK" : ((c == PAPER) ? "PAPER" : "SCISSORS"),
                    (shape == ROCK) ? "ROCK" : ((shape == PAPER) ? "PAPER" : "SCISSORS"),
                    (c == shape) ? "Draw!" : ((((c + 1) % SHAPES) == shape) ? ++win, "Computer wins!" : "Player wins!")
                );                
                break;
            default:
                puts((c == 'h') ? HELP_MSG : ((c == 'e') ? "Exit the game" : "Not a choise"));
                if (c == 'e') {
                    printf("Played: %3d, Win rate: %.2lf%% \n", (int) cnt, (win / cnt) * 100.0);
                }
                break;
            }
            if (c == 'e') {
                break;
            }
        }
    }

    return 0;
}
```

```Text
*** ROCK PAPER SCISSORS ***
r: Rock, p: Paper, s: Scissors, h: Help, e: Exit
r
Player: [ROCK], Computer: [ROCK], Draw!
r
Player: [ROCK], Computer: [SCISSORS], Player wins!
r
Player: [ROCK], Computer: [ROCK], Draw!
r
Player: [ROCK], Computer: [ROCK], Draw!
r
Player: [ROCK], Computer: [ROCK], Draw!
p
Player: [PAPER], Computer: [ROCK], Player wins!
p
Player: [PAPER], Computer: [PAPER], Draw!
p
Player: [PAPER], Computer: [ROCK], Player wins!
s
Player: [SCISSORS], Computer: [SCISSORS], Draw!
s
Player: [SCISSORS], Computer: [ROCK], Computer wins!
s
Player: [SCISSORS], Computer: [PAPER], Player wins!
1
Not a choise
q
Not a choise
h
r: Rock, p: Paper, s: Scissors, h: Help, e: Exit
e
Exit the game
Played:  11, Win rate: 9.09%
```
