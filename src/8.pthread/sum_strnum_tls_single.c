/* vim: set ts=4 sw=4: */
/* Filename    : sum_strnum_tls.c
 * Description : specified-data key (pthread TLS) 
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : single threaded
 */

#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdalsp.h"
#define NUM_THREADS		3
#define LEN_SUM_STR		16


char *sum_strnum(const char *, const char *);

/* pthread TLS */
pthread_once_t once_tls_key = PTHREAD_ONCE_INIT;
pthread_key_t		tls_key;
void init_tls_key(void);	/* init TLS */
void destroy_tls(void *);	/* destructor */

int main()
{
	char	*ret_str;

	ret_str = sum_strnum("1", "4");
    printf("1 + 4 = %s (%p)\n", ret_str, ret_str);

	ret_str = sum_strnum("5", "2");
	printf("5 + 2 = %s (%p)\n", ret_str, ret_str);
    return EXIT_SUCCESS;
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
