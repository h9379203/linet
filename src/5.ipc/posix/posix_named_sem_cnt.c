/* vim: set ts=4 sw=4: */
/* Filename    : posix_named_sem_cnt.c
 * Description : test posix named semaphore.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#define NAME_POSIX_SEM	"/my_psem"
const int	max_child =	30;
sem_t	*p_psem;
int process_child(int);
int main(int argc, char *argv[])
{
	int	i, n_count, sem_value, status;
	if (argc != 2) {
		printf("%s [counter]\n", argv[0]); 
		exit(EXIT_FAILURE);
	}
	n_count = atoi(argv[1]);

	p_psem = sem_open(NAME_POSIX_SEM, O_CREAT|O_EXCL, 0600, n_count); /* 생성 */
	if (p_psem == SEM_FAILED) {
		if (errno != EEXIST) {
			perror("FAIL: sem_open");
			exit(EXIT_FAILURE);
		}
		p_psem = sem_open(NAME_POSIX_SEM, 0); /* 기존의 생성되어있는 세마포어를 열기 */
		printf("[%d] Attach to an existed sem\n", getpid());
	} else
		printf("[%d] Create new sem\n", getpid());
	sem_getvalue(p_psem, &sem_value);
	printf("[%d] sem_getvalue = %d\n", getpid(), sem_value);

	for(i=0; i<max_child; i++) {
		printf("[%d] iteration(%d) : Atomically decrease\n", getpid(), i);
		sem_wait(p_psem);	/* EINTR 처리가 필요하다. */
		switch ( fork() ) {
			case 0:		/* child */
				process_child(i);
				sem_post(p_psem);	/* atomically increase */
				exit(EXIT_SUCCESS);
			case -1: 	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d]\n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parent */
				break;
		}
		usleep(10000);
	}
	for(i=0; i<max_child ; i++) {
		pid_t	pid_child;
		if ((pid_child = waitpid(-1, &status, 0)) == -1) {
			/* err */
		}
	}

	sem_getvalue(p_psem, &sem_value);
	printf("[%d] sem_getvalue = %d\n", getpid(), sem_value);
	
	if (sem_unlink(NAME_POSIX_SEM) == -1) { /* remove sem */
		perror("FAIL: sem_unlink"); 
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int process_child(int i) {
	if (i == 11) abort();	/* terminate 12th child */
	printf("\t[Child:%d] sleep(2)\n", i);
	sleep(2); /* sleep 2 sec */
	return 0;
}

