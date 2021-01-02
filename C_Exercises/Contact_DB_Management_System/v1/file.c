#include <stdio.h>

#include "contact.h"
#include "file.h"

#define DB "contacts.txt"

extern int numContact;

void readFile(User *user)
{
    FILE  *contacts;

    contacts = fopen(DB, "r");
    if (contacts != NULL) {
        while (feof(contacts) == 0) {
            fscanf(contacts, "%s %s", user->name, user->phone_number);
            user++;
        }
        numContact = sizeof *user / sizeof(User);
        fclose(contacts);
    }

    return;
}

void writeFile(User *user)
{
    FILE *contacts;

    contacts = fopen(DB, "w");
    if (contacts != NULL) {
        while (numContact--) {
            fprintf(contacts, "%s %s\n", user->name, user->phone_number);
            user++;
        }
        fclose(contacts);
    } else {
        printf("Error writing to %s.\n", DB);
    }

    return;
}
