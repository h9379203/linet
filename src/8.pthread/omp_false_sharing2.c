#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define NUM_THREADS		4
#define ITER_LOOP		400000000
int t0_sum_mod2, t1_sum_mod2, t2_sum_mod2, t3_sum_mod2;
int t0_sum_mod3, t1_sum_mod3, t2_sum_mod3, t3_sum_mod3;
int count_sheep(int, int *, int *);
#define COUNT_SHEEP(t_idx)	count_sheep(t_idx, &t##t_idx##_sum_mod2, &t##t_idx##_sum_mod3)
int main()
{
#ifdef _OPENMP
	omp_set_num_threads(NUM_THREADS);
#endif
#pragma omp parallel sections
	{
#pragma omp section
		{
			COUNT_SHEEP(0);
		}
#pragma omp section
		{
			COUNT_SHEEP(1);
		}
#pragma omp section
		{
			COUNT_SHEEP(2);
		}
#pragma omp section
		{
			COUNT_SHEEP(3);
		}
	}
	return 0;
}

int count_sheep(int idx, int *sum_mod2, int *sum_mod3)
{
	int i;
	for (i=0; i<ITER_LOOP; i++) {
		*sum_mod2 += (i % 2);
		*sum_mod3 += (i % 3);
	}
	printf("[idx:%d] sum(%d,%d)\n", idx, *sum_mod2, *sum_mod3);
	return 0;
}
