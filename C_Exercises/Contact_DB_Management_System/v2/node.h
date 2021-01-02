#ifndef __NODE_H__
#define __NODE_H__

enum {
    NAME_SIZE = 32,
    PHONE_NUMBER_SIZE = 32
};

typedef struct {
    char name[NAME_SIZE];
    char phone_number[PHONE_NUMBER_SIZE];
} ArrayListNode;

typedef struct {
    int           maxNodeCount;     /* Maximum number of nodes */
    int           currentNodeCount; /* Current number of nodes */
    ArrayListNode *ptrNode;         /* A one-dimensional array for storing nodes */
} ArrayList;

#endif

