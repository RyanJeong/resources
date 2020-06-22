# File IO<br>
##  Declaration<br>
```C
FILE *fp;
FILE *fopen(char *name, char *mode);
```

## fopen Function<br>
```C
fp = fopen(name, mode);
```

### Mode & Description<br>
* Text files<br>
    * "r" : read<br>
    * "w" : write<br>
    * "a" : append<br>

* Binary files(Some systems distinguish between text and binary files.)<br>
    * "rb" : read<br>
    * "wb" : write<br>
    * "ab" : append<br>

> If a file that does not exist is opened for writing or appending, it is created if possible. Opening an existing file for writing causes the old contents to be discarded, while opening for appending preserves them. Trying to read a file that does not exist is an error, and there may be other causes of error as well, like trying to read a file when you don't have permission. If there is any error, fopen will return NULL.<br>

##  getc Function<br>
* getc returns the next character from the stream referred to by fp; it returns EOF for end of file or error.<br>
```C
int getc(FILE *fp)
```
##  putc Function<br>
* putc writes the character c to the file fp and returns the character written, or EOF if an error occurs.<br>
```C
int putc(int c, FILE *fp)
```

### Like getchar and putchar, getc and putc may be macros instead of functions.<br>
```C
#define getchar() getc(stdin)
#define putchar(c) putc((c), stdout)
```

## fscanf and fprintf Function<br>
```C
int fscanf(FILE *fp, char *format, ...)
int fprintf(FILE *fp, char *format, ...)
```
* These are identical to scanf and printf, **except** that the first argument is a file pointer that specifies the file to be read or written<br>

## stdin, stdout, and stderr<br>
* When a C program is started, the operating system environment is responsible for opening three files and providing pointers for them. <br>
    * These files are the standard input, the standard output, and the standard error; <br>
    * the corresponding file pointers are called stdin, stdout, and stderr, and are declared in <stdio.h><br>
```C
int i;

fscanf(stdin, "%d", &i); 
fprintf(stdout, "Integer %d entered.\n", i);
```

## fclose Function<br>
```C
int fclose(FILE *fp)
```
* the **inverse of fopen**, it breaks the connection between the file pointer and the external name that was established by fopen, freeing the file pointer for another file.<br>
* Since most operating systems **have some limit on the number of files** that a program may have open simultaneously, it's a good idea to free the file pointers when they are no longer needed.<br>
* There is also another reason for fclose on an output file - **it flushes the buffer in which putc is collecting output**. 
* fclose is called automatically for each open file when a program terminates normally.<br>

## freopen Function<br>
```C
FILE* freopen(char *filename, char *mode, FILE *stream);
```
* C89, 116. The primary use of the freopen function is to change the file associated with a standard text stream (stderr, stdin, or stdout), as those identifiers need not be modifiable lvalues to which the value returned by the fopen function may be assigned.
    * The file pointers stdin and stdout are objects of type FILE *. They are constants, however, not variables, so it is not possible to assign to them.<br>
    * You can close stdin and stdout if they are not needed. They can also
be reassigned by the library function freopen.<br>

## ferror Function<br>
* The function ferror returns non-zero if an error occurred on the stream fp.<br>
```C
int ferror(FILE *fp)
```
* Although output errors are rare, they do occur (for example, if a disk fills up), so a production program should check this as well.<br>

## feof Function<br>
* The function feof(FILE *) is analogous to ferror; it returns non-zero if end of file has occurred on the specified file.<br>
```C
int feof(FILE *fp)
```

## fflush Function<br>
* flushes the **output buffer** of a stream.<br>
    * fflush(input buffer, e.g. stdin) is **undefined behavior**.<br>
```C
int fflush(FILE *stream)
```
* Flushing output on a buffered stream means transmitting all accumulated characters to the file. There are many circumstances when buffered output on a stream is flushed automatically:<br>
    * When you try to do output and the output buffer is full.<br>
    * When the stream is closed.<br>
    * When the program terminates by calling exit.<br>
    * When a newline is written, if the stream is line buffered.<br>
    * Whenever an input operation on any stream actually reads data from its file.<br>

## fgets Function<br>
```C
char *fgets(char *line, int maxline, FILE *fp)
```
* fgets reads the next input line (including the newline) from file fp into the character array line; at most maxline-1 characters will be read.<br>
* The resulting line is terminated with '\0'. <br>
* Normally fgets returns line; on end of file or error it returns NULL. <br>

## fgets Function<br>
```C
int fputs(char *line, FILE *fp)
```
* It returns EOF if an error occurs, and non-negative otherwise.<br>

```C
#include <stdio.h>

enum {
    SIZE = 256
};

int main()
{
    char buf[SIZE];

    /* 1. fgets: fgets reads the next input line (including the newline) from file
       2. fputs: fputs writes a string (which need not contain a newline) to a file */
    fputs(fgets(buf, SIZE, stdin), stdout);
    /* 1. fgets: fgets reads the next input line (including the newline) from file
       2.  puts: adds the terminating '\n' */
    puts(fgets(buf, SIZE, stdin));
    /* 1.  gets: deletes the terminating '\n'
       2. fputs: fputs writes a string (which need not contain a newline) to a file */
    fputs(gets(buf), stdout);
    /* 1.  gets: deletes the terminating '\n'
       2.  puts: adds the terminating '\n' */
    puts(gets(buf));

    return 0;
}
```

## File IO with Error Handling Example:<br>
```C
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
```
