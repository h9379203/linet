/* vim: set ts=4 sw=4: */
/* Filename    : sig_basic.c
 * Description : simple signal model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : sigaction & signal mask
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void sa_handler_usr(int signum);

int main()
{
	struct sigaction sa_usr1;
	struct sigaction sa_usr2;

	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = sa_handler_usr;
	sigfillset(&sa_usr1.sa_mask);	/* fill up signal block mask. 
									   So it can't be allowed interrupted by other signal */

	memset(&sa_usr2, 0, sizeof(struct sigaction));
	sa_usr2.sa_handler = sa_handler_usr;
	sigemptyset(&sa_usr2.sa_mask);	/* empty signal block mask, 
									   so other process can interrupt 
									   when is running this handler */
#ifdef WITH_SA_NODEFER
	sa_usr2.sa_flags = SA_NODEFER;
#endif

	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	printf("[MAIN] SIGNAL-Handler installed, pid(%d)\n", getpid());
	for(;;) {
		pause();
		printf("[MAIN] Recv SIGNAL...\n");
	}
	return EXIT_SUCCESS;
}

/* Name : signal handler func about SIGUSR1 & SIGUSR2
 * Desc : sleep 10 sec & print message every second.
 * Argv : 
 * Ret  : None
 */
void sa_handler_usr(int signum)
{
	int	i;
	for (i=0; i<10; i++) {
		printf("\tSignal(%s):%d sec.\n", signum == SIGUSR1 ? "USR1":"USR2", i);
		sleep(1);
	}
}
