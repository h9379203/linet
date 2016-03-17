/* vim: set ts=4 sw=4: */
/* Filename    : omp_loop2.c
 * Description : simple OpenMP model
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#include <stdio.h>
#include <omp.h>
int main() 
{
	int	i;
	/* combine two clauses */
#pragma omp parallel for
	for (i=0; i<8; i++) {
		printf("[%d] Hello OpenMP\n", i);
	}
	/* implicit barrier */
	return 0;
}
