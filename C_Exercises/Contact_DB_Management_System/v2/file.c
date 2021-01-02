#include <stdio.h>

#include "arraylist.h"
#include "file.h"

#define DB "contacts.txt"

void readFile(ArrayList *pArrayList)
{
    FILE          *contacts;
    ArrayListNode *pUser;

    pUser    = pArrayList->ptrNode;
    contacts = fopen(DB, "r");
    if (contacts != NULL) {
        fscanf(contacts, "%d %d", &(pArrayList->maxNodeCount), &(pArrayList->currentNodeCount));
        while (feof(contacts) == 0) {
            fscanf(contacts, "%s %s", pUser->name, pUser->phone_number);
            pUser++;
        }
        fclose(contacts);
    }

    return;
}

/* use while-loop instaed of for-loop; */
void writeFile(ArrayList *pArrayList)
{
    FILE          *contacts;
    int           len, cap;
    ArrayListNode *pUser;

    pUser    = pArrayList->ptrNode;
    contacts = fopen(DB, "w");
    if (contacts != NULL) {
        cap = getArrayListCapacity(pArrayList);
        len = getArrayListLength(pArrayList);
        fprintf(contacts, "%d %d\n", cap, len);
        while (len--) {
            fprintf(contacts, "%s %s\n", pUser->name, pUser->phone_number);
            pUser++;
        }
        fclose(contacts);
    } else {
        printf("Error writing to %s.\n", DB);
    }

    return;
}
