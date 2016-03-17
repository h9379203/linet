#define _XOPEN_SOURCE	600
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#define ITER_LOOP	5
struct tls_data {
	int		idx;
	char	str[32];
} tls;
#pragma omp threadprivate(tls)

int main()
{
	int i;
	printf("[Master] tls(%s) (%p)\n", tls.str, tls.str);
#pragma omp parallel
	{
#pragma omp single nowait
		{
#pragma omp task
			{
				tls.idx = omp_get_thread_num();
				strcpy(tls.str, "12345");
				for (i=0;i<ITER_LOOP;i++) {
					printf("[Thread:%d] (i=%d) %s (%p)\n", 
							tls.idx, i, tls.str, tls.str);
					sleep(1);
				}
			}
#pragma omp task
			{
				tls.idx = omp_get_thread_num();
				strcpy(tls.str, "67890");
				for (i=0;i<ITER_LOOP;i++) {
					printf("[Thread:%d] (i=%d) %s (%p)\n", 
							tls.idx, i, tls.str, tls.str);
					sleep(1);
				}
			}
#pragma omp task
			{
				tls.idx = omp_get_thread_num();
				strcpy(tls.str, "+-*/%");
				for (i=0;i<ITER_LOOP;i++) {
					printf("[Thread:%d] (i=%d) %s (%p)\n", 
							tls.idx, i, tls.str, tls.str);
					sleep(1);
				}
			}
		}
	}
	printf("[Master] tls(%s) (%p)\n", tls.str, tls.str);
	return 0;
}
