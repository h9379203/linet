/* vim: set ts=4 sw=4: */
/* Filename    : fifo_write.c
 * Description : test writing to FIFO(named PIPE)
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
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

#define PATH_FIFO	"/tmp/my_fifo"

int main()
{
	int		fd, rc_write, rc_getline;
	char	*p_buf = NULL;
	size_t	len_buf = 0;

	if ((fd = open(PATH_FIFO, O_WRONLY, 0644)) == -1) {
		fprintf(stderr, "FAIL: open(%s)\n", strerror(errno));
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
