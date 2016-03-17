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

void *start_func(void *);
void clean_thread(struct thread_arg *);
char *sum_strnum(const char *, const char *);

/* pthread TLS */
pthread_once_t once_tls_key = PTHREAD_ONCE_INIT;
pthread_key_t		tls_key;
void init_tls_key(void);	/* init TLS */
void destroy_tls(void *);	/* destructor */

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
	pthread_key_delete(tls_key);
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
	flockfile(stdout);
    printf("%s + %s = %s (%p)\n", 
			t_arg->x, t_arg->y, ret_str, ret_str);
	funlockfile(stdout);
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

/* apply pthread specific key */
char *sum_strnum(const char *s1, const char *s2)
{
	char *tls_str;
	pthread_once(&once_tls_key, init_tls_key);	/* init once */
	if ((tls_str = pthread_getspecific(tls_key)) == NULL) { 
		/* not exist TLS */
		tls_str = malloc(LEN_SUM_STR);
		pthread_setspecific(tls_key, tls_str); /* register TLS */
	}
	snprintf(tls_str, LEN_SUM_STR, "%d", atoi(s1) + atoi(s2));
	return tls_str;
}


/* init TLS key */
void init_tls_key(void)
{
	pthread_key_create(&tls_key, destroy_tls);
}

/* destructor */
void destroy_tls(void *tls)
{
	printf("destructor: TID(%ld) TLS(%p)\n", pthread_self(), tls);
	free(tls);
}
