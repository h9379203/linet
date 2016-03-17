/* Filename    : linked_ch.c
 * Description :
 * Notes       :
 * 1st Release : By SunYoung Kim<sunyzero@gmail.com>, 2011-07-12 19:49:38 (Tue)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <spawn.h>
#include <sys/wait.h>
char file_log[64];
void cat_logfile();

int main(int argc, char *argv[])
{
	int 	fd0, fd1;
	FILE	*fp0;
	char	buf[64];
	snprintf(file_log, sizeof(file_log), "%s.log", argv[0]);
	if ( (fd0 = open(file_log, O_CREAT|O_TRUNC|O_RDWR, 0644)) == -1) {
		/* error */
	}
	write(fd0, "1234567890abcdefghij", 20);
	cat_logfile();	/* print current file */

	if ( (fd1 = dup(fd0)) == -1) {
		/* error */
	}
	write(fd1, "OPQRSTU", 7);
	cat_logfile();

	if ( (fp0 = fdopen(fd1, "a+")) == NULL) {
		/* error */
	}
	printf("\tfd0(%d) fd1(%d)\n", fd0, fd1);

	lseek(fd1, 2, SEEK_SET);
	write(fd1, ",fd1,", 5);
	cat_logfile();
	write(fd0, ",fd0,", 5);
	cat_logfile();
	int x = fread(buf, 5, 1, fp0);
	printf("\tread buf=\"%.5s\"(%d)\n", buf, x);
	fwrite("(^o^)", 5, 1, fp0); fflush(fp0);
	cat_logfile();
	fclose(fp0);
	close(fd1);
	close(fd0);
	return 0;
}

void cat_logfile()
{
	static int cnt;
	char    *argv_child[] = {"cat", file_log, NULL};
	printf("%d={", cnt++);
	fflush(stdout);
	posix_spawnp(NULL, argv_child[0], NULL, NULL, argv_child, NULL);
	wait(NULL);
	printf("}\n");
	fflush(stdout);
}
