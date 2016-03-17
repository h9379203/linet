/* vim: set ts=4 sw=4: */
/* Filename    : sched.c
 * Description : realtime scheduler
 * Author      : Sun Young Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>

void print_sched(void);

int main() 
{
	printf("SCHED_FIFO(%d) SCHED_RR(%d) SCHED_OTHER(%d)\n",
			SCHED_FIFO, SCHED_RR, SCHED_OTHER);
	printf("SCHED_RR priority = %d ~ %d\n", 
			sched_get_priority_max(SCHED_RR), sched_get_priority_min(SCHED_RR));
	print_sched();

	struct sched_param 	schedp = { .sched_priority = 10 };
	if (sched_setscheduler(0, SCHED_RR, &schedp) == -1) {
		perror("sched_setscheduler");
		exit(EXIT_FAILURE);
	}
	print_sched();

	return 0;
}

void print_sched() 
{
	struct sched_param 	schedp;

	printf("* sched_getscheduler = %d\n", sched_getscheduler(0));

	sched_getparam(0, &schedp);
	printf("* sched_getparam = %d\n", schedp.sched_priority);
}
