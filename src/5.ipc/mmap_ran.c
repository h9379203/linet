/* vim: set ts=4 sw=4: */
/* Filename    : mmap_ran.c
 * Description : Memory mapped file
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : SUSv4(IEEE Std. 1003.1-2008) applied.
 */
#define _XOPEN_SOURCE	700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#define MMAP_FILENAME	"mmapfile.dat"
#define MMAP_SIZE		64

char	contents[128] = "1234567890_^_^_^_^_^abcdefghij_-_-_-_-_-1234567890_^_^_^_^_^abcdefghij_-_-_-_-_-1234567890";

int main(int argc, char *argv[])
{
	int		fd, n_write, flag_mmap = MAP_SHARED;
	char	*p_map;
	int     n_read = 0; size_t  n_input = 128;
	char    *p_input = (char *) malloc(n_input);
	/* make mmap file */
	if ((fd = open(MMAP_FILENAME, O_RDWR|O_CREAT, 0664)) == -1) {
		printf("Fail: open(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
#if 0
	if (ftruncate(fd, MMAP_SIZE) == -1) {
		printf("Fail: ftruncate(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
#else
	if ((n_write = write(fd, contents, sizeof(contents))) < 0) {
		printf("Fail: write(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("* Worte %d bytes -> %s\n", n_write, MMAP_FILENAME);
#endif
	if ((p_map = mmap((void *)0, MMAP_SIZE, PROT_READ|PROT_WRITE, flag_mmap, fd, 0)) 
		== MAP_FAILED) {
		printf("Fail: mmap(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	close(fd);
	printf("* mmap file : %s\n", MMAP_FILENAME);
#if 1
	int ret;
	if (( ret = posix_madvise(p_map, MMAP_SIZE, POSIX_MADV_RANDOM)) != 0) {
		printf("Fail: posix_madvise(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("posix_madvise : %d\n", ret);
	char *p_str = malloc(4096);
	if (( ret = posix_madvise(p_str, 4096, POSIX_MADV_RANDOM)) != 0) {
		printf("Fail: posix_madvise(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
#endif
	while (1) {
		printf("'*' print current mmap otherwise input text to mmap. >>");
		if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) { /* error */
			return -1;
		}

		if (p_input[0] == '*') {
			if (msync(p_map, MMAP_SIZE, MS_SYNC) == -1) { /* synchronize mmap = flushing */
				printf("Fail: msync(): (%d:%s)\n", errno, strerror(errno));
				return EXIT_FAILURE;
			}
			printf("Current mmap -> '%.*s'\n", MMAP_SIZE, p_map);
		} else {
			p_input[n_read-1] = 0; /* remove newline */
			memcpy(p_map, p_input, n_read);
		}
	}
	return EXIT_SUCCESS;
}
