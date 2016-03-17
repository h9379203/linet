/* vim: set ts=4 sw=4: */
/* Filename    : open64_fd.c
 * Description : open with file descriptor (64bit LFS)
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#define _LARGEFILE_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int		fd, len_read;
	char	buf[0xff];

	printf("Size of off_t : %ld\n", sizeof(off_t));
	if (argc != 2) {
		printf("[ERROR] missing argument(e.g. filename to open).\n");
		exit(EXIT_FAILURE);
	}

#ifdef _LARGEFILE64_SOURCE
	fd = open64(argv[1], O_RDONLY|O_LARGEFILE, 0);
#else
	fd = open(argv[1], O_RDONLY, 0);
#endif
	if (fd == -1) {
		printf("[ERROR] open():%d\n", errno);
		exit(EXIT_FAILURE);
	}

	printf("Opened fd = %d", fd);		/* buffering */
#ifdef _LARGEFILE64_SOURCE
	lseek64(fd, -100, SEEK_END);
	len_read = read64(fd, buf, sizeof(buf)); 
#else
	lseek(fd, -100, SEEK_END);
	len_read = read(fd, buf, sizeof(buf)); 
#endif
	len_read = read(fd, buf, sizeof(buf)); 
	if (len_read == -1) {
		printf("[ERROR] read():%d\n", errno);
		exit(EXIT_FAILURE);
	}

	close(fd);
	return (EXIT_SUCCESS);
}

