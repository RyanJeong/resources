#include <stdio.h>

#include "card.h"

int rank(Card c)
{

    return c % RANK_SIZE;
}

int suit(Card c)
{

    return (c % CARD_SIZE) / RANK_SIZE;
}

void showCard(Card c)
{
    static char *ranks[] = {
        "Deuce of ",
        "Three of ",
        "Four of ",
        "Five of ",
        "Six of ",
        "Seven of ",
        "Eight of ",
        "Nine of ",
        "Ten of ",
        "Jack of ",
        "Queen of ",
        "King of ",
        "Ace of "};
    static char *suits[] = {
        "Clubs",
        "Diamonds",
        "Hearts",
        "Spades"};
    printf("%s%s\n", ranks[rank(c)], suits[suit(c)]);

    return;
}

void testCard(void)
{
    Card c;

    for (c = 0; c < CARD_SIZE; ++c) {
        showCard(c);
    }

    return;
}
