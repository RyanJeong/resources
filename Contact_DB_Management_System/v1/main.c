#include <stdio.h>

#include "file.h"
#include "contact.h"

enum {
    QUIT,
    ADD,
    MODIFY,
    FIND,
    PRINT,
    DELETE,
    INIT
};

void printMenu(void);

int main(void)
{
    User user[DB_SIZE];
    int  input;

    input = INIT;
    readFile(user);
    do {
        printMenu();
        scanf("%d", &input);
        /* getchar() */
        while (getchar() != '\n') {
            ;
        }
        switch (input) {
        case ADD:
            addContact(user);
            break;
        case MODIFY:
            modifyContact(user);
            break;
        case FIND:
            findContact(user);
            break;
        case PRINT:
            printContact(user);
            break;
        case DELETE:
            deleteContact(user);
            break;
        case QUIT:
            printf("Quit the database.\n");
            writeFile(user);
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

