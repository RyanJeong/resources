#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraylist.h"

ArrayList *createArrayList(int size)
{
/*
 * pArrayList : Pointer to the array list which memory allocated.
 */
    ArrayList *ptrArrayList;

    ptrArrayList = NULL;
    if (size > 0) {
        ptrArrayList = (ArrayList *) malloc(sizeof(ArrayList));
        if (ptrArrayList != NULL) {
            ptrArrayList->maxNodeCount  = size;
            ptrArrayList->currentNodeCount = 0;
            ptrArrayList->ptrNode      = (ArrayListNode *) malloc(sizeof(ArrayListNode) * size);
            if (ptrArrayList->ptrNode != NULL) {
                memset(ptrArrayList->ptrNode, 0, sizeof(ArrayListNode) * size);
            } else {
                printf("[ERROR] Memory allocation failed.\n");
                free(ptrArrayList);
                ptrArrayList = NULL;
            }
        } else {    /* malloc returns NULL if there is insufficient memory available */
            printf("[ERROR] Memory allocation failed.\n");
        }
    } else {
        printf("[ERROR] Maximum number of nodes must be greater than 0.\n");
    }

    return ptrArrayList;
}

void deleteArrayList(ArrayList *ptrArrayList)
{
    if (ptrArrayList != NULL) {
        free(ptrArrayList->ptrNode);
        free(ptrArrayList);
    }
    
    return;
}

int isArrayListFull(ArrayList *ptrArrayList)
{
/*
 * result : Function execution result which can be a FALSE or a TRUE.
 */
    int result;
    
    result = FALSE;
    if (ptrArrayList != NULL) {
        if (ptrArrayList->currentNodeCount == ptrArrayList->maxNodeCount) {
            result = TRUE;
        }
    }

    return result;
}

int isArrayListEmpty(ArrayList *ptrArrayList)
{
/*
 * result : Function execution result which can be a FALSE or a TRUE.
 */
    int result;

    result = FALSE;
    if (ptrArrayList != NULL) {
        if (ptrArrayList->currentNodeCount == 0) {
            result = TRUE;
        }
    }

    return result;
}

int addNode(ArrayList *ptrArrayList, int position, ArrayListNode node)
{
/*
 * result: Function execution result which can be a SUCCESS or a FAILURE.
 * i     : A loop control variable.
 * count : the value of the member called currentNodeCount from the struct that ptrArrayList points to.
 */
    int result, i, count;
    
    result = FAILURE;
    count  = getArrayListLength(ptrArrayList);
    if (ptrArrayList != NULL) {
        if (isArrayListFull(ptrArrayList) == FALSE) {
            if (position >= 0 && position <= count) {   /* <= count */
                for (i = count-1; i >= position; i--) {
                    ptrArrayList->ptrNode[i+1] = ptrArrayList->ptrNode[i];
                }
                ptrArrayList->ptrNode[position] = node;
                ptrArrayList->currentNodeCount++;
                result = SUCCESS;
            } else {
                printf("[ERROR] The position [%d] is out of range.\n"
                       "Specify a position between 0 and %d.\n", 
                       position, count);
            }
        } else {
            printf("[ERROR] The array list is full.\n");
        }
    }

    return result;
}

int addNodeFirst(ArrayList *ptrArrayList, ArrayListNode node)
{
    
    return addNode(ptrArrayList, 0, node);
}

int addNodeLast(ArrayList *ptrArrayList, ArrayListNode node)
{

    return addNode(ptrArrayList, getArrayListLength(ptrArrayList), node);
}

int removeNode(ArrayList *ptrArrayList, int position)
{
/*
 * result : Function execution result which can be a SUCCESS or a FAILURE.
 * i      : A loop control variable.
 * count  : the value of the member called currentNodeCount from the struct that ptrArrayList points to.
 */
    int result, i, count;

    result = FAILURE;
    count  = getArrayListLength(ptrArrayList);
    if (ptrArrayList != NULL) {
        if (isArrayListEmpty(ptrArrayList) == FALSE) {
            if (position >= 0 && position < count) {   /* < count */
                for (i = position; i < count-1; i++) {
                    ptrArrayList->ptrNode[i] = ptrArrayList->ptrNode[i+1];
                }
                ptrArrayList->currentNodeCount--;
                result = SUCCESS;
            } else {
                printf("[ERROR] The position [%d] is out of range.\n"
                       "Specify a position between 0 and %d.\n", 
                       position, count-1);
            }
        } else {
            printf("[ERROR] The array list is empty.\n");
        }
    }
    
    return result;
}

void removeAll(ArrayList *ptrArrayList)
{
    if (ptrArrayList != NULL) {
        ptrArrayList->currentNodeCount = 0;
    }

    return;
}

int getArrayListLength(ArrayList *ptrArrayList)
{
/*
 * result: Function execution result which can be a 0 or current number of nodes.
 */
    int result;
    
    result = 0;
    if (ptrArrayList != NULL) {
        result = ptrArrayList->currentNodeCount;
    }

    return result;
}

int getArrayListCapacity(ArrayList *ptrArrayList)
{
/*
 * result: Function execution result which can be a 0 or current number of nodes.
 */
    int result;

    result = 0;
    if (ptrArrayList != NULL) {
        result = ptrArrayList->maxNodeCount;
    }

    return result;
}

ArrayListNode *getNode(ArrayList *ptrArrayList, int position)
{
/*
 * ptrNode : An address of node to be returned.
 * count   : the value of the member called currentNodeCount from the struct that ptrArrayList points to.
 */
    ArrayListNode *ptrNode;
    int           count;

    ptrNode = NULL;
    count   = getArrayListLength(ptrArrayList);
    if (ptrArrayList != NULL) {
        if (isArrayListEmpty(ptrArrayList) == FALSE) {
            if (position >= 0 && position < count) {   /* < count */
                ptrNode = &(ptrArrayList->ptrNode[position]);
            } else {
                printf("[ERROR] The position [%d] is out of range.\n"
                       "Specify a position between 0 and %d.\n", 
                       position, count-1);
            }
        } else {
            printf("[ERROR] The array list is empty.\n");
        }
    }

    return ptrNode;
}

void printArrayList(ArrayList *ptrArrayList)
{
/*
 * i        : A loop control variable.
 * count    : the value of the member called currentNodeCount from the struct that ptrArrayList points to.
 * maxCount : the value of the member called maxNodeCount from the struct that ptrArrayList points to.
 */
    int i, count, maxCount;

    count    = getArrayListLength(ptrArrayList);
    maxCount = getArrayListCapacity(ptrArrayList);
    if (ptrArrayList != NULL) {
        printf("Maximum number of nodes: %d\n"
               "Current number of nodes: %d\n", 
               ptrArrayList->maxNodeCount,
               ptrArrayList->currentNodeCount);
        for (i = 0; i < count; i++) {
            printf("[index: %3d], %d\n", i, getNode(ptrArrayList, i)->data);
        }
        while (i < maxCount) {
            printf("[index: %3d], EMPTY\n", i++);
        }
    } else {
        printf("Create an array list before execute this function.\n");
    }

    return;
}
