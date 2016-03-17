/* Filename    : forkexec_parent.c
 * Programmer  :  
 * Description :
 * Notes       :
 * 1st Release : By SunYoung Kim<sunyzero@gmail.com>, 2011-04-28 02:38:53
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
	pid_t	pid_child;
	printf("Parent[%d]: Start\n", getpid());
	int fd = open("forkexec.log", O_WRONLY|O_CREAT|O_APPEND, 0644);
	if (fd == -1) {
		perror("FAIL: open");
		exit(EXIT_FAILURE);
	}
	dprintf(fd, "Parent[%d]: Open log file(fd=%d)\n", getpid(), fd);

#ifdef APPLY_FD_CLOEXEC
	int ret_fcntl;
	if ( (ret_fcntl = fcntl(fd, F_SETFD, FD_CLOEXEC)) == -1) {
		perror("FAIL: fcntl(F_SETFD, FD_CLOEXEC)");
		exit(EXIT_FAILURE);
	}
#endif
	/* fork-exec */
	char *argv_exec[] = { "forkexec_child", (char *)NULL };
	switch( (pid_child = fork()) )
	{
		case 0:		/* child process */
			execv( argv_exec[0], argv_exec );
			break;
		case -1:	/* error */
			perror("FAIL: FORK");
			break;
		default:	/* parent process */
			(void)wait(NULL);	/* wait for child */
			break;
	}
	printf("Parent[%d]: Exit\n", getpid());
	return 0;
}

