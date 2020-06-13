#include <stdio.h>
#include <string.h>

#include "contact.h"

int numContact;

void addContact(User *user)
{
    char name[NAME_SIZE];
    int  i;
    
    if (numContact < DB_SIZE) {
        printf("%32s", "Enter contact name: ");
        scanf("%s", name);
        for (i = 0; i < numContact; i++, user++) {
            if (strcmp(user->name, name) == 0) {
                printf("Contact %s has already been added. Please modify this contact.\n", name);

                return;
            }
        }
        strcpy(user->name, name);
        printf("%32s", "Enter contact phone number: ");
        scanf("%s", user->phone_number);
        numContact++;
        printf("Contact added.");
    } else {
        printf("[Error] The database is full.\n");
    }

    return;
}

void modifyContact(User *user)
{
    char name[NAME_SIZE];
    int  i, j;

    if (numContact > 0) {
        printf("Enter contact name to modify: ");
        scanf("%s", name);
        for (i = 0; i < numContact; i++) {
            if (strcmp((user + i)->name, name) == 0) {
                printf("%32s", "Enter contact name: ");
                scanf("%s", name);
                for (j = 0; j < numContact; j++) {
                    if (i != j && strcmp((user + j)->name, name) == 0) {
                        printf("Contact %s has already been added. Please enter a different name.\n", name);

                        return;
                    }
                }
                strcpy((user + i)->name, name);
                printf("%32s", "Enter contact phone number: ");
                scanf("%s", (user + i)->phone_number);
                printf("Contact has been modified.\n");

                return;
            }
        }
        printf("Contact %s was not found.\n", name);
    } else {
        printf("[Error] There are no contacts to modify.\n");
    }

    return;
}

void findContact(User *user)
{
    char name[NAME_SIZE];
    int  i;

    if (numContact > 0) {
        printf("Enter contact name: ");
        scanf("%s", name);
        for (i = 0; i < numContact; i++, user++) {
            if (strcmp(user->name, name) == 0) {
                printf("%-20s%-15s\n", "Name", "Phone Number");
                printf("----------------------------------\n");
                printf("%-20s%-15s\n", name, user->phone_number);

                return;
            }
        }
        printf("Contact %s was not found.\n", name);
    } else {
        printf("[Error] There are no contacts to find.\n");
    }

    return;
}

void deleteContact(User *user)
{
    char name[NAME_SIZE];
    int  i, j;

    if (numContact > 0) {
        printf("Enter contact name: ");
        scanf("%s", name);
        for (i = 0; i < numContact; i++, user++) {
            if (strcmp(user->name, name) == 0) {
                for (numContact--, j = i; j < numContact; j++, user++) {
                    strcpy(user->name, (user + 1)->name);
                    strcpy(user->phone_number, (user + 1)->phone_number);
                }
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

void printContact(User *user)
{
    int i;

    if (numContact > 0) {
        printf("%-20s%-15s\n", "Name", "Phone Number");
        printf("----------------------------------\n");
        for (i = 0; i < numContact; i++, user++) {
            printf("%-20s%-15s\n", user->name, user->phone_number);
        }
    } else {
        printf("[Error] There are no contacts to display.\n");
    }

    return;
}
