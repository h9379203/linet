#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int num_steps=200000000; /* integration 횟수: 2억번 (너무 많으면 줄이자.) */

struct timespec diff_timespec(struct timespec t1, struct timespec t2);
int main()
{
	int	ret;
#ifdef _POSIX_CPUTIME
	struct timespec     ts1, ts2, ts_diff;
	clockid_t	clock_cpu;
	if ((ret = clock_getcpuclockid(0, &clock_cpu)) != 0) { 
		return EXIT_FAILURE;
	}
	clock_gettime(clock_cpu, &ts1);
#endif
	int i;
	double x, step, sum = 0.0;
	step = 1.0/(double) num_steps;
	for (i=0; i<num_steps; i++) {
		x = (i+0.5) * step;
		sum += 4.0/(1.0 + x*x);
	}
	printf("pi = %.8f (sum = %.8f)\n", step*sum, sum);
	sleep(1);
#ifdef _POSIX_CPUTIME
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
	ts_diff = diff_timespec(ts1, ts2);
	printf("elapsed cpu time = %ld.%09ld\n", ts_diff.tv_sec, ts_diff.tv_nsec);
#endif
	return EXIT_SUCCESS;
}

struct timespec diff_timespec(struct timespec t1, struct timespec t2)
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
