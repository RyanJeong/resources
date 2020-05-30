#include <stdio.h>

#include "arraylist.h"

int main(void)
{
    ArrayList     *pArrayList;

    pArrayList = createArrayList(6);
    if (pArrayList != NULL) {
        ArrayListNode node;

        node.data = 1;
        addNode(pArrayList, 0, node);
        node.data = 2;
        addNode(pArrayList, 1, node);
        node.data = 0;
        addNodeFirst(pArrayList, node);
        node.data = 4;
        addNodeLast(pArrayList, node);
        printArrayList(pArrayList);
/*
        Output:
        Maximum number of nodes: 6
        Current number of nodes: 4
        [index:   0], 0
        [index:   1], 1
        [index:   2], 2
        [index:   3], 4
        [index:   4], EMPTY
        [index:   5], EMPTY
*/

        removeNode(pArrayList, 1);
        printArrayList(pArrayList);
/*
        Maximum number of nodes: 6
        Current number of nodes: 3
        [index:   0], 0
        [index:   1], 2
        [index:   2], 4
        [index:   3], EMPTY
        [index:   4], EMPTY
        [index:   5], EMPTY
*/
        node.data = 5;
        addNodeFirst(pArrayList, node);
        node.data = 7;
        addNodeLast(pArrayList, node);
        node.data = 9;
        addNode(pArrayList, 3, node);
        printArrayList(pArrayList);
/*
        Maximum number of nodes: 6
        Current number of nodes: 6
        [index:   0], 5
        [index:   1], 0
        [index:   2], 2
        [index:   3], 9
        [index:   4], 4
        [index:   5], 7
*/

        removeAll(pArrayList);
        printArrayList(pArrayList);
/*
        Maximum number of nodes: 6
        Current number of nodes: 0
        [index:   0], EMPTY
        [index:   1], EMPTY
        [index:   2], EMPTY
        [index:   3], EMPTY
        [index:   4], EMPTY
        [index:   5], EMPTY
*/

        deleteArrayList(pArrayList); 
    }

    return 0;
}
