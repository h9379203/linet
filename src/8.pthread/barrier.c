/* vim: set ts=4 sw=4: */
/* Filename    : barrier.c
 * Description : pthread barrier
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include "stdalsp.h"
#define NUM_THREADS	5

pthread_barrier_t	pt_barrier;	/* barrier */
struct thread_arg {
	pthread_t	tid;	/* thread id 	*/
	int			idx;	/* thread index */
} t_arg[NUM_THREADS];

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);
#define GET_TIME0(a)	get_time0(a, sizeof(a))
char * get_time0(char *buf, size_t sz_buf);

int main()
{
	int		i;
	pthread_barrier_init(&pt_barrier, NULL, NUM_THREADS -1);
	for(i=0; i<NUM_THREADS; i++) {
		t_arg[i].idx = i;
		if (pthread_create(&t_arg[i].tid, NULL, start_thread, (void *)&t_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
	}
	clean_thread(t_arg);
	pthread_barrier_destroy(&pt_barrier);
	return 0;
}

/* Name : start_thread
 * Desc : thread function with message
 * Argv : int idx (child index)
 * Ret  : None
 */
void *start_thread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	char	ts_now[20];
	int 	ret;

	pr_out("[Thread:%d] [%s] sleep(%d)", 
			t_arg->idx, GET_TIME0(ts_now), t_arg->idx + 2);
	sleep(t_arg->idx + 2);
	ret = pthread_barrier_wait(&pt_barrier);
	if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
		pr_out("[Thread:%d] PTHREAD_BARRIER_SERIAL_THREAD", t_arg->idx);
	}
	pr_out("\t[Thread:%d] [%s] wake up", t_arg->idx, GET_TIME0(ts_now));
	pthread_exit(t_arg);
}

/* Name : clean_thread
 * Desc : clean(join) thread
 * Argv : None
 * Ret  : None
 */
void clean_thread(struct thread_arg *t_arg)
{
	int     i;
	struct thread_arg *t_arg_ret;

	for (i=0; i<NUM_THREADS; i++, t_arg++) {
		pthread_join(t_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread_join : %d - %lu", t_arg->idx, t_arg->tid);
	}
}



/* Name : get_time0
 * Desc : print current time with UNIX time stamp
 * Argv : None
 * Ret  : Not null(if success) NULL(fail to convert)
 */
char * get_time0(char *buf, size_t sz_buf)
{
/* #define STR_TIME_FORMAT		"%y-%m-%d/%H:%M:%S" */
#define STR_TIME_FORMAT		"%H:%M:%S"
	time_t	t0;
	struct tm	tm_now;

	if (buf == NULL) return NULL;
	if (time(&t0) == ((time_t)-1)) {
		return NULL;
	}
	localtime_r(&t0, &tm_now);

	if (strftime(buf, sz_buf, STR_TIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}

	return buf;
}
