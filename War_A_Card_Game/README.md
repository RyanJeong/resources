# War! A Card Game<br>
```Text
# How to run?
$ make
$ ./war
```
##  Goals:<br>
* Practice with linked lists and pointers.<br>
* Appreciate the central role played by data structures.<br>
* Learn how to design a "large" program.<br>
* Learn how to read a "large" program.<br>
    
##  Basics
* Players: 2<br>
* Deck: Standard 52-card deck<br>
   * 2-3-4-5-6-7-8-9-10-J-Q-K-A<br>
   * S->H->D->C(Card suit doesn't matter in this game.)<br>
* Goal: To win all of the cards<br>

##  Setup<br>
* Shuffle the deck and deal the cards so that each player gets 26 cards. Players keep their cards in front of them, face down.<br>

##  Gameplay<br>
* Each player simultaneously turns over their top card. The higher card wins the pair; the winning player takes both cards and puts them at the bottom of their face-down pile. (Ace is high card; 2 is low card.)<br>
* In the event of a tie, the players have a "war." Each player places three cards face down in the middle of the table and then turns a fourth card face up. The player with the higher of these cards takes all 10 cards which are now in the middle. If these two cards are also a tie, additional cards are turned face-up, one at a time, until one player wins; that player takes the entire set of cards.<br>
   * Before turn start:<br>
      * Player 1: 10D 9S 8D KH 7D 5H 6S<br>
      * Player 2: 10H 7H 5C QC 2C 4H 6D<Br>
   * After turn start:<br>
      * player 1: 5H 6S 10D 9S 8D KH 7D 10H 7H 5C QC 2C<br>
      * player 2: 4H 6D<br>

##  Winning<br>
* A player wins when he/she has all the cards.<br>

##  Main routine in the program<br>
* Create the deck of cards.<br>
* Shuffle the cards.<br>
* Deal the cards.<br>
* Play the game.<br>

##  Represent datas<br>
* The cards.<br>
   * Represent 52 cards using an integer between 0 and 51.<br>
* The deck.<br>
   * Use a linked list to represent the deck and hands.<br>
* The hands.<br>
   * Use printf() method for debugging<br>
