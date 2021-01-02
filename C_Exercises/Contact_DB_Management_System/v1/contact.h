#ifndef __CONTACT_H__
#define __CONTACT_H__

enum {
    DB_SIZE = 100,
    NAME_SIZE = 32,
    PHONE_NUMBER_SIZE = 32
};

typedef struct {
    char name[NAME_SIZE];
    char phone_number[PHONE_NUMBER_SIZE];
} User;

void addContact(User *);
void modifyContact(User *);
void findContact(User *);
void printContact(User *);
void deleteContact(User *);

#endif
