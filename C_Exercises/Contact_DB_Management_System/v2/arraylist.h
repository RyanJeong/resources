#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

#include "node.h"

enum {
    FALSE, 
    TRUE,

    SUCCESS = 0,
    FAILURE
};

ArrayList     *createArrayList(int);
void          deleteArrayList(ArrayList *);
int           isArrayListFull(ArrayList *);
int           isArrayListEmpty(ArrayList *);
int           addNode(ArrayList *, int, ArrayListNode);
int           addNodeFirst(ArrayList *, ArrayListNode);
int           addNodeLast(ArrayList *, ArrayListNode);
int           removeNode(ArrayList *, int);
void          removeAll(ArrayList *);
int           getArrayListLength(ArrayList *);
int           getArrayListCapacity(ArrayList *);
ArrayListNode *getNode(ArrayList *, int);
# ifdef MY_CONTROL_MACRO
void          printArrayList(ArrayList *);
# endif
#endif
