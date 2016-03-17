/* vim: set ts=4 sw=4: */
/* Filename    : omp_pi_cputime.c
 * Description : calculate pi
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : numerical integration method
 *               ALSP Chapter.10 Realtime Extensions
 */
#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
int num_steps=400000000; /* integration 횟수: 8억번 (너무 많으면 줄이자.) */

struct timespec diff_ts(struct timespec t1, struct timespec t2);

clockid_t	clock_cpu;
int main()
{
	struct timespec		ts1, ts2, ts_diff;
#ifdef _POSIX_CPUTIME
	if (clock_getcpuclockid(0, &clock_cpu) == -1) {
		perror("clock_getcpuclockid");
		exit(EXIT_FAILURE);
	}
	clock_gettime(clock_cpu, &ts1);
	printf("1: clock_gettime = %ld.%09ld\n", ts1.tv_sec, ts1.tv_nsec);
#endif
	int i;
	double x, step, sum = 0.0;
	step = 1.0/(double) num_steps;
#pragma omp parallel
#pragma omp for private(x) reduction(+:sum) schedule(static) nowait
	for (i=0; i<num_steps; i++) {
		x = (i+0.5) * step;
		sum += 4.0/(1.0 + x*x);
	}
	printf("pi = %.8f (sum = %.8f), 4*atan(1) = %.8f\n", step*sum, sum, atan(1)*4);

#ifdef _POSIX_CPUTIME
	clock_gettime(clock_cpu, &ts2);
	ts_diff = diff_ts(ts1, ts2);
	printf("2: elapsed cpu time = %ld.%09ld\n", ts_diff.tv_sec, ts_diff.tv_nsec);
#endif
	exit(EXIT_SUCCESS);
}

struct timespec diff_ts(struct timespec t1, struct timespec t2)
{
	struct timespec t;
	t.tv_sec = t2.tv_sec - t1.tv_sec;
	t.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	if (t.tv_nsec < 0) {
		t.tv_sec--;
		t.tv_nsec += 1000000000;
	}
	return t;
}
