/* vim: set ts=4 sw=4: */
/* Filename    : fifo_read.c
 * Description : test reading from FIFO(named PIPE)
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define PATH_FIFO	"/tmp/my_fifo"

int main()
{
	int		fd, rc_read = 0;
	char	a_buf[0xff];

	if (mkfifo(PATH_FIFO, 0644) == -1) {
		if (errno != EEXIST) {
			fprintf(stderr, "FAIL: mkfifo(%s)\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if ((fd = open(PATH_FIFO, O_RDONLY, 0644)) == -1) {
		fprintf(stderr, "FAIL: open(%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((rc_read = read(fd, a_buf, sizeof(a_buf))) == -1) { 
			/* error */ 
		}
		if (rc_read == 0) { /* close pipe */
			printf("broken pipe\n"); 
			exit(EXIT_FAILURE);
		}
		printf("[%1$d byte] %2$.*1$s\n", rc_read, a_buf);
	}
	return (EXIT_SUCCESS);
}

