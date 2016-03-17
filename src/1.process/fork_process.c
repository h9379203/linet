/* vim: set ts=4 sw=4: */
/* Filename    : fork_process.c
 * Description : fork process
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{
	int		i=0;
	pid_t	ret;

	for (i=0; i<3; i++) {
		ret = fork();
		printf("[%d] PID(%d) PPID(%d)\n", i, getpid(), getppid());
#ifndef OMIT_SWITCH
		switch(ret) {
			case 0:		/* child */
				pause(); return 0;
				break;
			case -1:	/* error */
				break;
			default:	/* parent */
				break;
		}
#endif
	}
	wait(NULL); /* for parent */

	return 0;
}

