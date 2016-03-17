/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop3.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : omp_get_thread_num
 */
#include <stdio.h>
#include <omp.h>
int main() 
{
	int	i;
	/* combine two clauses */
#pragma omp parallel for
	for (i=0; i<8; i++) {
		printf("[%d] Hello OpenMP (%d)\n", i, omp_get_thread_num());
	}
	/* implicit barrier */
	return 0;
}
