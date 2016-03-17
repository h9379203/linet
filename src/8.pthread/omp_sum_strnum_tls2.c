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
#include <omp.h>
#include "stdalsp.h"
#define NUM_THREADS		3
#define LEN_SUM_STR		16

char *sum_strnum(const char *, const char *);

int main()
{
#pragma omp parallel 
#pragma omp single
	{
#pragma omp task
		{
			char *x = "1", *y = "3";
			char *ret_str = sum_strnum(x, y);
			if (omp_get_thread_num() == 0) usleep(500000);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "4", *y = "4";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			sleep(1);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "1", *y = "5";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			sleep(2);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "2", *y = "5";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			if (omp_get_thread_num() == 0) usleep(500000);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "2", *y = "1";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			if (omp_get_thread_num() == 0) usleep(500000);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "5", *y = "4";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			if (omp_get_thread_num() == 0) usleep(500000);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
#pragma omp task
		{
			char *x = "7", *y = "8";
			char *ret_str = sum_strnum(x, y);
#pragma omp taskyield
			if (omp_get_thread_num() == 0) usleep(500000);
			printf("%s + %s = %s (%p)\n",
					x, y, ret_str, ret_str);
		}
	} /* omp parallel, sections */
    return EXIT_SUCCESS;
}

/* apply pthread specific key */
char *sum_strnum(const char *s1, const char *s2)
{
	static char tls_str[LEN_SUM_STR];
#pragma omp threadprivate(tls_str)
	snprintf(tls_str, LEN_SUM_STR, "%d", atoi(s1) + atoi(s2));
	return tls_str;
}
