/* vim: set ts=4 sw=4: */
/* Filename    : omp_sum_strnum_tls.c
 * Description : OpenMP TLS
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
#define LEN_SUM_STR		16

char *sum_strnum(const char *, const char *);

int main()
{
#pragma omp parallel sections
	{
#pragma omp section
		{
			char *x = "1", *y = "3";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)", 
					omp_get_thread_num(), x, y, ret_str, ret_str);
		}
#pragma omp section
		{
			char *x = "4", *y = "4";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)", 
					omp_get_thread_num(), x, y, ret_str, ret_str);
		}
#pragma omp section
		{
			char *x = "1", *y = "5";
			char *ret_str = sum_strnum(x, y);
			pr_out("[T:%d] %s + %s = %s (%p)", 
					omp_get_thread_num(), x, y, ret_str, ret_str);
		}
	} /* omp parallel, sections */
    return EXIT_SUCCESS;
}

char *sum_strnum(const char *s1, const char *s2)
{
	static char tls_str[LEN_SUM_STR];
#pragma omp threadprivate(tls_str)
	snprintf(tls_str, LEN_SUM_STR, "%d", atoi(s1) + atoi(s2));
	return tls_str;
}
