/* vim: set ts=4 sw=4: */
/* Filename    : popen_ex2.c
 * Description : popen example for writing
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#define _XOPEN_SOURCE 	700
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main()
{
	FILE *fp_popen;
	int		i, rc_write, rc_getline;
	char	*p_buf = NULL;
	size_t	len_buf = 0;

	if ((fp_popen = popen("sort", "w")) == NULL) {
		fprintf(stderr, "FAIL: popen(%s)\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("* waiting for input : \n");

	for(i=0; i<5; i++) {
		if ((rc_getline = getline(&p_buf, &len_buf, stdin)) == -1) {
			/* error */
			return EXIT_FAILURE;
		}
		if ((rc_write = fwrite(p_buf, sizeof(char), rc_getline, fp_popen)) == -1) { 
			/* error */ 
		}
		if (rc_write == 0) break; /* broken pipe */
		free(p_buf);
		p_buf = NULL;
	}
	printf("* Sorting data ->\n");
	pclose(fp_popen);
	return EXIT_SUCCESS;
}

