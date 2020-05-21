#ifndef __CARD_H__
#define __CARD_H__

enum {
    CARD_SIZE = 52,
    RANK_SIZE = 13,
    SUIT_SIZE = 4,

    DEUCE = 0,
    TRHEE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,

    CLUBS = 0,
    DIAMONDS,
    HEARTS,
    SPADES
};

typedef int Card;

int   rank(Card);
int   suit(Card);
void  showCard(Card c);
void  testCard(void);

#endif
