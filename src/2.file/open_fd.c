/* vim: set ts=4 sw=4: */
/* Filename    : open_fd.c
 * Description : low level file handling
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

int main()
{
	int		fd;
	char	buf_msg[64];
	printf("Low lovel file handle\n");
	if ((fd = open("fd_test.log", O_CREAT|O_WRONLY, 0644)) == -1) {
		fprintf(stderr, "Fail: open: %d(%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	dprintf(fd, "PID[%d] Low level file handle\n", getpid());
	strcpy(buf_msg, "write: Test message\n");
	write(fd, buf_msg, strlen(buf_msg));
	close(fd);
	return EXIT_SUCCESS;
}

