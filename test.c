#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
	char buffer[30];
	strncpy(buffer, "asdf", 4);
	printf(buffer);
	return 0;
}
