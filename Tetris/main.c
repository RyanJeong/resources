#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "game.h"
#include "print.h"

int main()
{
    int choice;

    while ((choice = getChoise()) > 0) {
        switch (choice) {
        case START_GAME:
            break;
        case PRINT_GAME_MANUAL:
            break;
        case PRINT_GAME_RESULT:
            break;
        default:
            break;
        }
    }

    return 0;
}

int getChoise(void)
{
    int c;

    while (1) {
		system("clear");
        puts("");
        puts("");
        puts("");
        puts("\t\t\t*** Tetris ***");
        puts("");
        puts("");
        puts("\t\t\tGame Menu");
        puts("");
        puts("\t\t\t[1] Game Start");
        puts("\t\t\t[2] Game Manual");
        puts("\t\t\t[3] Ranking");
        puts("\t\t\t[4] Exit Game");
        while ((c = getchar()) != EOF) {
            switch (c) {
            case '1':
            case '2':
            case '3':
            case '4':
                break;
            defualt:
                break;
            }
        }
        
        return (c == '4') ? EXIT_GAME : c;
    }

    return 0;
}
