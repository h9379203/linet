/* Filename    : memory_lock.c
 * Description :
 * Notes       :
 * 1st Release : By SunYoung Kim<sunyzero@gmail.com>, 2011-08-01 03:23:02 (Mon)
 */
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int		ret, sz_mem = atoi(argv[1]) * 1024;
	printf("pgsize = %ld(%d)\n", sysconf(_SC_PAGESIZE), getpagesize());

	char	*p_str = (char *)malloc(sizeof(char) * sz_mem);
	if (p_str == NULL) {
		perror("Fail: malloc");
		exit(EXIT_FAILURE);
	}
	printf("* malloc = %p\n", (void *)p_str);

	if ((ret = mlockall(MCL_CURRENT)) == -1) {
		perror("Fail: mlockall");
		exit(EXIT_FAILURE);
	}

	free(p_str);
	return EXIT_SUCCESS;
}
