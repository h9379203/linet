/* vim: set ts=4 sw=4: */
/* Filename    : pthread_pi_cputime.c
 * Description : calculate pi
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : numerical integration method
 *               ALSP Chapter.10 Realtime Extensions
 */
#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS		3
static int num_steps=1000000000;
double pi, step;
struct start_arg {
	int		idx;
	int     i_start;
	int     i_end;
	double  sum;
} start_arg[NUM_THREADS];

struct timespec diff_ts(struct timespec t1, struct timespec t2);

void *start_func(void *arg)
{
	int     i;
	double  x, sum = 0.0;
	struct start_arg *p_arg = (struct start_arg *) arg;
#ifdef _POSIX_THREAD_CPUTIME
	int		ret;
	struct timespec     ts1, ts2, ts_diff;
	clockid_t	clock_cpu;

	if ((ret = pthread_getcpuclockid(pthread_self(), &clock_cpu)) != 0) {
        return NULL;
    }
	clock_gettime(clock_cpu, &ts1);
	printf("[T%d] 1: clock_gettime = %ld.%09ld\n", p_arg->idx, ts1.tv_sec, ts1.tv_nsec);
#endif

	for (i=p_arg->i_start; i<p_arg->i_end; i++) {
		x = (i+0.5) * step;
		sum += 4.0/(1.0 + x*x);
	}
	p_arg->sum = sum;

#ifdef _POSIX_THREAD_CPUTIME
	clock_gettime(clock_cpu, &ts2);
	ts_diff = diff_ts(ts1, ts2);
	printf("[T%d] 2: elapsed cpu time = %ld.%09ld\n", p_arg->idx, ts_diff.tv_sec, ts_diff.tv_nsec);
#endif
	return NULL;
}

int main()
{
	int     i;
	double  sum;
	pthread_t   pt_id[2];

	step = 1.0/(double) num_steps;
	start_arg[0].i_start = 0;
	start_arg[0].i_end   = num_steps>>1;
	start_arg[1].i_start = start_arg[0].i_end + 1;
	start_arg[1].i_end   = start_arg[1].i_start + (num_steps>>2);
	start_arg[2].i_start = start_arg[1].i_end + 1;
	start_arg[2].i_end   = num_steps;

	printf("%d~%d, %d~%d, %d~%d\n",
			start_arg[0].i_start, start_arg[0].i_end ,
			start_arg[1].i_start, start_arg[1].i_end ,
			start_arg[2].i_start, start_arg[2].i_end );

	for (i=3; i--; ) { /* create pthread */
		start_arg[i].idx = i;
		if (pthread_create(&pt_id[i], NULL, start_func, &start_arg[i])){
			perror("pthread_create");
			return 1;
		}
	}
	for (i=3; i--; ) { /* join pthread : explicit barrier */
		if (pthread_join(pt_id[i], NULL)){
			perror("pthread_join");
			return 1;
		}
	}
	sum = start_arg[0].sum + start_arg[1].sum + start_arg[2].sum;
	printf("PI = %.8f (sum = %.8f)\n", step*sum, sum);
	return EXIT_SUCCESS;
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

