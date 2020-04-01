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
                    printf("Played: %3d, Win rate: %.2f%% \n", (int) cnt, (win / cnt) * 100.0);
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
