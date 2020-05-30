#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

enum {
    FALSE, 
    TRUE,

    SUCCESS = 0,
    FAILURE
};

typedef struct {
    int data;
} ArrayListNode;

typedef struct {
    int           maxNodeCount;     /* Maximum number of nodes */
    int           currentNodeCount; /* Current number of nodes */
    ArrayListNode *ptrNode;         /* A one-dimensional array for storing nodes */
} ArrayList;

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
void          printArrayList(ArrayList *);
#endif
