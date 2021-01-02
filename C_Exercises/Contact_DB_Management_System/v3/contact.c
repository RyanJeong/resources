#include <stdio.h>

#define DB "contacts.dj"

void loadContact(void)
{
    FILE    *contacts;

    contacts = fopen(DB, "r");   /* open a file for load contacts */
    firstc      = NULL;

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

    return;
}

void saveContact(void)
{
    currentc = firstc;

    if(currentc == NULL)
	    return;		/*no data to write*/

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

    return;
}
