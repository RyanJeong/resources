#ifndef __DB_H__
#define __DB_H__

enum {
    QUIT,
    ADD,
    MODIFY,
    FIND,
    SHOW,
    DELETE,
    SIZE = 20
};

typedef struct contact {
    int             id;             /* unique account number */
    char            name[SIZE];     /* contains name */
    char            phone[SIZE];    /* contains phone number */
    char            email[SIZE];    /* contains email address */
    int             count;          /* count is used to input comments into array */
    struct contact  *next;          /* next is used to navigate through structures. */
} Contact;

#endif
