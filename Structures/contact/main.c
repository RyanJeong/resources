#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "db.h"
#include "contact.h"

void    printMenu(void);
void    addNewcontact(void);
void    listAll(void);
void    deletecontact(void);
void    modifycontact(void);
int     findcontact(void);
int     prompt(void);
int     findnum(int);

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

Contact *firstc, *currentc, *newc; 
int     id = 0;

int main()
{
    int     c;
    FILE    *contacts;

    firstc      = NULL;
    contacts    = fopen(DB, "r");   /* open a file for load contacts */

    if (contacts != NULL) {
        firstc = (Contact *) malloc(sizeof(Contact));   /* use of malloc to set aside memory relative to size of structure contact */
	    currentc = firstc;                              /* make first record current */
        while(1) {                                      /* endless while loop. a NULL pointer in final node ends loop */
		    newc = (Contact *) malloc(sizeof(Contact));
		    fread(currentc, sizeof(Contact), 1, contacts);
		    if (currentc->next == NULL) {               /* NULL indicates end of node list */
			    break;
            }
		    currentc->next  = newc; /* pointer referencing next node*/
            currentc->count =0;     /* initiates count for comments*/
		    currentc        = newc; /* make current record new*/
	    }
	    fclose(contacts);           /* close file - good practice to cloe files after use*/
	    id = currentc->id;
    }
    do {
        printMenu();
        while ((c = getchar()) != EOF) {
            if (!isspace(c)) {
                if (isdigit(c)) {
                    c -= '0';
                    break;
                } else {
                    printf("Wrong input! Try again.\n");
                }
            }
        }
        getchar();  /*  flush buffer    */
        printf("%s\n", 
                (c == ADD)      ? "Add a new contact"   : 
                (c == MODIFY)   ? "Modify a contact"    :
                (c == FIND)     ? "Find a contact"      :
                (c == SHOW)     ? "Show all contacts"   : 
                (c == DELETE)   ? "Delete a contact"   :
                "Quit the database");
        switch (c) {
        case ADD:
            break;
        case MODIFY:
            break;
        case FIND:
            break;
        case SHOW:
            break;
        case DELETE:
            break;
        case QUIT:
            break;
        default:
            break;
        }
    } while (c != QUIT);
        


    /*
        switch(ch)     
        {
            case '1':
                puts("Add a new contact\n");
                addNewcontact();
                break;
	    case '2':
		puts("Delete a contact\n");
		deletecontact();
		break;
	    case '3':
		puts("List all contacts\n");
		listAll();
		break;
	    case '4':
		puts("Modify a contact\n");
		modifycontact();
		break;
	    case '5':
		puts("Find a contact by name\n");
		findcontact();
		break;
            case 'Q':
                puts("Save and quit\n");
                default:
                break;
        }
    } while(ch != 'Q');
    */
/*
 * Save the records to disk
 */
    currentc = firstc;

    if(currentc == NULL)
	    return(0);		/*no data to write*/

    contacts = fopen(DB,"w");   /*open file to write*/

    if(contacts == NULL)
    {
	    printf("Error writing to %s\n",DB);
	    return(1);
    }
    				/* Write each record to disk*/
    while(currentc != NULL)
    {
	    fwrite(currentc,sizeof(struct contact),1,contacts);
	    currentc = currentc->next;
    }
    fclose(contacts);             /*closes data file*/
    return(0);
}

void printMenu(void) 
{
#ifndef _WIN32
    system("clear");
#else
    system("cls");
#endif
    printf("*** Contact Database ***\n"
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
/*----------------------------------------------------------------------------*/
void addNewcontact(void) /* add new contact function*/
{
    newc = (struct contact *)malloc(sizeof(struct contact));
    /*allocates memory for new structure.*/
/*
 * Checks to see whether this is the first record in file
 * If so, then all pointers are initialized to this record,
 */
    if(firstc==NULL)
        firstc = currentc = newc;
/*
 * if not, end of structure list is obtained
 */
    else
    {
        currentc = firstc;      /* make the first record the current one*/

        while(currentc->next != NULL)currentc = currentc->next;
                                /* and loop through all records*/
        currentc->next = newc;  /* pointer to next node */
        currentc = newc;        /* make current record the new one*/
    }
/* update the structure */
    id++;
    printf("%27s: %5i\n","contact number",id);
    currentc->id = id;    /*cnum is used to give unique account numbers*/

    printf("%27s: ","Enter contact name");
    gets(currentc->name);
    printf("%27s: ","Enter contact Phone number");
    gets(currentc->phone);
    printf("%27s: ","Enter contact email");
    gets(currentc->email);
    printf("contact added!");
    currentc->count=0;
/*
 * gives the new record a NULL pointer
 * to show it's the last record:
 */
    currentc->next = NULL;
}
/*----------------------------------------------------------------------------*/
void listAll(void) /* list all contacts function*/
{
    if(firstc==NULL)
        puts("There are no contacts to display!"); /*prints message*/

    else
    {
	    printf("%6s %-20s %-15s %-15s\n","Acct#","Name","Phone","Email");
        puts("------ -------------------- ------------- -------------------");
                            /*prints table titles*/
        currentc=firstc;

        do
        {

                printf("%6d: %-20s %-15s %-20s\n",\
                currentc->id,\
                currentc->name,\
                currentc->phone,\
                currentc->email);
                /*prints values of number, name, phone and email*/
        }

        while((currentc=currentc->next) != NULL);
    }
}
/*----------------------------------------------------------------------------*/
void deletecontact(void)     /*delete contact function */
{
    int record;
    struct contact *previousa;
    if(firstc==NULL)
    {
        puts("There are no contacts to delete!");
	return;
    }

    listAll();		/* show all records*/
    printf("Enter contact account number to delete: ");
    scanf("%d",&record);
    currentc = firstc;

    while(currentc != NULL)
    {
        if(currentc->id == record)
	{
	    if(currentc == firstc)	/*if record to be deleted is the first record*/
		firstc=currentc->next; /*reset firstc to point at next record as first*/
	    else
		previousa->next = currentc->next;/*previous pointer used if record*/
                                         /*to delete is not the first*/
        free(currentc); /*frees memory <deletes>*/
	    printf("contact %d deleted!\n",record);
	    return;
	}

	else
	{
        previousa = currentc;
	    currentc = currentc->next;
	}
    }
    printf("contact %d not found!\n",record);
 }
/*----------------------------------------------------------------------------*/
void modifycontact(void)   /*modify contact function*/
{
    int record, result;
    if(firstc==NULL)
    {
        puts("There are no contacts to modify!");
	return;
    }

    listAll();		/* show all records */
    printf("Enter contact account number to modify or change: ");
    scanf("%d",&record);  /*scan user input to record*/
    result = findnum(record);

    if( result >0 ){
	    printf("Contact %d:\n",currentc->id);
	    printf("Name: %s\n",currentc->name);
	    if(prompt())
		    gets(currentc->name);
	    printf("Phone: %s\n",currentc->phone);
	    if(prompt())
		    gets(currentc->phone);
	    printf("Email: %s\n",currentc->email);
	    if(prompt())
		    gets(currentc->email);
	    return;
	}
    printf("contact %d was not found!\n",record);
}
/*----------------------------------------------------------------------------*/
int findnum (int recordnum)
{
    int record;
    record = recordnum;
    currentc = firstc;
    while(currentc != NULL)
    {

        if(currentc->id == record)
	     {
           return 1;
         }

	else
	{
	    currentc = currentc->next;
	}
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
int findcontact(void) /* find contact function*/
{
     char buff[20];

     if(firstc==NULL)
	{
        puts("There are no contacts to find!");
	    return 1;
    }

    printf("Enter contact name: ");
    gets(buff);
    currentc = firstc;
    while(currentc != NULL)
    {
        if( strcmp(currentc->name, buff) == 0 )
	    {
			printf("%6s %-20s %-15s %-15s\n","Acct#","Name","Phone","Email");
			/*prints table titles*/
            printf("%6d: %-20s %-15s %-20s\n",\
            currentc->id,\
            currentc->name,\
            currentc->phone,\
            currentc->email);
            /*prints values of number, name, phone and email*/

			return 0;
	    }
		else
		{
			currentc = currentc->next;
		}
    }
    printf("contact %s was not found!\n",buff);
          return 1;
}
/*----------------------------------------------------------------------------*/
int prompt(void)
{
	char ch;
	printf("Update? (Y to update any other key to not)");
	ch = getchar();
	ch = toupper(ch);
    getchar();
	if(ch == 'Y')
	{
		printf("Enter new value: ");
		return(1);
	}
	else
		return(0);
}
/*----------------------------------------------------------------------------*/
/* END OF PROGRAM */
