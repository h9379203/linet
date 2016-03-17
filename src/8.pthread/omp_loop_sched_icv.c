/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop_sched_icv.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : omp_get_thread_num
 */
#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
int main(int argc, char *argv[]) 
{
	int	i;
	struct timespec	tspec;
	omp_sched_t		schedtype;
	if (argc != 3) {
		printf("Usage: %s <static|dynamic|guided> <chunk size>\n", argv[0]);
		return 0;
	}
	if (!strcmp(argv[1], "static")) {
		schedtype = omp_sched_static;
	} else if (!strcmp(argv[1], "dynamic")) {
		schedtype = omp_sched_dynamic;
	} else if (!strcmp(argv[1], "guided")) {
		schedtype = omp_sched_guided;
	} else {
		printf("Unknown scheduling: %s\n", argv[1]);
		return 0;
	}
	int chunk_size = atoi(argv[2]);
	printf("schedule(%d) modifier(%d)\n", schedtype, chunk_size);
	omp_set_schedule(schedtype, chunk_size);
#pragma omp parallel for schedule(runtime) private(tspec)
	for (i=0; i<40; i++) {
		clock_gettime(CLOCK_REALTIME, &tspec);
		tspec.tv_sec = tspec.tv_nsec % 3;
		tspec.tv_nsec += (tspec.tv_nsec % 500000000);	/* .5 sec 이내 추가 */
		if (tspec.tv_nsec > 999999999) {
			tspec.tv_nsec = (tspec.tv_nsec % 999999999);
		}
		printf("[%02d] [thread:%d] sleep(%ld.%09ld)\n", i, omp_get_thread_num(),
				tspec.tv_sec, tspec.tv_nsec);
		nanosleep(&tspec, NULL);
	}
	/* implicit barrier */
	return 0;
}
