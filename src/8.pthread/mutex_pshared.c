/* vim: set ts=4 sw=4: */
/* Filename    : mutex_pshared.c
 * Description : example of pthread mutex
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 2nd Ed.
 */
#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <errno.h>
#include "stdalsp.h"
#define NUM_THREADS     6
#define SHM_SEGMENT		4096
const char *shm_path = "/alsp_mutex";
int		shm_fd;		/* shared memory fd 	 */

struct thread_arg {
	pthread_t   tid;    /* thread id    */
	int         idx;    /* thread index */
} *t_arg;

struct shr_data {
	pid_t		prev_pid;
	int			prev_idx;
	time_t		prev_time;
	pthread_mutex_t		mutex;
} *shr_data;


void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main(int argc, char *argv[])
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));
	/* get posix shm memory */
	if ( (shm_fd = shm_open(shm_path, O_CREAT|O_RDWR|O_EXCL, 0660)) == -1) {
		if (errno == EEXIST) {
			shm_fd = shm_open(shm_path, O_RDWR, 0660);
		} else {
			/* error */
			exit(EXIT_FAILURE);
		}
		ftruncate(shm_fd, SHM_SEGMENT);
	}
	shr_data = (struct shr_data *) mmap((void *)0, SHM_SEGMENT,
			PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shr_data == MAP_FAILED) { /* error */
		exit(EXIT_FAILURE);
	}

	if (argc > 1 && argv[1][0] == 'c') { /* initialize mutex if create */
		pr_out("init mutex");
		pthread_mutexattr_t	mutexattr;
		memset(shr_data, 0, sizeof(struct shr_data));
		pthread_mutexattr_init(&mutexattr); /* init mutex attribute var */
		pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&shr_data->mutex, &mutexattr);
	}
	for(i=0; i<NUM_THREADS; i++) {
		t_arg[i].idx = i;
		if ((ret = pthread_create(&t_arg[i].tid, NULL, start_thread, (void *)&t_arg[i]))) {
			pr_err("pthread_create : %s", strerror(ret));
			return 0;
		}
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
	int		ret;
	sleep(t_arg->idx); /* defered */
	if ((ret = pthread_mutex_lock(&shr_data->mutex))) {
		if (ret == EDEADLK) {
			pr_err("\t lock : EDEADLK detected");
		} else {
			pr_err("\t lock (errno = %s)", strerror(ret));
		}
	}
	pr_out("[%d,%d] => [%d,%d]", 
			shr_data->prev_pid, shr_data->prev_idx,
			getpid(), t_arg->idx);
	sleep(t_arg->idx + 1);
	shr_data->prev_pid = getpid();
	shr_data->prev_idx = t_arg->idx;
	if ((ret = pthread_mutex_unlock(&shr_data->mutex))) {
		pr_err("\t unlock: (errno = %s)", strerror(ret));
	}
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
		pr_out("pthread_join : %d", t_arg->idx);
	}
}
