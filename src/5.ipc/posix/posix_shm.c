/* vim: set ts=4 sw=4: */
/* Filename    : posix_shm.c
 * Description : shared memory test code for POSIX shm
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : SUSv4(IEEE Std. 1003.1-2008) applied.
 * Rel         : 2nd Ed.
 */
#define _XOPEN_SOURCE	700
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define NAME_POSIX_SHM		"/mmapfile"
#define SZ_SHM_SEGMENT		4096

int main()
{
	int 	shm_fd;
	char 	*shm_ptr;
	int     n_read = 0; size_t  n_input = 128;
    char    *p_input = (char *) malloc(n_input);
	char	buf_err[128];	/* error message */

	printf("* SHM Name : %s\n", NAME_POSIX_SHM);
	if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR|O_CREAT|O_EXCL, 0660)) > 0) { /* create */
		printf("* Create SHM : /dev/shm/%s\n", NAME_POSIX_SHM);
		if (ftruncate(shm_fd, SZ_SHM_SEGMENT) == -1) /* truncate the file */
			exit(EXIT_FAILURE);
	} else {
		if (errno != EEXIST) { /* error */
			strerror_r(errno, buf_err, sizeof(buf_err));
			printf("FAIL: shm_open(): %s\n", buf_err);
			exit(EXIT_FAILURE);
		}
		if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR, 0)) == -1) { /* open */
			strerror_r(errno, buf_err, sizeof(buf_err));
			printf("FAIL: shm_open(): %s\n", buf_err);
			exit(EXIT_FAILURE);
		}
	}

	/* make mmap on shm */
	shm_ptr = (char *)mmap(NULL, SZ_SHM_SEGMENT, PROT_READ|PROT_WRITE, 
			MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		printf("FAIL: mmap()\n");
		exit(EXIT_FAILURE);
	}
	close(shm_fd);
	printf("'*' PRint current shm.\n'.' Exit.\n");
	printf("otherwise change shm to your input.\n");
	while (1) {
		printf("\n>>");
		if ( (n_read = (int) getline(&p_input, &n_input, stdin)) == -1) { /* error */
			return -1;
		}

		if (p_input[0] == '.') {
			break;
		} else if (p_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			memcpy(shm_ptr, p_input, n_read);
		}
	}
	munmap(shm_ptr, SZ_SHM_SEGMENT);
	printf("* Would you remove shm (name : %s) (y/n)", NAME_POSIX_SHM);
	if ( (n_read = (int)getline(&p_input, &n_input, stdin)) == -1) { /* error */
		return -1;
	}
	if (p_input[0] == 'y') {
		shm_unlink(NAME_POSIX_SHM);	/* remove shm */
	}
	return 0;
}
