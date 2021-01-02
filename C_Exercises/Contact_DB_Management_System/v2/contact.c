#include <stdio.h>
#include <string.h>

#include "arraylist.h"
#include "contact.h"

void addContact(ArrayList *pArrayList)
{
    ArrayListNode user;
    ArrayListNode *pUser;
    int           i, len;
    
    len   = getArrayListLength(pArrayList);
    printf("%32s", "Enter contact name: ");
    scanf("%s", user.name);
    for (i = 0; i < len; i++) {
        pUser = getNode(pArrayList, i);
        if (strcmp(pUser->name, user.name) == 0) {
            printf("Contact %s has already been added. Please modify this contact.\n", user.name);

            return;
        }
    }
    printf("%32s", "Enter contact phone number: ");
    scanf("%s", user.phone_number);
    if (addNodeLast(pArrayList, user) == SUCCESS) {
        printf("Contact added.");
    }

    return;
}

/* modify-> 1. remove, 2. add at current position */
void modifyContact(ArrayList *pArrayList)
{
    ArrayListNode user;
    ArrayListNode *pUser;
    int           i, j, len;

    if (isArrayListEmpty(pArrayList) == FALSE) {
        len   = getArrayListLength(pArrayList);
        printf("Enter contact name to modify: ");
        scanf("%s", user.name);
        for (i = 0; i < len; i++) {
            pUser = getNode(pArrayList, i);
            if (strcmp(pUser->name, user.name) == 0) {
                printf("%32s", "Enter contact name: ");
                scanf("%s", user.name);
                for (j = 0; j < len; j++) {
                    pUser = getNode(pArrayList, j);
                    if (i != j && strcmp(pUser->name, user.name) == 0) {
                        printf("Contact %s has already been added. Please enter a different name.\n", user.name);

                        return;
                    }
                }
                removeNode(pArrayList, i);
                printf("%32s", "Enter contact phone number: ");
                scanf("%s", user.phone_number);
                if (addNode(pArrayList, i, user) == SUCCESS) {
                    printf("Contact has been modified.\n");
                }

                return;
            }
        }
        printf("Contact %s was not found.\n", user.name);
    } else {
        printf("[Error] There are no contacts to modify.\n");
    }

    return;
}

void findContact(ArrayList *pArrayList)
{
    ArrayListNode *pUser;
    char          name[NAME_SIZE];
    int           i, len;

    if (isArrayListEmpty(pArrayList) == FALSE) {
        len = getArrayListLength(pArrayList);
        printf("Enter contact name to find: ");
        scanf("%s", name);
        for (i = 0; i < len; i++) {
            pUser = getNode(pArrayList, i);
            if (strcmp(pUser->name, name) == 0) {
                printf("%-20s%-15s\n", "Name", "Phone Number");
                printf("----------------------------------\n");
                printf("%-20s%-15s\n", pUser->name, pUser->phone_number);

                return;
            }
        }
        printf("Contact %s was not found.\n", name);
    } else {
        printf("[Error] There are no contacts to find.\n");
    }

    return;
}

void deleteContact(ArrayList *pArrayList)
{
    ArrayListNode *pUser;
    char          name[NAME_SIZE];
    int           i, len;

    if (isArrayListEmpty(pArrayList) == FALSE) {
        len = getArrayListLength(pArrayList);
        printf("Enter contact name to delete: ");
        scanf("%s", name);
        for (i = 0; i < len; i++) {
            pUser = getNode(pArrayList, i);
            if (strcmp(pUser->name, name) == 0) {
                removeNode(pArrayList, i);
                printf("Contact %s deleted.\n", name);

                return;
            }
        }
        printf("Contact %s was not found.\n", name);
    } else {
        printf("[Error] There are no contacts to delete.\n");
    }

    return;
}

void printContact(ArrayList *pArrayList)
{
    ArrayListNode *pUser;
    int           i, len;

    if (isArrayListEmpty(pArrayList) == FALSE) {
        len = getArrayListLength(pArrayList);
        printf("Maximum number of contacts: %d\n"
               "Current number of contacts: %d\n", 
               pArrayList->maxNodeCount,
               pArrayList->currentNodeCount);
        printf("----------------------------------\n");
        printf("%-20s%-15s\n", "Name", "Phone Number");
        printf("----------------------------------\n");
        for (i = 0; i < len; i++) {
            pUser = getNode(pArrayList, i);
            printf("%-20s%-15s\n", pUser->name, pUser->phone_number);
        }
    } else {
        printf("[Error] There are no contacts to display.\n");
    }

    return;
}
