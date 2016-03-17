/* vim: set ts=4 sw=4: */
/* Filename    : fifo_write_sigpipe.c
 * Description : test writing to FIFO(named PIPE)
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : install signal handler on SIGPIPE
 */
#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define PATH_FIFO	"/tmp/my_fifo"
int inst_sighandler();
void sighandler_pipe(int);

int main()
{
	int		fd, rc_write, rc_getline;
	char	*p_buf = NULL;
	size_t	len_buf = 0;

	if (inst_sighandler()) {
		/* error */
	}

	if ((fd = open(PATH_FIFO, O_WRONLY, 0644)) == -1) {
		fprintf(stderr, "FAIL: open() (%d:%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("To FIFO >>");
		fflush(stdout);
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) {
			/* error */
			return EXIT_FAILURE;
		}

		if (p_buf[rc_getline - 1] == '\n') rc_getline--;
		
		if ((rc_write = write(fd, p_buf, rc_getline)) == -1) { /* error */ }
		printf("* Writing %d bytes...\n", rc_write);
		free(p_buf);
		p_buf = NULL;
	}
	return (EXIT_SUCCESS);
}

int inst_sighandler()
{
	struct sigaction sa_pipe;

	sa_pipe.sa_handler = sighandler_pipe;
	sigfillset(&sa_pipe.sa_mask);
	sa_pipe.sa_flags = SA_RESTART;

	if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1) {
		/* error */
	}

	return 0;
}

void sighandler_pipe(int signum)
{
	printf(">SIGPIPE(%d) occured\n", signum);
}
