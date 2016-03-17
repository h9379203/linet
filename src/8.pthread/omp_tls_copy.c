#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
struct tls_data {
	int		cnt;
	char	data[32];
} tls;
#pragma omp threadprivate(tls)

int main()
{
	tls.cnt = 5;
	strcpy(tls.data, "12345");
	printf("[Master] tls(%.*s)\n", tls.cnt, tls.data);
	omp_set_num_threads(4);
#pragma omp parallel copyin(tls)
	{
#pragma omp sections 
		{
#pragma omp section
			{
				printf("[T:%d] 1:tls(%.*s)\n", 
						omp_get_thread_num(), tls.cnt, tls.data);
				sleep(1);
			}
#pragma omp section
			{
				printf("[T:%d] 1:tls(%.*s)\n", 
						omp_get_thread_num(), tls.cnt, tls.data);
				sleep(1);
			}
		} /* sections */
#pragma omp single copyprivate(tls)
		{
			tls.cnt = 3;
			strcpy(tls.data, "xyz");
			printf("[T:%d] single copyprivate(%.*s)\n", 
					omp_get_thread_num(), tls.cnt, tls.data);
		} /* single */
#pragma omp sections 
		{
#pragma omp section
			{
				printf("[T:%d] 2:tls(%.*s)\n", 
						omp_get_thread_num(), tls.cnt, tls.data);
				sleep(1);
			}
#pragma omp section
			{
				printf("[T:%d] 2:tls(%.*s)\n", 
						omp_get_thread_num(), tls.cnt, tls.data);
				sleep(1);
			}
		} /* sections */
	} /* parallel */
	printf("[Master] tls(%.*s)\n", tls.cnt, tls.data);
	return 0;
}
