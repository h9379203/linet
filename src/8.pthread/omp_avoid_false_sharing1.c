#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define NUM_THREADS     4
#define ITER_LOOP       400000000
struct sheep {
	int   cnt;
	char  padding[60];
};  /* 64바이트 캐시 라인에 정렬한 구조체 선언 */
struct sheep cnt_sheep[NUM_THREADS];

int count_sheep(int);
int main()
{
	int i;
#ifdef _OPENMP
	omp_set_num_threads(NUM_THREADS);
#endif
#pragma omp parallel for
	for (i=0; i<NUM_THREADS; i++) {
		count_sheep(i);
	}
	return 0;
}

int count_sheep(int idx)
{
	int i;
	struct sheep *s = &cnt_sheep[idx];
	for (i=idx; i<ITER_LOOP; i++) {
		s->cnt += (i % 2);
	}
	printf("[idx:%d] sum(%d) (%p)\n", idx, s->cnt, s);
	return 0;
}
