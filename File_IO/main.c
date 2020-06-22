#include <stdio.h>
#include <stdlib.h> /* exit() function declared here */
#include <ctype.h>

void filecopy(FILE *, FILE *);

int main(int argc, char *argv[])
{
    FILE *src, *dst;

    src = fopen("source.txt", "r");
    dst = fopen("destination.txt", "w");
    /* check error */
    if (src == NULL || dst == NULL) {
        printf("Unable to open file\n");

        exit(1);
    }
    filecopy(src, dst);
    fclose(src);
    fclose(dst);

    return 0;
}

/* filecopy: copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp)
{
    int c;

    while ((c = getc(ifp)) != EOF) {
        /* The putc function normally return a copy of the byte that is has written - as confirmation of success.
           If there is an error it returns EOF instead. */
        if (putc(islower(c) ? toupper(c) : tolower(c), ofp) == EOF) {
            printf("Error in writing to file\n");

            exit(1);
        }
    }
    if (feof(ifp)) {
        printf("End of file\n");
    }
    if (ferror(ifp)) {
        printf("Error in reading from file\n");
    }

    return;
}
