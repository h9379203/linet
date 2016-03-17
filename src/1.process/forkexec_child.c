#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	dprintf(STDOUT_FILENO, "Child[%d]: Start\n", getpid());
	dprintf(3, "Child[%d]: fd(3): Test fd.\n", getpid());
	close(3);
	dprintf(STDOUT_FILENO, "Child[%d]: Exit\n", getpid());
	return 0;
}
