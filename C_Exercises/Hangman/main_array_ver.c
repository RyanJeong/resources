/*
 *  Keywords in C Programming
 *  -----------------------------
 *  auto        break       case        char
 *  const       continue    default     do
 *  double      else        enum        extern
 *  float       for         goto        if
 *  int         long        register    return
 *  short       signed      sizeof      static
 *  struct      switch      typedef     union
 *  unsigned    void        volatile    while
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> /*  for functions related with string   */
#include <time.h>   /*  for random number generation        */
#include <ctype.h>

#define TITLE       "*** Hangman ***"
#define BLANK       "        "
#define GALLOWS     "    +---"
#define HEAD        "    O   "
#define UPPER       "   /|\\  "
#define BODY        "    |   "
#define LOWER       "   / \\  "

enum {
    ATTEMPTS    = 10,
    STATE_INIT  = 0,
    MAX_SIZE    = 32
};

/*
 *  int prepareGame(char [], char []);
 *  ----------------
 *  Prepare the game
 *  return the target word's length
 */
int prepareGame(char [], char []);

/*
 *  int playGame(char **, char **, int);
 *  ----------------
 *  Implement the rules of hangman game
 *  Return number of misses guesses
 */
int playGame(char [], char [], int);

/*
 *  void drawHangman(int);
 *  ----------------
 *  Draw part of the hangman when the players guess misses
 */
void drawHangman(int);

int main(void)
{
/*
 *  word: 맞추어야 하는 단어를 저장하는 변수
 *  form: word의 글자 수만큼 '*' 문자를 저장하는 변수
 *  len : word의 길이를 저장하는 변수
 */
    char    word[MAX_SIZE], form[MAX_SIZE];
    int     len;

    if ((len = prepareGame(word, form)) <= 0) {
        printf("An occurred error while preparing the game.\n");
    
        return 1;
    }
    printf("Answer: %s\n"
           "You %s!\n", 
           word, 
           (playGame(word, form, len) < ATTEMPTS) ? "win!" : "lose!");

    return 0;
}

int prepareGame(char word[], char form[])
{
/*
 *  words   : C언어의 keyword들을 보관하는 변수 
 *  index   : keyword들 중 하나를 가리키기 위해 사용하는 인덱스
 *  len     : 특정 keyword의 단어 길이
 */
    int         index, len;
    static char *words[] = {
        "auto", "beark", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if", 
        "int", "long", "register", "return", 
        "short", "signed", "sizeof", "static", 
        "struct", "switch", "typedef", "union", 
        "unsigned", "void", "volatile", "while"
    };

    drawHangman(STATE_INIT);
    srand(time(NULL));
    index   = (rand() % (sizeof(words) / sizeof(char *)));
    len     = strlen(*(words + index));
    strcpy(word, (words[index]));
    memset(form, '*', len);
    form[len] = '\0';
    
    return len;
}

int playGame(char word[], char form[], int len)
{
/*
 *  misses      : 단어 추측에 실패한 횟수
 *  i           : 반복문 제어 변수
 *  c           : getchar() 결과를 보관하는 변수
 *  check       : 사용자로부터 입력받은 문자가 정답 내 포함 여부를 저장하는 변수
 *  remains     : 정답까지 남은 문자 갯수
 *  listOfChar  : 단어 추측에 실패한 문자 목록
 */
    int     misses, i, c, check, remains;
    char    listOfChar[ATTEMPTS];

    misses  = 0;
    remains = len;
    while ((misses < ATTEMPTS) && remains) {
        printf("Hint: %s, Input a character.\n"
               "Misses: %d / %d\n",
               form, misses, ATTEMPTS);
        for (i = 0; i < misses; ++i) {
            printf("%c ", listOfChar[i]);
        }
        putchar('\n');
        while ((c = getchar()) != EOF) {
            if (!isspace(c)) {
                if (isalpha(c)) {
                    if (isupper(c)) {
                        /*  Handle offset   */
                        c += ('a' - 'A');
                    }
                    for (check = i = 0; i < len; ++i) {
                        if (word[i] == c && form[i] == '*') {
                            form[i] = c;
                            ++check;
                            --remains;
                        }
                    }
                    /*  Miss    */
                    if (!check) {
                        listOfChar[misses++] = c;
                    }
                    drawHangman(misses);
                    break;
                } else {
                    printf("Wrong input!\n");
                }
            }
        }
    }

    return misses;
}

void drawHangman(int misses)
{
#ifndef _WIN32
    system("clear");
#else
    system("cls");
#endif
    printf("%s\n"
           "%s%c\n"
           "%s%c\n"
           "%s%c\n"
           "%s%c\n"
           "%s%c\n",
           "*** Hangman ***",
           (misses > 5) ? GALLOWS : BLANK, (misses > 4) ? '+' : ' ',
           (misses > 6) ? HEAD : BLANK,    (misses > 3) ? '|' : ' ',
           (misses > 7) ? UPPER : BLANK,   (misses > 2) ? '|' : ' ',
           (misses > 8) ? BODY : BLANK,    (misses > 1) ? '|' : ' ',
           (misses > 9) ? LOWER : BLANK,   (misses > 0) ? '|' : ' ');

    return;
}
