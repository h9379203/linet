/* vim: set ts=4 sw=4: */
/* Filename    : sysv_sem_counting.c
 * Description : test SysV semaphore.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include "ipcalsp.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
const int	max_child =	30;
int process_child(int);

int main(int argc, char *argv[])
{
	int	i, n_count, status, sem_id;
	if (argc != 2) {
		printf("%s [counter]\n", argv[0]); 
		exit(EXIT_FAILURE);
	}
	n_count = atoi(argv[1]);

	sem_id = sysv_semget(NULL, 0x12340001, 1, n_count, 0660); /* get SysV sem */
	printf("* Get [Counting semaphore:%d] (%d)\n", sem_id, n_count);
	for(i=0; i<max_child; i++) {
		printf("-- Atomically decrease (%d)\n", i);
		sysv_semwait(sem_id, 0);	/* atomically decrease */
		switch ( fork() ) {
			case 0:		/* child */
				process_child(i);
				sysv_sempost(sem_id, 0);	/* atomically increase */
				exit(EXIT_SUCCESS);
			case -1: 	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d]\n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parent */
				break;
		}
	}
	for(i=0; i<max_child ; i++) {
		pid_t	pid_child;
		if ((pid_child = waitpid(-1, &status, 0)) == -1) {
			/* err */
		}
	}
	if (sysv_semrm(sem_id) == -1) { /* remove sem from system */
		perror("FAIL: sysv_semrm"); 
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int process_child(int i) {
	if (i == 11) abort();	/* terminate 12th child */
	printf("\t[Child:%d] Proccessing - sleep(3)\n", i);
	sleep(3); /* sleep 3sec */
	return 0;
}

