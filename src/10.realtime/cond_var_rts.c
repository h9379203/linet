/* vim: set ts=4 sw=4: */
/* Filename    : cond_var_rts.c
 * Description : condition variable
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#define WITH_SIGEV_NOTIFICATION
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "stdalsp.h"

#define NUM_THREADS	10
#define MAX_ITEMS	64
#define PATH_FIFO	"/tmp/my_fifo"

struct workqueue {
	int			item[MAX_ITEMS];
	int			idx;		/* last index 	*/
	int			cnt;		/* item counter */
	pthread_mutex_t	mutex;
	pthread_cond_t	cv;
} *wq;

void *tfunc_a(void *);			/* thread  (A) */
void *tfunc_b(void *);			/* thread  (B) */

#ifdef WITH_SIGEV_NOTIFICATION
void *start_sigthread(void *);	/* signal thread */
#endif
struct thread_arg {
	pthread_t	tid;	/* thread id 	*/
	int			idx;
	void *(*func)();
} t_arg[NUM_THREADS] = {
	{0, 0, tfunc_a},
	{0, 0, tfunc_b},
	{0, 0, tfunc_b},
	{0, 0, tfunc_b},
#ifdef WITH_SIGEV_NOTIFICATION
	{0, 0, start_sigthread},
#endif
	{0, 0, NULL}
};

int push_item(struct workqueue *wq, const char *item, int cnt);
int pop_item(struct workqueue *wq, int *item);
int process_job(int *);
void clean_thread(struct thread_arg *);

int main()
{
	int		i;
	if ((wq = calloc(1, sizeof(struct workqueue))) == NULL) {
		pr_err("calloc(%s)", strerror(errno)); 
		exit(EXIT_FAILURE);
	};

#ifdef WITH_SIGEV_NOTIFICATION
	sigset_t    sigset_mask;
	sigfillset(&sigset_mask);
	sigdelset(&sigset_mask, SIGINT);
	sigdelset(&sigset_mask, SIGTERM);
	pthread_sigmask(SIG_SETMASK, &sigset_mask, NULL);
#endif

	pthread_mutex_init(&wq->mutex, NULL);
	pthread_cond_init(&wq->cv, NULL);

	/* Start thread */
	for(i=0; i<NUM_THREADS && t_arg[i].func != NULL; i++) {
		t_arg[i].idx = i;
		if (pthread_create(&t_arg[i].tid, NULL, 
					t_arg[i].func, (void *)&t_arg[i])) {
			return EXIT_FAILURE;
		}
		pr_out("pthread_create : tid = %lu", t_arg[i].tid);
	}
	clean_thread(t_arg);
	return EXIT_SUCCESS;
}

/* Name : tfunc_a
 * Desc : Thread (A)
 * Argv :
 * Ret  : None
 */
void *tfunc_a(void *arg)
{
	int		fd, ret_read = 0;
	char	buf[MAX_ITEMS/2];

	pr_out(" >> Thread (A) started!");
	/* make FIFO */
	if (mkfifo(PATH_FIFO, 0644) == -1) {
		if (errno != EEXIST) {
			pr_err("[A] FAIL: mkfifo : %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	if ((fd = open(PATH_FIFO, O_RDONLY, 0644)) == -1) {
		pr_err("[A] FAIL: open : %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((ret_read = read(fd, buf, sizeof(buf))) == -1) {
			pr_err("[A] FAIL: read : %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (ret_read == 0) { /* closed by peer */
			pr_err("[A] broken pipe: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		push_item(wq, buf, ret_read);	/* push item */

		pr_out("[A] cond_broadcast");
		pthread_cond_broadcast(&wq->cv);
	} /* loop: while */
	return NULL;
}

/* Name : tfunc_b
 * Desc : Thread (B)
 * Argv : 
 * Ret  : None
 */
void *tfunc_b(void *arg)
{
	int		item;
	union sigval    si_val;
	pr_out(" >> Thread (B) started!");
	while (1) {
		pop_item(wq, &item);
		process_job(&item);

#ifdef WITH_SIGEV_NOTIFICATION
		si_val.sival_ptr = arg; /* send thread arg address pointer */
		if (sigqueue(getpid(), SIGRTMIN, si_val)) {
			perror("sigqueue"); 
			exit(EXIT_FAILURE);
		}
#endif

	} /* loop: while */
	return NULL;
}

#ifdef WITH_SIGEV_NOTIFICATION
void *start_sigthread(void *arg)
{
    struct thread_arg *thr;
    sigset_t    sigset_mask;
	siginfo_t	info;
    int     ret_signo;
    printf("* Start signal thread (tid = %lu)\n", (long)pthread_self());
    sigemptyset(&sigset_mask);
    sigaddset(&sigset_mask, SIGRTMIN);
    while (1) {
        if ((ret_signo = sigwaitinfo(&sigset_mask, &info)) == -1) {
            pr_err("FAIL:sigwaitinfo(%s)\n", strerror(errno));
        }
		if (ret_signo == SIGRTMIN) {
			thr = (struct thread_arg *)info.si_value.sival_ptr;
			printf("\t[RTS] notification from (%lu).\n", thr->tid);
		} else {
			printf("\t[RTS] unknown.\n");
		}
    } /* loop: while */
    return t_arg;
}
#endif

/* Name : push_item
 * Desc : 
 * Argv : 
 * Ret  : counter
 */
int push_item(struct workqueue *wq, const char *item, int cnt) 
{
	int i, j;
	/* i = counter, j = (idx+cnt)%MAX_ITEMS */
	/* 예제니까 MAX_ITEMS보다 더 많은 cnt가 들어와서 overflow 된다면 그냥 버리자. */
	pthread_mutex_lock(&wq->mutex); /* lock */
	for (i=0, j= (wq->idx + wq->cnt)%MAX_ITEMS; i<cnt; i++, j++, wq->cnt++) {
		if (wq->cnt == MAX_ITEMS) { /* overflow */
			pr_err("[Q:%d,%d] queue full : wq(idx,cnt=%d,%d)", i, j, wq->idx, wq->cnt);
			break;
		}
		if (j == MAX_ITEMS) j=0;	/* circular queue로 구현하기 위해... */
		wq->item[j] = (int) item[i];
		pr_out("[Q:%d,%d] push (idx,cnt=%d,%d) : item=(%c)", i, j, wq->idx, wq->cnt, item[i]);
	} /* loop: for */
	pthread_mutex_unlock(&wq->mutex); /* unlock */
	return i;
}

/* Name : pop_item
 * Desc : 
 * Argv : 
 * Ret  : 1(success), 0(fail)
 */
int pop_item(struct workqueue *wq, int *item)
{
	pthread_mutex_lock(&wq->mutex); /* lock */
    while (1) {
        if (wq->cnt > 0) {
            if (wq->idx == MAX_ITEMS) wq->idx = 0;
            *item = wq->item[wq->idx];
            wq->idx++;
            wq->cnt--;
            pr_out("[B] pop(%d,%d) item(%c) (tid=%ld)",
                    wq->idx, wq->cnt, (char)*item, pthread_self());
            break;
        } else {
            pr_out("[B] cond_wait (tid=%ld)", pthread_self());
            pthread_cond_wait(&wq->cv, &wq->mutex); /* wait */
            pr_out("[B] Wake up (tid=%ld)", pthread_self());
        }
    } /* loop: while */
    pthread_mutex_unlock(&wq->mutex); /* unlock */
    return 0;
}

/* Name : process_job
 * Desc : 
 * Argv : 
 * Ret  : None
 */
int process_job(int *item)
{
	pr_out("[B] item=%d", *item);
	sleep(*item % 5 + 1);
	return 0;
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
	printf("[Clean] All thread are joined.\n");
}
