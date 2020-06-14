#include <stdio.h>

#include "arraylist.h"
#include "file.h"
#include "contact.h"

enum {
    QUIT,
    ADD,
    MODIFY,
    FIND,
    PRINT,
    DELETE,
    INIT,

    DB_SIZE = 100
};

void printMenu(void);

int main(void)
{   
    ArrayList *pArrayList;
    int       input;

    pArrayList = createArrayList(DB_SIZE);
    if (pArrayList == NULL) {

        return 1;
    }
    input = INIT;
    readFile(pArrayList);
    do {
        printMenu();
        scanf("%d", &input);
        /* getchar() */
        while (getchar() != '\n') {
            ;
        }
        switch (input) {
        case ADD:
            addContact(pArrayList);
            break;
        case MODIFY:
            modifyContact(pArrayList);
            break;
        case FIND:
            findContact(pArrayList);
            break;
        case PRINT:
            printContact(pArrayList);
            break;
        case DELETE:
            deleteContact(pArrayList);
            break;
        case QUIT:
            printf("Quit the database.\n");
            writeFile(pArrayList);
            break;
        default:
            printf("Wrong input.\n");
            break;
        }
    } while (input != QUIT);

    return 0;
}

void printMenu(void)
{
    printf("\n*** Contact Database ***\n"
           "------------------------\n"
           "[1] Add a new contact\n"
           "[2] Modify a contact\n"
           "[3] Find a contact\n"
           "[4] Show all contacts\n"
           "[5] Delete a contact\n"
           "[0] Save and quit\n"
           "------------------------\n"
           "Enter your choice:");

    return;
}

