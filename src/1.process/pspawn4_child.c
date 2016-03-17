#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main()
{
	dprintf(STDOUT_FILENO, "Child[%d]: Start\n", getpid());
	dprintf(STDOUT_FILENO, "Child[%d]: raise(SIGUSR1)\n", getpid());
	raise(SIGUSR1);
	dprintf(STDOUT_FILENO, "Child[%d]: raise(SIGUSR2)\n", getpid());
	raise(SIGUSR2);
	dprintf(STDOUT_FILENO, "Child[%d]: raise(SIGTERM)\n", getpid());
	raise(SIGTERM);
	dprintf(STDOUT_FILENO, "Child[%d]: Exit\n", getpid());
	return 0;
}
