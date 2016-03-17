/* vim: set ts=4 sw=4: */
/* Filename    : spinlock.c
 * Description : example of pthread spin
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 2nd Ed.
 */
#define _XOPEN_SOURCE 700
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include "stdalsp.h"
#define NUM_THREADS     4

struct thread_arg {
	pthread_t   tid;    /* thread id    */
	int         idx;    /* thread index */
} *t_arg;

pthread_spinlock_t spinlock;
int	flag_spin = 101;

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main()
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

	for(i=0; i<NUM_THREADS; i++) {
		t_arg[i].idx = i;
		if ((ret = pthread_create(&t_arg[i].tid, NULL, start_thread, (void *)&t_arg[i]))) {
			pr_err("pthread_create : %s", strerror(ret));
			return 0;
		}
		pr_out("pthread_create : tid = %lu", t_arg[i].tid);
	}
	clean_thread(t_arg);
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
	pthread_spin_lock(&spinlock);
	/* 원래는 spinlock구간에서 stduout 출력 작업도 자제해야 한다. 
	 * 허나 이 예제는 보여주기 위함이니까 이해를 바란다.
	 * */
	pr_out("[Thread:%d] flag_spin(%d)", t_arg->idx, flag_spin); 
	flag_spin++;
	pthread_spin_unlock(&spinlock);
	return t_arg; /* an alternative func, pthread_exit(t_arg) */
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
