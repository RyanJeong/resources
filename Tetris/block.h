#ifndef __BLOCK_H__
#define __BLOCK_H__

/*  J, I, L, O, S, T, Z */

char blockJ[4][4][4] = {
    0, 1, 0, 0,     0, 1, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,
    1, 0, 0, 0,     1, 1, 1, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 0, 0,     1, 0, 0, 0,     1, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 1, 0,     0, 0, 1, 0,     0, 0, 0, 0,     0, 0, 0, 0};
char blockI[4][4][4] = {
    1, 1, 1, 1,     0, 0, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0, 
    0, 0, 0, 1,     0, 0, 0, 1,     0, 0, 0, 1,     0, 0, 0, 1,
    0, 0, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,     1, 1, 1, 1,
    1, 0, 0, 0,     1, 0, 0, 0,     1, 0, 0, 0,     1, 0, 0, 0};
char blockL[4][4][4] = {
    1, 0, 0, 0,     1, 0, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,
    1, 1, 1, 0,     1, 0, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    0, 1, 1, 0,     0, 0, 1, 0,     0, 0, 1, 0,     0, 0, 0, 0,
    0, 0, 0, 0,     0, 0, 1, 0,     1, 1, 1, 0,     0, 0, 0, 0};

char blockO[4][4][4] = {
    1, 1, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 0, 0,     1, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0};

char blockS[4][4][4] = {
    1, 0, 0, 0,     1, 1, 0, 0,     0, 1, 0, 0,     0, 0, 0, 0,
    0, 1, 1, 0,     1, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    0, 1, 0, 0,     0, 1, 1, 0,     0, 0, 1, 0,     0, 0, 0, 0,
    0, 0, 0, 0,     0, 1, 1, 0,     1, 1, 0, 0,     0, 0, 0, 0};

char blockT[4][4][4] = {
    1, 0, 0, 0,     1, 1, 0, 0,     1, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 1, 0,     0, 1, 0, 0,     0, 0, 0, 0,     0, 0, 0, 0,
    0, 0, 1, 0,     0, 1, 1, 0,     0, 0, 1, 0,     0, 0, 0, 0,
    0, 0, 0, 0,     0, 1, 0, 0,     1, 1, 1, 0,     0, 0, 0, 0};

char blockZ[4][4][4] = {
    0, 1, 0, 0,    1, 1, 0, 0,      1, 0, 0, 0,     0, 0, 0, 0,
    1, 1, 0, 0,    0, 1, 1, 0,      0, 0, 0, 0,     0, 0, 0, 0,
    0, 0, 1, 0,    0, 1, 1, 0,      0, 1, 0, 0,     0, 0, 0, 0,
    0, 0, 0, 0,    1, 1, 0, 0,      0, 1, 1, 0,     0, 0, 0, 0};
#endif
