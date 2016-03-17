/* vim: set ts=4 sw=4: */
/* Filename    : sysv_sem_graph.c
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
const int	max_child =	5;
const int	cnt_sem_proc = 4;
int proc_a();
int proc_b();
int proc_c();
int proc_d();
int proc_e();

struct funclist {
	int	idx;
	int	(*func)();
} funclist[] = { 
	{0, proc_e}, 
	{1, proc_d},
	{2, proc_c},
	{3, proc_b},
	{4, proc_a}, 
	{5, NULL} 
};

int sem_id;

int main(int argc, char *argv[])
{
	int	i, status;

	sem_id = sysv_semget(NULL, 0x1234000a, cnt_sem_proc, 0, 0660); /* get SysV sem */
	printf("* Get semaphore for proc_b~e(%d)\n", cnt_sem_proc);

	for(i=0; i<max_child || funclist[i].func != NULL; i++) {
		switch ( fork() ) {
			case 0:		/* child */
				funclist[i].func();
				exit(EXIT_SUCCESS);
			case -1: 	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d]\n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parent */
				break;
		}
		usleep(5000);
	}

	for(i=0; i<max_child || funclist[i].func != NULL; i++) {
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

int proc_a() {
	printf("[Process A] Create\n");
	sleep(1);
	printf("\tPost semaphore(a)*2 -> next(B,C)\n");
	sysv_sempost(sem_id, 0);
	sysv_sempost(sem_id, 0);
	return 0;
}

int proc_b() {
	printf("[Process B] Create\n");
	sysv_semwait(sem_id, 0);
	printf("\tEnter: prev(A)\n");
	sleep(7);
	printf("\tPost semaphore(b) -> next(D)\n");
	sysv_sempost(sem_id, 1);
	return 0;
}

int proc_c() {
	printf("[Process C] Create\n");
	sysv_semwait(sem_id, 0);
	printf("\tEnter: prev(A)\n");
	sleep(1);
	printf("\tPost semaphore(c) -> next(D)\n");
	sysv_sempost(sem_id, 2);
	return 0;
}
int proc_d() {
	printf("[Process D] Create\n");
	sysv_semwait(sem_id, 2);
	printf("\tEnter: prev(C)\n");
	sleep(2);
	printf("\tPost semaphore(d) -> next(E)\n");
	sysv_sempost(sem_id, 3);
	return 0;
}

int proc_e() {
	printf("[Process E] Create\n");
	sysv_semwait(sem_id, 1);
	printf("\tEnter: prev(B)\n");
	sysv_semwait(sem_id, 3);
	printf("\tEnter: prev(D)\n");
	printf("\tAll Processes Completed!\n");
	return 0;
}
