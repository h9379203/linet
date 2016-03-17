/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop_sched.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : omp_get_thread_num
 */
#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
int main() 
{
	int	i;
	struct timespec	tspec;
#pragma omp parallel for schedule(dynamic) private(tspec)
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
