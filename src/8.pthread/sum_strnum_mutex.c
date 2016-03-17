/* vim: set ts=4 sw=4: */
/* Filename    : sum_strnum_tls.c
 * Description : specified-data key (pthread TLS)
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       :
 */

#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdalsp.h"
#define NUM_THREADS		3
#define LEN_SUM_STR		16
struct thread_arg {
	pthread_t   tid;   /* thread ID */
	int			idx;
	char		*x, *y;
} t_arg[NUM_THREADS];

pthread_mutex_t	 mutex = PTHREAD_MUTEX_INITIALIZER;

void *start_func(void *);
void clean_thread(struct thread_arg *);
char *sum_strnum(const char *, const char *);

int main()
{
	t_arg[0].idx = 0;
	t_arg[0].x = "1";
	t_arg[0].y = "3";
    if (pthread_create(&t_arg[0].tid, NULL, start_func, &t_arg[0]) != 0) {
        exit(1);
    }
	t_arg[1].idx = 1;
	t_arg[1].x = "4";
	t_arg[1].y = "4";
    if (pthread_create(&t_arg[1].tid, NULL, start_func, &t_arg[1]) != 0) {
        exit(1);
    }
	t_arg[2].idx = 2;
	t_arg[2].x = "1";
	t_arg[2].y = "5";
    if (pthread_create(&t_arg[2].tid, NULL, start_func, &t_arg[2]) != 0) {
        exit(1);
    }
	clean_thread(t_arg);
    return EXIT_SUCCESS;
}	


/* Name : start_func
 * Desc : Thread
 * Argv :
 * Ret  : 
 */
void *start_func(void *arg)
{
	struct thread_arg	*t_arg = (struct thread_arg *)arg;
	char	*ret_str = sum_strnum(t_arg->x, t_arg->y);
	if (t_arg->idx == 0) usleep(500000);
    printf("%s + %s = %s (%p)\n", 
			t_arg->x, t_arg->y, ret_str, ret_str);
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

char *sum_strnum(const char *s1, const char *s2)
{
	static char buf_sum[LEN_SUM_STR];
	pthread_mutex_lock(&mutex);
	snprintf(buf_sum, sizeof(buf_sum), "%d", atoi(s1) + atoi(s2));
	pthread_mutex_unlock(&mutex);
	return buf_sum;
}

