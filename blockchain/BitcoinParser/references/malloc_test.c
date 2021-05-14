#include <stdio.h>
#include <stdlib.h>

int main()
{
	char	*a;
	int		*b;
	long	*c;
	size_t	i;
	
	a = (char *) malloc(sizeof(char) * 10);
	b = (int *) malloc(sizeof(int) * 10);
	c = (long *) malloc(sizeof(long) * 10);
	
	for (i = 0; i < 10; ++i) {
		printf("%p %p %p\n", (a + i), (b + i), (c + i));
	}
	free(c);
	free(b);
	free(a);

	return 0;
}

/*
0x5610b6798260 0x5610b6798280 0x5610b67982b0
0x5610b6798261 0x5610b6798284 0x5610b67982b8
0x5610b6798262 0x5610b6798288 0x5610b67982c0
0x5610b6798263 0x5610b679828c 0x5610b67982c8
0x5610b6798264 0x5610b6798290 0x5610b67982d0
0x5610b6798265 0x5610b6798294 0x5610b67982d8
0x5610b6798266 0x5610b6798298 0x5610b67982e0
0x5610b6798267 0x5610b679829c 0x5610b67982e8
0x5610b6798268 0x5610b67982a0 0x5610b67982f0
0x5610b6798269 0x5610b67982a4 0x5610b67982f8
*/
