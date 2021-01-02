#include <stdio.h>
#include <string.h>

int main()
{
	char *bug = NULL;
	
	strcpy(bug, "DEBUG");
	printf("buf is %s\n", bug);

	return 0;
}
